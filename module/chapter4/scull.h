#ifndef __SCULL_H__
#define __SCULL_H__

#ifndef __USER__
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/semaphore.h>
#include <asm/uaccess.h>
#include <linux/ioctl.h>


struct scull_qset {
    void **data;
    struct scull_qset *next;
    int qset_quantum;
    int qset_qset;
};

struct scull_dev {
    struct scull_qset *data;
    int quantum;
    int qset;
    unsigned long size;
    unsigned int access_key;
    struct semaphore sem;
    struct cdev cdev;
};

extern int scull_major;
extern int scull_nr_devs;
extern int scull_quantum;
extern int scull_qset;

#endif //__USER__


#define SCULL_IOC_MAGIC 'k'

#define SCULL_IOCRESET      _IO(SCULL_IOC_MAGIC, 0)
#define SCULL_IOCSQUANTUM   _IOW(SCULL_IOC_MAGIC, 1, int)
#define SCULL_IOCSQSET      _IOW(SCULL_IOC_MAGIC, 2, int)
#define SCULL_IOCTQUANTUM   _IO(SCULL_IOC_MAGIC, 3)
#define SCULL_IOCTQSET      _IO(SCULL_IOC_MAGIC, 4)
#define SCULL_IOCGQUANTUM   _IOR(SCULL_IOC_MAGIC, 5, int)
#define SCULL_IOCGQSET      _IOR(SCULL_IOC_MAGIC, 6, int)
#define SCULL_IOCQQUANTUM   _IO(SCULL_IOC_MAGIC, 7)
#define SCULL_IOCQQSET      _IO(SCULL_IOC_MAGIC, 8)
#define SCULL_IOCXQUANTUM   _IOWR(SCULL_IOC_MAGIC, 9, int)
#define SCULL_IOCXQSET      _IOWR(SCULL_IOC_MAGIC, 10, int)
#define SCULL_IOCHQUANTUM   _IO(SCULL_IOC_MAGIC, 11)
#define SCULL_IOCHQSET      _IO(SCULL_IOC_MAGIC, 12)

#define SCULL_IOC_MAX       14

#endif //__SCULL_H__
