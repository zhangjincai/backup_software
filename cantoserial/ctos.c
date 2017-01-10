#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h> 
#include <arpa/inet.h> 
#include <net/if.h> 
#include <linux/socket.h> 
#include <linux/can.h> 
#include <linux/can/error.h> 
#include <linux/can/raw.h> 

#include "lib_general.h"
#include "version.h"
#include "lib_ctos.h"




#define CTOS_API_ERR

#ifdef CTOS_API_ERR
#define CTOS_ERR(fmt, args...)	 do{ \
	fprintf(stderr, "ERROR: %s:%d, ", __FUNCTION__,__LINE__); \
	fprintf(stderr, fmt, ##args); \
}while(0)
#else
#define CTOS_ERR(fmt, args...)
#endif


/*
 * 最大CAN 总线数
 */
#define CAN_BUS_NO_MAX				(2)			

/*
 * 最大帧序列
 */
#define CAN_FRAME_SEQ_MAX				(64)

/*
 *   CAN 帧数据长度
 */
#define CAN_FRAME_LEN_MAX				(8)		


/*
 *  帧标识
 */
#define CAN_FRAME_MARK_SINGLE			(0)			
#define CAN_FRAME_MARK_HEAD			(1)
#define CAN_FRAME_MARK_MIDDLE			(2)
#define CAN_FRAME_MARK_TAIL				(3)

/*
 * 优先级
*/
#define CAN_FRAME_PRIORITY_HIGH		(0)
#define CAN_FRAME_PRIORITY_LOW		(1)

struct can_frame_metadata //CAN 元数据帧定义?
{
	//扩展帧:29bit标识符(可自定义)
	unsigned int d_addr:7;		//目的地址
	unsigned int s_addr:7;		//源地址 
	unsigned int frame_mark:2;	//帧标识
	unsigned int frame_seq:6;	//帧序号
	unsigned int random:3;		//随机码 
	unsigned int rsv:3;			//保留 
	unsigned int priority:1;	//优先级
	
	unsigned int err:1;				//错误帧标志 (0 = data frame, 1 = error frame)
	unsigned int rtr:1;				//远程发送请求标志 (1 = rtr frame)
	unsigned int eff:1;				//帧格式标志 (0 = standard 11 bit, 1 = extended 29 bit)
	unsigned char can_dlc;			/* 数据长度: 0 .. 8 */
	unsigned char data[8] __attribute__((aligned(8))); 		//字节对齐是64bit
};
typedef struct can_frame_metadata can_frame_metadata_t;


struct ctos
{
	unsigned char can_bus;		/* CAN 总线号*/
	unsigned char can_id;			/* CAN ID 号*/
	unsigned int baud_rate;		/* CAN 速度*/
	unsigned int can_buf_num;		/* CAN 缓冲区个数, 单位: 个*/
	unsigned int rb_sz;			/* 循环缓冲区大小，单位: 字节*/

	int sockfd;				/* CAN 描述字, 用户不能修改*/
	void *mutex;				/* 缓冲区锁, 用户不能修改*/
	unsigned char sn;  		/* 流水号, 用户不能修改*/
	void *pack_ptr;			/* 缓冲区指针, 用户不能修改*/
	void *regular_rb;      	 	/* 缓冲区指针, 用户不能修改*/
	void *ringbuffer;        		/* 缓冲区指针, 用户不能修改*/

	unsigned int send_delay;
}__attribute__((packed));


#define CAN_SEND_DELAY_20K                         	 (4000)
#define CAN_SEND_DELAY_50K                         	 (2000)
#define CAN_SEND_DELAY_100K                          (1000)
#define CAN_SEND_DELAY_125K                          (800)
#define CAN_SEND_DELAY_250K                          (600)
#define CAN_SEND_DELAY_500K                       	 (500)
#define CAN_SEND_DELAY_800K                       	 (400)
#define CAN_SEND_DELAY_1000K						 (200)


/*
 *  锁桩定义
 */
#define STAKE_NUM_MAX					(128) 	//最大支持桩机数量
#define STAKE_BUF_MAX					(320)	//每个桩机的最大缓冲区大小
#define STAKE_FRAME_MAX					(64)

