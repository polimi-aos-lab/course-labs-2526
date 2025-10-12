#include "linux/types.h"
#include <linux/kthread.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");

atomic_t shared_variable = ATOMIC_INIT(0);
uint64_t iter = (1 << 20);

static int add_thread(void *data) {
  uint64_t i;
  for (i = 0; i < iter; i++) {
    int s;
    do {
      s = atomic_read((const atomic_t *)&shared_variable);
    } while (atomic_cmpxchg(&shared_variable, s, s + 1) != s);
  }
  pr_info("[ADD] finished with: %lld\n", atomic_read(&shared_variable));
  return 0;
}

static int sub_thread(void *data) {
  uint64_t i;
  for (i = 0; i < iter; i++) {
    int s;
    do {
      s = atomic_read((const atomic_t *)&shared_variable);
    } while (atomic_cmpxchg(&shared_variable, s, s - 1) != s);
  }
  pr_info("[SUB] finished with: %lld\n", atomic_read(&shared_variable));
  return 0;
}

static int __init my_module_init(void) {
  kthread_run(add_thread, NULL, "add_thread");
  kthread_run(sub_thread, NULL, "sub_thread");
  return 0;
}

module_init(my_module_init);
