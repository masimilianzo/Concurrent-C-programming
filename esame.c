#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <sys/stat.h>

float a=20.0;
float b;
float c;
pid_t pid;
pid_t child_pid[3];
int local_child_index;
int fd12;
int fd23;
int fd31;
time_t t;


int main() {

int i;

srand((unsigned) time(&t));



if(mkfifo("apipe", 0660)==0) printf("fifo 1 created\n");
else printf("error creating fifo 1\n");


if(mkfifo("bpipe", 0660)==0) printf("fifo 2 created\n");
else printf("error creating fifo 2\n");


if(mkfifo("cpipe", 0660)==0) printf("fifo 3 created\n");
else printf("error creating fifo 3\n");
	

for (i=0; i<3; i++)
{
	pid=fork();
	
	if(pid==0)
	{	
		local_child_index = i;
		child_pid[i]=getpid();
		break;
	}
}

if (pid==0)
{
	int fifo_read_open = 0;
	int fifo_write_open = 0;
	
	switch(local_child_index)
	{
	case 0:
		
		while(1)
		{
			b=rand()%21+10;
			c=(a+b)/2;				
			
			if ( !fifo_read_open ) 
			{
				printf("F1[%d] opening read\n", child_pid[local_child_index]);
				fd31=open("cpipe", O_RDONLY);
				printf("F1[%d] opened read\n", child_pid[local_child_index]);
				fifo_read_open = 1;
			}
			
			if ( !fifo_write_open )
			{
				printf("F1[%d] opening write\n", child_pid[local_child_index]);
				fd12=open("apipe", O_WRONLY);
				printf("F1[%d] opened write\n", child_pid[local_child_index]);
				fifo_write_open = 1;
			}
			
			write(fd12, &c, sizeof(float));
			
			printf("F1: a = %1f, b = %1f, c=%lf\n", a, b, c);
			
			read(fd31, &a, sizeof(float));
			
			if (c>27)
			{
				exit(0);
			}
		}

	case 1:
		while(1)
		{
			b=rand()%21+10;
			c=(a+b)/2;				
			
			if ( !fifo_read_open ) 
			{
				printf("F2[%d] opening read\n", child_pid[local_child_index]);
				fd12=open("apipe", O_RDONLY);
				fifo_read_open = 1;
				printf("F2[%d] opened read\n", child_pid[local_child_index]);
			}
			
			if ( !fifo_write_open )
			{
				printf("F2[%d] opening write\n", child_pid[local_child_index]);
				fd23=open("bpipe", O_WRONLY);
				fifo_write_open = 1;
				printf("F2[%d] opened write\n", child_pid[local_child_index]);
			}
			
			write(fd23, &c, sizeof(float));
			
			printf("F2: a = %1f, b = %1f, c=%lf\n", a, b, c);
			
			read(fd12, &a, sizeof(float));
			
			if (c>27)
			{
				exit(0);
			}
			
		}

	case 2:
		while(1)
		{
			
			b=rand()%21+10;
			c=(a+b)/2;				
			
			if ( !fifo_write_open )
			{
				printf("F3[%d] opening write\n", child_pid[local_child_index]);
				fd31=open("cpipe", O_WRONLY);
				fifo_write_open = 1;
				printf("F3[%d] opened write\n", child_pid[local_child_index]);
			}
			
			if ( !fifo_read_open ) 
			{
				printf("F3[%d] opening read\n", child_pid[local_child_index]);
				fd23=open("bpipe", O_RDONLY);
				fifo_read_open = 1;
				printf("F3[%d] opened read\n", child_pid[local_child_index]);
			}
			
			write(fd31, &c, sizeof(float));
			
			printf("F3: a = %1f, b = %1f, c=%lf\n", a, b, c);
			
			read(fd23, &a, sizeof(float));
			
			if (c>27)
			{
				exit(0);
			}
		}
	}
}

if (pid>0)
{
	wait(NULL);
	unlink("apipe");
	unlink("bpipe");
	unlink("cpipe");
	kill(SIGKILL, child_pid[0]);
	kill(SIGKILL, child_pid[1]);
	kill(SIGKILL, child_pid[2]);
	sleep(1);
	printf("average exceeded 27, terminating program..\n");
}

return 0;

}
