#ifndef __LITEFS_H__
#define __LITEFS_H__

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/bio.h>
#include <linux/blkdev.h>
#include <linux/fs.h>
#include <linux/buffer_head.h>
#include <linux/mpage.h>

#define LOG_INFO(fmt, args...)  printk(KERN_INFO "INFO: file %s, func %s, line %d: "fmt"\n", __FILE__,__func__, __LINE__, ##args)

#define LOG_ERR(fmt, args...)   printk(KERN_ERR "ERROR: file %s, func %s, line %d: "fmt"\n", __FILE__,__func__, __LINE__, ##args)

#define INODE_BITMAP_BLOCK  10
#define DATA_BITMAP_BLOCK   20
#define FIRST_INODE_BLOCK   40
#define FIRST_DATA_BLOCK    50

#define LITE_BLOCKBITS      10
#define LITE_BLOCKSIZE      (1 << LITE_BLOCKBITS)
#define LITE_MAX_FILE_SIZE  (1 << 30)
#define LITE_MAGIC  12345


struct lite_fs_super_info {
    __u64   s_blocks_count;
    __u64   s_inodes_count;
    __u64   s_free_blocks_count;
    __u64   s_free_inodes_count;
    __u64   s_first_inode_bitmap;
    __u64   s_first_data_bitmap;
    __u64   s_first_inode_block;
    __u64   s_first_data_block;
    __u64   s_magic;
};

#define LITE_FS_DIRENT_SIZE 64
#define LITE_FS_NAME_SIZE   (LITE_FS_DIRENT_SIZE - 8 - 2 - 1 - 1)

struct lite_fs_dirent {
    __u64   inode;
    __u16   rec_len;
    __u8    name_len;
    __u8    file_type;
    char    name[LITE_FS_NAME_SIZE];
};

#endif //__LITEFS_H__
