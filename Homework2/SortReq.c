/******************************************************************************************
*	@file		SortReq.c
*	@brief		This program runs in the user space. It passes the unsorted
*			array to the kernel via a system call, which then sorts the
*			data and return a sorted array back to the user. This progr
*			-am calls the system call multiple times to generate failu-
*			and success test cases for the kernel. It aslo acoounts for
*			the success and failure statuses 
*	@Tools_Used	GCC  
*	@Author		Souvik De
*	@Date		02/10/2019
*
*******************************************************************************************/

#include<stdlib.h>
#include<sys/types.h>
#include<linux/kernel.h>
#include<sys/syscall.h>
#include<unistd.h>
#include<stdio.h>
#include<time.h>

#define LENGTH			(256)	//Length of the buffer (sorted & unsorted)
#define SYS_SORTACK		(398)	//System Call table index

int main()
{
	int parser = 0;
	int syscall_state = 0;
	int32_t *unsorted = (int32_t*)malloc(LENGTH * sizeof(int32_t));	//Allocate space for sorted data elements
	int32_t *sorted = (int32_t*)malloc(LENGTH * sizeof(int32_t));	//Allocate space for unsorted data elements
	
	srand(time(0));	//Using current time as seed
	printf("\nUnsorted Array From User :\n\n");

	/* Print Unsorted Data Elements */
	for(parser = 0 ; parser < LENGTH ; parser++)
	{
		unsorted[parser] = rand() % LENGTH;
		printf("%d ",unsorted[parser]);
	}
	printf("\n\n");

	/* Print Size of Unsorted Data */
	printf("Size of Unsorted Array = %d - From User\n", LENGTH * sizeof(int32_t));
	
	/* Run System Call repeatedly to test for failure and success cases */
	for(parser = 0 ; parser < 4 ; parser++)
	{	
		if(parser == 0)		//Test for failure by passsing null pointer
			syscall_state = syscall(SYS_SORTACK, NULL, LENGTH, sorted);
		else if(parser == 1)	//Test for failure by passing length less than 256
			syscall_state = syscall(SYS_SORTACK, unsorted, LENGTH - 200, sorted);
		else if(parser == 2)	//Test for Kmalloc failure by passing negative length
			syscall_state = syscall(SYS_SORTACK, unsorted, LENGTH - 300, sorted);
		else			//Success condition
			syscall_state = syscall(SYS_SORTACK, unsorted, LENGTH, sorted);		
		/* Check if System Call is a success */
		if(syscall_state == 0)
		{
			/* Print Size of Sorted data and sorted elements as returned by the Kernel */
			printf("\nSize of Sorted Array = %d - From User", LENGTH * sizeof(int32_t));
			printf("\n\nSorted Array From User :\n\n");
			for(parser = 0 ; parser < LENGTH ; parser++)
			{
				printf("%d ",sorted[parser]);
			}
			printf("\n\n");
		}
		else
		{
			/* Print Failure Message and Status Code */
			printf("\nSystem Call Failure! Failure Status = %ld\n",syscall_state);
		}
	}	
	
	return 0;
}


