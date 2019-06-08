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

static void sbull_transfer(struct sbull_dev *dev, unsigned long sector,
                    unsigned long nsect, char *buffer, int write)
{
    unsigned long offset = sector * KERNEL_SECTOR_SIZE;
    unsigned long nbytes = nsect * KERNEL_SECTOR_SIZE;

    if ((offset + nbytes) > dev->size) {
        INFO("Beyond-end write (%ld %ld)", offset, nbytes);
        return ;
    }
    if (write) {
        memcpy(dev->data + offset, buffer, nbytes);
    } else {
        memcpy(buffer, dev->data + offset, nbytes);
    }
}


static void sbull_request(struct request_queue *q)
{
    struct request *req = NULL;

    INFO();

    /*
    while ((req = elv_next_request(q)) != NULL) {
        struct sbull_dev *dev = req->rq_disk->private_data;
        
        //sbull_transfer(dev, req->sector, req->current_nr_sectors,
        //                req->buffer, rq_data_dir(req));
        end_request(req, 1);
    }*/
}

static void setup_device(struct sbull_dev *dev, int which)
{
    memset(dev, 0, sizeof(struct sbull_dev));
    dev->size = nsectors * hardsect_size;
    dev->data = vmalloc(dev->size);
    if (dev->data == NULL) {
        INFO("vmalloc failed");
        return ;
    }

    spin_lock_init(&dev->lock);

    dev->queue = blk_init_queue(sbull_request, &dev->lock);
    if (dev->queue == NULL) {
        INFO("blk_init_queue failed");
        goto out_vfree;
    }
    dev->queue->queuedata = dev;

    dev->gd = alloc_disk(SBULL_MINORS);
    if (dev->gd == NULL) {
        INFO("alloc_disk failed");
        goto out_vfree;
    }

    dev->gd->major = sbull_major;
    dev->gd->first_minor = which * SBULL_MINORS;
    dev->gd->fops = &sbull_ops;
    dev->gd->queue = dev->queue;
    dev->gd->private_data = dev;
    snprintf(dev->gd->disk_name, 32, "sbull%c", which + 'a');
    set_capacity(dev->gd, nsectors * (hardsect_size / KERNEL_SECTOR_SIZE));
    add_disk(dev->gd);
    return;

  out_vfree:
    if (dev->data) {
        vfree(dev->data);
        dev->data = NULL;
    }
}


static int sbull_init(void)
{
    int i = 0;
    INFO();
    sbull_major = register_blkdev(0, "sbull");
    if (sbull_major <= 0) {
        INFO("register_blkdev failed\n");
    }

    Devices = (struct sbull_dev *)kmalloc(sizeof(struct sbull_dev) * ndevices, GFP_KERNEL);
    if (Devices == NULL) {
        INFO("kmalloc failed");
        goto out_unregister;
    }
    for (i = 0; i < ndevices; i ++) {
        setup_device(Devices + i, i);
    }

    return 0;

  out_unregister:
    unregister_blkdev(sbull_major, "sbull");
    return 0;
}

static void sbull_exit(void)
{
    int i = 0;
    INFO();
    for (i = 0; i < ndevices; i ++) {
        struct sbull_dev *dev = Devices + i;

        if (dev->gd) {
            del_gendisk(dev->gd);
            put_disk(dev->gd);
            dev->gd = NULL;
        }
        if (dev->queue) {
            blk_cleanup_queue(dev->queue);
            dev->queue = NULL;
        }
        if (dev->data) {
            vfree(dev->data);
            dev->data = NULL;
        }
    }
    if (sbull_major > 0) {
        unregister_blkdev(sbull_major, "sbull");
    }
}

module_init(sbull_init);
module_exit(sbull_exit);
