#include<sys/mman.h>
#include<sys/stat.h>   
#include<fcntl.h>
#include<errno.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<stdint.h>
#include<pthread.h>
#include<semaphore.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<sys/time.h>
#include<signal.h>

#define SHRD_MEM 	("smemory")
#define SEM_LOCK 	("semphr_lock")
#define LOG_FILE	("log.txt")

enum led_state{LED_OFF, LED_ON};

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

void write_log(int IsFileCreated, int IsJustMessage, char *status, mesg_t *message)
{
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
	if(IsJustMessage)
		fprintf(log_file_ptr, "[%lu.%06lu] %s \n", Now.tv_sec,Now.tv_usec,status);	
	else
		fprintf(log_file_ptr, "[%lu.%06lu] %s : message = %s, led_control = %d \n", Now.tv_sec,Now.tv_usec,status,message->string,message->led_control);

	fclose(log_file_ptr);
}

void _handler_kill(int signal)
{
	printf("Killed by Ctrl-C\n");
	write_log(1,1,"Process-2 Killed by Ctrl-C",NULL);
	exit(1);
}

int main()
{
	int sharedmemory_FD;
	int sharedmemory_size;
	mesg_t Message;
	mesg_t *msgptr;
	char str[100];

	write_log(0,1,"IPC Method - Shared Memory (Starting)",msgptr);

	struct sigaction kill_action;
	memset (&kill_action, 0, sizeof (kill_action));
	kill_action.sa_handler = &_handler_kill;
	sigaction (SIGINT, &kill_action, NULL);


	sharedmemory_FD = shm_open(SHRD_MEM, O_CREAT | O_RDWR, 0666);

	sprintf(str,"Process-2 File Descriptor = %d",sharedmemory_FD);
	write_log(1,1,str,msgptr);

	if(sharedmemory_FD < 0 )
	{
		perror("Errror in opening Shared Memory");
		exit(1);
	}

	sharedmemory_size = sizeof(mesg_t);

	void *sharedmemory_map = mmap(NULL, sharedmemory_size, PROT_READ | PROT_WRITE, MAP_SHARED , sharedmemory_FD, 0);
	if(sharedmemory_map == (void *)-1)
	{
		perror("Error in mapping shared memory");
		exit(1);
	}

	sem_t * semaphore_var = sem_open(SEM_LOCK, O_CREAT, 0666, 0);
	sem_t * semaphore_var2 = sem_open("sem2", O_CREAT, 0666, 0);
	if(semaphore_var == NULL)
	{
		perror("Error in opening Semaphore");
		exit(1);	
	}

	int j = 0;

	for(j = 0 ; j < 10 ; j++)
	{
		sem_wait(semaphore_var);

	    msgptr = &Message;

	    memcpy((char *)msgptr,(char*)sharedmemory_map, sharedmemory_size);

	    printf("%s with led control=%d\n",msgptr->string, msgptr->led_control);

	  	write_log(1,0,"Process-2 Receving",msgptr);

	    sprintf(str, "%d %s %s %d %s",j+1 , messagelookup[j+10]," - from Process2 ( PID = ",getpid(), ")");
		strcpy(Message.string, str);
		//printf("%s \n", str);
		Message.length = strlen(Message.string);
		if(j < 1 || j > 8)
			Message.led_control=LED_ON;

		msgptr = &Message;

	    memcpy((char*)sharedmemory_map, (char *)msgptr, sharedmemory_size);
	    write_log(1,0,"Process-2 Sending",msgptr);

	    sem_post(semaphore_var2);
	}

	close(sharedmemory_FD);
  	sem_unlink(SEM_LOCK);

  	write_log(1,1,"Process-2 Killed Normally",NULL);

	return 0;
}