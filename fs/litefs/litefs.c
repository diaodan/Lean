#include "litefs.h"

MODULE_LICENSE("Dual BSD/GPL");


static struct inode *lite_fs_alloc_inode(struct super_block *sb)
{
    struct lite_fs_inode_info *inode = NULL;
    LOG_INFO();

    inode = (struct lite_fs_inode_info *)kmalloc(sizeof(*inode), GFP_KERNEL);
    if (inode == NULL) {
        return ERR_PTR(-ENOMEM);
    }

    inode_init_once(&inode->vfs_inode);

    return &inode->vfs_inode;
}

static void lite_fs_destroy_inode(struct inode *inode)
{
    LOG_INFO();
    kfree(container_of(inode, struct lite_fs_inode_info, vfs_inode));
}

static struct super_operations lite_fs_sops = {
    .alloc_inode    = lite_fs_alloc_inode,
    .destroy_inode  = lite_fs_destroy_inode,
//    .write_inode    = lite_fs_write_inode,
};

int lite_fs_setup_root_dir(struct super_block *sb, struct lite_fs_super_info *lsb)
{
    struct buffer_head *bh = NULL;
    struct buffer_head *dirbh = NULL;
    struct lite_fs_inode *lite_inode = NULL;
    char *kaddr = NULL;
    unsigned int ret = 0;
    unsigned int ino_block;
    struct lite_fs_dirent *dirent = NULL;
    unsigned int root_ino = LITE_FS_ROOT_INO;
    int ino_offset_in_block;
    LOG_INFO();

    bh = sb_bread(sb, lsb->s_first_inode_bitmap_block);
    if (bh == NULL) {
        LOG_ERR("LITE fs :get first bitmap failed");
        return -EIO;
    }

    kaddr = kmap(bh->b_page);
    ret = lite_fs_set_bit(LITE_FS_ROOT_INO, (kaddr + bh_offset(bh)));
    kunmap(bh->b_page);
    //if (ret) {
    //    LOG_INFO("LITE fs root has been setup");
    //    brelse(bh);
    //    return 0;
    //}
    LOG_INFO("LITE fs: root setup");

    get_bh(bh);
    lock_buffer(bh);
    bh->b_end_io = end_buffer_write_sync;
    submit_bh(WRITE, bh);
    wait_on_buffer(bh);
    brelse(bh);


    ino_block = root_ino >> (LITE_FS_PER_BLOCK_INODE);
    bh = sb_bread(sb, lsb->s_first_inode_block + ino_block);
    if (bh == NULL) {
        LOG_ERR("LITE fs :get inode block %llu failed", lsb->s_first_inode_block + ino_block);
        return -EIO;
    }
    ino_offset_in_block = (root_ino & (LITE_FS_PER_BLOCK_INODE - 1)) << LITE_FS_INODE_SIZEBITS;

    kaddr = kmap(bh->b_page);
    lite_inode = (struct lite_fs_inode *)(((char *)kaddr + bh_offset(bh)) 
                    + ((root_ino & (LITE_FS_PER_BLOCK_INODE -1)) << LITE_FS_INODE_SIZEBITS));
    memset((char *)lite_inode, 0, LITE_FS_INODE_SIZE);
    lite_inode->i_mode = S_IFDIR;
    lite_inode->i_link_count = 1;
    lite_inode->i_block[0] = lsb->s_first_data_block;
    lite_inode->i_size = LITE_FS_DIRENT_SIZE << 1;// 2 entry (. and ..)
    lite_inode->i_blocks = 1;
    kunmap(bh->b_page);

    get_bh(bh);
    lock_buffer(bh);
    bh->b_end_io = end_buffer_write_sync;
    submit_bh(WRITE, bh);
    wait_on_buffer(bh);
    brelse(bh);

    bh = sb_bread(sb, lsb->s_first_data_bitmap_block);
    if (bh == NULL) {
        LOG_ERR("LITE fs :get data bitmap block %llu failed", lsb->s_first_data_bitmap_block);
        return -EIO;
    }

    kaddr = kmap(bh->b_page);
    lite_fs_set_bit(0, (kaddr + bh_offset(bh)));
    kunmap(bh->b_page);

    get_bh(bh);
    lock_buffer(bh);
    bh->b_end_io = end_buffer_write_sync;
    submit_bh(WRITE, bh);
    wait_on_buffer(bh);
    brelse(bh);

    dirbh = sb_bread(sb, lsb->s_first_data_block);
    if (dirbh == NULL) {
        LOG_ERR("LITE fs :get data block %llu failed", lsb->s_first_data_block);
        return -EIO;
    }

    kaddr = kmap(dirbh->b_page);
    dirent = (struct lite_fs_dirent *)((char *)kaddr + bh_offset(dirbh));
    dirent->inode = LITE_FS_ROOT_INO;
    dirent->file_type = LITE_FT_DIR;
    memset(dirent->name, 0, LITE_FS_NAME_LEN);
    memcpy(dirent->name, ".", strlen("."));
    dirent->name_len = strlen(".");
    dirent->rec_len = LITE_FS_DIRENT_SIZE;

    dirent ++;
    dirent->inode = LITE_FS_ROOT_INO;
    dirent->file_type = LITE_FT_DIR;

    memset(dirent->name, 0, LITE_FS_NAME_LEN);
    memcpy(dirent->name, "..", strlen(".."));
    dirent->name_len = strlen("..");
    dirent->rec_len = LITE_FS_DIRENT_SIZE;

    kunmap(dirbh->b_page);
    
    get_bh(dirbh);
    lock_buffer(dirbh);
    dirbh->b_end_io = end_buffer_write_sync;
    submit_bh(WRITE, dirbh);
    wait_on_buffer(dirbh);
    brelse(dirbh);

    return 0;
}

