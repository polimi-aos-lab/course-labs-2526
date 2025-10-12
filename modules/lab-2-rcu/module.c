#include "linux/rculist.h"
#include "linux/rcupdate.h"
#include "linux/spinlock_types.h"
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/slab.h>

struct list_element {
  int data;
  struct list_head list;
};

static LIST_HEAD(my_list);
static DEFINE_SPINLOCK(list_lock);

static int read_list_thread(void *data) {
  while (!kthread_should_stop()) {
    struct list_element *entry;
    rcu_read_lock();
    pr_info("[ ");
    list_for_each_entry_rcu(entry, &my_list, list) {
      pr_info("%d ", entry->data);
    }
    pr_info("] \n");
    rcu_read_unlock();
    msleep(100);
  }
  return 0;
}

static int manipulate_list_thread(void *data) {
  while (!kthread_should_stop()) {
    struct list_element *entry, *temp;
    entry = kmalloc(sizeof(struct list_element), GFP_KERNEL);
    spin_lock(&list_lock);
    if (!list_empty(&my_list)) {
      temp = list_first_entry(&my_list, struct list_element, list);
      list_del_rcu(&temp->list);
      entry->data = temp->data + 1;
      spin_unlock(&list_lock);
      synchronize_rcu();
      kfree(temp);
      spin_lock(&list_lock);
    }
    list_add_rcu(&entry->list, &my_list);
    spin_unlock(&list_lock);
    msleep(200);
  }
  return 0;
}

static struct task_struct *read;
static struct task_struct *manipulate;

static int __init my_module_init(void) {
  int i;
  for (i = 0; i < 10; i++) {
    struct list_element *entry =
        kmalloc(sizeof(struct list_element), GFP_KERNEL);
    entry->data = i;
    INIT_LIST_HEAD(&entry->list);
    spin_lock(&list_lock);
    list_add_tail(&entry->list, &my_list);
    spin_unlock(&list_lock);
  }
  read = kthread_run(read_list_thread, NULL, "read_list_thread");
  manipulate =
      kthread_run(manipulate_list_thread, NULL, "manipulate_list_thread");
  return 0;
}

static void __exit my_module_exit(void) {
  if (read) {
    kthread_stop(read);
  }
  if (manipulate) {
    kthread_stop(manipulate);
  }
}

MODULE_LICENSE("GPL");
module_init(my_module_init);
module_exit(my_module_exit);
