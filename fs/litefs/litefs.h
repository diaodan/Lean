#ifndef __LITEFS_H__
#define __LITEFS_H__

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/bio.h>
#include <linux/blkdev.h>
#include <linux/fs.h>
#include <linux/buffer_head.h>

#define LOG_INFO(fmt, args...)  printk(KERN_INFO "INFO: file %s, func %s, line %d: "fmt"\n", __FILE__,__func__, __LINE__, ##args)

#define LOG_ERR(fmt, args...)   printk(KERN_ERR "ERROR: file %s, func %s, line %d: "fmt"\n", __FILE__,__func__, __LINE__, ##args)

struct lite_fs_super_info {
    __u64   s_blocks_count;
    __u64   s_inodes_count;
    __u64   s_free_blocks_count;
    __u64   s_free_inodes_count;
    __u64   s_first_inode_bitmap;
    __u64   s_first_data_bitmap;
    __u64   s_first_inode_block;
    __u64   s_first_data_block;
};

#endif //__LITEFS_H__
