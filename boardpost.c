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

#define MSGSIZE 500

char *fifo1 = "S2C";
char *fifo2 = "C2S";

void handler(int sig) //handler gia to ctrl+c
{
    if (sig == SIGINT)
    	printf("no escape\n");
}

int main(int argc, char *argv[])
{
	struct stat sb;
	char path[50], *ind,dir[1000];
	int fd,fd1;
	char c,oneword[500],buff[255];
	if(argc<2){
		printf("Usage: boardpost <pathname>  (aborting...)\n");
		exit(1);
	}
	strcpy(path,argv[1]);
	while(stat(path, &sb) == -1){
		printf("The path you entered does not exist. Enter a new one:\n");
		scanf("%s",path);
		if(!strcmp(path,"exit")) return 0;
	}
	if (getcwd(dir, sizeof(dir)) == NULL)
	       		exit(2);
	chdir(path);
	printf("Path found\n");
	c = fscanf(stdin,"%s",oneword);
	signal(SIGINT, handler);
	while(1){
		if(!strcmp(oneword,"list")){ //lista twn dia8esimwn kanaliwn
			int nwrite;
			char msgbuf[500];
			if ( (fd=open(fifo2, O_WRONLY| O_NONBLOCK)) < 0)
				{ perror("fifo list open error"); exit(1); }
			if ((nwrite=write(fd, "display", strlen("display")+1)) == -1)
				{ perror("Error in Writing"); exit(2); }
			close(fd);
			if ( (fd1=open(fifo1, O_RDWR )) < 0){
				perror("fifo open problem"); exit(3);	
			}
			if ( read(fd1, msgbuf, MSGSIZE+1) <= 0) {
					perror("problem in reading"); exit(5);
				}
			printf("A list of all the available bulletin channels:\n");
			fflush(stdout);
			printf("%s", msgbuf);
			fflush(stdout);
			close(fd1);
		}
		if(!strcmp(oneword,"write")){			//stelnei to anagnwristiko W/ ws meros prwtokollou epikoinwnias
			int id,i,nwrite;			//kai olo to stdin input monokommata
			char name[10];
			char idC[5],msgbuff[256];
			char toSend[257];
			strcpy(toSend,"W/");
			i=0;
			fgets(buff, 255, stdin);
			ind = strtok (buff," ");
			
  			while (ind != NULL){
				strcat(toSend,ind);
				strcat(toSend," ");
    				ind = strtok (NULL, " ");
  			}
			if ( (fd=open(fifo2, O_WRONLY)) < 0)
				{ perror("fifo2 boardpost open error"); exit(1); }
			if ((nwrite=write(fd, toSend, strlen(toSend)+1)) == -1)
				{ perror("Error in Writing"); exit(2); }
			close(fd);
		}
		if(!strcmp(oneword,"send")){	//dinetai h entoli ston server na psaksei arxeio me titlo "filename"
			char msgbuf[256];
			int nwrite;
			fgets(buff, 255, stdin);
			ind = strtok (buff," ");
			if ( (fd=open(fifo2, O_WRONLY)) < 0)
				{ perror("send fife open error"); exit(1); }
			if ((nwrite=write(fd, "send", strlen("send")+1)) == -1)
				{ perror("Error in Writing"); exit(2); }
  			while (ind != NULL){
				if ((nwrite=write(fd, ind, strlen(ind)+1)) == -1)
				{ perror("Error in Writing"); exit(2); }
    				ind = strtok (NULL, " ");
  			}
			if ((nwrite=write(fd, dir, strlen(dir)+1)) == -1)
				{ perror("Error in Writing"); exit(2); }
			close(fd);
		}
		if(!strcmp(oneword,"exit")){ //aplo exit
			printf("exiting...\n");
			return 0;
		}
		c = fscanf(stdin,"%s",oneword);
	}
	close(fd);
	return 0;
}