struct stake_frame_buf
{
	unsigned char has_frame_head;
	unsigned char cnt;
	unsigned char len;
	unsigned char random[STAKE_FRAME_MAX];
	unsigned char data[STAKE_BUF_MAX];
}__attribute__((packed));
typedef struct stake_frame_buf stake_frame_buf_t;


struct stake_frame
{
	unsigned int err_statistics;
	unsigned int err_random;
	struct stake_frame_buf frame_buf[STAKE_NUM_MAX];	
};
typedef struct stake_frame stake_frame_t;


static void *can_recv_thread(void *arg);
static void *can_explain_thread(void *arg);


void can_printf(struct can_frame_metadata *meta)
{
	assert(meta);

	int i;

	fprintf(stderr,"\n ------------------ can frame print -------------------- \n");
		
	fprintf(stderr, "s_addr: 0x%02x\n", meta->s_addr);
	fprintf(stderr, "d_addr: 0x%02x\n", meta->d_addr);
	fprintf(stderr, "frame_mark: 0x%02x\n", meta->frame_mark);
	fprintf(stderr, "frame_seq: %d\n", meta->frame_seq);
	fprintf(stderr, "random: %d\n", meta->random);
	fprintf(stderr, "rsv: %d\n", meta->rsv);
	fprintf(stderr, "priority: %d\n", meta->priority);
	fprintf(stderr, "err: %d\n", meta->err);
	fprintf(stderr, "rtr: %d\n", meta->rtr);
	fprintf(stderr, "eff: %d\n", meta->eff);

	fprintf(stderr, "can_dlc: %d\n", meta->can_dlc);

	fprintf(stderr, "data: ");
	
	for(i = 0; i < 8; i++)
	{
		fprintf(stderr, "0x%02x ", meta->data[i]);
	}

	fprintf(stderr,"\n ------------------ end -------------------- \n");
}


