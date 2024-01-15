#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/ktime.h>
#include <linux/slab.h>
#include <linux/list.h>

struct timing_info {
    struct list_head node;
    ktime_t begin_time;
    ktime_t finish_time;
};

MODULE_AUTHOR("IO-16 Oleh Buhaienko");
MODULE_DESCRIPTION("Laboratorna robota 3");
MODULE_LICENSE("GPL");

static LIST_HEAD(timing_list);

static uint total = 1;
module_param(total, uint, 0444);
MODULE_PARM_DESC(total, "Total count");

static int __init this_module_init(void);
static void __exit this_module_exit(void);

static int __init this_module_init(void) {
    uint counter = 0;
    struct timing_info *new_record;

    pr_info("Total: %d\n", total);

    BUG_ON(total > 5);
    
    for (counter = 0; counter < total; counter++) {
        new_record = kmalloc(sizeof(*new_record), GFP_KERNEL);

        if (!new_record) {
            pr_err("Failed to allocate memory!\n");
            this_module_exit(); 
            return -ENOMEM;
        }

        new_record->begin_time = ktime_get();
        pr_info("Entry number\n");
        new_record->finish_time = ktime_get();

        list_add_tail(&new_record->node, &timing_list);
    }

    return 0;
}

static void __exit this_module_exit(void) {
    struct timing_info *current_record, *next_record;

    list_for_each_entry_safe(current_record, next_record, &timing_list, node) {
        pr_info("Time for execution: %lld ns",
        ktime_to_ns(current_record->finish_time - current_record->begin_time));

        list_del(&current_record->node);
        kfree(current_record);
    }

    if (!list_empty(&timing_list)) {
        pr_alert("Timing list is not empty on exit\n");
    }
}

module_init(this_module_init);
module_exit(this_module_exit);