static int lite_fs_fill_super(struct super_block *sb, void *data, int silent)
{
    struct buffer_head *bh = NULL;
    struct lite_fs_super_info *lsb = NULL;
    struct lite_fs_super_info *sbi = NULL;
    unsigned int blocksize;
    struct inode *root = NULL;
    int ret = 0;

    LOG_INFO();

    sbi = kzalloc(sizeof(*sbi), GFP_KERNEL);
    if (!sbi) {
        return  -ENOMEM;
    }

    sb->s_fs_info = sbi;

    blocksize = sb_set_blocksize(sb, 1024);
    LOG_INFO();
    lsb->s_blocksize = blocksize;
    lsb->s_blocks_per_page = PAGE_CACHE_SIZE / lsb->s_blocksize;

    if (!(bh = sb_bread(sb, SUPER_BLOCKS))) {
        LOG_ERR("unable to read superblock");
        goto failed_lsb;
    }
    LOG_INFO();

    lsb = (struct lite_fs_super_info *)((char *)bh->b_data);

    //blocksize = 1k
    lsb->s_blocks_count = sb->s_bdev->bd_part->nr_sects >>(blocksize >> 9);

    /*
     * inode blocks is 1/128 for all blocks
     */
    lsb->s_inode_blocks = lsb->s_blocks_count >> INODE_BLOCK_RATIO_BITS;
    lsb->s_inode_count  = lsb->s_inode_blocks << (lsb->s_blocksize >> LITE_FS_INODE_SIZEBITS);
    lsb->s_first_inode_block = FIRST_USEABLE_BLOCK + SUPER_BLOCKS;
    lsb->s_inode_bitmap_blocks = (lsb->s_inode_count + (lsb->s_blocksize << 4) - 1) / (lsb->s_blocksize << 4); 

    lsb->s_first_inode_bitmap_block = (lsb->s_first_inode_block + lsb->s_inode_blocks
                                + lsb->s_blocks_per_page - 1) & ~(lsb->s_blocks_per_page - 1);
    lsb->s_data_blocks = lsb->s_blocks_count - (lsb->s_first_inode_bitmap_block + lsb->s_inode_bitmap_blocks);
    lsb->s_data_bitmap_blocks = (lsb->s_data_blocks + (lsb->s_blocksize << 4) -1 ) / (lsb->s_blocksize << 4);
    lsb->s_first_data_bitmap_block = (lsb->s_first_inode_bitmap_block + lsb->s_inode_bitmap_blocks
                                + lsb->s_blocks_per_page -1) & ~(lsb->s_blocks_per_page - 1);
    lsb->s_first_data_block = (lsb->s_first_data_bitmap_block + lsb->s_data_bitmap_blocks
                                + lsb->s_blocks_per_page -1) & ~(lsb->s_blocks_per_page - 1);
    lsb->s_data_blocks = lsb->s_blocks_count - lsb->s_first_data_block;
    lsb->s_magic = LITE_MAGIC;

    LOG_INFO("super block  %lu", bh->b_blocknr);

    *sbi = *lsb;

    get_bh(bh);
    lock_buffer(bh);
    bh->b_end_io = end_buffer_write_sync;
    submit_bh(WRITE, bh);
    wait_on_buffer(bh);

    LOG_INFO();


    LOG_INFO("lite filesystem superblock information");
    LOG_INFO("block count:              %llu", sbi->s_blocks_count);
    LOG_INFO("blocksize:                %llu", sbi->s_blocksize);
    LOG_INFO("block per page:           %llu", sbi->s_blocks_per_page);
    LOG_INFO("inode blocks:             %llu", sbi->s_inode_blocks);
    LOG_INFO("inode count:              %llu", sbi->s_inode_count);
    LOG_INFO("first inode block:        %llu", sbi->s_first_inode_block);
    LOG_INFO("inode bitmap blocks:      %llu", sbi->s_inode_bitmap_blocks);
    LOG_INFO("first inode bitmap block: %llu", sbi->s_first_inode_bitmap_block);
    LOG_INFO("data bitmap blocks:       %llu", sbi->s_data_bitmap_blocks);
    LOG_INFO("first data bitmap block:  %llu", sbi->s_first_data_bitmap_block);
    LOG_INFO("first data block:         %llu", sbi->s_first_data_block);
    LOG_INFO("data blocks:              %llu", sbi->s_data_blocks);

    if ((ret = lite_fs_setup_root_dir(sb, lsb))) {
        LOG_ERR();
        return ret;
    }

    sb->s_blocksize = LITE_BLOCKSIZE;
    sb->s_maxbytes = LITE_MAX_FILE_SIZE;
    sb->s_magic = LITE_MAGIC;
    sb->s_blocksize_bits = LITE_BLOCKBITS;

    sb->s_op = &lite_fs_sops;

    root = lite_fs_iget(sb, LITE_FS_ROOT_INO);
    if (IS_ERR(root)) {
        ret = PTR_ERR(root);
        goto failed_lsb;
    }

    sb->s_root = d_alloc_root(root);
    if (!sb->s_root) {
        iput(root);
        LOG_ERR("LITE FS: get root inode failed\n");
        goto failed_lsb;
    }
    brelse(bh);
    return 0;

failed_lsb:
    LOG_ERR("LITE fs: failed fill super");
    if (bh) {
        brelse(bh);
        bh = NULL;
    }
    return ret;
}



static int lite_fs_get_sb(struct file_system_type *fs_type, int flags,
        const char *dev_name, void *data, struct vfsmount *mnt)
{
    LOG_INFO();

    return get_sb_bdev(fs_type, flags, dev_name, data, lite_fs_fill_super, mnt);
}

static void lite_fs_kill_block_super(struct super_block *sb)
{
    LOG_INFO();
    kill_block_super(sb);
}

static struct file_system_type lite_fs_type = {
    .owner      = THIS_MODULE,
    .name       = "lite_fs",
    .get_sb     = lite_fs_get_sb,
    .kill_sb    = lite_fs_kill_block_super,
};


static int init_lite_fs(void)
{
    int err;

    printk(KERN_ALERT "init\n");
    LOG_INFO();

    err = register_filesystem(&lite_fs_type);
    if (err) {
        LOG_ERR("register filesystem failed\n");
    }
    return err;
}


static void exit_lite_fs(void)
{
    printk(KERN_ALERT "exit\n");
    LOG_INFO();
    unregister_filesystem(&lite_fs_type);
}

module_init(init_lite_fs);
module_exit(exit_lite_fs);
