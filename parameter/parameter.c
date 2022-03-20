#include <linux/module.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("sean");
MODULE_DESCRIPTION("parameter");

static int param_a = 1;

module_param(param_a, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(param_a, "int param");

static int __init ModuleInit(void) {
    printk("hello world param is %d\n", param_a);
    return 0;
}

static void __exit ModuleExit(void) {
    printk("good bye param\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);

