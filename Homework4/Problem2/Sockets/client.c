#include<stdio.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<string.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<sys/time.h>
#include<signal.h>

#define PORT 		(8800)
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
	write_log(1,1,"Client Process Killed by Ctrl-C",NULL);
	exit(1);
}
  
int main(int argc, char const *argv[])
{
	int socket_FD = 0;
	struct sockaddr_in address;
	struct sockaddr_in serv_addr;

	char str[100];

	mesg_t message;
	mesg_t *msgptr;

	struct sigaction kill_action;
	memset (&kill_action, 0, sizeof (kill_action));
	kill_action.sa_handler = &_handler_kill;
	sigaction (SIGINT, &kill_action, NULL);

	socket_FD = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_FD < 0)
    {
       	perror("Error in creating socket");
       	exit(EXIT_FAILURE);
    }
	else
	{
		printf("Socket created successfully\n");
		sprintf(str,"client side socket fd = %d",socket_FD);
		write_log(1,1,str,NULL);

	}
  
	memset(&serv_addr, '0', sizeof(serv_addr));
  
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
      
	// Convert IPv4 and IPv6 addresses from text to binary form
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) 
	{
        	perror("\nUnsupported Address\n");
        	return -1;
	}

   	if (connect(socket_FD, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
   	{
       	perror("Error in Connection");
       	exit(EXIT_FAILURE);
   	}
	else
	{
        printf("\nConnection Successfull \n");
                
    }


    int j = 0;

    for(j = 0 ; j < 10 ; j++)
    {
	   	sprintf(str, "%d %s %s %d %s",j+1 , messagelookup[j]," - from client ( PID = ",getpid(), ")");
		strcpy(message.string, str);
		message.length = strlen(message.string);
		if(j >= 1 && j < 9)
			message.led_control = LED_OFF;
		else
			message.led_control = LED_ON;

	    send(socket_FD , (void *)&message , sizeof(mesg_t) , 0 );

	    msgptr = &message;

	    write_log(1,0,"Client Sending",msgptr);

	    read( socket_FD , msgptr, sizeof(mesg_t));

	    write_log(1,0,"Client Receiving",msgptr);
	    printf("%s with led_control=%d\n",msgptr->string,msgptr->led_control);
	}

	write_log(1,1,"Client Process Killed Normally",NULL);
    
	return 0;

}