static int baud_rate_set(struct ctos *ct)
{
	assert(ct);
	
	char s_bitrate[64] = {0};
	char s_ifconfig_down[64] = {0};
	char s_ifconfig_up[64] = {0};
	unsigned char can_bus = ct->can_bus;
	unsigned int baud_rate = ct->baud_rate;
		
	if(can_bus > CAN_BUS_NO_MAX)
	{
		fprintf(stderr, "can%d number set out of range\n", can_bus);
		return LIB_CTOS_ERROR;
	}

	if(baud_rate > (BAUD_RATE_END - 1))
	{
		fprintf(stderr, "can%d baud rate set out of range\n", can_bus);
		return LIB_CTOS_ERROR;
	}

	switch(baud_rate)
	{
		/*
		case BAUD_RATE_20kbps:
		{
			sprintf(s_ifconfig_down, "/sbin/ifconfig can%d down", can_bus);
			sprintf(s_bitrate, "/sbin/ip link set can%d type can bitrate 20000", can_bus);
			sprintf(s_ifconfig_up, "/sbin/ifconfig can%d up", can_bus);

			ct->send_delay = CAN_SEND_DELAY_20K;
		}
		break;
		*/
		case BAUD_RATE_50kbps:
		{
			sprintf(s_ifconfig_down, "/sbin/ifconfig can%d down", can_bus);
			sprintf(s_bitrate, "/sbin/ip link set can%d type can bitrate 50000", can_bus);
			sprintf(s_ifconfig_up, "/sbin/ifconfig can%d up", can_bus);

			ct->send_delay = CAN_SEND_DELAY_50K;
		}
		break;		

		/*
		case BAUD_RATE_100kbps:
		{
			sprintf(s_ifconfig_down, "/sbin/ifconfig can%d down", can_bus);
			sprintf(s_bitrate, "/sbin/ip link set can%d type can bitrate 100000", can_bus);
			sprintf(s_ifconfig_up, "/sbin/ifconfig can%d up", can_bus);

			ct->send_delay = CAN_SEND_DELAY_100K;
		}
		break;
		*/
		
		case BAUD_RATE_125kbps:
		{
			sprintf(s_ifconfig_down, "/sbin/ifconfig can%d down", can_bus);
			sprintf(s_bitrate, "/sbin/ip link set can%d type can bitrate 125000", can_bus);
			sprintf(s_ifconfig_up, "/sbin/ifconfig can%d up", can_bus);
		
			ct->send_delay = CAN_SEND_DELAY_125K;
		}
		break;

		case BAUD_RATE_250kbps:
		{
			sprintf(s_ifconfig_down, "/sbin/ifconfig can%d down", can_bus);
			sprintf(s_bitrate, "/sbin/ip link set can%d type can bitrate 250000", can_bus);
			sprintf(s_ifconfig_up, "/sbin/ifconfig can%d up", can_bus);
	
			ct->send_delay = CAN_SEND_DELAY_250K;
		}
		break;

		case BAUD_RATE_500kbps:
		{
			sprintf(s_ifconfig_down, "/sbin/ifconfig can%d down", can_bus);
			sprintf(s_bitrate, "/sbin/ip link set can%d type can bitrate 500000", can_bus);
			sprintf(s_ifconfig_up, "/sbin/ifconfig can%d up", can_bus);

			ct->send_delay = CAN_SEND_DELAY_500K;
		}
		break;
		
#if 0
		case BAUD_RATE_800kbps:
		{
			sprintf(s_ifconfig_down, "/sbin/ifconfig can%d down", can_bus);
			sprintf(s_bitrate, "/sbin/ip link set can%d type can bitrate 800000", can_bus);
			sprintf(s_ifconfig_up, "/sbin/ifconfig can%d up", can_bus);
			
			ct->send_delay = CAN_SEND_DELAY_800K;
		}
		break;

		case BAUD_RATE_1000kbps:
		{
			sprintf(s_ifconfig_down, "/sbin/ifconfig can%d down", can_bus);
			sprintf(s_bitrate, "/sbin/ip link set can%d type can bitrate 1000000", can_bus);
			sprintf(s_ifconfig_up, "/sbin/ifconfig can%d up", can_bus);
			
			ct->send_delay = CAN_SEND_DELAY_1000K;
		}
		break;
#endif

		default: //BAUD_RATE_250kbps
		{
			sprintf(s_ifconfig_down, "/sbin/ifconfig can%d down", can_bus);
			sprintf(s_bitrate, "/sbin/ip link set can%d type can bitrate 250000", can_bus);
			sprintf(s_ifconfig_up, "/sbin/ifconfig can%d up", can_bus);	
			
			ct->send_delay = CAN_SEND_DELAY_250K;
		}
		break;
	}

	fprintf(stderr, "%s\n", s_ifconfig_down);
	fprintf(stderr, "%s\n", s_bitrate);
	fprintf(stderr, "%s\n", s_ifconfig_up);
	
	system(s_ifconfig_down);
	system(s_bitrate);
	system(s_ifconfig_up);
			
	return LIB_CTOS_EOK;
}

