#include "edu.h"


MODULE_AUTHOR("lilu");
MODULE_LICENSE("Dual BSD/GPL");


int edu_major;
struct edu_dev *e_dev;



int edu_open(struct inode *inode, struct file *filp)
{
    struct edu_dev *dev = NULL;

    dev = container_of(inode->i_cdev, struct edu_dev, cdev);
    filp->private_data = dev;
    return 0;
}

ssize_t edu_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos)
{
    return 0;
}

ssize_t edu_write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos)
{
    return count;
}


int edu_release(struct inode *inode, struct file *filp)
{
    return 0;
}

long edu_compat_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    return 0;
}

long edu_unlocked_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    int err = 0;
    int retval = 0;
    struct edu_dev *dev = (struct edu_dev *)filp->private_data;

    if (_IOC_TYPE(cmd) != EDU_IOC_MAGIC) {
        return -ENOTTY;
    }
    if (_IOC_NR(cmd) > EDU_IOC_MAX) {
        return -ENOTTY;
    }

    if (_IOC_DIR(cmd) & _IOC_READ) {
        err = !access_ok((void __user *)arg, _IOC_SIZE(cmd));
    } else if (_IOC_DIR(cmd) & _IOC_WRITE) {
        err = !access_ok((void __user *)arg, _IOC_SIZE(cmd));
    }

    if (err) {
        return -EFAULT;
    }

    switch (cmd) {
        case EDU_IOCECHO:
            printk(KERN_INFO "edu: EDU_IOCECHO\n");
            break;
        default:
            return -ENOTTY;
    }
    return retval;
}

struct file_operations edu_fops = {
    .owner  = THIS_MODULE,
    .read   = edu_read,
    .write  = edu_write,
    .open   = edu_open,
    .release= edu_release,
    .unlocked_ioctl = edu_unlocked_ioctl,
    .compat_ioctl   = edu_compat_ioctl,
};



int edu_init_module(void)
{
    int result, err;
    dev_t dev = 0, devno;

    result = alloc_chrdev_region(&dev, 0, 1, "edu");
    if (result < 0) {
        printk(KERN_ERR "edu: cann't get major\n");
        return result;
    }

    edu_major = MAJOR(dev);

    e_dev = (struct edu_dev *)kmalloc(sizeof(struct edu_dev), GFP_KERNEL);
    if (e_dev == NULL) {
        result = -ENOMEM;
        goto fail;
    }
    memset(e_dev, 0, sizeof(struct edu_dev));

    devno = MKDEV(edu_major, 0);
    cdev_init(&e_dev->cdev, &edu_fops);
    e_dev->cdev.owner = THIS_MODULE;
    e_dev->cdev.ops = &edu_fops;
    err = cdev_add(&e_dev->cdev, devno, 1);
    if (err) {
        printk(KERN_ERR "edu: Error add edu chardev\n");
        goto fail;
    }
    
    return 0;
fail:
    if (e_dev) {
        kfree(e_dev);
        e_dev = NULL;
    }
    return err;
}

static void edu_exit(void)
{
    dev_t devno = MKDEV(edu_major, 0);

    if (e_dev) {
        cdev_del(&e_dev->cdev);
        kfree(e_dev);
        e_dev = NULL;
    }

    unregister_chrdev_region(devno, 1);
    return ;
}

module_init(edu_init_module);
module_exit(edu_exit);
