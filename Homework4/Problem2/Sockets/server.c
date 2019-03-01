#include<stdio.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>
#include<sys/time.h>
#include<signal.h>

#define	PORT		(8800)
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
	write_log(1,1,"Server process Killed by Ctrl-C",NULL);
	exit(1);
}

int main()
{
	int server_FD, new_socket_FD;
	struct sockaddr_in server_address;
	int socket_attach_status;
	int opt = 1;
	int server_address_length = sizeof(server_address);
	char str[100];

	mesg_t message;
	mesg_t *msgptr;
	
	write_log(0,1,"IPC Method = Sockets (starting)",NULL);

	struct sigaction kill_action;
	memset (&kill_action, 0, sizeof (kill_action));
	kill_action.sa_handler = &_handler_kill;
	sigaction (SIGINT, &kill_action, NULL);

	server_FD = socket(AF_INET, SOCK_STREAM, 0);
	if(server_FD == 0)
	{
                perror("Error while creating Socket");
                exit(EXIT_FAILURE);
    }
	else
	{
		printf("Socket created successfully \n");
		sprintf(str,"Server side socket fd = %d",server_FD);
		write_log(1,1,str,NULL);
		
	}

	/*Attaching socket to port 8800*/
	socket_attach_status = setsockopt(server_FD, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
	if(socket_attach_status)
	{
                perror("Error in attaching socket to port");
                exit(EXIT_FAILURE);
    }

	server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);
	
	
	/*Forcefully attaching socket to the port 8800*/
    if (bind(server_FD, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        	perror("Error while binding socket");
        	exit(EXIT_FAILURE);
    }

   	if (listen(server_FD, 3) < 0)
   	{
       	perror("Error while Listening");
       	exit(EXIT_FAILURE);
   	}

	printf("Listening...\n");

	new_socket_FD = accept(server_FD, (struct sockaddr *)&server_address, (socklen_t*)&server_address_length);
    if (new_socket_FD < 0)
    {
       	perror("Error in accept");
       	exit(EXIT_FAILURE);
    }
	
    int j = 0;

    for(j = 0; j < 10 ; j++)
    {
	    msgptr = &message;

	   	read( new_socket_FD , msgptr, sizeof(mesg_t));
	   	printf("%s with led control=%d\n",msgptr->string, msgptr->led_control);
	   	write_log(1,0,"Server Receiving",msgptr);

	   	sprintf(str, "%d %s %s %d %s",j+1 , messagelookup[j+10]," - from server ( PID = ",getpid(), ")");
		strcpy(message.string, str);
		message.length = strlen(message.string);
		if(j < 1 || j > 8)
			message.led_control=LED_ON;

	   	send(new_socket_FD , (void *)&message , sizeof(mesg_t) , 0 );
	   	write_log(1,0,"Server Sending",msgptr);
    }

    write_log(1,1,"Server Process Killed Normally",NULL);

   	return 0;
}