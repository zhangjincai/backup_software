#ifndef __GPS_C__
#define __GPS_C__

#include "include/lib_gps.h"

#define uchar unsigned char


int __gps_init(lib_serial_t *serial, char *DEV_NAME, int speed)
{
	memset(serial,0,sizeof(serial));
	
	int __ret = -1;
	
	strcpy(serial->pathname, DEV_NAME);
	serial->flags = O_RDWR;
	serial->speed = speed;
	serial->databits = 8;
	serial->stopbits = 1;

	__ret = lib_serial_init(serial);
	if(__ret<0)
	{
		return GPS_ERROR;
	}

	return GPS_EOK;
}

int __gps_resease(lib_serial_t *serial)
{
	lib_serial_close(serial);

	printf("The program terminated!\n");
	
	exit(0);
}

int gps_nmea_crc(uchar *crcbuf, int size)
{
	int i = 0;
	uchar ck=0;
	
	for(i=0; i<size; i++)
	{
		ck^=crcbuf[i];
	}

	int a,b;
	
	a=ck/16+48;
	b=ck%16+48;
	if(b>57 && b<65)
		b = b+7;	
	
	if(crcbuf[strlen((char *)crcbuf)-2]!=a || crcbuf[strlen((char *)crcbuf	)-1]!=b){
		printf("crc error!\n");
		return GPS_ERROR;
	}

	return GPS_EOK;
}

void gps_ubx_crc(uchar *retbuf,uchar *crcbuf, int size)
{
	int i = 0;

	memset(retbuf,0,2);
	for(i=0; i<size; i++)
	{
		retbuf[0] += crcbuf[i];
		retbuf[1] += retbuf[0];
	}
}

int gps_set_rate(int fd,int ms_rate)
{
	int __ret = -1;
	uchar txbuf[1024] = {0};
	
	txbuf[0] = 0xb5;
	txbuf[1] = 0x62;
	txbuf[2] = 0x06;
	txbuf[3] = 0x08;
	txbuf[4] = 0x06;
	txbuf[5] = 0x00;
	txbuf[6] = 0xff;
	txbuf[7] = 0x0f;
	txbuf[8] = 0x01;
	txbuf[9] = 0x00;
	txbuf[10] = 0x00;
	txbuf[11] = 0x00;

	gps_ubx_crc(&txbuf[12], &txbuf[2], 10);

	__ret = lib_serial_send(fd, txbuf, 12);
	if(__ret<0)
	{
		return __ret;
	}
	
	lib_printf("________serial send____________\n", txbuf, 14);

	return GPS_EOK;
}

int gps_gga_off(int fd)
{
	int __ret = -1;
	uchar txbuf[1024] = {0};
	
	txbuf[0] = 0xb5;
	txbuf[1] = 0x62;
	txbuf[2] = 0x06;
	txbuf[3] = 0x01;
	txbuf[4] = 0x08;
	txbuf[5] = 0x00;
	txbuf[6] = 0xf0;
	txbuf[7] = 0x00;
	txbuf[8] = 0x00;
	txbuf[9] = 0x00;
	txbuf[10] = 0x00;
	txbuf[11] = 0x00;
	txbuf[12] = 0x00;
	txbuf[13] = 0x01;
	txbuf[14] = 0x00;
	txbuf[15] = 0x24;

	__ret = lib_serial_send(fd, txbuf, 16);
	if(__ret<0)
	{
		return __ret;
	}
	
	lib_printf("________serial send____________\n", txbuf, 16);

	return GPS_EOK;
}