static int can_socket_init(int *fd, const unsigned char can_id, const unsigned int can_bus)
{
	assert(fd);

	int ret = -1;
	int sockfd = -1;
	char s_can[16] = {0};
	struct sockaddr_can caddr;
	struct ifreq cifr;
	struct can_filter cfilter;
	int loopback; /* 0 = disabled, 1 = enabled (default) */
	int recv_own_msgs;
	
	memset(&caddr, 0, sizeof(struct sockaddr_can));
	memset(&cifr, 0, sizeof(struct ifreq));
	memset(&cfilter, 0, sizeof(struct can_filter));

	if(can_bus > CAN_BUS_NO_MAX)
	{
		fprintf(stderr, "can%d bus number set out of range\n", can_bus);
		return LIB_CTOS_ERROR;
	}
	
	sockfd = socket(PF_CAN, SOCK_RAW, CAN_RAW); 
	if(sockfd < 0)
	{
		fprintf(stderr, "can%d socket build failed!\n", can_bus);
		return LIB_CTOS_ERROR;
	}

	lib_setfd_noblock(sockfd);

	sprintf(s_can, "can%d", can_bus);
	strcpy(cifr.ifr_name, s_can); 

	ret = ioctl(sockfd, SIOCGIFINDEX, &cifr); //把接口的索引存入ifr_ifindex
	if(ret < 0)
	{
		fprintf(stderr, "can%d ioctl SIOCGIFINDEX failed!\n", can_bus);
		goto ERR;
	}

	caddr.can_family = PF_CAN; 
	caddr.can_ifindex = cifr.ifr_ifindex;
	ret = bind(sockfd, (struct sockaddr *)&caddr, sizeof(caddr));
	if(ret < 0)
	{
		fprintf(stderr, "can%d bind failed!\n", can_bus);
		goto ERR;	
	}

	/* CAN_RAW_LOOPBACK */ //本地回环功能默认是开启的
	loopback = 0;
	ret = setsockopt(sockfd, SOL_CAN_RAW, CAN_RAW_LOOPBACK, &loopback, sizeof(loopback));
	if(ret < 0)
	{
		fprintf(stderr, "can%d setsockopt CAN_RAW_LOOPBACK failed!\n", can_bus);
		goto ERR;
	}

	/* CAN_RAW_RECV_OWN_MSGS */ //在发送套接字上的回环功能默认是关闭的
	recv_own_msgs = 0;
	ret = setsockopt(sockfd, SOL_CAN_RAW, CAN_RAW_RECV_OWN_MSGS, &recv_own_msgs, sizeof(recv_own_msgs));
	if(ret < 0)
	{
		fprintf(stderr, "can%d setsockopt CAN_RAW_RECV_OWN_MSGS failed!\n", can_bus);
		goto ERR;
	}
	
	/* CAN ID */
	cfilter.can_id = can_id | CAN_EFF_FLAG; //CAN_EFF_FLAG:扩展帧的标识
	cfilter.can_mask = 0x7F; //过滤规则的匹配：<received_can_id> & mask == can_id & mask

	ret = setsockopt(sockfd, SOL_CAN_RAW, CAN_RAW_FILTER, &cfilter, sizeof(struct can_filter)); 
	if(ret < 0)
	{
		fprintf(stderr, "can%d setsockopt filter id {0x%02x} failed!\n", can_bus, can_id);
		goto ERR;
	}

	fprintf(stderr, "can%d setsockopt filter id {0x%02x} ok\n", can_bus, can_id);

	*fd = sockfd;

	fprintf(stderr, "can%d socket init ok\n", can_bus);
	
	return LIB_CTOS_EOK;

ERR:
	close(sockfd);
	fprintf(stderr, "can%d socket init failed!\n", can_bus);
	return LIB_CTOS_ERROR;
}

static unsigned char frame_sn_get(lib_ctos_t *ctos)
{
	ctos->sn++;
	
	if(ctos->sn > 7)
		ctos->sn = 0;

	return ctos->sn;
}

static int stake_cmd_buf_init(lib_ctos_t *ctos)
{
	 ctos->pack_ptr = malloc(sizeof(struct stake_frame));
	 if(ctos->pack_ptr == NULL)
	 	return LIB_CTOS_ERROR;

	 return LIB_CTOS_EOK;
}

static void stake_cmd_buf_detach(lib_ctos_t *ctos)
{
	if(ctos->pack_ptr != NULL)
	{
		free(ctos->pack_ptr);
		ctos->pack_ptr = NULL;
	}
}

