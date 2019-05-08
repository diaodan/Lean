#ifndef __SCULL_H__
#define __SCULL_H__

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


struct scull_qset {
    void **data;
    struct scull_qset *next;
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





#endif //__SCULL_H__
