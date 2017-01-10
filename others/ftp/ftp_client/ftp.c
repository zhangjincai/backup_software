#include <stdio.h>
#include <stdlib.h>
#include<unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "ftp.h"
#include "sock.h"

int Init_FTP(void)
{
	if(ClientInit()<0)
	{
		DBG("ClientInit Failed!\n");
		return -1;
	}
	if(ConnectCmdSocket()<0)
	{
		DBG("ConnectCmdSocket Failed!\n");
		return -1;
	}
	
	return 0;
}

int LogIn(char *user, char *password)
{
	char temp[20]={0};
	
	sprintf(temp, "USER %s\r\n", user);
	if(SendCmmand(temp)<0)
	{
		DBG(("SendCommand failed:%s", temp));
		return -1;
	}
	
	sprintf(temp, "PASS %s\r\n", password);
	if(SendCmmand(temp)<0)
	{
		DBG(("SendCommand failed:%s", temp));
		return -1;
	}
		
	return 0;
}

int LogOut(void)
{	
	char *temp="QUIT\r\n";
	
	if(SendCmmand(temp)<0)
	{
		DBG(("SendCommand failed:%s", temp));
		CloseSocket();
		return -1;
	}
	CloseSocket();
		
	return 0;
}

int GetPort(char *str)
{
	int port=0;
	char CommerCnt=0;
	char *p=str;
	int h=0;
	int l=0;
	
	while(CommerCnt!=4)
	{
		if(*p == ',')
			CommerCnt++;
		p++;
	}
	h = atoi(p);
	
	p++;p++;
	l = atoi(p);
	
	port = (h*256) + l;
	printf("server data port=%d\n", port);
	
	return port;
}

int SetDataPort(void)
{
	char *cmd="PASV\r\n";
	int len = strlen(cmd);
	
	if(write(SocketCmd, cmd, len)!=len)
	{
		DBG("SetDataPort:socket write failed!\n");
		return -1;
	}
	
	len=0;
	if( (len = read(SocketCmd, BufferReply, 100))<0 )
	{
		DBG("SetDataPort:socket read failed!\n");
		return -1;
	}
	printf("%s", BufferReply);
	SERVER_DATA_PORT = GetPort (BufferReply);
	bzero(BufferReply, BUF_SIZE);
	
	ServerAddr_Data.sin_family = AF_INET;
	ServerAddr_Data.sin_addr.s_addr = inet_addr(ServerIP);
	ServerAddr_Data.sin_port = htons(SERVER_DATA_PORT);
	
	return 0;
}

int PrePareForSendData(void)
{
	if(SendCmmand("TYPE I\r\n")<0)
	{
		DBG(("SendCommand failed:%s", "TYPE I"));
		return -1;
	}
	if(SetDataPort()<0)
	{
		DBG(("SetDataPort failed!\r\n"));
		return -1;
	}
	if( connect (SocketData, (struct sockaddr*)&ServerAddr_Data, sizeof(ServerAddr_Data))<0)
	{
		perror("connect");
		DBG("ConnectDataSocket:Connect Failed!\n");
		return -1;
	}
	
	return 0;
}

int FinishForSendData(void)
{
	char *temp="QUIT\r\n";

	if(close(SocketData)<0)
	{
		DBG("FinishForSendData:close SocketData failed.\n");
		close(SocketCmd);
		return -1;
	}
	ReceiveReply(BufferReply, BUF_SIZE);
	
	if(SendCmmand(temp)<0)
	{
		DBG(("SendCommand failed:%s", temp));
		close(SocketCmd);
		return -1;
	}
	close(SocketCmd);
	
	return 0;
}

int SendDataToFTP(char *name, char *pass, char *FileName, char *buf, int FileLen)
{
	char temp[128]={0};
	
	if(Init_FTP()<0)
	{
		return -1;
	}
	
	if(LogIn(name, pass)<0)
	{
		return -1;
	}
	if(PrePareForSendData()<0)
	{
		LogOut();
		return -1;
	}
	sprintf(temp, "APPE %s\r\n", FileName);
	if(SendCmmand(temp)<0)
	{
		DBG(("SendCommand failed:%s", temp));
		LogOut();
		return -1;
	}
	
	if(SendData (buf, FileLen)<0)
	{
		DBG("SendData failed.\n");
		LogOut();
		return -1;
	}
	
	FinishForSendData();
	
	return 0;
}

