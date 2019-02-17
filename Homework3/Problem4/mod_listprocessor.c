/******************************************************************************************
*	@file		mod_listprocessor.c
*	@brief		This program sorts a seed data of 50 animals and sorts them
*			in the Linux Kernel Module. Two linked lists are created to
*			process the data. One list contains the unique elements wi-
*			their respective count. The other list uses command line p-
*			arameters to filter out the data elemets with different co-
*			binations of animal type and count greater than. 
*	@Tools_Used	GCC  
*	@Author		Souvik De
*	@Date		02/10/2019
*
*******************************************************************************************/

#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/timer.h>
#include<linux/list.h>
#include<linux/slab.h>
#include<linux/string.h>

MODULE_LICENSE("GPL");				//License
MODULE_DESCRIPTION("Animal Filter");		//Module Description
MODULE_AUTHOR("Souvik De");			//Author

static char *animals[50];			//Accepts seed data from user
static char *animal_type = "";			//Accepts data from user to filter data on the basis of animal type
static int count_greater_than = 0;		//Accepts data from user to filter data on the basis of thier occurance
static int animal_count = 0;			//Length of seed array

/* Module Parameter setup and descriptions */
module_param_array(animals, charp, &animal_count, S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(animals, "List of Animals");

module_param(animal_type, charp , S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(animal_type, "Animal type specifier");

module_param(count_greater_than, int , S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(count_greater_than, "Filter animal type by given count");

/* Animal Data Structure */
struct animal_data_struct 
{
  	int count;			//Number of occurances
	char* animal;			//Animal Name
  	struct list_head unique_list;	//List containing unique entries
 	struct list_head filter_list;	//List containing filtered entries
};

/* Initialize heads of respective lists */
LIST_HEAD(my_unique_list);
LIST_HEAD(my_filter_list);

/* This is invoked when this module is inserted into the kernel
 * It serves as an entry point to the module. Entry and exit
 * functions in modules are typically defined static to res-
 * external access.  */
static int __init animalfilter_init(void)
{
	int i = 0, j = 0, count = 0, uniquecount = 0 , filtercount = 0;
	char temp[12];
	struct animal_data_struct *animalobj = NULL;	

	printk(KERN_ALERT "Mode Inserted Successfully");
	printk(KERN_ALERT "Using Linked List to process seed data");

	/* Print seed array from user */
	for(i = 0 ; i < animal_count ; i++)
	{
		printk(KERN_ALERT "%s",animals[i]);
	}

	/* Sort seed data */
	printk(KERN_ALERT "Sorting seed data");
	
	for (i = 0; i < animal_count - 1 ; i++) 
    	{ 
        	for (j = 0 ; j < animal_count - i - 1 ; j++) 
        	{ 
            		if (strcmp(animals[j], animals[j + 1]) > 0) 
            		{ 
              	 		strcpy(temp, animals[j]); 
              	 	 	strcpy(animals[j], animals[j+1]); 
                		strcpy(animals[j+1], temp); 
            		} 
        	} 
    	} 
	
	printk(KERN_ALERT "Processing in linked list");
	printk(KERN_ALERT "Unique Collection");

	/* Remove dplicate items and count occurances of each entry */
	for(i = 0 ; i < animal_count ; i++)
	{
		count = 0;
		for(j = 0 ; j < animal_count ; j++)
		{
			if((strcmp(animals[i],animals[j]) == 0) && (strcmp(animals[i],"") != 0))
			{
				count++;
				/* Check if a particular animal occurs more than once and then remove duplicates */
				if(count > 1)
					strcpy(animals[j],"");
			}
		}
		if(count > 0)
		{
			/* Process Unique items and their count in linked list */
			uniquecount++;					//No of nodes in the unique list
			/* Attempt to allocate nodes and check for failure */
			animalobj = (struct animal_data_struct *)kmalloc(sizeof(struct animal_data_struct), GFP_KERNEL);
			if(animalobj == NULL)
				return 0;
	
			animalobj->animal = animals[i];
			animalobj->count = count;

			/* To Identify items that had multiple occurances */
			if(count > 1)
				printk(KERN_ALERT "%s\t - %d times (Duplicates Removed)",animals[i],count);
			else	
				printk(KERN_ALERT "%s\t - %d time",animals[i],count);			
			list_add(&animalobj->unique_list, &my_unique_list );					

			/* Process data in filtered list */
			/* If animal type and count is defined */
			if(strcmp(animal_type,"") != 0)
			{
				if((strcmp(animalobj->animal,animal_type) == 0 ) && (animalobj->count > count_greater_than))
				{
					list_add(&animalobj->filter_list, &my_filter_list );		//Add to filtered list
					filtercount++;							//Track No. of nodes on the filtered list
				}
			}
			else
			{
			/* If animal type filter is not defined */
				if(count_greater_than > 0)
				{
					/* If only count greater than filter is defined */
					if(animalobj->count > count_greater_than)
					{
						list_add(&animalobj->filter_list, &my_filter_list );	//Add to filtered list
						filtercount++;						//Track No. of nodes on the filtered list
					}
				}
				else
				{
					/* With no filters applied */
					list_add(&animalobj->filter_list, &my_filter_list );		//Add to filtered list	
					filtercount++;							//Track No. of nodes on the filtered list
				}	
			}
		}
	}

	printk(KERN_ALERT "Number of nodes in unique list: %d",uniquecount);	//Number of nodes in the unique list
	printk(KERN_ALERT "Memory allocated for unique list: %ld bytes",sizeof(struct animal_data_struct) * uniquecount);	//Memory allocated for unique list
	printk(KERN_ALERT "Number of nodes in filtered list: %d",filtercount);	//Number of nodes in the filtered list
	printk(KERN_ALERT "Memory allocated for filtered list: %ld bytes",sizeof(struct animal_data_struct) * filtercount);	//Memory allocated for filtered list

	return 0;
}

/* This is invoked when this module is removed from the kernel
 * It serves as an exit point from the module. Entry and exit
 * functions in modules are typically defined static to res-
 * external access.  */
static void __exit animalfilter_exit(void)
{
	struct list_head *position, *q;
  	struct animal_data_struct *animal_obj;

	printk(KERN_ALERT "Filtered Collection");
	/* Display filter type applied */
	printk(KERN_ALERT "Search criteria: Animal Type=%s | Count greater than=%d",animal_type,count_greater_than);

	/*Traverse filtered list and print entries*/
  	list_for_each(position, &my_filter_list )
	{
    		animal_obj = list_entry(position, struct animal_data_struct, filter_list );
    		printk(KERN_ALERT "%s\t- %d", animal_obj->animal, animal_obj->count );
  	}
	
	printk(KERN_ALERT "Freeing Data Structure");
	/* Free allocated memory */
  	list_for_each_safe(position, q, &my_unique_list )
	{
    		struct animal_data_struct *tmp;
    		tmp = list_entry(position, struct animal_data_struct, unique_list);
    		list_del(position);
    		kfree(tmp);
  	}

	printk(KERN_ALERT "Module Removed Successfully");
}


module_init(animalfilter_init);	//Register Module Entry Point
module_exit(animalfilter_exit);	//Register Module Exit Point
