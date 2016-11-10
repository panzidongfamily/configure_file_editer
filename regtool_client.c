#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <libgen.h>

#include "regtool_head.h"
#define APP_NAME(cmd) basename(realpath(cmd, NULL))

static int  functions(char *filename, char *cmd, char *group, char *key, char *value);
static void Usage(enum DETAILS show);
static void version(char *cmd);

static char *filename;
static char *group;
static char *key;
static char *value;
static char *cmd;


//printf("%s:%d\n", __func__, __LINE__);
int main(int argc, char *argv[])
{
	int num_args=0, ret;
	cmd = basename((char*)argv[0]);

	char ch;
	while(1)
	{
		ch = getopt(argc, argv, "f:hv");
		if(ch==-1)
			    break;

		switch(ch)
		{
			case 'f': filename=optarg; 	break;
			case 'h': Usage(YES);		break;
			case 'v': version(cmd);		break;
			default : Usage(NO);        break;
		}
	}

	num_args = argc - optind;
	if (num_args > 0) 
	    group = argv[optind++];
	if (num_args > 1)
	    key = argv[optind++];
	if (num_args > 2)
	    value = argv[optind++];
	ret = functions(filename, cmd, group, key, value);
	if(ret==-1)
		Usage(NO);
	else if(ret==1)
		Usage(YES);

	return 0;
}

int functions(char *filename, char *cmd, char *group, char *key, char *value)
{
	int func, arc;
	pid_t pid;
	Reg_Msg msg;
	Reg_Ret ret;

	if(strcmp(cmd, "regget") == 0)
		func=1,arc=3;
	else if(strcmp(cmd, "regset") == 0)
		func=2,arc=4;
	else if(strcmp(cmd, "regdel") == 0)
		func=3,arc=2;
	else if(strcmp(cmd, "regrm") == 0)
		func=4,arc=3;
	else
		return 1;

	if(filename==NULL || group==NULL)
		return -1;

	pid=getpid();

	msg.type=pid;
	msg.core.func=func;
	strncpy(msg.core.path, realpath(filename, NULL), PATH_SIZE);
	strncpy(msg.core.group, group, GROUP_SIZE);
	if((arc-2) > 0 )
	{	
		if(key!=NULL)
			strncpy(msg.core.key, key, KEY_SIZE);
		else
			return -1;
	}
	if((arc-3) > 0)
	{	
		if(value!=NULL)
			strncpy(msg.core.value, value, VALUE_SIZE);
		else
			return -1;
	}
	ret.type=pid;

	if(!access(filename, F_OK))
	{
		if((access(filename, R_OK)==0) && (access(filename, W_OK)==0))
		{
			key_t k_id=ftok("/", 100);
			if(k_id==-1)
				perror("ftok"),exit(1);

			int msg_id=msgget(k_id, 0666); //客户端不能创建消息队列
			if(msg_id==-1)
				perror("msgget"),fprintf(stderr,"The server isn't running now.\n"),exit(1);

			if(msgsnd(msg_id, (void *)&msg, sizeof(struct Reg_Core), 0)==-1)
				perror("msgsnd"),exit(1);

			if(msgrcv(msg_id, (void *)&ret, sizeof(struct Reg_Res), pid, 0)==-1)
				perror("msgrcv"),exit(1);
			if(func==1)//只有regget需要输出结果
			{
				if(!ret.res.res)//执行成功
					printf("%s\n",ret.res.value),exit(0);
				else
					exit(1);	//执行失败
			}
		}
		else
			printf("Do You Have Permission To Read Or Write The File?\n"),exit(1);
	}
	else
		fprintf(stderr, "File <%s> Not Exist!", filename),exit(1);

	return 0;
}

void Usage(enum DETAILS show)
{
	if(show==YES)
	{
		fprintf(stderr ,"\nThis program parses Key-value config files, ");
		fprintf(stderr, "But it's just a \nsimple read-write files located on your local system, And it \nalso need a server to finish it's works, if you want the \nprofile to take effect, please let the program that uses the \nconfiguration file reads the configuration file again.\n");
		fprintf(stderr, "And you should use this program with it's links:\n");
		fprintf(stderr, "\tregget --> %s\n", APP_NAME(cmd));
		fprintf(stderr, "\tregset --> %s\n", APP_NAME(cmd));
		fprintf(stderr, "\tregdel --> %s\n", APP_NAME(cmd));
		fprintf(stderr, "\tregrm  --> %s\n", APP_NAME(cmd));
		fprintf(stderr, "Dont't use '%s' directly!\n\n", APP_NAME(cmd));
	}
	if(strcmp(cmd, "regset") == 0)
		fprintf(stderr, "Usage: %s [-f <filename>] <group> <key> <value>\n", cmd);
	else if(strcmp(cmd, "regget") == 0)
		fprintf(stderr, "Usage: %s [-f <filename>] <group> <key>\n", cmd);
	else if(strcmp(cmd, "regdel") == 0)
		fprintf(stderr, "Usage: %s [-f <filename>] <group>\n", cmd);
	else if(strcmp(cmd, "regrm") == 0)
		fprintf(stderr, "Usage: %s [-f <filename>] <group> <key>\n", cmd);
	else
		fprintf(stderr, "Usage: %s [-f <filename>] <group> <key> <value>\n", "reg<tools>");
	if(show==YES)
	{
		fprintf(stderr ,"\n-f\t-file\t\tSpecify the file name\n");
		fprintf(stderr ,"-v\t-version\tShow version of this program\n");
		fprintf(stderr ,"-h\t-help\t\tPrint this help text\n\n");
		fprintf(stderr, "Copyright (C) 2015 www.cloud-times.com \tAll rights reserved.\n\n");
	}
	if(show==YES)
		exit(0);
	else if(show==NO)
		exit(1);
}

void version(char *cmd)
{
	int name=0;

	if(strcmp(cmd, "regget") == 0)
		name=1;
	else if(strcmp(cmd, "regset") == 0)
		name=1;
	else if(strcmp(cmd, "regdel") == 0)
		name=1;
	else if(strcmp(cmd, "regrm") == 0)
		name=1;
	else
		name=0;

	fprintf(stderr, "%s version %s\n", name?cmd:APP_NAME(cmd), VERSION);
	exit(0);
}
