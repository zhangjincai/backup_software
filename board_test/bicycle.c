#include "include/bicycle.h"
#include "include/lib_gps.h"
#include "include/lib_ctos.h"
#include "include/lib_wireless.h"
#include "include/lib_wifi.h"
#include "include/ftdi.h"
#include "include/libusb.h"
#include "include/LIB_SHA204.h" 


int analysts(unsigned char *rxbuf, lib_serial_t *serial)
{
	if(rxbuf[0]!=0x55 || rxbuf[1]!=0xaa){

		printf("the package`s head is error!\n");
		return -1;
	}

	if(rxbuf[2]!=0x07){

		printf("the package`s length is error!\n");
		return -1;
	}

	unsigned char check = crc16(rxbuf,rxbuf[2]);
	if(rxbuf[5]!=check){

		printf("the package`s check is error!\n");
		return -1;
	}

	if(rxbuf[6]!=0x00){

		printf("the package`s tail is error!\n");
		return -1;
	}


	switch(rxbuf[3])
    {
        case 0x01:{
            if(rxbuf[4]==0x1)
           		gps_test(serial);
            break;
        }
        case 0x02:{
            if(rxbuf[4]==0x1)
           		wifi_test(serial);
			break;
        }
        case 0x04:{
            if(rxbuf[4]==0x1)
            	thrg_test(serial);
            break;
        }
        case 0x08:{
            if(rxbuf[4]==0x1)
            	can_test(serial);
            break;
        }
        case 0x10:{
            if(rxbuf[4]==0x1)
				rj45_test(serial);
            break;
        }
		case 0x11:{
            if(rxbuf[4]==0x1)
				ftdi_test(serial);
            break;
        }
		case 0x12:{
            if(rxbuf[4]==0x1)
				i2c_test(serial);
            break;
        }
		case 0x14:{
            if(rxbuf[4]==0x1)
				encryption_test(serial);
            break;
        }
		case 0x18:{
            if(rxbuf[4]==0x1)
				ttyO2_test(serial);
            break;
        }
        default:
			{
				printf("the package`s command is error!\n");
				break;
        	}
        }

	return 0;
}

void gps_test(lib_serial_t *serial)
{
	int flag = true;
	int i;
	lib_serial_t gps_serial;
	unsigned char rxbuf[1024];
	unsigned char txbuf[7];
	int ret = -1;
	int rret;
	
	txbuf[0] = 0x55;   //tou 1
	txbuf[1] = 0xaa;   //tou 2
	txbuf[2] = 0x07;   //minglingzi
	txbuf[3] = 0x01;   //shujuchangdu
	txbuf[4] = 0x04;
	
	rret = __gps_init(&gps_serial,GPS_DEV_NAME,9600);
	if(rret<0)
	{
		fprintf(stderr, "%s: err = %d\n", __FUNCTION__, rret);
	}else{
		ret = gps_gga_on(gps_serial.sfd);
		if(ret<0)
		{
			printf("gps_test_write error!\n");
		}else{			
			ret = lib_serial_readn_select(gps_serial.sfd, rxbuf, 7, 1000);
			if(ret<0)
			{
				printf("gps_test_read error!\n");
			}else
				txbuf[4] = 0x02;
		}
	}

	if(rret=0)
		lib_serial_close(&gps_serial);

	txbuf[5] = crc16(txbuf, txbuf[2]);
	txbuf[6] = 0x00;   //shuu neirong

	ret = lib_serial_send(serial->sfd, txbuf, 7);
	if(ret<=0)
	{
		printf("************lib_serial_send error!************\n");
		
	 }else
		lib_printf("txbuf",txbuf,7);
	
	printf("\n");
}

