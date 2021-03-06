#ifndef __LITEFS_H__
#define __LITEFS_H__

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/bio.h>
#include <linux/blkdev.h>
#include <linux/fs.h>
#include <linux/buffer_head.h>
#include <linux/mpage.h>

#define LITE_DBUG

#ifdef LITE_DBUG
#define LOG_INFO(fmt, args...)  printk(KERN_INFO "INFO: func %s, line %d: "fmt"\n", __func__, __LINE__, ##args)

#define LOG_ERR(fmt, args...)   printk(KERN_ERR "ERROR: func %s, line %d: "fmt"\n", __func__, __LINE__, ##args)
#else

#define LOG_INFO(fmt, args...)  printk(KERN_INFO "INFO: "fmt"\n", ##args)

#define LOG_ERR(fmt, args...)   printk(KERN_ERR "ERROR: "fmt"\n", ##args)
#endif //LITE_DEBUG


/*
 *
 * super block
 *
 */

#define INODE_BLOCK_RATIO      128  // 1/128 percent
#define INODE_BLOCK_RATIO_BITS 7

#define FIRST_USEABLE_BLOCK     4
#define SUPER_BLOCK             8

#define LITE_BLOCKBITS      10
#define LITE_BLOCKSIZE      (1 << LITE_BLOCKBITS)
#define LITE_MAX_FILE_SIZE  (1 << 30)
#define LITE_MAGIC  12345

struct lite_fs_super_info {
    __u64   s_blocks_count;
    __u64   s_bitmap_size;

    __u64   s_blocksize;
    __u64   s_blocks_per_page;
    __u64   s_inode_blocks;
    __u64   s_inode_count;
    __u64   s_first_inode_block;
    __u64   s_free_inode_count;

    __u64   s_inode_bitmap_blocks;
    __u64   s_first_inode_bitmap_block;

    __u64   s_data_bitmap_blocks;
    __u64   s_first_data_bitmap_block;
    __u64   s_first_data_block;
    __u64   s_data_blocks;
    __u64   s_free_data_block_count;

    __u64   s_magic;
};

struct lite_fs_vfs_super_info {
    struct lite_fs_super_info disk_info;
    spinlock_t inode_bitmap_lock;
    spinlock_t vfs_super_lock;
};

/*
 *
 * namei 
 *
 */

struct lite_fs_dirent;

extern struct inode_operations lite_fs_dir_inode_iops;
extern int lite_fs_match(int namelen, const char *name, struct lite_fs_dirent *de);
extern int lite_fs_commit_chunk(struct page *page, loff_t pos, unsigned len);

/*
 *
 * directory
 *
 */


//disk save file type
enum {
    LITE_FT_UNKNOWN,
    LITE_FT_REG_FILE,
    LITE_FT_DIR,
    LITE_FT_CHRDEV,
    LITE_FT_BKLDEV,
    LITE_FT_FIFO,
    LITE_FT_SOCK,
    LITE_FT_SYMLINK,
    LITE_FT_MAX
};


#define LITE_FS_DIRENT_SIZE 64
#define LITE_FS_NAME_LEN (LITE_FS_DIRENT_SIZE - 8 - 2 - 1 - 1)

struct lite_fs_dirent {
    __u64   inode;
    __u16   rec_len;
    __u8    name_len;
    __u8    file_type;
    char    name[LITE_FS_NAME_LEN];
};


extern struct file_operations lite_fs_dir_fops;

extern struct page *lite_fs_get_page(struct inode *dir, unsigned long index);
extern void lite_fs_put_page(struct page *page);
extern unsigned lite_fs_last_byte(struct inode *inode, unsigned long n);
extern void lite_fs_set_dirent_type(struct lite_fs_dirent *dirent, struct inode *inode);
extern struct lite_fs_dirent *lite_fs_next_dentry(struct lite_fs_dirent *de);
extern ino_t lite_fs_inode_by_name(struct inode *dir, struct qstr *child);
extern int lite_fs_delete_entry(struct lite_fs_dirent *dirent, struct page *page);
extern struct lite_fs_dirent *lite_fs_find_entry(struct inode *dir_inode, struct qstr *child, struct page **res_page);


/*
 *
 * inode
 *
 */

#define LITE_FS_LINK_MAX    1000

#define LITE_FS_ROOT_INO    1


#define LITE_FS_INODE_SIZEBITS  9
#define LITE_FS_INODE_SIZE      (1 << 9) //512 byte for a inode
#define LITE_FS_N_BLOCKS        32
#define LITE_FS_PER_BLOCK_INODE (LITE_BLOCKSIZE/LITE_FS_INODE_SIZE)
#define LITE_FS_INO_OFFSET_IN_BLOCK(ino) ((ino) & (LITE_FS_PER_BLOCK_INODE - 1))

struct lite_fs_inode {
    __u64   i_size;
    __u16   i_mode;
    __u16   i_link_count;
    __u32   i_atime;
    __u32   i_ctime;
    __u32   i_mtime;
    __u32   i_dtime;
    __u32   i_blocks;
    __u32   i_flags;
    __u32   i_block[LITE_FS_N_BLOCKS];
};

struct lite_fs_vfs_inode_info {
    __u32   i_block[LITE_FS_N_BLOCKS];
    struct inode vfs_inode;
};

extern struct inode *lite_fs_iget(struct super_block *sb, unsigned long ino);
extern struct address_space_operations lite_fs_aops;
extern int lite_fs_get_block(struct inode *inode, sector_t iblock, struct buffer_head *bh_result, int create);
extern struct lite_fs_inode *lite_fs_get_raw_inode(struct super_block *sb, unsigned long ino, struct buffer_head **buffer);
extern int __lite_fs_write_begin(struct file *, struct address_space *, loff_t, 
                                unsigned , unsigned , struct page **, void **);
extern int lite_fs_write_inode(struct inode *inode, int do_sync);
extern void lite_fs_delete_inode(struct inode *inode);

/*
 *
 * regular file
 *
 */

extern struct inode_operations lite_fs_file_inode_iops;
extern struct file_operations lite_fs_file_fops;



//extern struct address_space_operations lite_fs_aops;


#include <asm-generic/bitops/le.h>

#define lite_fs_test_bit(nr, addr)  generic_test_le_bit((nr), (unsigned long *)(addr))
#define lite_fs_set_bit(nr, addr)   generic___test_and_set_le_bit((nr), (unsigned long *)(addr))
#define lite_fs_clear_bit(nr, addr) generic___test_and_clear_le_bit((nr), (unsigned long *)(addr))
#define lite_fs_find_first_zero_bit(addr, size) \
                                    generic_find_first_zero_le_bit((unsigned long *)(addr), (size))
#define lite_fs_find_next_zero_bit(addr, size, offset) \
                                    generic_find_next_zero_le_bit((unsigned long *)(addr), (size), (offset))
#define lite_fs_find_next_bit(addr, size, offset) \
                                    generic_find_next_le_bit((unsigned long *)(addr), (size), (offset))

#define lite_fs_set_bit_atomic(lock, nr, addr) \
    ({ \
        int ret;    \
        spin_lock(lock);    \
        ret = lite_fs_set_bit((nr), (unsigned long *)(addr)); \
        spin_unlock(lock);  \
        ret;    \
     })
#define lite_fs_clear_bit_atomic(lock, nr, addr)    \
    ({  \
        int ret;    \
        spin_lock(lock);    \
        ret = lite_fs_clear_bit((nr), (unsigned long *)(addr)); \
        spin_unlock(lock);  \
        ret;    \
     })

        

#endif //__LITEFS_H__
