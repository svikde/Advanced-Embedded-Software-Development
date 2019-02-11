/******************************************************************************************
*	@file		PeriodicTask.c
*	@brief		This program runs every 10mins as per the cron configuration
*			in Beaglebone Green. It uses system default and the custom sort
*			system calls added to the kernel periodically.
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
#include<sys/time.h>

#define LENGTH			(256)	//Length of the buffer (sorted & unsorted)
#define SYS_SORTACK		(398)	//System Call table index

int main()
{
	time_t Time = time(NULL);	
	struct timeval Now;

	int parser = 0;
	int syscall_state = 0;
	int32_t *unsorted = (int32_t*)malloc(LENGTH * sizeof(int32_t));
	int32_t *sorted = (int32_t*)malloc(LENGTH * sizeof(int32_t));

	/* Get Current Process ID */
	printf("\n\nCurrent Process ID = %d",getpid());
	/* Get User ID */
	printf("\nCurrent User ID = %d",getuid());
	gettimeofday(&Now,0);				//Get time since epoch
	/* Print time sice epoch */
	printf("\nCuurent Time (since Epoch): Seconds = %lu Microseconds = %lu",Now.tv_sec,Now.tv_usec);

	srand(time(0));					//Using current time as seed
	printf("\nUnsorted Array From User :\n\n");
	/* Print Unsorted Array */
	for(parser = 0 ; parser < LENGTH ; parser++)
	{
		unsorted[parser] = rand() % LENGTH;
		printf("%d ",unsorted[parser]);
	}
	printf("\n\n");

	/* Print Unsorted Array Size*/
	printf("Size of Unsorted Array = %d - From User\n", LENGTH * sizeof(int32_t));
	
	/* System Call to the Kernel Sort Function as pointed by the System Call Table */
	syscall_state = syscall(SYS_SORTACK, unsorted, LENGTH, sorted);	
	
	/* Check if System Call is a success */
	if(syscall_state == 0)
	{
		/* Print Size of Sorted array and sorted elements as returned by the Kernel */
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
	
	return 0;
}
