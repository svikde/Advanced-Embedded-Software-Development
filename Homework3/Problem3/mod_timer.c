/******************************************************************************************
*	@file		mod_timer.c
*	@brief		This program creates a kernel module that logs the name
*			specified as a parameter to the kernel evey 500ms. The
*			period for which it will print the name is also passed
*			as a parameter to the kernel
*	@Tools_Used	GCC  
*	@Author		Souvik De
*	@Date		02/10/2019
*
*******************************************************************************************/

#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/timer.h>

#define Time_Delay	(500)		//Program prints data every 500ms

MODULE_LICENSE("GPL");			//License
MODULE_DESCRIPTION("My Timer Module");	//Module Description
MODULE_AUTHOR("Souvik De");		//Author

char *name;				//Accepts name to be printed from the user
static int period_counter = 0;		//Accepts the period for which the timer will run
static struct timer_list my_timer;	//Timer object

/* Module parameters and descriptions */
module_param(name, charp , S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(name, "Stores my name");

module_param(period_counter, int , S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(period_counter, "The timer is triggered for given period");

/* This is a callback function for the timer, it is called everytime the timer counts down to timeout */
void timer_handler(struct timer_list *timer_data)
{
	static int count = 0;
	int err = 0;

	count++;	//Track no of times the timer ran
	printk(KERN_ALERT "\n%s here, the timer ran %d times",name,count);	//Display name and times the timer ran
	if(count != (period_counter*1000)/Time_Delay)	//Continue loading the timer until the specified period or length of time
	{
		printk(KERN_ALERT "\nTimer will be fired in %dms",Time_Delay);
		err = mod_timer(&my_timer, jiffies + msecs_to_jiffies(Time_Delay));	//Reload timer
		if(err)
			printk(KERN_ALERT "\nError encountered in timer module!");
	}
}

/* This is invoked when this module is inserted into the kernel
 * It serves as an entry point to the module. Entry and exit
 * functions in modules are typically defined static to res-
 * external access.  */
static int __init mytimermod_init(void)
{
	int err = 0;	
		
	printk(KERN_ALERT "\nMode Inserted Successfully");
	/* Initialize timer and set callback function */
	timer_setup(&my_timer, timer_handler, 0);
	printk(KERN_ALERT "\nTimer will be fired in %dms",Time_Delay);
	/* Use jiffies to offset 500ms from current time  */
	err = mod_timer(&my_timer, jiffies + msecs_to_jiffies(Time_Delay));

	if(err)
		printk(KERN_ALERT "\nError encountered in timer moduel!");

	return 0;
}

/* This is invoked when this module is removed from the kernel
 * It serves as an exit point from the module. Entry and exit
 * functions in modules are typically defined static to res-
 * external access.  */
static void __exit mytimermod_exit(void)
{
	int err = 0;
	/* Delete timer */
	err = del_timer(&my_timer);
	if(err)
		printk(KERN_ALERT "\nTimer stil in use!");
	printk(KERN_ALERT "\nModule Removed Successfully\n");

}


module_init(mytimermod_init);	//Register Module Entry Point
module_exit(mytimermod_exit);	//Register Module Exit Point