static int stake_cmd_buf_put(lib_ctos_t *ctos, can_frame_metadata_t *meta)
{
	int ret = -1;
	int cnt = 0;
	int i;
	int f_tail_err = 0;
	//static unsigned char h_random = 0xff;
	unsigned int array_id = 0;
	unsigned char frame_seq = meta->frame_seq;
	unsigned char frame_mark = meta->frame_mark;
	unsigned char frame_err = meta->err;
	unsigned char stake_id = meta->s_addr;  //锁桩CAN ID

	stake_frame_t *frame = (stake_frame_t *)ctos->pack_ptr;	
	if(frame == NULL)
		goto ERR;
	
	if(frame_err == 1)
	{
		frame->err_statistics++;   //错误帧统计
		goto ERR;
	}

	lib_ringbuffer_t *rb = (lib_ringbuffer_t *)ctos->ringbuffer;
	if(rb == NULL)
		goto ERR;
		
	if((stake_id >= STAKE_NUM_MAX) || (stake_id == ctos->can_id))
		goto ERR;

	/* 帧标识*/
	switch(frame_mark)
	{
		case CAN_FRAME_MARK_HEAD: //头帧
		{
			memset(&(frame->frame_buf[stake_id]), 0, sizeof(struct stake_frame_buf));

			array_id = frame_seq * CAN_FRAME_LEN_MAX;
			memcpy(&(frame->frame_buf[stake_id].data[array_id]), meta->data, CAN_FRAME_LEN_MAX);
			cnt = frame->frame_buf[stake_id].cnt = 0;
			frame->frame_buf[stake_id].len += CAN_FRAME_LEN_MAX;
			frame->frame_buf[stake_id].has_frame_head = 1;
			frame->frame_buf[stake_id].random[cnt] = meta->random;
			//h_random = meta->random;
		}
		break;

		case CAN_FRAME_MARK_MIDDLE: //中间帧
		{
			array_id = frame_seq * CAN_FRAME_LEN_MAX;
			memcpy(&(frame->frame_buf[stake_id].data[array_id]) , meta->data, CAN_FRAME_LEN_MAX);
			cnt = frame->frame_buf[stake_id].cnt++;
			frame->frame_buf[stake_id].len += CAN_FRAME_LEN_MAX;
			frame->frame_buf[stake_id].random[cnt] = meta->random;
		}
		break;

		case CAN_FRAME_MARK_TAIL: //尾帧
		{
			array_id = frame_seq * CAN_FRAME_LEN_MAX;
			memcpy(&(frame->frame_buf[stake_id].data[array_id]), meta->data, CAN_FRAME_LEN_MAX);
			cnt = frame->frame_buf[stake_id].cnt++;
			
			if((frame->frame_buf[stake_id].cnt != frame_seq) || (frame->frame_buf[stake_id].has_frame_head != 1)) //必须要收到头帧
			{
				fprintf(stderr, "stake id %d without head frame!\n", stake_id);
				return LIB_CTOS_EXCMEM;
			}

			frame->frame_buf[stake_id].has_frame_head = 0;
			frame->frame_buf[stake_id].len += CAN_FRAME_LEN_MAX;
			frame->frame_buf[stake_id].random[cnt] = meta->random;
					
			for(i = 0; i < cnt; i++) 			//判断随机数
			{
				//if(frame->frame_buf[stake_id].random[i]  != h_random)
				if(frame->frame_buf[stake_id].random[i]  != frame->frame_buf[stake_id].random[0])
				{
					f_tail_err = 1;
					frame->err_random++;
					//fprintf(stderr, "ctos random number stake id:%d, h_random:%d, frame idx:%d, random:%d error!\n", stake_id, h_random, i, frame->frame_buf[stake_id].random[i]);
					
					fprintf(stderr, "ctos random number stake id:%d, h_random:%d, frame idx:%d, random:%d error!\n", stake_id, frame->frame_buf[stake_id].random[0], i, frame->frame_buf[stake_id].random[i]);
				}
			}

			if(f_tail_err == 1)  //发现有随机数错误,直接返回
			{
				f_tail_err = 0;
				memset(&(frame->frame_buf[stake_id]), 0, sizeof(struct stake_frame_buf));
				return LIB_CTOS_ERROR;
			}

			lib_mutex_lock((lib_mutex_t *)ctos->mutex);
			ret =  lib_ringbuffer_putforce(rb, frame->frame_buf[stake_id].data, frame->frame_buf[stake_id].len);
			lib_mutex_unlock((lib_mutex_t *)ctos->mutex);

			memset(&(frame->frame_buf[stake_id]), 0, sizeof(struct stake_frame_buf));
			
			return ret;
		}

		case CAN_FRAME_MARK_SINGLE: //单帧
		{
			frame->frame_buf[stake_id].len = CAN_FRAME_LEN_MAX;

			lib_mutex_lock((lib_mutex_t *)ctos->mutex);
			ret =  lib_ringbuffer_putforce(rb, meta->data, CAN_FRAME_LEN_MAX);
			lib_mutex_unlock((lib_mutex_t *)ctos->mutex);
			
			return ret;
		}
	}

	return LIB_CTOS_EOK;

ERR:
	fprintf(stderr, "%s error!\n", __FUNCTION__);
	return LIB_CTOS_ERROR;
}




char *lib_ctos_version(void)
{
	return CTOS_VERSION;
}

