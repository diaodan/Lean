#include "litefs.h"
struct inode_operations lite_fs_file_inode_iops = {

};

struct file_operations lite_fs_file_fops = {
    .read   = do_sync_read,
    .write  = do_sync_write,
    .open   = generic_file_open,
    .llseek = generic_file_llseek,
};


