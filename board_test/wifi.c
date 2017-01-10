#ifndef __WIFI_C__
#define __WIFI_C__

#include "include/lib_wifi.h"

int  invalid_ipaddr (char *str)  
{  
   if (str == NULL || *str == '\0')  
      return WIFI_ERROR;  
  
   union  
   {  
      struct sockaddr addr;  
      struct sockaddr_in6 addr6;  
      struct sockaddr_in addr4;  
   } a;  
   memset (&a, 0, sizeof (a));  
   if (1 == inet_pton (AF_INET, str, &a.addr4.sin_addr))  
      return WIFI_EOK;  
   else if (1 == inet_pton (AF_INET6, str, &a.addr6.sin6_addr))  
      return WIFI_EOK;  
   return WIFI_ERROR;  
}

void __wifi_resease(lib_serial_t *serial)
{
	unsigned char rxbuf[1024] = {0};
	// 进入透明传输模式 *******************
	__sendAT(serial->sfd, "AT+ENTM\r",rxbuf);

	lib_serial_close(serial);
}

int __sendAT(int fd, const unsigned char *cmd,unsigned char *rxbuf)
{
	if(fd == 0)
		return WIFI_ERROR;

	if(cmd == NULL)
		return WIFI_ERROR;
	
	int __ret = -1;
	__ret = lib_serial_send(fd, cmd, strlen(cmd));
	if(__ret<0)
	{
		return WIFI_ERROR;
	}
	
	// 第三次接受! 
	memset(rxbuf,'\0',sizeof(rxbuf));
	__ret = lib_serial_readn_select(fd, rxbuf, 1024, 1500);
	if(__ret<0)
	{
		return __ret;
	}else if(__ret == 0){
		printf("Not Data!\n");
		return WIFI_EEMPTY;
	}

	return WIFI_EOK;
}

int __wifi_init(lib_serial_t *serial)
{
	int __ret = -1;
	
	// init the serial
	__ret = lib_serial_init(serial);
	if(__ret<0)
	{
	  printf("************lib_serial_init************\n");
	  return WIFI_ERROR;
	}

	// 初始化AT模式
	__ret  = __init_AT_mode(serial->sfd);
	if(__ret <0)
	{
		printf("Init_AT_Mode error!\n");
		return WIFI_ERROR;
	}

	return WIFI_EOK;
}


int __init_AT_mode(int fd)
{
	int __ret = -1;
	unsigned char txbuf[1024] = {0};
	unsigned char rxbuf[1024] = {0};
	
	memset(txbuf,'\0',sizeof(txbuf));
	memset(rxbuf,'\0',sizeof(rxbuf));

	txbuf[0] = 0x2b;
	__ret = lib_serial_send(fd, txbuf, 1);
	if(__ret<0)
	{
		printf("send1 +************lib_serial_write_select************\n");
		return WIFI_ERROR;
	}else{
		printf("Send1 :%s\n",txbuf);
	}
	msleep(50);

	__ret = lib_serial_send(fd, txbuf, 1);
	if(__ret<0)
	{
		printf("send2 +************lib_serial_write_select************\n");
		return WIFI_ERROR;
	}else{
		printf("Send2 :%s\n",txbuf);
	}
	msleep(50);

	__ret = lib_serial_send(fd, txbuf, 1);	  
	if(__ret<0)
	{
		printf("send3 +************lib_serial_write_select************\n");
		return WIFI_ERROR;
	}else{
		printf("Send3 :%s\n",txbuf);
	}

	// 第一次接受!  等待A
	memset(rxbuf,'\0',sizeof(rxbuf));
	__ret = lib_serial_read_select(fd,rxbuf, sizeof(rxbuf), 1500);
	if(__ret<0)
	{
		printf("read the data fail!\n");
		return WIFI_ETIMEOUT;
	}else if(rxbuf[0]!=97){
		printf("read the data != a\n  ret = %d  rxbuf = %s\n", __ret, rxbuf);
		return WIFI_ERROR;
	}else{
		// 有应答信号则再发送A回复
		printf("recv :%s\n",rxbuf);
	
		 msleep(500);
		 txbuf[0] = 'a';
		 __ret = lib_serial_send(fd, txbuf, 1);
		 if(__ret<0)
		 {
			printf("sendA************lib_serial_write_select************\n");
			return WIFI_ERROR;
		 }else{
		 	printf("Send a :%s\n",txbuf);
		 }

		// 等待+ok
		memset(rxbuf,'\0',sizeof(rxbuf));
		  __ret = lib_serial_readn_select(fd, rxbuf, 1024, 1000);
		  if(__ret<0)
		  {
			printf("************lib_serial_read_select************\n");
			return WIFI_ETIMEOUT;
		  }
		  printf("recv  :%s\n",rxbuf);
	}
	return WIFI_EOK;
}

