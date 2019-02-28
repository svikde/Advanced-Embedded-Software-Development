/* Total Number of Processes = 2n where n is number of fork system calls */


#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/time.h>
#include<pthread.h>
#include<sys/wait.h>

#define LOG_FILE	("log.txt")

enum led_state{LED_OFF, LED_ON};
pthread_mutex_t resource_lock;

typedef struct Message
{
	char string[100];
	int length;
	int led_control;
}mesg_t;

FILE *log_file_ptr = NULL;

char *messagelookup[] = {
	"Hello there ! ",
	"I'm clearing LED",
	"So, how is it going ?",
	"Do you want to toggle the LED ?",
	"What is your favourite IPC method ?",
	"one, testing",
	"Two",
	"Three",
	"Four",
	"Lets set the led",
	"Hi there !",
	"Cool, I'm fine with that.",
	"Very well...I was testing IPC.",
	"Nah..not now.",
	"Duhh...Pipes",
	"Sure, one it is",
	"Two received",
	"Like the musketeers, three !",
	"Yes yes four",
	"Cool, set LED received"
};

void write_log(int IsFileCreated, char *status, mesg_t message)
{
	pthread_mutex_lock(&resource_lock);

	struct timeval Now;

	if(!IsFileCreated)
	{
		log_file_ptr = fopen(LOG_FILE,"w");
		if(log_file_ptr == NULL)
		{
			printf("PID = %d failed to open file in Write mode\n", getpid());
			exit(0);
		}
		printf("PID = %d opened file in Write mode\n", getpid());
	}
	else
	{
		log_file_ptr = fopen(LOG_FILE,"a");
		if(log_file_ptr == NULL)
		{
			printf("PID = %d failed to open file in Append mode\n", getpid());
			exit(0);
		}
		printf("PID = %d opened file in Append mode\n", getpid());
	}

	gettimeofday(&Now,NULL);
	fprintf(log_file_ptr, "[%lu.%06lu] %s : message = %s, led_control = %d \n", Now.tv_sec,Now.tv_usec,status,message.string,message.led_control);

	fclose(log_file_ptr);

	pthread_mutex_unlock(&resource_lock);
}

int main()
{
	/* 0th Index = Read | 1st Index = Write */
	int FD_Parent2Child[2];
	int FD_Child2Parent[2];
	pid_t child_pid;
	char str[100];

	mesg_t message;

	if(pipe(FD_Parent2Child) == -1)
	{
		perror("Failed to setup pipe from Parent to Child");
		return 1;
	}

	if(pipe(FD_Child2Parent) == -1)
	{
		perror("Failed to setup pipe from Child to Parent");
		return 1;
	}

	if(pthread_mutex_init(&resource_lock, NULL) != 0)
	{
		printf("\nMutex initialization failed. Exiting\n");
		return 0;
	}

	child_pid = fork();
	/* Creation of child was unsuccessful */
	if(child_pid < 0)
	{
		perror("Fork failed to create child");
		return 1;
	}
	/* Returned to parent or caller. The value contains PID of newly created  child process */
	if(child_pid > 0)
	{
		int j = 0;

		printf("\nParent process Ivoked\n");

		for(j = 0 ; j < 10 ; j++)
		{
			sprintf(str, "%d %s %s %d %s",j+1 , messagelookup[j]," - from parent ( PID = ",getpid(), ") through Parent Pipe");
			strcpy(message.string, str);
			message.length = strlen(message.string);
			if(j == 9)
				message.led_control = LED_ON;
			else
				message.led_control = LED_OFF;

			/* Sending Data from Parent */
			close(FD_Parent2Child[0]);	//Close Parent to Child Read
			write(FD_Parent2Child[1], &message, sizeof(mesg_t));
			if(j == 0)
				write_log(0,"Parent Sending", message);
			else
				write_log(1,"Parent Sending", message);

			/* Reading data from Child */
			close(FD_Child2Parent[1]);
			read(FD_Child2Parent[0], &message, sizeof(mesg_t));
			write_log(1,"Parent Receiving", message);
			printf("Read by Parent : %s , Led control = %d\n", message.string, message.led_control);
		}
		
	}
	else
	{
		/* Returned to the newly created child process */
		int i = 0;

		printf("\nChild Process Invoked\n");


		for(i = 0 ; i < 10 ; i++)
		{
			/* Reading Data from Child */
			close(FD_Parent2Child[1]);	//Close Parent to Child Write
			read(FD_Parent2Child[0], &message, sizeof(mesg_t));
			write_log(1,"Child Receiving", message);
			printf("Read by Child : %s , Led control = %d\n",message.string, message.led_control);

			sprintf(str,"%d %s %s %d %s",i+1 , messagelookup[i + 10]," - from Child ( PID = " ,getpid(), ") through Child Pipe");
			strcpy(message.string, str);
			strcpy(message.string,str);
			message.length = strlen(message.string);
		//	message.led_control = LED_ON;

			/* Write data to parent */
			close(FD_Child2Parent[0]);
			write(FD_Child2Parent[1], &message, sizeof(mesg_t));
			write_log(1,"Child Sending", message);
		}

	}

	pthread_mutex_destroy(&resource_lock);

	return 0;
}