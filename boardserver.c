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
#include "serveraux.h"

#define MSGSIZE 100

char *fifo1 = "S2C";
char *fifo2 = "C2S";
char *fifo3 = "B2S";
char *fifo4 = "S2B";


//ston server dhmiourgountai ola ta pipes, 2 gia ka8e epikoinwnia

serverNode *search(int id, serverNode *sn);

void unlinkFiles(serverNode *sn);

int main(int argc, char *argv[]){
	int fd1,fd2,fd3,fd4, createchannel,ch,getmsgs,sendm,sh,shuthelper,shcounter;
	char mydir[1000];
	serverNode *sn;
	sn=NULL;
	createchannel=0;
	getmsgs=0;
	if (getcwd(mydir, sizeof(mydir)) == NULL)
	       		exit(2);
	if ( mkfifo(fifo1, 0666) == -1 ){
		if ( errno!=EEXIST ) { perror("receiver: mkfifo"); exit(6); };
		}
	if ( mkfifo(fifo2, 0666) == -1 ){
		if ( errno!=EEXIST ) { perror("receiver: mkfifo"); exit(6); };
		}
	if ( (fd2=open(fifo2, O_RDWR)) < 0){
		perror("fifo open problem"); exit(3);	
		}
	if ( mkfifo(fifo3, 0666) == -1 ){
		if ( errno!=EEXIST ) { perror("receiver: mkfifo"); exit(6); };
		}
	if ( (fd3=open(fifo3, O_RDWR )) < 0){
		perror("fifo open problem"); exit(3);	
		}
	if ( mkfifo(fifo4, 0666) == -1 ){
		if ( errno!=EEXIST ) { perror("receiver: mkfifo"); exit(6); };
		}
	for(;;){
		fd_set fds;
  		int maxfd;
  		int res;
  		char msgbuf[256];
		FD_ZERO(&fds); //clear tou FD set gia tin select
		FD_SET(fd2, &fds);
		FD_SET(fd3, &fds);
		maxfd = fd2 > fd3 ? fd2 : fd3;
		select(maxfd + 1, &fds, NULL, NULL, NULL); //opou entopistei data flow, auto to kanali tha epilextei
		if (FD_ISSET(fd3, &fds)){ //board
			res = read(fd3, msgbuf, sizeof(msgbuf));
			if ( res < 0 ) {
				perror("3 problem in reading"); exit(5);
				}
			if(res>0){
				if(!strcmp(msgbuf,"createchannel")){ //to kanali dimiourgeitai leksi pros leksi
					createchannel = 1;
					ch = 0;
				}
				if(createchannel==1){
					int id;
					char name[20];
					if(ch==1)
						id=atoi(msgbuf);
					if(ch==2){
						strcpy(name,msgbuf);
						msgList *node = malloc(sizeof(msgList));
						node = NULL;
						fileList *fnode = malloc(sizeof(fileList));
						fnode = NULL;
						serverNode *newchannel = malloc(sizeof(serverNode));
						newchannel->msgs = node;
						newchannel->files = fnode;
						newchannel->id = id;
						newchannel->filesNo=0;
						strcpy(newchannel->name,name);
						newchannel->next = sn;
						sn = newchannel;
						createchannel =0;
					}
			
					ch++;
				}
				if(!strcmp(msgbuf,"display")){ //debugging helper!
					int nwrite;
					char test[500];
					char idC[10];
					test[0]=0;
					if ( (fd4=open(fifo4, O_WRONLY)) < 0){
						perror("fifo open problem"); exit(3);	
					}
					serverNode *p;
					p = sn;
					while(p!=NULL){
						idC[0]=0;
						//printf("id : %d  |  name : %s  \n",p->id,p->name);
						sprintf(idC, "%d", p->id);
						strcat(test,"id : ");
						strcat(test,idC);
						strcat(test," | name : ");
						strcat(test,p->name);
						p=p->next;
					}
					if ((nwrite=write(fd4, test, strlen(test)+1)) == -1)
						{ perror("Error in Writing"); exit(2); }
					close(fd4);
				}
				if(!strcmp(msgbuf,"getmessages")){
					getmsgs=1;
					ch=0;
				}
				if(getmsgs==1){			//stin getmessages epistrefoun ola ta minimata enos kanaliou
					char send[1000];	//kai episis ola ta filenames.Ta arxeia DEN typwnontai sto terminal
					int id;			//alla metaferontai sto background sto <path>
					if(ch==1){
						if ( (fd4=open(fifo4, O_WRONLY)) < 0){
							perror("fifo open problem"); exit(3);	
						}
						id = atoi(msgbuf);
						serverNode *tmp = search(id,sn);
						int nwrite;
						msgList *tmp1;
						fileList *tmp2;
						tmp2=tmp->files;
						tmp1=tmp->msgs;
						send[0]=0;
						while(tmp1!=NULL){
							strcat(send,tmp1->msg);
							tmp1=tmp1->next;
						}
						strcat(send,"\nFilenames: "); //ousiastika kataskevazw ENA string pou einai concatenate
						int files;			//apo ola ta filenames/messages tou kanaliou <id>
						files = 0;
						while(tmp2!=NULL){
							if(tmp->filesNo){
								strcat(send,tmp2->filename);
								strcat(send," ");
								tmp2=tmp2->next;
							}else files++;
						}
						strcat(send,"\n");	
						if ((nwrite=write(fd4, send, strlen(send)+1)) == -1)
								{ perror("Error in Writing"); exit(2); }
						close(fd4);
					}
					if(ch==2){ //dimiourgia(metafora-antigrafi) tou arxeiou pou eixe kanei send o xristis
						serverNode *tmp = search(id,sn);
						if(tmp->filesNo){
							char path[strlen(msgbuf)];
							strcpy(path,msgbuf);
							fileList *aux = tmp->files;
							int newfd, nwrite2;
							if((newfd = creat(aux->filename, S_IRWXU)) < 0){
	    							perror("creat error"); exit(5);
							}
							if ((nwrite2=write(newfd, aux->text, aux->size+1)) == -1)
								{ perror("Error in Writing"); exit(2); }
							close(newfd);
							getmsgs=0;
						}
					}
					ch++;
				}
				if(!strcmp(msgbuf,"shutdown")){ //diagrafi pipes kai free tou server node
					unlink(fifo1);
					unlink(fifo2);
					unlink(fifo3);
					unlink(fifo4);
					unlinkFiles(sn);
					free(sn);
					return 0;
				}
				fflush(stdout);
			}
		}
		if (FD_ISSET(fd2, &fds)){  //boardpost
			res = read(fd2, msgbuf, sizeof(msgbuf));
			if (  res < 0 ) {
				perror("2 problem in reading"); exit(5);
			}
			if(res>0){
				if(!strcmp(msgbuf,"send")){
					sendm=1;	
					sh=0;
				}
				if(sendm==1){ 			//antigrafi olou tou arxeiou se domi
					int id;
					char filename[20],path[100],filefd;
					if(sh==1) id=atoi(msgbuf);
					if(sh==2) strcpy(filename,msgbuf);
					if(sh==3){
						strcpy(path,msgbuf);
						char c;
						filename[strlen(filename)-1]=0;
						int count;
						count=0;
						chdir(path);
						if((filefd=open(filename,O_RDONLY,0))<0){
							perror("file reading problem"); exit(4);
						}
						while (read(filefd, &c, sizeof(char)) != 0) {
    							//printf("%c", c);
							count++;
  						}
						close(filefd);
						serverNode *newnode = search(id,sn);
						fileList *aux = malloc(sizeof(fileList));	
						aux->text = malloc(count*sizeof(char));
						aux->filename = malloc(strlen(filename)*sizeof(char));
						aux->id = id;
						aux->size = count;
						strcpy(aux->filename,filename);
						aux->text[0]=0;
						int i = 0;
						if((filefd=open(filename,O_RDONLY,0))<0){
							perror("file opening problem"); exit(4);
						}
						while (read(filefd, &c, sizeof(char)) != 0) {
    							aux->text[i] = c;
							i++;
  						}
						close(filefd);
						chdir(mydir);
						aux->next = newnode->files;
						newnode->files = aux;
						
					}
					sh++;
				}
				if(!strcmp(msgbuf,"display")){ //emfanisi listas dia8esimwn kanaliwn
					int nwrite;
					char test[500];
					char idC[10];
					test[0]=0;
					if ( (fd1=open(fifo1, O_WRONLY)) < 0){
						perror("fifo open problem"); exit(3);	
					}
					serverNode *p;
					p = sn;
					while(p!=NULL){
						idC[0]=0;
						sprintf(idC, "%d", p->id);
						strcat(test,"id : ");
						strcat(test,idC);
						strcat(test," | name : ");
						strcat(test,p->name);
						p=p->next;
					}
					if ((nwrite=write(fd1, test, strlen(test)+1)) == -1)
						{ perror("Error in Writing"); exit(2); }
					close(fd1);
				}
				if(msgbuf[0]=='W' && msgbuf[1]=='/'){ //prwtokollo W/ gia write metaksy tous
					char *ind,*neo,msgnew[strlen(msgbuf)],final[strlen(msgbuf)];
					int i,id;
					final[0]=0;
					msgnew[0]=0;
					strcpy(msgnew,msgbuf);
					ind = strtok(msgbuf," ");
					i=0;
					while(ind!=NULL){
						if(i!=0){
							strcat(final,ind);	//xwrizei to id giati to minima erxetai me ti morfi:
							strcat(final," ");	// W/id message
						}
						ind=strtok(NULL," ");
						i++;
					}
					neo = strtok(msgnew,"/");
					id=atoi(strtok(NULL,msgnew));
					serverNode *newnode;
					newnode=search(id,sn);
					msgList *aux = malloc(sizeof(msgList));	
					aux->msg = malloc(20*sizeof(char));
					strcpy(aux->msg,final);
					aux->next = newnode->msgs;
					newnode->msgs = aux;
				}
			}
		}
	}
}

serverNode *search(int id, serverNode *sn) //apli search function
{
	serverNode *p;
	p=sn;
	while(p!=NULL){
		if(p->id==id) return p;
		p=p->next;
	}
	return NULL;
}

void unlinkFiles(serverNode *sn)	//voithitiki synartisi gia unlinking gia to shutdown
{
	serverNode *tmp;
	fileList *tmp1;
	tmp=sn;
	tmp1=tmp->files;
	while(tmp!=NULL){
		tmp1=tmp->files;
		while(tmp1!=NULL){
			unlink(tmp1->filename);
			tmp1=tmp1->next;
		}
		tmp1=tmp1->next;
	}
}