int wifi_get_lann(int fd, struct wifi_lann *lann)
{
	int __ret = -1;
	char seps[] = "=,\r";
	
	if(lann == NULL)
		return WIFI_ERROR;
	
	unsigned char buf[1024] = {0};
	__ret = __sendAT(fd, "AT+LANN\r",buf);
	if(__ret<0){
		printf("send AT+LANN error!\n");
		return WIFI_ERROR;
	}

	unsigned char *p;

	p = strtok(buf,seps);
	p = strtok(NULL, seps);

	if(p)
	{
		p=strtok(NULL, seps);
	}else{
		return WIFI_ERROR;
	}

	if(p)
	{
		strcpy(lann->lan_ip,p);
		p=strtok(NULL, seps);
	}else{
		return WIFI_ERROR;
	}
	
	if(p)
	{
		strcpy(lann->lan_mask,p);
		p=strtok(NULL, seps);
	}else{
		return WIFI_ERROR;
	}

	return WIFI_EOK;
}

int wifi_set_lann(int fd, struct wifi_lann *lann)
{
	printf("********wifi_set_lann*********\n");
	if(fd == 0)
		return WIFI_ERROR;
	
	int __ret = -1;
	unsigned char rxbuf[1024] = {0};
	unsigned char str[] = "AT+LANN=";

	__ret = invalid_ipaddr(lann->lan_ip);
	if(__ret<0){
		printf("input lann->lan_ip data error! lann->lan_ip = %s\n",lann->lan_ip);
		return WIFI_ERROR;
	}
	__ret = invalid_ipaddr(lann->lan_mask);
	if(__ret<0){
		printf("input lann->lan_mask data error! lann->lan_mask = %s\n",lann->lan_mask);
		return WIFI_ERROR;
	}
	
	strcat(str,lann->lan_ip);
	strcat(str,",");
	strcat(str,lann->lan_mask);
	strcat(str,"\r");
	
	__ret = __sendAT(fd, str,rxbuf);
	if(__ret<0){
		printf("wifi_set_lann error!\n");
		return WIFI_ERROR;
	}
	
	return 0;
}


int Showinfo_lann(struct wifi_lann *lann)
{
	if(lann == NULL)
		return WIFI_ERROR;
	
	printf("lann->lan_ip = %s\n",lann->lan_ip);
	printf("lann->lan_mask = %s\n",lann->lan_mask);	
	
	return WIFI_EOK;
}

int wifi_get_netp(int fd, struct wifi_netp *netp)
{	
	char seps[] = "=,\r";
	
	if(netp == NULL)
		return WIFI_ERROR;

	int __ret = -1;
	char buf[1024] = {0};	
	__ret = __sendAT(fd, "AT+NETP\r",buf);
	if(__ret<0){
		printf("send AT+NETP error!\n");
		return WIFI_ERROR;
	}
	
	unsigned char *p;

	p = strtok(buf,seps);
	p = strtok(NULL, seps);

	if(p)
	{
		p=strtok(NULL, seps);
	}else{
		return WIFI_ERROR;
	}
	
	if(p)
	{
		strcpy(netp->protocol,p);
		p=strtok(NULL, seps);
	}else{
		return WIFI_ERROR;
	}

	if(p)
	{
		strcpy(netp->type,p);
		p=strtok(NULL, seps);
	}else{
		return WIFI_ERROR;
	}
	
	if(p)
	{
		netp->port=atoi(p);
		p=strtok(NULL, seps);
	}else{
		return WIFI_ERROR;
	}
	
	if(p)
	{
		strcpy(netp->ipaddr,p);
	}else{
		return WIFI_ERROR;
	}

	return WIFI_EOK;
}

