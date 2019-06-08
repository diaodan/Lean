#include <linux/init.h>
#include <linux/module.h>
#include "sbull.h"

MODULE_LICENSE("DUAL BSD/GPL");

static int hello_init(void)
{
    INFO();
    printk(KERN_ALERT "Hello world\n");

    return 0;
}

static void hello_exit(void)
{
    INFO("test");
    printk(KERN_ALERT "Good bye\n");
}

module_init(hello_init);
module_exit(hello_exit);