lib_ctos_t *lib_ctos_create(ctos_config_t *conf)
{
	assert(conf);

	int err = -1;
	pthread_t recv_thr, explain_thr;
	int blk_cnt = conf->can_buf_num;
	int blk_size = sizeof(struct can_frame);
	lib_ctos_t *ctos = NULL;

	/* 分配内存*/
	ctos = (lib_ctos_t *)malloc(sizeof(lib_ctos_t));
	if(ctos == NULL)
		goto ERR;

	memset(ctos, 0, sizeof(lib_ctos_t));
	memcpy(ctos, conf, sizeof(ctos_config_t));//can总线参数设置
	
	/* 初始化CAN 设备*/
	err = baud_rate_set(ctos);
	if(err != LIB_CTOS_EOK)
		goto ERR;

	err = can_socket_init(&(ctos->sockfd), ctos->can_id, ctos->can_bus);
	if(err != LIB_CTOS_EOK)
		goto ERR;

	/* 创建规则缓冲区*/
	ctos->regular_rb = lib_regular_rb_create(blk_cnt, blk_size);
	if(ctos->regular_rb == NULL)
		goto ERR;

	/* 创建桩机缓冲区*/
	 err = stake_cmd_buf_init(ctos);
	 if(err != LIB_GE_EOK)
	 	goto ERR;

	/* 创建缓冲锁*/
	ctos->mutex = lib_mutex_create();
	if(ctos->mutex == NULL)
		goto ERR;

	/* 创建循环缓冲区*/
	ctos->ringbuffer = lib_ringbuffer_create(ctos->rb_sz);
	if(ctos->ringbuffer == NULL)
		goto ERR;
	
	/* 创建CAN 接收线程*/
	err = lib_normal_thread_create(&recv_thr, can_recv_thread, (void *)ctos);
	if(err != LIB_GE_EOK)
	{
		fprintf(stderr, "can%d recv thread failed!\n", ctos->can_bus);
		goto ERR;
	}

	/* 创建CAN 解析线程 */
	err = lib_normal_thread_create(&explain_thr, can_explain_thread, (void *)ctos);
	if(err != LIB_GE_EOK)
	{
		fprintf(stderr, "can%d explain thread failed!\n", ctos->can_bus);
		goto ERR;
	}

	fprintf(stderr, "lib ctos init ok\n");
	
	return ctos;;

ERR:
	lib_rwlock_destroy((lib_mutex_t *)ctos->mutex);
	lib_ringbuffer_destroy((lib_ringbuffer_t *)ctos->ringbuffer);
	
	stake_cmd_buf_detach(ctos);
	
	if(ctos->sockfd > 0)
	{
		close(ctos->sockfd);
		ctos->sockfd = -1;
	}
	
	lib_regular_rb_destroy((lib_regular_rb_t *)ctos->regular_rb);

	if(ctos != NULL)
	{
		free(ctos);
		ctos = NULL;
	}
	
	fprintf(stderr, "lib ctos init failed!\n");
	return LIB_CTOS_NULL;
}


int lib_ctos_destroy(lib_ctos_t *ctos)
{
	assert(ctos);

	lib_mutex_destroy((lib_mutex_t *)ctos->mutex);
	lib_ringbuffer_destroy((lib_ringbuffer_t *)ctos->ringbuffer);
	stake_cmd_buf_detach(ctos);
	
	if(ctos->sockfd > 0)
	{
		close(ctos->sockfd);
		ctos->sockfd = -1;
	}
	
	lib_regular_rb_destroy((lib_regular_rb_t *)ctos->regular_rb);

	if(ctos != NULL)
	{
		free(ctos);
		ctos = NULL;
	}
	
	return LIB_CTOS_EOK;
}

int lib_ctos_set_baudrate(lib_ctos_t *ctos, const unsigned int baud_rate)
{
	assert(ctos);

	return baud_rate_set(ctos);
}

int lib_ctos_get(lib_ctos_t *ctos, unsigned char *data, const unsigned int len)
{
	assert(ctos);
	assert(data);

	int ret = -1;
	lib_ringbuffer_t *rb = (lib_ringbuffer_t *)ctos->ringbuffer;
	if(rb == NULL)
		return LIB_CTOS_ERROR;

	lib_mutex_lock((lib_mutex_t *)ctos->mutex);
	//ret = lib_ringbuffer_get(rb, data, len);
	ret = lib_ringbuffer_getforce(rb, data, len);
	lib_mutex_unlock((lib_mutex_t *)ctos->mutex);
	
	return ret;
}

