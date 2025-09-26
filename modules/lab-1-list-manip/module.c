#include "linux/hrtimer.h"
#include "linux/time.h"
#include "linux/timer.h"
#include <linux/module.h>
#include <linux/prandom.h> // prandom
#include <linux/sched/task.h>
#include <linux/slab.h> // kmalloc
                        //

struct list_node {
  int data;
  struct list_node *next;
};

struct list_node *head;
struct hrtimer my_timer;

static void add_node(int data) {
  struct list_node *new_node = kmalloc(sizeof(*new_node), GFP_KERNEL);
  new_node->data = data;
  new_node->next = head;
  head = new_node;
}

#define TIME_1SEC_NS 1000000000L
enum hrtimer_restart my_timer_handler(struct hrtimer *timer) {
  struct list_node *cur = head;
  unsigned int rnd;

  pr_info("Timer invoked\n");

  rnd = prandom_u32();
  add_node(rnd);

  while (cur) {
    if (!cur->next->next) {
      kfree(cur->next);
      cur->next = NULL;
    }
    cur = cur->next;
  }
  hrtimer_forward_now(timer, ns_to_ktime(TIME_1SEC_NS));
  return HRTIMER_RESTART;
}

static int __init my_module_init(void) {
  int i;
  for (i = 0; i < 3; i++) {
    add_node(i);
  }

  hrtimer_init(&my_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
  my_timer.function = my_timer_handler;
  hrtimer_start(&my_timer, ns_to_ktime(3 * TIME_1SEC_NS), HRTIMER_MODE_REL);
  return 0;
}
static void __exit my_module_exit(void) {}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Subtle memory corruption :-)");
