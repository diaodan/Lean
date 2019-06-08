#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include "sbull.h"

MODULE_LICENSE("DUAL BSD/GPL");
int major = -1;

static int hello_init(void)
{
    int ret=0;
    INFO();
    printk(KERN_ALERT "Hello world\n");
    ret = register_blkdev(0, "sbull");
    if (ret < 0) {
        INFO("register_blkdev failed\n");
    }
    major = ret;

    return 0;
}

static void hello_exit(void)
{
    INFO("%s", "test");
    printk(KERN_ALERT "Good bye\n");
    if (major > 0) {
        unregister_blkdev(major, "sbull");
    }
}

module_init(hello_init);
module_exit(hello_exit);
