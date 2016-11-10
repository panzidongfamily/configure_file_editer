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

static char   *cmd;
static struct Reg_Msg g_msg;
static struct Reg_Ret g_ret;
static int    g_msg_id;

static void Usage(enum DETAILS show);
static void version(char *cmd);
static void signal_handle(int num)
{
	msgctl(g_msg_id, IPC_RMID, NULL);
	exit(0);
}

int main(int argc, char *argv[])
{
	cmd = basename((char*)argv[0]);
	char ch;
	while(1)
	{
		ch = getopt(argc, argv, "hv");
		if(ch==-1)
			    break;
		switch(ch)
		{
			case 'h': Usage(YES);		break;
			case 'v': version(cmd);		break;
			default : Usage(YES);       break;
		}
	}

	daemon(0,0);

	signal(SIGHUP,  signal_handle);
	signal(SIGINT,  signal_handle);
	signal(SIGQUIT, signal_handle);
	signal(SIGTERM, signal_handle);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);

	key_t k_id=ftok("/", 100);
	if(k_id==-1)
		perror("ftok"),exit(1);

	g_msg_id = msgget(k_id, 0666 | IPC_CREAT | IPC_EXCL);  //保证使用的消息队列唯一

	if(g_msg_id == -1)	//用于regtool目地的消息队列已存在
	{
		g_msg_id = msgget(k_id, 0666); 		//获取旧队列
		msgctl(g_msg_id, IPC_RMID, 0);		//删除旧队列
		g_msg_id = msgget(k_id, 0666 | IPC_CREAT | IPC_EXCL);//创建新队列
		if(g_msg_id==-1)
			perror("msgget"),exit(2);
	}

	while(1)
	{
		memset(&g_msg, 0, sizeof(g_msg));
		memset(&g_ret, 0, sizeof(g_ret));

		if(msgrcv(g_msg_id, (void *)&g_msg, sizeof(struct Reg_Core), 0, 0)==-1)
			perror("msgrcv"),exit(1);

		g_ret.type=g_msg.type;

		GKeyFile* kf = g_key_file_new();
		g_key_file_load_from_file(kf, g_msg.core.path, G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS, NULL);

		if(g_msg.core.func==1)//regget
		{
			if(g_key_file_has_key(kf, g_msg.core.group, g_msg.core.key, NULL))
			{
				strncpy(g_ret.res.value, g_key_file_get_value(kf, g_msg.core.group, g_msg.core.key,NULL), VALUE_SIZE);
				g_ret.res.res=0;
			}
			else
				g_ret.res.res=1;
		}
		else if(g_msg.core.func==2)//regset
			g_key_file_set_value(kf, g_msg.core.group, g_msg.core.key, g_msg.core.value);
		else if(g_msg.core.func==3)//regdel
			g_key_file_remove_group(kf , g_msg.core.group, NULL);
		else if(g_msg.core.func==4)//regrm
			g_key_file_remove_key(kf, g_msg.core.group, g_msg.core.key, NULL);
		else
			g_ret.res.res=1;

		if(g_msg.core.func!=1)
		{
#if GLIB_VERSION_MAX_ALLOWED < GLIB_VERSION_2_40
			gsize size;
			gchar* content = g_key_file_to_data(kf, &size, &p_error);
			g_ret.res.res  = g_file_set_contents(g_msg.core.path, content, size, NULL);
#else
			g_ret.res.res = g_key_file_save_to_file(kf, g_msg.core.path, NULL);
#endif			
		}
		g_key_file_free(kf);
		sync();

		if(msgsnd(g_msg_id, (void *)&g_ret, sizeof(struct Reg_Res), 0)==-1)
			perror("msgsnd"),exit(1);

	}
	return 0;
}

void Usage(enum DETAILS show)
{
	if(show==YES)
	{
		fprintf(stderr ,"\nThis program is a server that to provide service to program \nwhat parses Key-value config files, ");
		fprintf(stderr, "And it's just a simple \nserver, don't use it directly!\n\n");
	}
	if(show==YES)
	{
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
	fprintf(stderr, "%s version %s\n", cmd, VERSION);
	exit(0);
}