int lib_ctos_getchar(lib_ctos_t *ctos, unsigned char *ch)
{
	assert(ctos);

	int ret = -1;
	lib_ringbuffer_t *rb = (lib_ringbuffer_t *)ctos->ringbuffer;
	if(rb == NULL)
		return LIB_CTOS_ERROR;

	lib_mutex_lock((lib_mutex_t *)ctos->mutex);
	ret =  lib_ringbuffer_getchar(rb, ch);
	lib_mutex_unlock((lib_mutex_t *)ctos->mutex);
	
	return ret;
}

int lib_ctos_put(lib_ctos_t *ctos, const unsigned char dest_addr, unsigned char *data, const unsigned int len)
{
	assert(ctos);
	assert(data);

	int i;
	int ret = -1;
	unsigned char random = frame_sn_get(ctos);
	int send_bytes = 0;
	int frame_len = sizeof(can_frame_metadata_t);
	int frame_num = len / CAN_FRAME_LEN_MAX;
	if(len % CAN_FRAME_LEN_MAX != 0)
		frame_num += 1;

	can_frame_metadata_t meta;

	if(frame_num == 1) //单帧结构
	{
		memset(&meta, 0, sizeof(can_frame_metadata_t));
		
		meta.priority = CAN_PRIORITY_LOW;
		meta.s_addr = ctos->can_id;
		meta.d_addr = dest_addr;
		meta.frame_seq = 0;
		meta.can_dlc = CAN_FRAME_LEN_MAX;
		//meta.random = frame_sn_get(ctos);
		meta.random = random;
		meta.eff = 1; //扩展帧	
		meta.frame_mark = CAN_FRAME_MARK_SINGLE;

		//ret = lib_tcp_writen_select(ctos->sockfd, &meta, frame_len, 1000);
		ret = lib_writen(ctos->sockfd, &meta, frame_len);
		if(ret != frame_len)
			return LIB_CTOS_ERROR;
	}
	
	for(i = 0; i < frame_num; i++)
	{
		memset(&meta, 0, sizeof(can_frame_metadata_t));
		
		meta.priority = CAN_PRIORITY_LOW;
		meta.s_addr = ctos->can_id;
		meta.d_addr = dest_addr;
		meta.frame_seq = i;
		meta.can_dlc = CAN_FRAME_LEN_MAX;
		//meta.random = frame_sn_get(ctos);
		meta.random = random;
		meta.eff = 1; //扩展帧
			
		if(i == 0) //头帧
		{
			meta.frame_mark = CAN_FRAME_MARK_HEAD;
			memcpy(meta.data, data + i * CAN_FRAME_LEN_MAX, CAN_FRAME_LEN_MAX);
			send_bytes += CAN_FRAME_LEN_MAX;
		}
		else if(i == (frame_num - 1)) //尾帧
		{
			meta.frame_mark = CAN_FRAME_MARK_TAIL;
			memcpy(meta.data, data + i * CAN_FRAME_LEN_MAX, len - send_bytes); //不足8字节，补0
		}
		else //中间帧
		{
			meta.frame_mark = CAN_FRAME_MARK_MIDDLE;
			memcpy(meta.data, data + i * CAN_FRAME_LEN_MAX, CAN_FRAME_LEN_MAX);
			send_bytes += CAN_FRAME_LEN_MAX;
		}

		//ret = lib_tcp_writen_select(ctos->sockfd, &meta, frame_len, 500);
		ret = lib_writen(ctos->sockfd, &meta, frame_len);
		if(ret != frame_len)
			return LIB_CTOS_ERROR;

		lib_usleep(ctos->send_delay);
	}
	
	return LIB_CTOS_EOK;	
}