void wifi_test(lib_serial_t *serial)
{
	int flag = true;
	lib_serial_t wifi_serial;
	unsigned char txbuf[7];
	int ret;
	
	txbuf[0] = 0x55;   //tou 1
	txbuf[1] = 0xaa;   //tou 2
	txbuf[2] = 0x07;   //minglingzi
	txbuf[3] = 0x02;   //shujuchangdu
	txbuf[4] = 0x04;

	memset(&wifi_serial, 0, sizeof(lib_serial_t));
		
	strcpy(wifi_serial.pathname,WIFI_DEV_NAME);
	wifi_serial.flags = O_RDWR;
	wifi_serial.speed = 57600;
	wifi_serial.databits = 8;
	wifi_serial.stopbits = 1;

	ret = __wifi_init(&wifi_serial);
	if(ret<0)
	{
		printf("Wifi_Init error!\n");
	}else
	{
		struct wifi_wmode wmode;
		struct wifi_wap wap;
		struct wifi_lann lann;
		struct wifi_wakey wakey;

		// 设置WMODE
		strcpy(wmode.wmode,"AP");
		wifi_set_wmode(wifi_serial.sfd,&wmode);

		// 设置LAN
		strcpy(lann.lan_ip,"192.168.100.254");
		strcpy(lann.lan_mask,"255.255.255.0");
		wifi_set_lann(wifi_serial.sfd,&lann);
		
		__wifi_resease(&wifi_serial);

		system("ifconfig eth0 192.168.100.1");

		if (!checkConnect("192.168.100.254", 4))   
			txbuf[4] = 0x02;

	}

	

	txbuf[5] = crc16(txbuf, txbuf[2]);
	txbuf[6] = 0x00;   //shuu neirong

	ret = lib_serial_send(serial->sfd, txbuf, 7);
	if(ret<=0)
	{
		printf("************lib_serial_send error!************\n");
		
	 }else
		lib_printf("txbuf",txbuf,7);
	
	printf("\n");
}


void thrg_test(lib_serial_t *serial)
{
	lib_wl_t *wl;
	lib_wl_hwver_t hwver;
	unsigned char txbuf[7];
	int ret;
	
	txbuf[0] = 0x55;   //tou 1
	txbuf[1] = 0xaa;   //tou 2
	txbuf[2] = 0x07;   //minglingzi
	txbuf[3] = 0x04;   //shujuchangdu
	txbuf[4] = 0x04;
	
	wl = lib_wl_new();
	if(wl==NULL)
	{
		printf("lib_wl_new error!\n");
	}else{
		ret = lib_wl_hwver_get(wl, &hwver, 2000);	
		if(ret<0)
			printf("lib_wl_hwver_get error!\n");
		else
		{
			txbuf[4] = 0x02;
			printf("firmversion : %s\n",hwver.firmversion);
		}	
	}
	
	txbuf[5] = crc16(txbuf, txbuf[2]);
	txbuf[6] = 0x00;

	ret = lib_serial_send(serial->sfd, txbuf, 7);
	if(ret<=0)
	{
		printf("************lib_serial_send error!************\n");
		
	 }else
		lib_printf("txbuf",txbuf,7);

	lib_wl_destroy(wl);

	printf("\n");
}

void can_test(lib_serial_t *serial)
{
	int ret = -1;
	unsigned char d_addr;
	unsigned char s_addr;
	int flag = true;
	unsigned char txbuf2[7];

	ctos_config_t conf;
	lib_ctos_t *ctos = NULL;

	txbuf2[0] = 0x55;   //tou 1
	txbuf2[1] = 0xaa;   //tou 2
	txbuf2[2] = 0x07;   //minglingzi
	txbuf2[3] = 0x08;   //shujuchangdu
	txbuf2[4] = 0x04;

	conf.can_bus = 1;
	conf.can_id = 0;
	conf.can_buf_num = 32;
	conf.baud_rate = BAUD_RATE_125kbps;
	conf.rb_sz = 4 * 1024;

	int len = 0;
	unsigned char rxbuf[512] = {0};
	unsigned char txbuf[512] = {0};
	
	s_addr = 0x00;	
	d_addr = 0x7f;
	
	ctos = lib_ctos_create(&conf);
	if(ctos == NULL)
	{ 
		flag = false;
		goto ERR;
	}

	len = package_make(txbuf, d_addr, s_addr, 0x81);
	
	if(len > 0)
	{
		ret = lib_ctos_put(ctos, d_addr, txbuf, len);
	}
	
	lib_msleep(12000);

	ret = lib_ctos_get(ctos, rxbuf, sizeof(rxbuf));
	if(ret > 0)
	{
		if(rxbuf[8]=='a' && rxbuf[9]=='a')
			txbuf2[4]=0x02;
	}
	
ERR:	
	txbuf2[5] = crc16(txbuf, txbuf[2]);
	txbuf2[6] = 0x00;   //shuu neirong

	ret = lib_serial_send(serial->sfd, txbuf2, 7);
	if(ret<=0)
	{
		printf("************lib_serial_send error!************\n");
		
	 }else
		lib_printf("txbuf",txbuf2,7);

	if(flag == false)
		lib_ctos_destroy(ctos);

	printf("\n");
}

