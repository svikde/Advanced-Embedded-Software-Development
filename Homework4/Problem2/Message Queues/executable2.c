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
#include<signal.h>

#define Q_NAME 		"/my_queue"
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

void _handler_kill(int signal)
{
	printf("Killed by Ctrl-C\n");
	write_log(1,1,"Process-2 Killed by Ctrl-C",NULL);
	exit(1);
}

int main()
{
	int value;
	char str[100];
	int j = 0;
	mqd_t msgqueue_FD;
	struct mq_attr msgqueue_FD_attr;
	msgqueue_FD_attr.mq_maxmsg = Q_SIZE;
	msgqueue_FD_attr.mq_msgsize = sizeof(mesg_t);

	mesg_t message;
	mesg_t *msgptr;

	write_log(0,1,"IPC Method = Message Queues (starting)",NULL);

	struct sigaction kill_action;
	memset (&kill_action, 0, sizeof (kill_action));
	kill_action.sa_handler = &_handler_kill;
	sigaction (SIGINT, &kill_action, NULL);

	msgqueue_FD = mq_open(Q_NAME, O_CREAT | O_RDWR, 0666, &msgqueue_FD_attr);
	if(msgqueue_FD == -1)
	{
		perror("Error in opennig message queue");
		exit(1);
	}

	sprintf(str,"Process-2 File Descriptor = %d",(int)msgqueue_FD);
	write_log(1,1,str,NULL);

    for(j = 0 ; j < 10 ; j++)
    {
		msgptr = &message;

		value = mq_receive(msgqueue_FD, (char *)msgptr, sizeof(mesg_t),0);
		if(value == -1)
		{
			perror("Error in receiving message");
			exit(1);
		}

		printf("%s with led control=%d\n",msgptr->string, msgptr->led_control);
		write_log(1,0,"Process-2 Receving",msgptr);

	    sprintf(str, "%d %s %s %d %s",j+1 , messagelookup[j+10]," - from Process2 ( PID = ",getpid(), ")");
		strcpy(message.string, str);
		message.length = strlen(message.string);
		if(j < 1 || j > 8)
			message.led_control=LED_ON;

		value = mq_send(msgqueue_FD, (char *)msgptr, sizeof(mesg_t),0);			
		if(value == -1)
		{
			perror("Send error");
			exit(1);
		}
		write_log(1,0,"Process-2 Sending",msgptr);
	}

	mq_close(msgqueue_FD);
	mq_unlink(Q_NAME);

	write_log(1,1,"Process-2 Killed Normally",NULL);

	return 0;
}