int lib_ctos_put_priority(lib_ctos_t *ctos, const unsigned char dest_addr, unsigned char *data, const unsigned int len, const unsigned char priority)
{
	assert(ctos);
	assert(data);

	int i;
	int ret = -1;
	unsigned char random = frame_sn_get(ctos);
	int send_bytes = 0;
	int frame_len = sizeof(can_frame_metadata_t);
	int frame_num = len / CAN_FRAME_LEN_MAX;
	if(len % CAN_FRAME_LEN_MAX != 0)
		frame_num += 1;

	can_frame_metadata_t meta;

	if(frame_num == 1) //单帧结构
	{
		memset(&meta, 0, sizeof(can_frame_metadata_t));
		
		meta.priority = priority;
		meta.s_addr = ctos->can_id;
		meta.d_addr = dest_addr;
		meta.frame_seq = 0;
		meta.can_dlc = CAN_FRAME_LEN_MAX;
		//meta.random = frame_sn_get(ctos);
		meta.random = random;
		meta.eff = 1; //扩展帧	
		meta.frame_mark = CAN_FRAME_MARK_SINGLE;

		//ret = lib_tcp_writen_select(ctos->sockfd, &meta, frame_len, 1000);
		ret = lib_writen(ctos->sockfd, &meta, frame_len);
		if(ret != frame_len)
			return LIB_CTOS_ERROR;
	}
	
	for(i = 0; i < frame_num; i++)
	{
		memset(&meta, 0, sizeof(can_frame_metadata_t));
		
		meta.priority = priority;
		meta.s_addr = ctos->can_id;
		meta.d_addr = dest_addr;
		meta.frame_seq = i;
		meta.can_dlc = CAN_FRAME_LEN_MAX;
		//meta.random = frame_sn_get(ctos);
		meta.random = random;
		meta.eff = 1; //扩展帧
			
		if(i == 0) //头帧
		{
			meta.frame_mark = CAN_FRAME_MARK_HEAD;
			memcpy(meta.data, data + i * CAN_FRAME_LEN_MAX, CAN_FRAME_LEN_MAX);
			send_bytes += CAN_FRAME_LEN_MAX;
		}
		else if(i == (frame_num - 1)) //尾帧
		{
			meta.frame_mark = CAN_FRAME_MARK_TAIL;
			memcpy(meta.data, data + i * CAN_FRAME_LEN_MAX, len - send_bytes);
		}
		else //中间帧
		{
			meta.frame_mark = CAN_FRAME_MARK_MIDDLE;
			memcpy(meta.data, data + i * CAN_FRAME_LEN_MAX, CAN_FRAME_LEN_MAX);
			send_bytes += CAN_FRAME_LEN_MAX;
		}

		//ret = lib_tcp_writen_select(ctos->sockfd, &meta, frame_len, 500);
		ret = lib_writen(ctos->sockfd, &meta, frame_len);
		if(ret != frame_len)
			return LIB_CTOS_ERROR;

		lib_usleep(ctos->send_delay);
	}
	
	return LIB_CTOS_EOK;	
}

static void *can_recv_thread(void *arg)
{
	int ret = -1;
	lib_ctos_t *ctos = (lib_ctos_t *)arg;
	struct can_frame cframe; //结构体can_frame是PF_CAN协议族套接字缓冲区的数组载体
	int len = sizeof(struct can_frame);
	
	while(1)
	{
		memset(&cframe, 0, sizeof(struct can_frame));
		
		ret = lib_readn_select(ctos->sockfd, &cframe, len, 3000);
		if(ret == len)
		{
			lib_regular_rb_put((lib_regular_rb_t *)ctos->regular_rb, (unsigned char *)&cframe, len);
		}	
	}

	return lib_thread_exit((void *)NULL);
}

static void *can_explain_thread(void *arg)
{
	int ret = -1;
	lib_ctos_t *ctos = (lib_ctos_t *)arg;
	can_frame_metadata_t can_metadata;
	int clen = sizeof(can_frame_metadata_t );
	
	while(1)
	{
		memset(&can_metadata, 0, sizeof(can_frame_metadata_t));
		
		ret = lib_regular_rb_get((lib_regular_rb_t *)ctos->regular_rb, (unsigned char *)&can_metadata, clen);
		if(ret == LIB_GE_EOK)
		{
			stake_cmd_buf_put(ctos, &can_metadata);
			//can_printf(&can_metadata);
		}
	}

	return lib_thread_exit((void *)NULL);
}





























