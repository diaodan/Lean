#ifndef __EDU_H__
#define __EDU_H__

#ifndef __USER__
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>


struct edu_dev {
    struct cdev cdev;
};

extern int edu_major;

#endif //__USER__

#define EDU_IOC_MAGIC 'k'

#define EDU_IOCECHO     _IO(EDU_IOC_MAGIC, 0)

#define EDU_IOC_MAX 10

#endif //__EDU_H__