void rj45_test(lib_serial_t *serial)
{
	int flag = true;
	unsigned char txbuf[7];
	unsigned char rxbuf[32];
	int ret;
	
	txbuf[0] = 0x55;   //tou 1
	txbuf[1] = 0xaa;   //tou 2
	txbuf[2] = 0x07;   //minglingzi
	txbuf[3] = 0x10;   //shujuchangdu

	memset(rxbuf,0,sizeof(rxbuf));
	ret = lib_serial_readn_select(serial->sfd, rxbuf, sizeof(rxbuf), 1500);
	if(ret<0)
	{
		printf("rj45_test read ip error!\n");
		return;
	}else
		printf("ip = %s\n",rxbuf);

	if (checkConnect(rxbuf, 4))   
        flag = false;

	if(flag==true)
	{
		txbuf[4] = 0x02;
	}else{
		txbuf[4] = 0x04;
	}

	txbuf[5] = crc16(txbuf, txbuf[2]);
	txbuf[6] = 0x00;   //shuu neirong

	ret = lib_serial_send(serial->sfd, txbuf, 7);
	if(ret<=0)
	{
		printf("************lib_serial_send error!************\n");
		
	 }else
		lib_printf("txbuf",txbuf,7);
	
	printf("\n");
}

void ftdi_test(lib_serial_t *serial)
{
	int f;
	unsigned char txbuf[32];
	unsigned char rxbuf[32];
	int ret;
	
	txbuf[0] = 0x55;   //tou 1
	txbuf[1] = 0xaa;   //tou 2
	txbuf[2] = 0x07;   //minglingzi
	txbuf[3] = 0x11;   //shujuchangdu
	txbuf[4] = 0x04;
 
	struct ftdi_context *ftdi = NULL;

	//ftdi = (struct ftdi_context *)malloc(sizeof(struct ftdi_context));
	int vid = 0x0403;
    int pid = 0x6001;
	if ((ftdi = ftdi_new()) == NULL)
    {
        fprintf(stderr, "ftdi_new failed\n");
	
        //return EXIT_FAILURE;
    }else{

		ftdi_set_interface(ftdi, INTERFACE_A);
			   
	    // Open device
	    f = ftdi_usb_open(ftdi, vid, pid);

		if (f < 0)
			printf("ftdi_usb_open error!\n");
		else
		{
			lib_msleep(200);
			ftdi_usb_close(ftdi); 
			txbuf[4] = 0x02;
		}

		ftdi_free(ftdi);
    }

	//free(ftdi);

	txbuf[5] = crc16(txbuf, txbuf[2]);
	txbuf[6] = 0x00;   //shuu neirong

	ret = lib_serial_send(serial->sfd, txbuf, 7);
	if(ret<=0)
	{
		printf("************lib_serial_send error!************\n");
		
	 }else
		lib_printf("txbuf",txbuf,7);	
	
	printf("\n");
}

void i2c_test(lib_serial_t *serial)
{
	int f;
	unsigned char txbuf[7];
	unsigned char rxbuf[32];
	unsigned int sn32 = 1;
	int ret;
	
	txbuf[0] = 0x55;   //tou 1
	txbuf[1] = 0xaa;   //tou 2
	txbuf[2] = 0x07;   //minglingzi
	txbuf[3] = 0x12;   //shujuchangdu
	txbuf[4] = 0x04;
	
	ret = lib_SN_consume_read32(&sn32);
	if(ret==0)
		txbuf[4] = 0x02;

	txbuf[5] = crc16(txbuf, txbuf[2]);
	txbuf[6] = 0x00;   //shuu neirong

	ret = lib_serial_send(serial->sfd, txbuf, 7);
	if(ret<=0)
	{
		printf("************lib_serial_send error!************\n");
		
	 }else
		lib_printf("txbuf",txbuf,7);
	
	printf("\n");
}

