#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/timer.h>

#define Time_Delay	(500)

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("My Timer Module");
MODULE_AUTHOR("Souvik De");

char *name;
static int period_counter = 0;
static struct timer_list my_timer;

module_param(name, charp , S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(name, "Stores my name");

module_param(period_counter, int , S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(period_counter, "The timer is triggered for given period");

void timer_handler(struct timer_list *timer_data)
{
	static int count = 0;
	int err = 0;
	count++;
	printk(KERN_ALERT "\n%s here, the timer ran %d times",name,count);
	if(count != (period_counter*1000)/Time_Delay)
	{
		printk(KERN_ALERT "\nTimer will be fired in %dms",Time_Delay);
		err = mod_timer(&my_timer, jiffies + msecs_to_jiffies(Time_Delay));
		if(err)
			printk(KERN_ALERT "\nError encountered in timer module!");
	}
}

static int __init mytimermod_init(void)
{
	int err = 0;	
		
	printk(KERN_ALERT "\nMode Inserted Successfully");
	timer_setup(&my_timer, timer_handler, 0);
	printk(KERN_ALERT "\nTimer will be fired in %dms",Time_Delay);
	err = mod_timer(&my_timer, jiffies + msecs_to_jiffies(Time_Delay));

	if(err)
		printk(KERN_ALERT "\nError encountered in timer moduel!");

	return 0;
}

static void __exit mytimermod_exit(void)
{
	int err = 0;
	err = del_timer(&my_timer);
	if(err)
		printk(KERN_ALERT "\nTimer stil in use!");
	printk(KERN_ALERT "\nModule Removed Successfully\n");

}


module_init(mytimermod_init);
module_exit(mytimermod_exit);