int wifi_set_netp(int fd, struct wifi_netp *netp)
{
	printf("********wifi_set_netp*********\n");
	if(netp == NULL)
		return WIFI_ERROR;

	int __ret = -1;
	unsigned char port[10];
	unsigned char rxbuf[1024];
	
	if(strcmp(netp->protocol,"TCP") && strcmp(netp->protocol,"UDP"))
	{
		printf("input netp->protocol data error! netp->protocol = %s\n",netp->protocol);
		return WIFI_ERROR;
	}

	if(strcmp(netp->type,"SERVER") && strcmp(netp->protocol,"CLIENT"))
	{
		printf("input netp->type data error! netp->type = %s\n",netp->type);
		return WIFI_ERROR;
	}

	if(netp->port>32767 || netp->port<100)
	{
		printf("input netp->port data error!\n");
		return WIFI_ERROR;
	}

	__ret = invalid_ipaddr(netp->ipaddr);
	if(__ret<0){
		printf("input netp->ipaddr data error! netp->ipaddr = %s\n",netp->ipaddr);
		return WIFI_ERROR;
	}
	
	sprintf(port,"%d",netp->port);
		
	unsigned char str[] = "AT+NETP=";
	strcat(str,netp->protocol);
	strcat(str,",");
	strcat(str,netp->type);
	strcat(str,",");
	strcat(str,port);
	strcat(str,",");
	strcat(str,netp->ipaddr);
	strcat(str,"\r");

	__ret = __sendAT(fd, str,rxbuf);
	if(__ret<0){
		printf("wifi_set_netp error!\n");
		return WIFI_ERROR;
	}

	return WIFI_EOK;
}

int Showinfo_netp(struct wifi_netp *netp)
{
	if(netp == NULL)
		return WIFI_ERROR;
	
	printf("netp->pro = %s\n",netp->protocol);
	printf("netp->type = %s\n",netp->type);
	printf("netp->port = %d\n",netp->port);
	printf("netp->ipaddr = %s\n",netp->ipaddr);

	return WIFI_EOK;
}

int wifi_get_wakey(int fd, struct wifi_wakey *wakey)
{	
	char seps[] = "=,\r";
	
	if(wakey == NULL)
		return WIFI_ERROR;
	
	char buf[1024] = {0};
	int __ret = -1;
	__ret = __sendAT(fd, "AT+WAKEY\r",buf);
	if(__ret<0){
		printf("send AT+WAKEY error!\n");
		return WIFI_ERROR;
	}
	unsigned char *p;

	p = strtok(buf,seps);
	p = strtok(NULL, seps);

	if(p)
	{
		p=strtok(NULL, seps);
	}else{
		return WIFI_ERROR;
	}
	
	if(p)
	{
		strcpy(wakey->auth,p);
		p=strtok(NULL, seps);
	}else{
		return WIFI_ERROR;
	}

	if(p)
	{
		strcpy(wakey->encry,p);
		p=strtok(NULL, seps);
	}else{
		return WIFI_ERROR;
	}
	
	if(p)
	{
		strcpy(wakey->key,p);
	}else{
		return WIFI_ERROR;
	}

	return WIFI_EOK;
}

int  wifi_set_wakey(int fd, struct wifi_wakey *wakey)
{	
	printf("********wifi_set_wakey*********\n");
	if(wakey == NULL)
		return WIFI_ERROR;

	int __ret = -1;
	unsigned char rxbuf[1024] = {0};
	unsigned char str[] = "AT+WAKEY=";

	if(strcmp(wakey->auth,"OPEN") && strcmp(wakey->auth,"SHARED") &&
				strcmp(wakey->auth,"WPAPSK"))
	{
		printf("input wakey->auth data error! wakey->auth = %s\n",wakey->auth);
		return WIFI_ERROR;
	}

	if(strcmp(wakey->encry,"NONE") && strcmp(wakey->encry,"WEP-H") &&
				strcmp(wakey->encry,"WEP-A") && strcmp(wakey->encry,"TKIP") && 
				strcmp(wakey->encry,"AES") && strcmp(wakey->encry,"TKIPAES"))
	{
		printf("input wakey->encry data error! wakey->encry = %s\n",wakey->encry);
		return WIFI_ERROR;
	}

	strcat(str,wakey->auth);
	strcat(str,",");
	strcat(str,wakey->encry);
	if(strcmp(wakey->auth,"OPEN")){
		strcat(str,",");
		strcat(str,wakey->key);
	}
	strcat(str,"\r");
	
	__ret = __sendAT(fd, str,rxbuf);
	if(__ret<0){
		printf("wifi_set_wakey error!\n");
		return WIFI_ERROR;
	}

	return WIFI_EOK;
}


int Showinfo_wakey(struct wifi_wakey *wakey)
{
	if(wakey == NULL)
		return WIFI_ERROR;
	
	printf("wakey->auth = %s\n",wakey->auth);
	printf("wakey->encry = %s\n",wakey->encry);	
	printf("wakey->key = %s",wakey->key);
	if(strcmp(wakey->key,"\n"))
		printf("\n");
	
	return WIFI_EOK;
}

