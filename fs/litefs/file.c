#include "litefs.h"
struct inode_operations lite_fs_file_inode_iops = {

};

struct file_operations lite_fs_file_fops = {
    .llseek     = generic_file_llseek,
    .read       = do_sync_read,
    .write      = do_sync_write,
    .open       = generic_file_open,
    .aio_read   = generic_file_aio_read,
    .aio_write  = generic_file_aio_write,

};