void encryption_test(lib_serial_t *serial)
{
	int f;
	unsigned char txbuf[7];
	unsigned char rxbuf[32];
	int ret;
	
	txbuf[0] = 0x55;   //tou 1
	txbuf[1] = 0xaa;   //tou 2
	txbuf[2] = 0x07;   //minglingzi
	txbuf[3] = 0x14;   //shujuchangdu
	txbuf[4] = 0x04;

	ret = LIB_Encry_Verify();
	if(ret==LIB_EV_OK)
		txbuf[4] = 0x02;

	txbuf[5] = crc16(txbuf, txbuf[2]);
	txbuf[6] = 0x00;   //shuu neirong

	ret = lib_serial_send(serial->sfd, txbuf, 7);
	if(ret<=0)
	{
		printf("************lib_serial_send error!************\n");
		
	 }else
		lib_printf("txbuf",txbuf,7);
	
	printf("\n");
}

void ttyO2_test(lib_serial_t *serial)
{

	printf("\n");
}


unsigned char crc16(unsigned char *buf, unsigned short len)
{
    unsigned char a = 0x00;
    int i;
	int length = len -2;

    for(i=0; i<length; i++)
    {
        a = a ^ buf[i];
    }
	
    return a;
}

int checkConnect(char *dst, int cnt)   /////
{   
    int i = 0;   
    FILE *stream;   
    char recvBuf[16] = {0};   
    char cmdBuf[256] = {0};   
	
    if (NULL == dst || cnt <= 0)   
        return -1;   
  
    sprintf(cmdBuf, "ping %s -c %d | grep time= | wc -l", dst, cnt);   
    stream = popen(cmdBuf, "r");   
    fread(recvBuf, sizeof(char), sizeof(recvBuf)-1, stream);   
    pclose(stream);   
  
    if (atoi(recvBuf) > 0)   
    return 0;   
  
    return -1;   
}   

int package_make(unsigned char *data, const unsigned char d_addr, const unsigned char s_addr, const unsigned char cmd)
{
	assert(data);

	unsigned char buf[5] = "ab00&";
	unsigned int len = 0;
	int i;
	unsigned short crc16 = 0;
	unsigned short mlen = 5;

	data[len++] = 0x55;
	data[len++] = 0xaa;
	data[len++] = s_addr;
	data[len++] = d_addr;
	data[len++] = cmd;
	data[len++] = 0;
	
	memcpy(&data[len], &mlen, 2);
	len += 2;
	
	memcpy(&data[len], buf, 5);
	len += 5;

	crc16 = lib_crc16_with_byte(data, len);

	memcpy(&data[len], &crc16, 2);

	len += 2;

	return len;
}

void __sigint(int sig)
{

}

void __signals_init(void)
{
/*
 * signal处理不好，会导致进程自动退出
 * 加上了SIGTTOU,SIGTTIN,SIGTSTP后，处理了进程自动退出的BUG
 */
	
	struct sigaction sa;

	sa.sa_flags = 0;
	sigaddset(&sa.sa_mask, SIGPIPE);
    sigaddset(&sa.sa_mask, SIGCHLD); 
   	sigaddset(&sa.sa_mask, SIGINT);
	sigaddset(&sa.sa_mask, SIGHUP);
	sigaddset(&sa.sa_mask, SIGTERM); 
	sigaddset(&sa.sa_mask, SIGTTOU);
	sigaddset(&sa.sa_mask, SIGTTIN);
	sigaddset(&sa.sa_mask, SIGTSTP);

	sa.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &sa, NULL);

	sa.sa_handler = SIG_IGN;
	sigaction(SIGCHLD, &sa, NULL);

	sa.sa_handler = SIG_IGN;
	sigaction(SIGINT, &sa, NULL);

	sa.sa_handler = SIG_IGN;
	sigaction(SIGHUP, &sa, NULL);

	sa.sa_handler = __sigint;
	sigaction(SIGTERM, &sa, NULL);

	sa.sa_handler = SIG_IGN;
	sigaction(SIGTTIN, &sa, NULL);

	sa.sa_handler = SIG_IGN;
	sigaction(SIGTSTP, &sa, NULL);
}




