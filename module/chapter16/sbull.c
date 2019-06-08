#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/blkdev.h>
#include "sbull.h"

MODULE_LICENSE("DUAL BSD/GPL");


static int sbull_open(struct block_device *bdev, fmode_t mode)
{
    INFO();
    return 0;
}

static void sbull_release(struct gendisk *gen, fmode_t mode)
{
    INFO();
}

static int sbull_ioctl(struct block_device *bdev, fmode_t mode,
                        unsigned int cmd, unsigned long arg)
{
    INFO();
    return 0;
}

static int sbull_media_changed(struct gendisk *gen)
{
    INFO();
    return 0;
}

static int sbull_revalidate(struct gendisk *gen)
{
    INFO();
    return 0;
}


static struct block_device_operations sbull_ops = {
    .owner          = THIS_MODULE,
    .open           = sbull_open,
    .release        = sbull_release,
    .media_changed  = sbull_media_changed,
    .revalidate_disk= sbull_revalidate,
    .ioctl          = sbull_ioctl,
};


static int sbull_init(void)
{
    int ret=0;
    INFO();
    ret = register_blkdev(0, "sbull");
    if (ret < 0) {
        INFO("register_blkdev failed\n");
    }

    return 0;
}

static void sbull_exit(void)
{
    INFO("%s", "test");
    printk(KERN_ALERT "Good bye\n");
    if (sbull_major > 0) {
        unregister_blkdev(sbull_major, "sbull");
    }
}

module_init(sbull_init);
module_exit(sbull_exit);
