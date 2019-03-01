
/* The pthread_self() function returns the ID of
 * the calling thread. This is the same value t-
 * hat is returned in *thread in the pthread_cr-
 * eate(3) call that created this thread.  */

/* syscall(__NR_gettid) = TID of the calling thr-
 * ead*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<ctype.h>
#include<unistd.h>
#include<signal.h>
#include<pthread.h>
#include<sys/time.h>
#include<sys/syscall.h>

#define CHILD1_INP_FILE_NAME	("gdb.txt")
#define ASCII_VALUE_a			(97)
#define NUM_ALPHABETS			(26)
#define CHAR_COUNT_TSHLD		(100)

pthread_mutex_t resource_lock;

typedef struct my_thread
{
	int id;
	char* log;
	char* input_file;

}thread_t;

typedef struct Node
{
	char character;
	int count;
	struct Node* next;
}node;
	
struct timeval Now;

static timer_t timerid;
struct itimerspec trigger;

int child2_exit = 0;
int cpu_rep_exit = 0;
int kill_code = 0;

FILE *log_file_ptr = NULL;

void write_log(thread_t *requesting_thread, int IsKillClose, char *killtype)
{
	pthread_mutex_lock(&resource_lock);

	if(requesting_thread->id == 0)
	{
		if(!IsKillClose)
		{
			log_file_ptr = fopen(requesting_thread->log,"w");
			if(log_file_ptr == NULL)
			{
				printf("\nParent thread cannot Open Log file in Write mode. Exitting");
				exit(0);
			}
			fprintf(log_file_ptr, "Parent thread logged\n");
		}
		else
		{
			log_file_ptr = fopen(requesting_thread->log,"a");
			if(log_file_ptr == NULL)
			{
				printf("\nParent thread cannot Open Log file in Append mode. Exitting");
				exit(0);
			}
		}
	}
	else
	{
		log_file_ptr = fopen(requesting_thread->log,"a");
		if(log_file_ptr == NULL)
		{
			printf("\nThread - %d xannot Open Log file in Append mode. Exitting",requesting_thread->id);
			exit(0);
		}
		if(!IsKillClose)
			fprintf(log_file_ptr, "\nChild thread - #%d logged\n", requesting_thread->id);
	}
	
	if(!IsKillClose)
	{
		gettimeofday(&Now,NULL);
		fprintf(log_file_ptr, "Created at %lu.%06lu\n", Now.tv_sec,Now.tv_usec);
		fprintf(log_file_ptr, "Posix Thread ID : %ld\n", pthread_self());
		fprintf(log_file_ptr, "Linux Thread ID : %ld\n", syscall(__NR_gettid));
		fprintf(log_file_ptr, "PID : %d\n\n", getpid());
	}
	else
	{
		gettimeofday(&Now,NULL);
		fprintf(log_file_ptr, "\nThread - %d Killed at %lu.%06lu - %s\n\n", requesting_thread->id,Now.tv_sec,Now.tv_usec,killtype);
	}
	fclose(log_file_ptr);

	pthread_mutex_unlock(&resource_lock);
}

void create_list(node** head_ref)
{
	int parser = 0, ascii_reg = ASCII_VALUE_a;
	for(parser = 0; parser < NUM_ALPHABETS; parser++)
	{
		node *new_node = (node*)malloc(sizeof(node));
		new_node->character = (char)ascii_reg++;
		new_node->count = 0;
		new_node->next = *head_ref;
		*head_ref = new_node;
	}
}

void insert_character(node** head_ref, char c)
{
	node **temp = head_ref;
	while((*temp)->character != c)
	{
		*temp=(*temp)->next;
	}
	(*temp)->count++;
}

void copy_list_to_file(node *head_ref, thread_t *requesting_thread)
{
	pthread_mutex_lock(&resource_lock);

	log_file_ptr = fopen(requesting_thread->log,"a");
	if(log_file_ptr == NULL)
	{
		printf("\nChild - %d failed to open %s in Append mode. Exiting\n",requesting_thread->id,requesting_thread->log);
		exit(0);
	}

	fprintf(log_file_ptr, "Characters with a max frequency of 100 :\n");
	node *temp = head_ref;
	while(temp != NULL)
	{
		if(temp->count <= CHAR_COUNT_TSHLD)
			fprintf(log_file_ptr, "%c - %d\n", temp->character, temp->count);
		temp = temp->next;
	}
	fclose(log_file_ptr);

	pthread_mutex_unlock(&resource_lock);
}

void process_characters(thread_t *requesting_thread)
{
	FILE *inp_file_ptr = NULL;
	char cfile = 0;
	node *head = NULL;
	node *head_copy = NULL;

	inp_file_ptr = fopen(requesting_thread->input_file,"r");
	if(inp_file_ptr == NULL)
	{
		printf("\nChild - %d failed to open %s in Read mode. Exiting\n",requesting_thread->id,requesting_thread->input_file);
		exit(0);
	}

	create_list(&head);

	printf("\nCharacter linked list created successfully\n");

	while(!feof(inp_file_ptr))
	{
		cfile = tolower(fgetc(inp_file_ptr));
		if((cfile >= ASCII_VALUE_a) && (cfile <= (ASCII_VALUE_a + NUM_ALPHABETS -1)))
		{
			head_copy = head;
			insert_character(&head_copy,cfile);
		}
	}

	printf("\nCharacters counted successfully\n");

	copy_list_to_file(head, requesting_thread);

	printf("\nSuccessfully copied list data to log\n");

}

void report_cpu_utilization(union sigval sv)
{
	thread_t* req_thread = (thread_t*)sv.sival_ptr;
	char *path = req_thread->log;
	int signal = sv.sival_int;
	struct timespec thTimeSpec;

	FILE *log_file_ptr = NULL;
	FILE *cmd_ptr = NULL;
	char read_c = 0;

	if(signal == SIGVTALRM)
		printf("\nHello\n");

	cmd_ptr = popen("cat /proc/stat | head -n 2","r");
	if(cmd_ptr == NULL)
	{
		printf("\nChild - 2 failed to open pipe for /proc/stat (Read mode). Exiting\n");
		exit(0);
	}

	pthread_mutex_lock(&resource_lock);

	log_file_ptr = fopen(path,"a");
	if(log_file_ptr == NULL)
	{
		printf("\nChild - 2 failed to open %s in Append mode. Exiting\n",path);
		exit(0);
	}

	fprintf(log_file_ptr, "\nCPU Utilization :\n");
	while(!feof(cmd_ptr))
	{
		read_c = fgetc(cmd_ptr);
		fprintf(log_file_ptr, "%c", read_c);
	}
	fprintf(log_file_ptr, "\n\n");

	pclose(cmd_ptr);
	fclose(log_file_ptr);

	pthread_mutex_unlock(&resource_lock);

	printf("CPU Utilization Obtained\n");

	if(child2_exit)
	{
		cpu_rep_exit = 1;
		pthread_exit(NULL);
	}
	else
	{
		trigger.it_value.tv_sec = 0;
		trigger.it_value.tv_nsec = 100000000;
    	timer_settime(timerid, 0, &trigger, NULL);
	}
	
}

void _handler_kill_thread2(int signum)
{
	if(signum == SIGUSR1)
	{
		kill_code = 1;
		child2_exit = 1;
	}

	if(signum == SIGUSR2)
	{
		kill_code = 2;
		child2_exit = 1;
	}

	if(signum == SIGINT)
	{
		kill_code = 3;
		child2_exit = 1;
	}

}

void task(void *requesting_thread)
{
	thread_t * req_thread = (thread_t*) requesting_thread;
	printf("\nTask Executed :)\n");
	write_log(req_thread,0,"");
	if(req_thread->id == 1)
	{
		process_characters(req_thread);
		write_log(req_thread,1,"Normally");
		pthread_exit(NULL);
	}

	if(req_thread->id == 2)
	{
		struct sigevent sev;
		struct timespec mainTimeSpec;

		struct sigaction kill_action2;
		memset (&kill_action2, 0, sizeof (kill_action2));
		kill_action2.sa_handler = &_handler_kill_thread2;

		/* Set all `sev` and `trigger` memory to 0 */
        memset(&sev, 0, sizeof(struct sigevent));
        memset(&trigger, 0, sizeof(struct itimerspec));

        sev.sigev_notify = SIGEV_THREAD;
        sev.sigev_notify_function = &report_cpu_utilization;
        sev.sigev_value.sival_ptr = req_thread;

        timer_create(CLOCK_REALTIME, &sev, &timerid);
        trigger.it_value.tv_sec = 0;
		trigger.it_value.tv_nsec = 100000000;

        timer_settime(timerid, 0, &trigger, NULL);

        sigaction (SIGUSR1, &kill_action2, NULL);
        sigaction (SIGUSR2, &kill_action2, NULL);
        sigaction (SIGINT, &kill_action2, NULL);
        
        while(1)
        {
        	if(cpu_rep_exit)
        	{
        		if(kill_code == 1)
        		{
        			write_log(req_thread,1,"by SIGUSR1");
        			printf("Killed by SIGUSR1\n");
        		}
        		if(kill_code == 2)
        		{
        			write_log(req_thread,1,"by SIGUSR2");
        			printf("Killed by SIGUSR2\n");
        		}
        		if(kill_code == 3)
        		{
        			write_log(req_thread,1,"by Ctrl-C");
        			printf("Killed by Ctrl-C\n");
        		}
        		pthread_exit(NULL);
        	}
        }

        /* Delete (destroy) the timer */
        timer_delete(timerid);

		//report_cpu_utilization(req_thread);
	}
}

