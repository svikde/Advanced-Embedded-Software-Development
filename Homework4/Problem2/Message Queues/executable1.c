#include<fcntl.h> 
#include<sys/stat.h>
#include<mqueue.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<sys/time.h>

#define Q_NAME 		("/my_queue")
#define Q_SIZE		(8)
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

int main()
{
	mqd_t msgqueue_FD;
	char str[100];
	int value;
	struct mq_attr msgqueue_FD_attr;
	msgqueue_FD_attr.mq_maxmsg = Q_SIZE;
	msgqueue_FD_attr.mq_msgsize = sizeof(mesg_t);

	mesg_t message;
	mesg_t *msgptr;

	msgqueue_FD = mq_open(Q_NAME, O_CREAT | O_RDWR, 0666, &msgqueue_FD_attr);		//open queue

	if(msgqueue_FD == (mqd_t)-1)
	{
		perror("Error in opening Message Queue");
		exit(1);
	}
	sprintf(str,"Process-1 File Descriptor = %d",(int)msgqueue_FD);
	write_log(1,1,str,NULL);

    int j = 0;

    for(j = 0 ; j < 10 ; j++)
    {	
	    sprintf(str, "%d %s %s %d %s",j+1 , messagelookup[j]," - from Process1 ( PID = ",getpid(), ")");
		strcpy(message.string, str);
		message.length = strlen(message.string);
		if(j >= 1 && j < 9)
			message.led_control = LED_OFF;
		else
			message.led_control = LED_ON;

		msgptr = &message;

		value = mq_send(msgqueue_FD, (char*)msgptr, sizeof(mesg_t),0);	
		if(value == -1)
		{
			perror("Error in sending message");
			exit(1);
		}
		write_log(1,0,"Process-1 Sending",msgptr);

		value = mq_receive(msgqueue_FD, (char*)msgptr, sizeof(mesg_t), 0);	
		if(value == -1)
		{
			perror("Error while receiving");
			exit(1);
		}

		write_log(1,0,"Process-1 Receving",msgptr);
		printf("%s with led control=%d\n",msgptr->string, msgptr->led_control);
	}

	mq_close(msgqueue_FD);
	mq_unlink(Q_NAME);

	write_log(1,1,"Process-1 Killed Normally",NULL);
									
	return 0;
}