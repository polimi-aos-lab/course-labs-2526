#include "linux/irqreturn.h"
#include <asm/io.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/wait.h>

MODULE_LICENSE("GPL");

#define PORT_BASE 0x2F8
#define PORT_SIZE 8
#define PORT_IRQ 3

#define RBR PORT_BASE + 0
#define THR PORT_BASE + 0
#define IER PORT_BASE + 1
#define IIR PORT_BASE + 2
#define FCR PORT_BASE + 2
#define LSR PORT_BASE + 5

#define BUFSIZE 64

static spinlock_t txLock, rxLock;
static char rxbuffer[BUFSIZE];
static int putpos = 0, getpos = 0;
static int numchar = 0;
static wait_queue_head_t waiting;
static int major;

static irqreturn_t serial_irq(int irq, void *arg) {
  if ((inb(IIR) & 0xf) == 0x4) {
    char c;
    c = inb(RBR);
    spin_lock(&rxLock);
    if (numchar < BUFSIZE) {
      rxbuffer[putpos] = c;
      putpos = (putpos == (BUFSIZE - 1)) ? 0 : putpos + 1;
      numchar++;
    }
    wake_up(&waiting);
    spin_unlock(&rxLock);
    return IRQ_HANDLED;
  }
  return IRQ_NONE;
}

static ssize_t serialaos_read(struct file *f, char __user *buf, size_t size,
                              loff_t *o) {

  int result;
  char c;
  if (size < 1)
    return 0;

  spin_lock_irq(&rxLock);
  result = wait_event_interruptible_lock_irq(waiting, numchar > 0, rxLock);
  if (result < 0) // signal
  {
    spin_unlock_irq(&rxLock);
    return result;
  }
  c = rxbuffer[getpos];
  getpos = (getpos == (BUFSIZE - 1)) ? 0 : getpos + 1;
  numchar--;
  spin_unlock_irq(&rxLock);
  copy_to_user(buf, &c, 1);
  return 1;
}

static ssize_t serialaos_write(struct file *f, const char __user *buf,
                               size_t size, loff_t *o) {
  return 0;
}

static const struct file_operations serialaos_fops = {
    .owner = THIS_MODULE, .write = serialaos_write, .read = serialaos_read};

static int __init serialaos_init(void) {
  spin_lock_init(&txLock);
  spin_lock_init(&rxLock);
  init_waitqueue_head(&waiting);

  if (!request_region(PORT_BASE, PORT_SIZE, "serialaos")) {
    pr_info("Serial AOS, can't access\n");
  }

  request_irq(PORT_IRQ, serial_irq, 0, "serialaos", NULL);
  outb(0x0, FCR);
  outb(0x5, IER);
  major = register_chrdev(0, "serialos", &serialaos_fops);
  pr_info("serialaos: loaded\n");
  return 0;
}

module_init(serialaos_init);
