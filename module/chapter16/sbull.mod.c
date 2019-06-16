#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xf0f44a4f, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x194e1217, __VMLINUX_SYMBOL_STR(blk_cleanup_queue) },
	{ 0x254be6cf, __VMLINUX_SYMBOL_STR(put_disk) },
	{ 0x1e372777, __VMLINUX_SYMBOL_STR(del_gendisk) },
	{ 0xb5a459dc, __VMLINUX_SYMBOL_STR(unregister_blkdev) },
	{ 0xef437f77, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0x451a419d, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0x71a50dbc, __VMLINUX_SYMBOL_STR(register_blkdev) },
	{ 0x81f73e63, __VMLINUX_SYMBOL_STR(bio_endio) },
	{ 0x4c4fef19, __VMLINUX_SYMBOL_STR(kernel_stack) },
	{ 0x77c56cb9, __VMLINUX_SYMBOL_STR(blk_fetch_request) },
	{ 0x5134fed, __VMLINUX_SYMBOL_STR(blk_end_request) },
	{ 0x71de9b3f, __VMLINUX_SYMBOL_STR(_copy_to_user) },
	{ 0x999e8297, __VMLINUX_SYMBOL_STR(vfree) },
	{ 0x54cb7c91, __VMLINUX_SYMBOL_STR(add_disk) },
	{ 0x28318305, __VMLINUX_SYMBOL_STR(snprintf) },
	{ 0x7090b635, __VMLINUX_SYMBOL_STR(alloc_disk) },
	{ 0x2d3908af, __VMLINUX_SYMBOL_STR(blk_init_queue) },
	{ 0xd6ee688f, __VMLINUX_SYMBOL_STR(vmalloc) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "D56320983C62C7641DD1EE2");