int gps_gga_on(int fd)
{
	int __ret = -1;
	uchar txbuf[1024] = {0};
	
	txbuf[0] = 0xb5;
	txbuf[1] = 0x62;
	txbuf[2] = 0x06;
	txbuf[3] = 0x01;
	txbuf[4] = 0x08;
	txbuf[5] = 0x00;
	txbuf[6] = 0xf0;
	txbuf[7] = 0x00;
	txbuf[8] = 0x00;
	txbuf[9] = 0x01;
	txbuf[10] = 0x00;
	txbuf[11] = 0x00;
	txbuf[12] = 0x00;
	txbuf[13] = 0x01;
	txbuf[14] = 0x01;
	txbuf[15] = 0x29;

	__ret = lib_serial_send(fd, txbuf, 16);
	if(__ret<0)
	{
		return __ret;
	}
	
	lib_printf("________serial send____________\n", txbuf, 16);

	return GPS_EOK;
}

int gps_gll_off(int fd)
{
	int __ret = -1;
	uchar txbuf[1024] = {0};
	
	txbuf[0] = 0xb5;
	txbuf[1] = 0x62;
	txbuf[2] = 0x06;
	txbuf[3] = 0x01;
	txbuf[4] = 0x08;
	txbuf[5] = 0x00;
	txbuf[6] = 0xf0;
	txbuf[7] = 0x01;
	txbuf[8] = 0x00;
	txbuf[9] = 0x00;
	txbuf[10] = 0x00;
	txbuf[11] = 0x00;
	txbuf[12] = 0x00;
	txbuf[13] = 0x01;
	txbuf[14] = 0x01;
	txbuf[15] = 0x2b;
	
	__ret = lib_serial_send(fd, txbuf, 16);
	if(__ret<0)
	{
		return __ret;
	}
	
	lib_printf("________serial send____________\n", txbuf, 16);

	return GPS_EOK;
}

int gps_gll_on(int fd)
{
	int __ret = -1;
	uchar txbuf[1024] = {0};
	
	txbuf[0] = 0xb5;
	txbuf[1] = 0x62;
	txbuf[2] = 0x06;
	txbuf[3] = 0x01;
	txbuf[4] = 0x08;
	txbuf[5] = 0x00;
	
	txbuf[6] = 0xf0;
	txbuf[7] = 0x01;
	txbuf[8] = 0x00;
	txbuf[9] = 0x01;
	txbuf[10] = 0x00;
	txbuf[11] = 0x00;
	txbuf[12] = 0x00;
	txbuf[13] = 0x01;
	
	txbuf[14] = 0x02;
	txbuf[15] = 0x30;

	__ret = lib_serial_send(fd, txbuf, 16);
	if(__ret<0)
	{
		return __ret;
	}
	
	lib_printf("________serial send____________\n", txbuf, 16);

	return GPS_EOK;
}

int gps_gsa_off(int fd)
{
	int __ret = -1;
	uchar txbuf[1024] = {0};
	
	txbuf[0] = 0xb5;
	txbuf[1] = 0x62;
	txbuf[2] = 0x06;
	txbuf[3] = 0x01;
	txbuf[4] = 0x08;
	txbuf[5] = 0x00;
	
	txbuf[6] = 0xf0;
	txbuf[7] = 0x02;
	txbuf[8] = 0x00;
	txbuf[9] = 0x00;
	txbuf[10] = 0x00;
	txbuf[11] = 0x00;
	txbuf[12] = 0x00;
	txbuf[13] = 0x01;
	
	txbuf[14] = 0x02;
	txbuf[15] = 0x32;

	__ret = lib_serial_send(fd, txbuf, 16);
	if(__ret<0)
	{
		return __ret;
	}
	
	lib_printf("________serial send____________\n", txbuf, 16);

	return GPS_EOK;
}

int gps_gsa_on(int fd)
{
	int __ret = -1;
	uchar txbuf[1024] = {0};
	
	txbuf[0] = 0xb5;
	txbuf[1] = 0x62;
	txbuf[2] = 0x06;
	txbuf[3] = 0x01;
	txbuf[4] = 0x08;
	txbuf[5] = 0x00;
	
	txbuf[6] = 0xf0;
	txbuf[7] = 0x02;
	txbuf[8] = 0x00;
	txbuf[9] = 0x00;
	txbuf[10] = 0x00;
	txbuf[11] = 0x00;
	txbuf[12] = 0x00;
	txbuf[13] = 0x01;
	
	txbuf[14] = 0x02;
	txbuf[15] = 0x32;

	__ret = lib_serial_send(fd, txbuf, 16);
	if(__ret<0)
	{
		return __ret;
	}
	
	lib_printf("________serial send____________\n", txbuf, 16);

	return GPS_EOK;
}



