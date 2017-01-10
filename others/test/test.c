#include <stdio.h>
#include <string.h>

int str_to_hex(unsigned char *str_data, unsigned char *hex_data, int str_len)
{
    int i,j,temp_len;
    int tempData;
    unsigned char *str_buf = str_data;
    unsigned char *hex_buf = hex_data;
    
    for(i=0; i < str_len; i++)
    {
        if((str_buf[i] >= '0') && (str_buf[i] <= '9'))         
        {
          tempData = str_buf[i] - '0';
        }
        else if((str_buf[i] >= 'A') && (str_buf[i] <= 'F'))    
        {
          tempData = str_buf[i] - 0x37;
        }
        else if((str_buf[i] >= 'a') && (str_buf[i] <= 'f'))   
        {
          tempData = str_buf[i] - 0x57;
        }
        else
        {
          return 0;         //illegal data
        }
        str_buf[i] = tempData;
    }
    
    for(temp_len = 0, j = 0; j < i; j += 2)
    {
        hex_data[temp_len++] = (str_buf[j] << 4) | str_buf[j+1];
    }
    return temp_len;
}

int main(void)
{
	int i,size = 0;
	unsigned char hex[] = {0x01,0x02,0x03,0x04,0x05}, hdata[32] = {0}, sdata[32] = {0};
	//unsigned char string[] = "010700a4000002ddf190000200";	
	//unsigned char string[] = "0205c4fe00000090000300";	
	//unsigned char string[] = "030500b095080890000000";	
	//unsigned char string[] = "040700a4000002adf16a810400";	
	//unsigned char string[] = "050700a4000002adf390000500";	
	unsigned char data[] = "[\"010f47000e6f0a84085041592e41505059\",\"020f47000e41ac4603be53b92110010184\",\"030b47000a5100000401132338\",\"040f47000e6f0a84085041592e4d463158\",\"05394700386f3484085041592e5449434ca5289f0801009f0c21ffffffffffffffff000000000000000000000000000000002015070900000186a0\"]";
	printf("strlen = %d\n",strlen(data));
#if 1
	printf("%s\n",data);

	size = str_to_hex(data, hdata, strlen(data));
	for(i = 0; i < size; i++)
		printf("%02x",hdata[i]);
	printf("\n");
#endif
}

