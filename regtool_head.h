#ifndef __REGTOOL_HEAD_H_
#define __REGTOOL_HEAD_H_

enum DETAILS{YES, NO};

#define VERSION 	"2.0.0"
#define PATH_SIZE 	1024
#define GROUP_SIZE 	64
#define KEY_SIZE 	64
#define VALUE_SIZE  512

typedef struct Reg_Core
{
	int  func;				//功能码，1:regget,2:regset,3:regdel,4:regrm
	char path[PATH_SIZE];	//文件路径大小
	char group[GROUP_SIZE];	//组名称大小
	char key[KEY_SIZE];		//键名称大小
	char value[VALUE_SIZE];	//值大小
}Reg_Core;

typedef struct Reg_Res
{
	int res;				//执行结果，0成功，1失败
	char value[VALUE_SIZE];	//返回的结果值
}Reg_Res;



typedef struct Reg_Msg
{
	long int type;			
	struct Reg_Core core;	
}Reg_Msg;					//客户端发送给服务器的消息,服务器接收


typedef struct Reg_Ret
{
	long int type;
	struct Reg_Res res;
}Reg_Ret;					//服务器返回给客户端的消息,客户端接收



#endif//__REGTOOL_HEAD_H_