int gps_gsv_off(int fd)
{
	int __ret = -1;
	uchar txbuf[1024] = {0};
	
	txbuf[0] = 0xb5;
	txbuf[1] = 0x62;
	txbuf[2] = 0x06;
	txbuf[3] = 0x01;
	txbuf[4] = 0x08;
	txbuf[5] = 0x00;
	
	txbuf[6] = 0xf0;
	txbuf[7] = 0x03;
	txbuf[8] = 0x00;
	txbuf[9] = 0x00;
	txbuf[10] = 0x00;
	txbuf[11] = 0x00;
	txbuf[12] = 0x00;
	txbuf[13] = 0x01;
	
	txbuf[14] = 0x03;
	txbuf[15] = 0x39;

	__ret = lib_serial_send(fd, txbuf, 16);
	if(__ret<0)
	{
		return __ret;
	}
	
	lib_printf("________serial send____________\n", txbuf, 16);

	return GPS_EOK;
}

int gps_gsv_on(int fd)
{
	int __ret = -1;
	uchar txbuf[1024] = {0};
	
	txbuf[0] = 0xb5;
	txbuf[1] = 0x62;
	txbuf[2] = 0x06;
	txbuf[3] = 0x01;
	txbuf[4] = 0x08;
	txbuf[5] = 0x00;
	
	txbuf[6] = 0xf0;
	txbuf[7] = 0x03;
	txbuf[8] = 0x00;
	txbuf[9] = 0x01;
	txbuf[10] = 0x00;
	txbuf[11] = 0x00;
	txbuf[12] = 0x00;
	txbuf[13] = 0x01;
	
	txbuf[14] = 0x04;
	txbuf[15] = 0x3e;

	__ret = lib_serial_send(fd, txbuf, 16);
	if(__ret<0)
	{
		return __ret;
	}
	
	lib_printf("________serial send____________\n", txbuf, 16);

	return GPS_EOK;
}

int gps_rmc_off(int fd)
{
	int __ret = -1;
	uchar txbuf[1024] = {0};
	
	txbuf[0] = 0xb5;
	txbuf[1] = 0x62;
	txbuf[2] = 0x06;
	txbuf[3] = 0x01;
	txbuf[4] = 0x08;
	txbuf[5] = 0x00;
	
	txbuf[6] = 0xf0;
	txbuf[7] = 0x04;
	txbuf[8] = 0x00;
	txbuf[9] = 0x00;
	txbuf[10] = 0x00;
	txbuf[11] = 0x00;
	txbuf[12] = 0x00;
	txbuf[13] = 0x01;
	
	txbuf[14] = 0x04;
	txbuf[15] = 0x40;

	__ret = lib_serial_send(fd, txbuf, 16);
	if(__ret<0)
	{
		return __ret;
	}
	
	lib_printf("________serial send____________\n", txbuf, 16);

	return GPS_EOK;
}

int gps_rmc_on(int fd)
{
	int __ret = -1;
	uchar txbuf[1024] = {0};
	
	txbuf[0] = 0xb5;
	txbuf[1] = 0x62;
	txbuf[2] = 0x06;
	txbuf[3] = 0x01;
	txbuf[4] = 0x08;
	txbuf[5] = 0x00;
	
	txbuf[6] = 0xf0;
	txbuf[7] = 0x04;
	txbuf[8] = 0x00;
	txbuf[9] = 0x01;
	txbuf[10] = 0x00;
	txbuf[11] = 0x00;
	txbuf[12] = 0x00;
	txbuf[13] = 0x01;
	
	txbuf[14] = 0x05;
	txbuf[15] = 0x45;

	__ret = lib_serial_send(fd, txbuf, 16);
	if(__ret<0)
	{
		return __ret;
	}
	
	lib_printf("________serial send____________\n", txbuf, 16);

	return GPS_EOK;
}

