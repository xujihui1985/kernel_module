#include <linux/module.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("sean");
MODULE_DESCRIPTION("hello world");

static int __init ModuleInit(void) {
    printk("hello world\n");
    return 0;
}

static void __exit ModuleExit(void) {
    printk("good bye hello world\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);

