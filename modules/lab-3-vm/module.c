#include "linux/gfp.h"
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/random.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");

static void *random_us_ptr(void) {
  u64 rand_val;
  get_random_bytes(&rand_val, sizeof(rand_val));
  rand_val = rand_val & 0x00ffffff;
  return (void *)rand_val;
}

static void print_proc_info(void) {
  struct mm_struct *mm;
  struct vm_area_struct *vma, *vmaf;
  int i;
  mm = current->mm;
  pr_info("Current process is %s\n", current->comm);
  vmaf = mm->mmap;
  for (vma = vmaf; vma; vma = vma->vm_next) {
    pr_info("VMA: 0x%lx - 0x%lx \n", vma->vm_start, vma->vm_end);
  }
  for (i = 0; i < 1000; i++) {
    u64 v;
    int res;
    u64 *p = random_us_ptr();
    res = copy_from_user(&v, p, sizeof(u64));
    if (!res) {
      pr_info("Succesfully read v: %llx\n from %px\n", v, p);
    } else {
      pr_info("Survived reading data from %px \n", p);
    }
  }
}

struct my_struct {
  u64 field1, field2;
  u8 field3;
  spinlock_t lock;
} my_struct;

static int howmany = 0;

static void my_constructor(void *addr) {
  struct my_struct *p = (struct my_struct *)addr;
  memset(p, 0, sizeof(struct my_struct));
  spin_lock_init(&p->lock);
  howmany++;
  pr_info("my_constructor invoked : %d times\n", howmany);
}

struct kmem_cache *cc;
static void build_and_fill_my_kmem_cache(void) {
  int i;
  cc = kmem_cache_create("my_struct", sizeof(struct my_struct), 0, 0,
                         my_constructor);
  for (i = 0; i < 129; i++) {
    struct my_struct *p = kmem_cache_alloc(cc, GFP_KERNEL);
    pr_info("kmem_cache_alloc: %d \n", i);
  }
}

static int __init my_module_init(void) {
  // print_proc_info();
  build_and_fill_my_kmem_cache();
  return 0;
}

module_init(my_module_init);
