
#include <assert.h>
#include "escape.h"

#define NDEV_HD					0xF1
#define NDEV_TAIL				0xF2
#define NDEV_ESC_F0H			0xF0	//转义字符
#define NDEV_ESC_00H			0x00
#define NDEV_ESC_01H			0x01
#define NDEV_ESC_02H			0x02


/*
 * 转义解析-- 下行:返回转义后数据长度
 */
int __downlink_escape(unsigned char *dest, unsigned char *src, const int slen)
{
	assert(dest);
	assert(src);
	
	int idxI = 1, idxJ = 1;

	while(idxI <= (slen - 2))
	{
		if(src[idxI] == NDEV_ESC_F0H)
		{
			if(src[idxI + 1] == 0x01)
				dest[idxJ] = NDEV_HD;
			else if(src[idxI + 1] == 0x02)
				dest[idxJ] = NDEV_ESC_F0H;
			else if(src[idxI + 1] == 0x00)
				dest[idxJ] = NDEV_TAIL;

			idxI++;
		}
		else
			dest[idxJ] = src[idxI];

		idxI++;
		idxJ++;
	}

	dest[0] = NDEV_HD;
	dest[idxJ++] = NDEV_TAIL;

	return idxJ;
}

/*
 * 转义解析-- 上行:返回转义后数据长度
 */
int __uplink_escape(unsigned char *dest, unsigned char *src, const int slen)
{
	assert(dest);
	assert(src);	

	int idxI = 1, idxJ = 1;
	
	while(idxI <= (slen - 2))
	{
		switch(src[idxI])
		{
			case NDEV_HD:
			{
				dest[idxJ++] = NDEV_ESC_F0H;
				dest[idxJ++] = NDEV_ESC_01H;	
			}
			break;

			case NDEV_TAIL:
			{
				dest[idxJ++] = NDEV_ESC_F0H;
				dest[idxJ++] = NDEV_ESC_00H;				
			}
			break;

			case NDEV_ESC_F0H:
			{
				dest[idxJ++] = NDEV_ESC_F0H;
				dest[idxJ++] = NDEV_ESC_02H;					
			}
			break;
			
			default:
				dest[idxJ++] = src[idxI];
		}

		idxI++;
	}

	dest[0] = NDEV_HD;
	dest[idxJ++] = NDEV_TAIL;
		
	return idxJ;
}