int gps_vtg_off(int fd)
{
	int __ret = -1;
	uchar txbuf[1024] = {0};
	
	txbuf[0] = 0xb5;
	txbuf[1] = 0x62;
	txbuf[2] = 0x06;
	txbuf[3] = 0x01;
	txbuf[4] = 0x08;
	txbuf[5] = 0x00;
	
	txbuf[6] = 0xf0;
	txbuf[7] = 0x05;
	txbuf[8] = 0x00;
	txbuf[9] = 0x00;
	txbuf[10] = 0x00;
	txbuf[11] = 0x00;
	txbuf[12] = 0x00;
	txbuf[13] = 0x01;
	
	txbuf[14] = 0x05;
	txbuf[15] = 0x47;

	__ret = lib_serial_send(fd, txbuf, 16);
	if(__ret<0)
	{
		return __ret;
	}
	
	lib_printf("________serial send____________\n", txbuf, 16);

	return GPS_EOK;
}

int gps_vtg_on(int fd)
{
	int __ret = -1;
	uchar txbuf[1024] = {0};
	
	txbuf[0] = 0xb5;
	txbuf[1] = 0x62;
	txbuf[2] = 0x06;
	txbuf[3] = 0x01;
	txbuf[4] = 0x08;
	txbuf[5] = 0x00;
	
	txbuf[6] = 0xf0;
	txbuf[7] = 0x05;
	txbuf[8] = 0x00;
	txbuf[9] = 0x01;
	txbuf[10] = 0x00;
	txbuf[11] = 0x00;
	txbuf[12] = 0x00;
	txbuf[13] = 0x01;
	
	txbuf[14] = 0x06;
	txbuf[15] = 0x4c;

	__ret = lib_serial_send(fd, txbuf, 16);
	if(__ret<0)
	{
		return __ret;
	}
	
	lib_printf("________serial send____________\n", txbuf, 16);

	return GPS_EOK;
}

int gps_zda_off(int fd)
{
	int __ret = -1;
	uchar txbuf[1024] = {0};
	
	txbuf[0] = 0xb5;
	txbuf[1] = 0x62;
	txbuf[2] = 0x06;
	txbuf[3] = 0x01;
	txbuf[4] = 0x08;
	txbuf[5] = 0x00;
	txbuf[6] = 0xf0;
	txbuf[7] = 0x08;
	txbuf[8] = 0x00;
	txbuf[9] = 0x00;
	txbuf[10] = 0x00;
	txbuf[11] = 0x00;
	txbuf[12] = 0x00;
	txbuf[13] = 0x01;

	gps_ubx_crc(&txbuf[14], &txbuf[2], 12);

	__ret = lib_serial_send(fd, txbuf, 16);
	if(__ret<0)
	{
		return __ret;
	}
	
	lib_printf("________serial send____________\n", txbuf, 16);

	return GPS_EOK;
}

int gps_zda_on(int fd)
{
	int __ret = -1;
	uchar txbuf[1024] = {0};
	
	txbuf[0] = 0xb5;
	txbuf[1] = 0x62;
	txbuf[2] = 0x06;
	txbuf[3] = 0x01;
	txbuf[4] = 0x08;
	txbuf[5] = 0x00;
	txbuf[6] = 0xf0;
	txbuf[7] = 0x08;
	txbuf[8] = 0x00;
	txbuf[9] = 0x01;
	txbuf[10] = 0x00;
	txbuf[11] = 0x00;
	txbuf[12] = 0x00;
	txbuf[13] = 0x01;

	//unsigned char buf[2];
	gps_ubx_crc(&txbuf[14], &txbuf[2], 12);

	__ret = lib_serial_send(fd, txbuf, 16);
	if(__ret<0)
	{
		return __ret;
	}
	
	lib_printf("________serial send____________\n", txbuf, 16);

	return GPS_EOK;
}

