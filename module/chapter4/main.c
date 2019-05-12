#include "scull.h"

int scull_major = 0;
int scull_nr_devs = 4;
int scull_minor = 0;
int scull_quantum = 128;
int scull_qset = 16;

MODULE_AUTHOR("LILU");
MODULE_LICENSE("Dual BSD/GPL");

struct scull_dev *scull_devices;

static void scull_trim(struct scull_dev *dev)
{
    struct scull_qset *next = NULL, *dptr = NULL;
    int i, qset;
    qset = dev->qset;
    printk(KERN_INFO "%s\n", __func__);

    dptr = dev->data;

    for ( ; dptr; dptr = next) {
        if (dptr->data) {
            for (i = 0; i < qset; i++) {
                kfree(dptr->data[i]);
            }
            kfree(dptr->data);
            dptr->data = NULL;
        }
        next = dptr->next;
        kfree(dptr);
    }

    dev->size = 0;
    dev->quantum = scull_quantum;
    dev->qset = scull_qset;
    dev->data = NULL;

    return ;
}

static struct scull_qset * scull_follow(struct scull_dev *dev, int n)
{
    struct scull_qset *qs = dev->data;
    printk(KERN_INFO "%s\n", __func__);
    if (qs == NULL) {
        qs = dev->data = kmalloc(sizeof(struct scull_qset), GFP_KERNEL);
        if (qs == NULL) {
            return NULL;
        }
        memset(qs, 0, sizeof(struct scull_qset));

        if (qs->data == NULL) {
            qs->data = kmalloc(sizeof(char *) * dev->qset, GFP_KERNEL);
            if (qs->data == NULL) {
                kfree(qs->data);
                dev->data = NULL;
                return NULL;
            }
            memset(qs->data, 0, sizeof(char *) * dev->qset);
        }
    }
    while (n--) {
        if (!qs->next) {
            qs->next = kmalloc(sizeof(struct scull_qset), GFP_KERNEL);
            if (qs->next == NULL) {
                return NULL;
            }
            memset(qs->next, 0, sizeof(struct scull_qset));
            if (qs->next->data == NULL) {
                qs->next->data = kmalloc(sizeof(char *) * dev->qset, GFP_KERNEL);
                if (qs->next->data == NULL) {
                    kfree(qs->next);
                    qs->next = NULL;
                    return NULL;
                }
                memset(qs->next->data, 0, sizeof(char *) * dev->qset);
            }

        }
        qs = qs->next;
    }

    return qs;
}
        

int scull_open(struct inode *inode, struct file *filp)
{
    struct scull_dev *dev = NULL;

    printk(KERN_INFO "%s\n", __func__);

    dev= container_of(inode->i_cdev, struct scull_dev, cdev);
    filp->private_data = dev;
    if ((filp->f_flags & O_ACCMODE) == O_WRONLY) {
        scull_trim(dev);
    }

    return 0;
}

loff_t scull_llseek(struct file *filp, loff_t off, int whence)
{
    printk(KERN_INFO "%s\n", __func__);
    return 0;
}

ssize_t scull_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos)
{
    struct scull_dev *dev = filp->private_data;
    struct scull_qset *dptr;
    int itemsize = dev->quantum * dev->qset;
    int item, s_pos, q_pos, rest;
    ssize_t retval = 0;

    printk(KERN_INFO "%s ", __func__);
    printk(KERN_INFO "read %lu\n", count);

    if (down_interruptible(&dev->sem)) {
        return -ERESTARTSYS;
    }
    if (*f_pos >= dev->size) {
        goto out;
    }
    if (*f_pos + count > dev->size) {
        count = dev->size - *f_pos;
    }
    item = (long)*f_pos / itemsize;
    rest = (long)*f_pos % itemsize;
    s_pos = rest / dev->quantum;
    q_pos = rest % dev->quantum;

    dptr = scull_follow(dev, item);
    if (dptr == NULL || dptr->data == NULL || dptr->data[s_pos] == NULL) {
        goto out;
    }

    if (count > dev->quantum - q_pos) {
        count = dev->quantum - q_pos;
    }

    if (copy_to_user(buff, dptr->data[s_pos] + q_pos, count)) {
        retval = -EFAULT;
        goto out;
    }
    *f_pos += count;
    retval = count;
    printk(KERN_INFO "read %lu\n", count);
out:
    up(&dev->sem);
    return retval;
}