int wifi_get_wap(int fd, struct wifi_wap *wap)
{

	char seps[] = "=,\r";
	
	if(wap == NULL)
		return WIFI_ERROR;
	
	char buf[1024] = {0};
	int __ret = -1;
	__ret = __sendAT(fd, "AT+WAP\r",buf);
	if(__ret<0){
		printf("send AT+WAP error!\n");
		return WIFI_ERROR;
	}
	unsigned char *p;

	p = strtok(buf,seps);
	p = strtok(NULL, seps);

	if(p)
	{
		p=strtok(NULL, seps);
	}else{
		return WIFI_ERROR;
	}
	
	if(p)
	{
		strcpy(wap->wifi_mode,p);
		p=strtok(NULL, seps);
	}else{
		return WIFI_ERROR;
	}

	if(p)
	{
		strcpy(wap->ssid,p);
		p=strtok(NULL, seps);
	}else{
		return WIFI_ERROR;
	}
	
	if(p)
	{
		strcpy(wap->channel,p);
	}else{
		return WIFI_ERROR;
	}
	
	return WIFI_EOK;	
}

int  wifi_set_wap(int fd, struct wifi_wap *wap)
{	
	printf("********wifi_set_wap*********\n");
	if(wap == NULL)
		return WIFI_ERROR;
	
	int __ret = -1;
	unsigned char rxbuf[1024] = {0};
	unsigned char str[] = "AT+WAP=";

	if(strcmp(wap->wifi_mode,"11BG") && strcmp(wap->wifi_mode,"11B") &&
		strcmp(wap->wifi_mode,"11G") && strcmp(wap->wifi_mode,"11BGN") &&
		strcmp(wap->wifi_mode,"11N"))
	{
		printf("input wap->wifi_mode data error! wap->wifi_mode = %s\n",wap->wifi_mode);
		return WIFI_ERROR;
	}

	if( strcmp(wap->channel,"CH1") && strcmp(wap->channel,"AUTO") && 
		strcmp(wap->channel,"CH2") && strcmp(wap->channel,"CH3") &&
		strcmp(wap->channel,"CH4") && strcmp(wap->channel,"CH5") &&
		strcmp(wap->channel,"CH6") && strcmp(wap->channel,"CH7") &&
		strcmp(wap->channel,"CH8") && strcmp(wap->channel,"CH9") &&
		strcmp(wap->channel,"CH10") && strcmp(wap->channel,"CH11"))
	{
		printf("input wap->channel data error! wap->channel = %s\n",wap->channel);
		return WIFI_ERROR;
	}
	
	strcat(str,wap->wifi_mode);
	strcat(str,",");
	strcat(str,wap->ssid);
	strcat(str,",");
	strcat(str,wap->channel);
	strcat(str,"\r");
	
	__ret = __sendAT(fd, str,rxbuf);
	if(__ret<0){
		printf("wifi_set_wap error!\n");
		return WIFI_ERROR;
	}
	
	return WIFI_EOK;
}

int Showinfo_wap(struct wifi_wap *wap)
{
	if(wap == NULL)
		return WIFI_ERROR;
	
	printf("wap->wifi_mode = %s\n",wap->wifi_mode);
	printf("wap->ssid = %s\n",wap->ssid);
	printf("wap->channel = %s\n",wap->channel);
	
	return WIFI_EOK;
}

int wifi_get_wmode(int fd, struct wifi_wmode *wmode)
{	
	char seps[] = "=,\r";
	
	if(wmode == NULL)
		return WIFI_ERROR;
	
	char buf[1024] = {0};
	int __ret = -1;
	__ret = __sendAT(fd, "AT+WMODE\r",buf);
	if(__ret<0){
		printf("__sendAT AT+WMODE error!\n");
		return WIFI_ERROR;
	}
	
	unsigned char *p;

	p = strtok(buf,"=\n");
	p = strtok(NULL, "=\n");

	if(p)
	{
		p=strtok(NULL, seps);
	}else{
		return WIFI_ERROR;
	}

	if(p)
	{
		strcpy(wmode->wmode,p);
	}else{
		return WIFI_ERROR;
	}

	return WIFI_EOK;
}

int  wifi_set_wmode(int fd, struct wifi_wmode *wmode)
{	
	printf("********wifi_set_wmode*********\n");
	if(wmode == NULL)
		return WIFI_ERROR;

	if(strcasecmp(wmode->wmode,"STA") && strcasecmp(wmode->wmode,"AP"))
	{
		printf("input wmode->wmode data error! wmode->wmode = %s\n",wmode->wmode);
		return WIFI_ERROR;
	}

	int __ret = -1;
	unsigned char rxbuf[1024] = {0};	
	unsigned char str[] = "AT+WMODE=";
	
	strcat(str,wmode->wmode);
	strcat(str,"\r");
	
	__ret = __sendAT(fd, str,rxbuf);
	if(__ret<0){
		printf("wifi_set_wmode error!\n");
		return WIFI_ERROR;
	}
	
	return WIFI_EOK;
}