int main(int argc, char* argv[])
{
	pthread_t thread1, thread2;	
	int thread_status = 0;
	thread_t *t_parent = NULL;
	thread_t *t_child1 = NULL;
	thread_t *t_child2 = NULL;
	int thread_id_seed = 0;

	t_parent = (thread_t*)malloc(sizeof(thread_t));
	if(t_parent == NULL)
	{
		printf("\nMalloc for Parent Thread failed. Exiting\n");
		return 0;
	}

	t_parent->id = thread_id_seed++;
	t_parent->log = argv[1];
	t_parent->input_file = NULL;

	t_child1 = (thread_t*)malloc(sizeof(thread_t));
	if(t_child1 == NULL)
	{
		printf("\nMalloc for Child Thread-1 failed. Exiting\n");
		free(t_parent);
		return 0;
	}

	t_child1->id = thread_id_seed++;
	t_child1->log = argv[1];
	t_child1->input_file = CHILD1_INP_FILE_NAME;

	t_child2 = (thread_t*)malloc(sizeof(thread_t));
	if(t_child2 == NULL)
	{
		printf("\nMalloc for Child Thread-2 failed. Exiting\n");
		free(t_child1);
		return 0;
	}

	t_child2->id = thread_id_seed++;
	t_child2->log = argv[1];
	t_child2->input_file = NULL;

	if(pthread_mutex_init(&resource_lock, NULL) != 0)
	{
		printf("\nMutex initialization failed. Exiting\n");
		return 0;
	}

	write_log(t_parent,0,"");

	thread_status = pthread_create(&thread1,NULL,(void *)task, (void*) t_child1);
	if(thread_status != 0)
	{
		printf("\nChild thread - 1 cannot be created\n");
		return 0;
	}

	thread_status = pthread_create(&thread2,NULL,(void *)task, (void*) t_child2);
	if(thread_status != 0)
	{
		printf("\nChild thread - 2 cannot be created\n");
		return 0;
	}

	printf("\nid = %d, log = %s , input file = %s\n", t_child1->id, t_child1->log, t_child1->input_file);

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);

	write_log(t_parent,1,"Normally");

	pthread_mutex_destroy(&resource_lock);


	return 0;
}
