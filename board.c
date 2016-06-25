// board.c process that creates the server process
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define server "spid.txt"
#define MSGSIZE 500

char *fifo3 = "B2S";
char *fifo4 = "S2B";

void handler(int sig) //handler gia to ctrl+c
{
    if (sig == SIGINT)
    	printf("no escape\n");
}

int main(int argc, char *argv[]){
	struct stat sb;
	FILE *fp;
	char *ind;
	int fd,fd4;
	char cwd[1024], oneword[500],buff[255];
	if(argc<2){
		printf("Usage: board <pathname>  (aborting...)\n");
		exit(1);
	}
	if (stat(argv[1], &sb) == -1) {
		printf("Creating directory %s ...\n",argv[1]);
    		if (mkdir(argv[1],0777) == -1)
			perror(argv[0]);
		else
			printf("Success: directory %s is now constructed\n",argv[1]);
	}
	//to path tou xristi twra yparxei (eite proypirxe eite kataskevastike)
	if (getcwd(cwd, sizeof(cwd)) == NULL)
	       		exit(2);
	chdir(argv[1]);
	if(fopen(server,"r") == NULL){
		pid_t pid;
		//waitpid(-1,&status,WNOHANG | WUNTRACED);
		if((pid = fork())==1){
			printf("failed to fork\n");
			exit(1);
		}
		if(pid==0){ //child process aka Server Process
			fp = fopen(server,"w");
			fprintf(fp,"%d",getpid());
			fclose(fp);
			setpgid(0, 0);
			execl(strcat(cwd,"/boardserver"),"boardserver",NULL);
		}
		if(pid){
			printf("Please rerun board process.\n");
			return(0);
		}
	}
	char c;
	c = fscanf(stdin,"%s",oneword);
	signal(SIGINT, handler);
	while(1){
		if(!strcmp(oneword,"createchannel")){ 
			int nwrite;
			fgets(buff, 255, stdin);
			ind = strtok (buff," ");
			if ( (fd=open(fifo3, O_WRONLY| O_NONBLOCK)) < 0)
				{ perror("board fife open error"); exit(1); }
			if ((nwrite=write(fd, "createchannel", strlen("createchannel")+1)) == -1)
				{ perror("Error in Writing"); exit(2); }
  			while (ind != NULL){
				if ((nwrite=write(fd, ind, strlen(ind)+1)) == -1)
				{ perror("Error in Writing"); exit(2); }
    				ind = strtok (NULL, " ");
  			}
			printf("channel created!\n");
			close(fd);
		}
		if(!strcmp(oneword,"display")){ //debugging helper
			int nwrite;
			char msgbuf[256];
			if ((nwrite=write(fd, "display", strlen("display")+1)) == -1)
				{ perror("Error in Writing"); exit(2); }
			if ( (fd4=open(fifo4, O_RDWR )) < 0){
				perror("fifo open problem"); exit(3);	
			}
			if ( read(fd4, msgbuf, MSGSIZE+1) <= 0) {
					perror("problem in reading"); exit(5);
				}
				//printf("%d\n",read(fd4, msgbuf, MSGSIZE+1));
				fflush(stdout);
				printf("%s", msgbuf);
				fflush(stdout);
			close(fd4);
			
		}
		if(!strcmp(oneword,"getmessages")){ //antigrafei arxeia sto <path> kai emfanizei ta filenames tous sto terminal
			int nwrite,id;			//episis epistrefei pisw osa minimata yparxoun sto channel <id>
			char msgbuf[1000];
			fgets(buff, 255, stdin);
			ind = strtok (buff," ");
			if ( (fd=open(fifo3, O_WRONLY| O_NONBLOCK)) < 0)
				{ perror("board fife open error"); exit(1); }
			if ((nwrite=write(fd, "getmessages", strlen("getmessages")+1)) == -1)
				{ perror("Error in Writing"); exit(2); }
  			while (ind != NULL){
				if ((nwrite=write(fd, ind, strlen(ind)+1)) == -1)
				{ perror("Error in Writing"); exit(2); }
				id=atoi(ind);
    				ind = strtok (NULL, " ");
  			}
			if ((nwrite=write(fd, cwd, strlen(cwd)+1)) == -1)
				{ perror("Error in Writing"); exit(2); }
			close(fd);
			if ( (fd4=open(fifo4, O_RDWR )) < 0){
				perror("fifo open problem"); exit(3);	
			}
			if ( read(fd4, msgbuf, MSGSIZE+1) <= 0) {
					perror("problem in reading"); exit(5);
				}
				//printf("%d\n",read(fd4, msgbuf, MSGSIZE+1));
			printf("messages for channel %d: %s", id,msgbuf);
			close(fd4);
			//close(fd);
		}
		if(!strcmp(oneword,"shutdown")){ //ola termatizoun
			int nwrite;
			if ( (fd=open(fifo3, O_WRONLY| O_NONBLOCK)) < 0)
				{ perror("fifo 3 board fife open error"); exit(1); }
			if ((nwrite=write(fd, "shutdown", strlen("createchannel")+1)) == -1)
				{ perror("Error in Writing"); exit(2); }
			sleep(1);
			unlink(server);
			rmdir(argv[1]);
			printf("exiting ./board\n");
			
			return 0;
		}
		if(!strcmp(oneword,"exit")){
			printf("exiting ./board\n");
			return 0;
		}
		c = fscanf(stdin,"%s",oneword);
	}
	
	return 0;
}