int Showinfo_wmode(struct wifi_wmode *wmode)
{
	if(wmode == NULL)
		return WIFI_ERROR;
	
	printf("wmode->wmode = %s\n",wmode->wmode);
	
	return WIFI_EOK;
}


#if 1


int wifi_get_ip(int fd, unsigned char *ip)
{
	if(fd == -1)
		return WIFI_ERROR;
	
	int __ret = -1;
	struct wifi_lann lann;
		
	__ret = wifi_get_lann(fd, &lann);
	if(__ret<0){
		printf("wifi_get_lann error!\n");
		return WIFI_ERROR;
	}

	strcpy(ip,lann.lan_ip);
	
	return WIFI_EOK;
}

int wifi_set_ip(int fd, unsigned char *ip)
{
	if(fd == -1)
			return WIFI_ERROR;

	int __ret = -1;

	struct wifi_lann lann;
	strcpy(lann.lan_ip,ip);
	strcpy(lann.lan_mask,"255.255.255.0");

	__ret = wifi_set_lann(fd, &lann);
	if(__ret<0){
		printf("wifi_set_lann error!\n");
		return WIFI_ERROR;
	}
	
	return WIFI_EOK;
}

int wifi_get_port(int fd, unsigned short *port)
{
	if(fd == -1)
			return WIFI_ERROR;

	int __ret = -1;
	struct wifi_netp netp;
		
	__ret = wifi_get_netp(fd, &netp);
	if(__ret<0){
		printf("wifi_get_netp error!\n");
		return WIFI_ERROR;
	}

	*port = netp.port;
	
	return WIFI_EOK;
}

int wifi_set_port(int fd, unsigned short port)
{
	if(fd == -1)
			return WIFI_ERROR;

	int __ret = -1;
	struct wifi_netp netp;
	
	strcpy(netp.protocol,"TCP");
	strcpy(netp.type,"SERVER");
	netp.port=port;
	strcpy(netp.ipaddr,"10.10.100.100");

	__ret = wifi_set_netp(fd, &netp);
	if(__ret<0){
		printf("wifi_set_netp error!\n");
		return WIFI_ERROR;
	}
	
	return WIFI_EOK;
}

int wifi_get_key(int fd, unsigned char *key)	
{
	if(fd == -1)
			return WIFI_ERROR;

	int __ret = -1;
	struct wifi_wakey wakey;
		
	__ret = wifi_get_wakey(fd, &wakey);
	if(__ret<0){
		printf("wifi_get_wakey error!\n");
		return WIFI_ERROR;
	}

	strcpy(key,wakey.key);
	
	return WIFI_EOK;
}

int wifi_set_key(int fd, unsigned char *key)
{
	if(fd == -1)
			return WIFI_ERROR;

	int __ret = -1;

	struct wifi_wakey wakey;
	strcpy(wakey.auth,"WPAPSK");
	strcpy(wakey.encry,"TKIPAES");
	strcpy(wakey.key,key);

	__ret = wifi_set_wakey(fd, &wakey);
	if(__ret<0){
		printf("wifi_set_wakey error!\n");
		return WIFI_ERROR;
	}
	
	return WIFI_EOK;
}

int wifi_get_ssid(int fd, unsigned char *ssid)
{
	if(fd == -1)
			return WIFI_ERROR;

	int __ret = -1;
	struct wifi_wap wap;
		
	__ret = wifi_get_wap(fd, &wap);
	if(__ret<0){
		printf("wifi_get_wap error!\n");
		return WIFI_ERROR;
	}

	strcpy(ssid,wap.ssid);
	
	return WIFI_EOK;
}

int wifi_set_ssid(int fd, unsigned char *ssid)
{
	if(fd == -1)
			return WIFI_ERROR;

	int __ret = -1;

	struct wifi_wap wap;
	strcpy(wap.wifi_mode,"11BGN");
	strcpy(wap.ssid,ssid);
	strcpy(wap.channel,"AUTO");

	__ret = wifi_set_wap(fd, &wap);
	if(__ret<0){
		printf("wifi_set_wap error!\n");
		return WIFI_ERROR;
	}
	
	return WIFI_EOK;
}

#endif
#endif



