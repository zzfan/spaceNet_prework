#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
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
	{ 0xa139692d, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x555ab6b, __VMLINUX_SYMBOL_STR(netlink_kernel_release) },
	{ 0x9548dfb8, __VMLINUX_SYMBOL_STR(__netlink_kernel_create) },
	{ 0xd9a919f3, __VMLINUX_SYMBOL_STR(init_net) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x827b9cdf, __VMLINUX_SYMBOL_STR(netlink_unicast) },
	{ 0x9166fada, __VMLINUX_SYMBOL_STR(strncpy) },
	{ 0x42fd34e, __VMLINUX_SYMBOL_STR(__nlmsg_put) },
	{ 0x7e0c1ec4, __VMLINUX_SYMBOL_STR(__alloc_skb) },
	{ 0x754d539c, __VMLINUX_SYMBOL_STR(strlen) },
	{ 0xaebba10c, __VMLINUX_SYMBOL_STR(mutex_unlock) },
	{ 0xbd0ec062, __VMLINUX_SYMBOL_STR(netlink_rcv_skb) },
	{ 0xfc3492c1, __VMLINUX_SYMBOL_STR(mutex_lock) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "8808EC0F83433DA755DD405");