int gps_split_info(char *rxbuf, char (*line)[512], int *times)
{
	if(rxbuf == NULL)
		return -1;
	
	char *p;

	p = strtok(rxbuf,"\n");
	while(p)
	{
		strcpy(line[(*times)++],p);
		p=strtok(NULL, "\n");
	}

	return 0;
}

int gps_get_gga(char *rxbuf, struct gps_gga *gga)
{
	if(rxbuf == NULL)
		return -1;

	printf("gps_get_gga = %s\n",rxbuf);

	int __ret = -1;
	///////////////////////////////////////////////////////
	__ret = gps_nmea_crc((uchar *)(&rxbuf[1]), strlen(rxbuf)-4);
	if(__ret<0)
		return GPS_ERROR;
		
	char *p;
	double db2;

	p = strtok(rxbuf,",");
	if(p){
		p = strtok(NULL,",");
	}else
		return GPS_ERROR;

	
	if(p){
			p = strtok(NULL,",");
		}else
			return GPS_ERROR;

	if(p){	
		sscanf(p, "%lf", &db2);
		gga->latitude=db2/100;
		p = strtok(NULL,",");
	}else
		return GPS_ERROR;

	if(p){
		strncpy((char *)&(gga->ns_indicator),p,1);
		p = strtok(NULL,",");
	}else
		return GPS_ERROR;

	if(p){
		sscanf(p, "%lf", &db2);
		gga->longitude=db2/100;
		p = strtok(NULL,",");
	}else
		return GPS_ERROR;

	if(p){
		strncpy((char *)&(gga->ew_indicator),p,1);
		p = strtok(NULL,",");
	}else
		return GPS_ERROR;

	return GPS_EOK;
}

int gps_get_tm(char *rxbuf, struct gps_tm *tm)
{
	if(rxbuf == NULL)
		return GPS_ERROR;
	
	printf("gps_get_tm = %s\n",rxbuf);

	int __ret = -1;
	__ret = gps_nmea_crc((uchar *)&rxbuf[1], strlen(rxbuf)-4);
	if(__ret<0)
		return GPS_ERROR;
	
	char *p;
	char buf[32] = {0};

	p = strtok(rxbuf,",");
	
	if(p){
		p = strtok(NULL,",");
	}else
		return GPS_ERROR;

	if(p){
		memset(buf,'\0',sizeof(buf));
		memcpy(buf, p, 2);
		tm->tm_hour=atoi(buf)+8;
		
		memset(buf,0,sizeof(buf));
		memcpy(buf, p+2, 2);
		tm->tm_min=atoi(buf);
		
		memset(buf,0,sizeof(buf));
		memcpy(buf, p+4, 2);
		tm->tm_sec=atoi(buf);
		
		p = strtok(NULL,",");
	}else
		return GPS_ERROR;
		
	if(p){
		tm->tm_mday=atoi(p);
		p = strtok(NULL,",");
	}else
		return GPS_ERROR;

	if(p){
		tm->tm_mon=atoi(p);
		p = strtok(NULL,",");
	}else
		return GPS_ERROR;

	if(p){
		tm->tm_year=atoi(p);
		p = strtok(NULL,",");
	}else
		return GPS_ERROR;

	tm->tm_yday = get_yday(tm->tm_year,tm->tm_mon,tm->tm_mday);
	tm->tm_wday = (tm->tm_mday+2*tm->tm_mon+3*(tm->tm_mon+1)/5+tm->tm_year+tm->tm_year/4-tm->tm_year/100+tm->tm_year/400)%7+1;

	return 0;
}


