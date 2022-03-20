#include <linux/module.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("sean");
MODULE_DESCRIPTION("kthread demo");

/* Global variables for the threads */
static struct task_struct *kthread_1;
static struct task_struct *kthread_2;
static int t1 = 1, t2 = 2;

int thread_function(void *thread_nr) {
    unsigned int i = 0;
    int t_nr = *(int *) thread_nr;
    while(!kthread_should_stop()) {
        printk("kthread %d is excuted val: %d\n", t_nr, i++);
        msleep(t_nr * 1000);
    }
    printk("kthread thread %d finished \n", t_nr);
    return 0;
}

static int __init ModuleInit(void) {
    printk("kthread init threads\n");
    // create thread
    kthread_1 = kthread_create(thread_function, &t1, "kthread_1");
    if(kthread_1 != NULL) {
        // start the thread
        wake_up_process(kthread_1);
        printk("kthread 1 was created and running\n");
    } else {
        printk("kthread 1 could not be created\n");
        return -1;
    }

    kthread_2 = kthread_run(thread_function, &t2, "kthread_2");
    if (kthread_2 != NULL) {
        printk("kthread 2 is running now\n");
    } else {
        printk("kthread 2 could not be created\n");
        kthread_stop(kthread_1);
        return -1;
    }
    printk("kthread both threads are running now");
    return 0;
}

static void __exit ModuleExit(void) {
    printk("kthread stop both\n");
    kthread_stop(kthread_1);
    kthread_stop(kthread_2);
}

module_init(ModuleInit);
module_exit(ModuleExit);

