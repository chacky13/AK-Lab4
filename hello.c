#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/ktime.h>
#include <linux/types.h>

MODULE_AUTHOR("Andrii Chaika <andrewchaika13@gmail.com>");
MODULE_DESCRIPTION("Chaika lab4 AK");
MODULE_LICENSE("Dual BSD/GPL");

static uint print_count = 1;
module_param(print_count, uint, 0644);
MODULE_PARM_DESC(print_count, "Number of times to print 'Hello, world!'");

struct event_item {
    struct list_head list;
    ktime_t event_time;
};

static LIST_HEAD(event_list);

static int __init hello_init(void)
{
    struct event_item *item;
    unsigned int i;

    if (print_count == 0 || (print_count >= 5 && print_count <= 10)) {
        printk(KERN_WARNING "Warning: print_count is %u. Continuing anyway.\n", print_count);
    }

    if (print_count > 10) {
        printk(KERN_ERR "Error: print_count is too large (%u). Aborting.\n", print_count);
        return -EINVAL;
    }

    for (i = 0; i < print_count; i++) {
        item = kmalloc(sizeof(*item), GFP_KERNEL);
        if (!item) {
            printk(KERN_ERR "Memory allocation failed.\n");
            return -ENOMEM;
        }

        item->event_time = ktime_get();
        list_add_tail(&item->list, &event_list);

        printk(KERN_EMERG "Hello, world!\n");
    }

    return 0;
}

static void __exit hello_exit(void)
{
    struct event_item *item, *tmp;

    list_for_each_entry_safe(item, tmp, &event_list, list) {
        printk(KERN_INFO "Event time: %llu ns\n", ktime_to_ns(item->event_time));
        list_del(&item->list);
        kfree(item);
    }
}

module_init(hello_init);
module_exit(hello_exit);