int gps_get_zda(char *rxbuf, struct gps_zda *zda)
{
	if(rxbuf == NULL)
		return GPS_ERROR;
	
	printf("gps_get_zda = %s\n",rxbuf);

	int __ret = -1;
	__ret = gps_nmea_crc((uchar *)&rxbuf[1], strlen(rxbuf)-4);
	if(__ret<0)
		return GPS_ERROR;
	
	char *p;
	char buf[32] = {0};

	p = strtok(rxbuf,",");
	
	if(p){
		p = strtok(NULL,",");
	}else
		return GPS_ERROR;

	if(p){
		memset(buf,'\0',sizeof(buf));
		memcpy(buf, p, 2);
		zda->hour=atoi(buf);
		
		memset(buf,0,sizeof(buf));
		memcpy(buf, p+2, 2);
		zda->min=atoi(buf);
		
		memset(buf,0,sizeof(buf));
		memcpy(buf, p+4, 2);
		zda->sec=atoi(buf);
		
		memset(buf,0,sizeof(buf));
		memcpy(buf, p+7, 2);
		zda->msec=atoi(buf);
		
		p = strtok(NULL,",");
	}else
		return GPS_ERROR;
		
	if(p){
		zda->day=atoi(p);
		p = strtok(NULL,",");
	}else
		return GPS_ERROR;

	if(p){
		zda->mon=atoi(p);
		p = strtok(NULL,",");
	}else
		return GPS_ERROR;

	if(p){
		zda->year=atoi(p);
		p = strtok(NULL,",");
	}else
		return GPS_ERROR;

	return GPS_EOK;
}


void gps_show_zda(struct gps_zda *zda)
{
	printf("zda->msec = %d\n", zda->msec);
	printf("zda->sec = %d\n", zda->sec);
	printf("zda->min = %d\n", zda->min);
	printf("zda->hour = %d\n", zda->hour);
	printf("zda->day = %d\n", zda->day);
	printf("zda->mon = %d\n", zda->mon);
	printf("zda->year = %d\n", zda->year);	
}

void gps_show_tm(struct gps_tm *tm)
{
	printf("tm->sec = %d\n", tm->tm_sec);
	printf("tm->min = %d\n", tm->tm_min);
	printf("tm->hour = %d\n", tm->tm_hour);
	printf("tm->mday = %d\n", tm->tm_mday);
	printf("tm->yday = %d\n", tm->tm_yday);
	printf("tm->wday = %d\n", tm->tm_wday);
	printf("tm->mon = %d\n", tm->tm_mon);
	printf("tm->year = %d\n", tm->tm_year);	
}


void gps_show_gga(struct gps_gga *gga)
{
	printf("gga->latitude = %.7f\n", gga->latitude);
	printf("gga->ns_indicator = %c\n", gga->ns_indicator);
	printf("gga->longitude = %.7f\n", gga->longitude);
	printf("gga->ew_indicator = %c\n", gga->ew_indicator);
}

int leap(int y)
{
 int leapyear;
 if (y%4!=0)
     leapyear=0;
 else if (y%100!=0)
     leapyear=1;
 else if (y%400!=0)
     leapyear=0;
 else
     leapyear=1;
 return(leapyear);
}

int monthday(int m, int team)
{
   switch(m)
  {
   case 1:return(31);
   case 3:return(31);
   case 5:return(31);
   case 7:return(31);
   case 8:return(31);
   case 10:return(31);
   case 12:return(31);
   case 4:return(30);
   case 6:return(30);
   case 9:return(30);
   case 11:return(30);
   case 2:{if (team==1) 
   				{return(29);
   			} else if (team==0)
   			{return(28);}
			}
   }  

   return -1;
} 

int get_yday(int year,int mon,int day)
{
	int d,m;
	d=0;
	int team;
	team = leap(year);
	for(m=1;m<mon;m++)
		d+=monthday(m,team);
	
	day+=d;

	return day;
}

#endif