ssize_t scull_write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos)
{
    struct scull_dev *dev = filp->private_data;
    struct scull_qset *dptr;
    int itemsize = dev->quantum * dev->qset;
    int item, s_pos, q_pos, rest;
    int retval = -ENOMEM;

    printk(KERN_INFO "%s\n", __func__);
    printk(KERN_INFO "write %lu\n", count);

    if (down_interruptible(&dev->sem)) {
        return -ERESTARTSYS;
    }

    item = (long)*f_pos / itemsize;
    rest = (long)*f_pos % itemsize;
    s_pos = rest / dev->quantum;
    q_pos = rest % dev->quantum;

    dptr = scull_follow(dev, item);
    if (dptr == NULL) {
        goto out;
    }
    if (dptr->data == NULL) {
        dptr->data = kmalloc(sizeof(char *) * dev->qset, GFP_KERNEL);
        if (dptr->data == NULL) {
            goto out;
        }
        memset(dptr->data, 0, sizeof(char *) * dev->qset);
    }

    if (dptr->data[s_pos] == NULL) {
        dptr->data[s_pos] = kmalloc(dev->quantum, GFP_KERNEL);
        if (dptr->data[s_pos] == NULL) {
            goto out;
        }
        memset(dptr->data[s_pos], 0, dev->quantum);
    }

    if (count > dev->quantum - q_pos) {
        count = dev->quantum - q_pos;
    }
    
    if (copy_from_user(dptr->data[s_pos] + q_pos, buff, count)) {
        retval = -EFAULT;
        goto out;
    }

    *f_pos += count;
    retval = count;

    if (dev->size < *f_pos) {
        dev->size = *f_pos;
    }
    printk(KERN_INFO "write %lu dev size %d\n", count, dev->size);
out:
    up(&dev->sem);
    return retval;
}

int scull_release(struct inode *inode, struct file *filp)
{
    printk(KERN_INFO "%s\n", __func__);
    return 0;
}

struct file_operations scull_fops = {
    .owner = THIS_MODULE,
    .llseek = scull_llseek,
    .read   = scull_read,
    .write  = scull_write,
    .open   = scull_open,
    .release = scull_release,
};


static void scull_clean_up(void)
{
    int i;
    dev_t devno = MKDEV(scull_major, scull_minor);

    if (scull_devices) {
        for (i = 0; i < scull_nr_devs; i++) {
            scull_trim(&scull_devices[i]);
            cdev_del(&scull_devices[i].cdev);
        }
        kfree(scull_devices);
    }

    unregister_chrdev_region(devno, scull_nr_devs);

    return ;
}

static void scull_setup_cdev(struct scull_dev *dev, int index)
{
    int err;
    dev_t devno;
    
    devno = MKDEV(scull_major, scull_minor + index);

    cdev_init(&dev->cdev, &scull_fops);
    dev->cdev.owner = THIS_MODULE;
    dev->cdev.ops = &scull_fops;
    err = cdev_add(&dev->cdev, devno, 1);
    if (err) {
        printk(KERN_WARNING "Error %d adding scull %d\n", err, index);
    }
}


int scull_init_module(void)
{
    int result, i;
    dev_t dev = 0;

    result = alloc_chrdev_region(&dev, scull_minor, scull_nr_devs, "scull");
    if (result < 0) {
        printk(KERN_WARNING "scull: can't get magor %d\n", scull_major);
        return result;
    }
    scull_major = MAJOR(dev);

    scull_devices = (struct scull_dev*)kmalloc(scull_nr_devs * sizeof(struct scull_dev), GFP_KERNEL);
    if (scull_devices == NULL) {
        result = -ENOMEM;
        goto fail;
    }
    memset(scull_devices, 0, sizeof(struct scull_dev) * scull_nr_devs);

    for (i = 0; i < scull_nr_devs; i++) {
        scull_devices[i].quantum = scull_quantum;
        scull_devices[i].qset = scull_qset;
        scull_devices[i].size = 0;
        scull_devices[i].data = NULL;
        sema_init(&scull_devices[i].sem, 1);
        scull_setup_cdev(&scull_devices[i], i);
    }

    return 0;

fail:
    scull_clean_up();
    return result;
}

module_init(scull_init_module);
module_exit(scull_clean_up);
