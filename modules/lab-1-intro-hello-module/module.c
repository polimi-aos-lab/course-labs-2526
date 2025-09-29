#include <linux/module.h>

static int num = 5;

module_param(num, int, S_IRUGO);

MODULE_LICENSE("GPL"); // The license  under which the module is distributed.
MODULE_AUTHOR("Vittorio Zaccaria"); // The original author of the module (VZ).
MODULE_DESCRIPTION(
    "HelloWorld Linux Kernel Module."); // The Description of the module.
                                        //
// This function defines what happens when this module is inserted into the
// kernel. ie. when you run insmod command.

static void hello_test(void) { pr_err("This is a test!\n"); }

static int __init hello_init(void) {
  // Using a kernel parameter
  pr_info("Hello world (2025), num = %d!\n", num);

  // Printing an address
  pr_info("Address of hello_init = %p (%px)!\n", hello_init, hello_init);

  // Provoking an oops
  // char *p=NULL;
  // *p=1;
  hello_test();
  return 0; // Non-zero return means that the module couldn't be loaded.
}

// This function defines what happens when this module is removed from the
// kernel. ie.when you run rmmod command.
static void __exit hello_cleanup(void) { pr_info("Cleaning up module.\n"); }

module_init(hello_init);    // Registers the __init function for the module.
module_exit(hello_cleanup); // Registers the __exit function for the module.
