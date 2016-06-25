#ifndef _SERVER_H_
#define _SERVER_H_

struct node2{			//komvos listas gia kratima arxeiwn
	int id;
	char *filename;
	char *text;		//to swma tou arxeiou
	int size;
	struct node2 *next;
};

typedef struct node2 fileList;

struct node1{			//komvos listas gia kratima minimatwn
	int id;
	char *msg;
	struct node1 *next;
};

typedef struct node1 msgList;

struct node{			//kentrikos komvos listas tou server
	int id;
	char name[20];
	int filesNo;
	struct node1 *msgs;
	struct node2 *files;
	struct node *next;
};

typedef struct node serverNode;

serverNode *search(int id, serverNode *sn);
void unlinkFiles(serverNode *sn);

#endif
