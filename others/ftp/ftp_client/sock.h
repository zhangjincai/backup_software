#ifndef __SOCK_H
#define __SOCK_H

#define DBG(x)	printf(x)
#define	SERVER_CTRL_PORT	21
#define	CLIENT_CTRL_PORT	5000
#define	CLIENT_DATA_PORT	(CLIENT_CTRL_PORT+1)
#define BUF_SIZE			1024

extern char BufferReply[BUF_SIZE];
extern int SocketCmd;
extern int SocketData;
extern char *ServerIP;
extern int SERVER_DATA_PORT;
extern struct sockaddr_in ServerAddr_Cmd;
extern struct sockaddr_in ServerAddr_Data;
extern struct sockaddr_in ClientAddr_Cmd;
extern struct sockaddr_in ClientAddr_Data;

int ClientInit(void);
int ConnectCmdSocket(void);
int SendCmmand(char *cmd);
int SendData(char *buf, int len);
int ReceiveReply(char *buffer, int len);
int CloseSocket(void);

#endif
