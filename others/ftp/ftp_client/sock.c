#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "sock.h"

char BufferReply[BUF_SIZE]={0};
int SocketCmd=0;
int SocketData=0;
//char *ServerIP="10.13.0.21";
char *ServerIP="127.0.0.1";


int SERVER_DATA_PORT=0;
struct sockaddr_in ServerAddr_Cmd;
struct sockaddr_in ServerAddr_Data;
struct sockaddr_in ClientAddr_Cmd;
struct sockaddr_in ClientAddr_Data;

int ClientInit(void)
{
	//set server addr
	ServerAddr_Cmd.sin_family = AF_INET;
	ServerAddr_Cmd.sin_addr.s_addr = inet_addr(ServerIP);
	ServerAddr_Cmd.sin_port = htons(SERVER_CTRL_PORT);

	//create new socket
	if((SocketCmd = socket(AF_INET, SOCK_STREAM, 0)) ==0 )
	{
		DBG("Create SockedCmd Failed!\n");
		return -1;
	}
	DBG("Create socket SocketCmd.\n");
	if((SocketData = socket(AF_INET, SOCK_STREAM, 0)) ==0 )
	{
		DBG("Create SockedData Failed!\n");
		return -1;
	}
	DBG("Create socket SocketData.\n");

	//set client addr
	ClientAddr_Cmd.sin_family = AF_INET;
	ClientAddr_Cmd.sin_addr.s_addr = htonl(INADDR_ANY);
	ClientAddr_Cmd.sin_port = htons(CLIENT_CTRL_PORT);

	ClientAddr_Data.sin_family = AF_INET;
	ClientAddr_Data.sin_addr.s_addr = htonl(INADDR_ANY);
	ClientAddr_Data.sin_port = htons(CLIENT_DATA_PORT);

	//设置端口为可重复连接
	int on=1;
	if((setsockopt(SocketCmd, SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)))<0)  
	{  
		DBG("setsockopt:set sock opt failed!\n");
		return -1;
	}
	if((setsockopt(SocketData, SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)))<0)  
	{  
		DBG("setsockopt:set sock opt failed!\n");
		return -1;
	}

	if(bind(SocketData, (struct sockaddr*)&ClientAddr_Data, sizeof(ClientAddr_Data))<0)
	{
		DBG("bind SockedData Failed!\n");
		return -1;
	}
	//bind client addr
	if(bind(SocketCmd, (struct sockaddr*)&ClientAddr_Cmd, sizeof(ClientAddr_Cmd))<0)
	{
		DBG("bind SockedCmd Failed!\n");
		return -1;
	}

	return 0;
}

int ConnectCmdSocket(void)
{
	if( connect (SocketCmd, (struct sockaddr*)&ServerAddr_Cmd, sizeof(ServerAddr_Cmd))<0)
	{
		DBG("ConnectCmdSocket:Connect Failed!\n");
		CloseSocket();
		return -1;
	}
	ReceiveReply(BufferReply, BUF_SIZE);
	
	return 0;
}

int SendCmmand(char *cmd)
{
	if(cmd == NULL)
		return -1;

	int len = strlen(cmd);
	if(write(SocketCmd, cmd, len)!=len)
	{
		DBG("SendCmmand:socket write failed!\n");
		return -1;
	}
	ReceiveReply(BufferReply, BUF_SIZE);

	return 0;
}

int SendData(char *buf, int len)
{
	if(buf == NULL)
		return -1;

	if(write(SocketData, buf, len)!=len)
	{
		DBG("SendData:socket write failed!\n");
		return -1;
	}

	return 0;
}

int ReceiveReply(char *buf, int len)
{
	int length=0;

	if( (length = read(SocketCmd, buf, len))<0 )
	{
		DBG("ReceiveReply:read failed!\n");
		return -1;
	}
	printf("%s", buf);
	bzero (buf, BUF_SIZE);

	return length;
}

int CloseSocket(void)
{
	if(close(SocketCmd)<0)
	{
		DBG("CloseSocket:close SocketData failed!\n");
		return -1;
	}
	DBG("Close socket SocketCmd.\n");
		
	if(close(SocketData)<0)
	{
		DBG("CloseSocket:close SocketData failed!\n");
		return -1;
	}
	DBG("Close socket SocketData.\n");
	
	return 0;
}






















