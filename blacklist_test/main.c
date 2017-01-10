#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#include "lib_general.h"
#include "universal_file.h"
#include "lib_blacklist.h"



#define SQLITE3_PATHNAME		"/opt/universal/blacklist.db" //"/mnt/blacklist/blacklist.db"
#define BLACKLIST_TOTAL_PATH    "./blacklist_total.txt"




lib_bl_t *g_bl_a = NULL;

#define BL_NUM		4096

int main(int argc, char *argv[])
{
	fprintf(stderr, "Software Compiled Time: %s, %s.\r\n",__DATE__, __TIME__);

	//int i;
	int ret = -1;
	int idx = 0;
#if 0
	idx_array_t array;
	inc_idx_array_t inc;
	total_idx_array_t total;
	memset(&array, 0, sizeof(idx_array_t));
	memset(&inc, 0, sizeof(inc_idx_array_t));
	memset(&total, 0, sizeof(total_idx_array_t));
#endif

	//lib_begin_runtime_printf();

	g_bl_a = lib_bl_create_v2(SQLITE3_PATHNAME);
	if(g_bl_a == LIB_BL_NULL)
	{
		fprintf(stderr, "black list create failed!\n");
		return 0;
	}

#if 1 
	FILE *bl_file;
	char line_buf[64] = {0};
	const char *delim = "	";
	char *buf = NULL;
	//unsigned char hex[8] = {0};
	char *pid = NULL, *lid = NULL, *type = NULL;

	bl_file = fopen(BLACKLIST_TOTAL_PATH, "r");
	if(NULL == bl_file)
	{
		printf("fopen failed!\n");
	}

	//黑名单字符串处理
	while(NULL != fgets(line_buf, sizeof(line_buf), bl_file))
	{	
		idx++;
		printf("\nidx:%d, %s", idx, line_buf);
		//if(idx > 3) break;
#if 1
		buf = strtok(line_buf, delim);
		if(NULL != buf)
		{
			//printf("pid:%s\n", buf); //pid
			//lib_str_to_hex((unsigned char *)buf, strlen((const char *)buf), pid);
			//lib_printf("pid1", pid, 8);
			pid = buf;
	
			buf = strtok(NULL, delim);
			if(NULL != buf)
			{
				//printf("lid:%s\n", buf); //lid
				//lib_str_to_hex((unsigned char *)buf, strlen((const char *)buf), lid);
				//lib_printf("lid1", lid, 8);
				//printf("strlen(lid):%d\n", strlen(buf));
				lid = buf;
				
				buf = strtok(NULL, delim);
				if(NULL != buf)
				{
					//printf("type:%s\n", buf); //type
					//strcpy(&type, &buf);   
					type = buf;
				}  
			}  
#if 1	
			//printf("pid:%s\n", pid); //pid
			//printf("lid:%s\n", lid); //lid
			//printf("type:%s\n", type); //pid
			//插入数据库
			ret = lib_bl_insert_data_v2(g_bl_a, pid, lid, type);
			//printf("lib_bl_insert_data, ret:%d\n", ret);
			if(ret != 0)
			{
				printf("insert data error!\n");
				break;
			}
#endif
		}
		   #endif
	}
#endif

#if 0
	//数据库查询
	char *s_lid = "5100001000002541";
	ret = lib_bl_select_data_v2(g_bl_a, s_lid);
	printf("lib_bl_select_data, ret:%d\n", ret);
	
	//lib_end_runtime_printf();

	//记录条数查询
	unsigned int cnt = 0;
	ret = lib_bl_count_v2(g_bl_a, &cnt);
	printf("lib_bl_count_v2, cnt:%d\n", cnt);
	
	//最大记录数查询
	unsigned int max = 0;
	ret = lib_bl_max_v2(g_bl_a, &max);
	printf("lib_bl_max_v2, max:%d\n", max);

	//黑名单类型更新
	//ret = lib_bl_update_data_v2(g_bl_a, s_lid, "A");
	printf("lib_bl_update_data_v2, ret:%d\n", ret);

	//黑名单删除
	//ret = lib_bl_delete_table_any_v2(g_bl_a, s_lid);
	printf("lib_bl_delete_table_any_v2, ret:%d\n", ret);

	//删除表
	//ret = lib_bl_delete_table_v2(g_bl_a);
	printf("lib_bl_delete_table_v2, ret:%d\n", ret);

	//黑名单类型替换
	char *slid = "5100000641192374";  
	ret = lib_bl_replace_data_v2(g_bl_a, slid ,"B"); //测试未成功
	printf("lib_bl_replace_data_v2, ret:%d\n", ret);
#endif
	
#if 0
#if 0
while(1)
{	
	inc.total_seq = INC_IDX_ARRAY_SZ;
	
	for(i = 0; i < INC_IDX_ARRAY_SZ; i++)
	{
		inc.cnt_seq = i;
		lib_set_bit(inc.idx_array, i);
	}

	total.total_seq = TOTAL_IDX_ARRAY_SZ;
	for(i = 0; i < TOTAL_IDX_ARRAY_SZ; i++)
	{
		total.cnt_seq = i;
		lib_set_bit(total.idx_array, i);
	}


	//memcpy(&array.inc, &inc, sizeof(inc_idx_array_t));
	//ret = lib_bl_update_idx_array(g_bl_a, &array);	
	//fprintf(stderr, "1update ret = %d\n", ret);

	memcpy(&array.total, &total, sizeof(inc_idx_array_t));
	ret = lib_bl_update_idx_array(g_bl_a, &array);	
	fprintf(stderr, "2update ret = %d\n", ret);

	idx_array_t array1;
	memset(&array1, 0, sizeof(idx_array_t));
	ret = lib_bl_select_idx_array(g_bl_a, &array1);
	fprintf(stderr, "inc select ret = %d\n", ret);
	fprintf(stderr, "inc.total_seq = %d\n", array1.inc.total_seq);
	fprintf(stderr, "inc.cnt_seq = %d\n", array1.inc.cnt_seq);

	fprintf(stderr, "total select ret = %d\n", ret);
	fprintf(stderr, "total.total_seq = %d\n", array1.total.total_seq);
	fprintf(stderr, "total.cnt_seq = %d\n", array1.total.cnt_seq);

	memset(&inc, 0, sizeof(inc_idx_array_t));
	memset(&total, 0, sizeof(total_idx_array_t));
	

	for(i = 0; i < INC_IDX_ARRAY_SZ; i++)
	{
		if(lib_chk_bit(array1.inc.idx_array, i) != 1)
		{
			fprintf(stderr, "lib_chk_bit NOT 1:  %d\n", i);
		}
	}

	for(i = 0; i < TOTAL_IDX_ARRAY_SZ; i++)
	{
		if(lib_chk_bit(array1.total.idx_array, i) != 1)
		{
			fprintf(stderr, "total: lib_chk_bit NOT 1:  %d\n", i);
		}
	}

	//lib_sleep(1);
	lib_msleep(500);
}
#endif

	univ_file_st_t univ;
	memset(&univ, 0, sizeof(univ_file_st_t));

	for(i = 0; i < UNIV_FILE_LEN; i++)
	{
		univ.data[i] = i;
	}

	lib_begin_runtime_printf();

#if 0
	for(i = 1; i <= BL_NUM; i++)
	{
		univ.length = BL_NUM;
		univ.hd.file_seq = 0001;
		univ.hd.total = BL_NUM;
		univ.hd.div_seq = i;

		ret = lib_bl_replace_data(g_bl_a, UFC_TOTAL_BL, i, &univ);
		//fprintf(stderr, "ret = %d\n", ret);
	}
#endif

	for(i = 1; i <= BL_NUM; i++)
	{
		lib_bl_select_data(g_bl_a, UFC_TOTAL_BL, i, &univ);

		if(univ.hd.div_seq != i)
			fprintf(stderr, "div_seq error!\n");

		fprintf(stderr, "univ.hd.div_seq = %d\n", univ.hd.div_seq);
		
		memset(&univ, 0, sizeof(univ_file_st_t));
	
	}
	
	lib_end_runtime_printf();
#endif

	//fclose(bl_file);

	lib_bl_destroy_v2(g_bl_a);

	fprintf(stderr, "Software Quit!\n");

	
	return 0;
}





