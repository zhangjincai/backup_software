#include "lib_list.h"
#include "fn_hash.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static fn_hash_t *g_fn_hash_ndev = NULL;			//哈希解释

struct fn_ndev_val
{
	unsigned char key[3];
	unsigned char data[32]; 
};
typedef struct fn_ndev_val fn_ndev_val_t;


/* 节点机签到 */
int fn_ndev_reg(fn_ndev_val_t *val)
{
	printf("in fn_ndev_reg func ...\n");
	
	printf("val->data[0] = %d\n", val->data[0]);
	printf("val->key: %s\n", val->key);
}

int test(fn_ndev_val_t *val)
{
	printf("in test func ...\n");
	
	printf("val->data[0] = %d\n", val->data[0]);
	printf("val->key: %s\n", val->key);
}

int main(void)
{
	int i;
	
	fn_hash_val_t *val[2];
	for(i = 0; i < 2; i++)
	{
		val[i] = malloc(sizeof(fn_hash_val_t));
		if(val[i] == NULL){
			printf("malloc val[%d] failed!\n", i);
			return -1;
		}
	}
		
#if 1
	strcpy(val[0]->key, "AB"); 
	val[0]->data[0] = 88;
	
	strcpy(val[1]->key, "BD"); 
	val[1]->data[0] = 99;
	
	/* 哈希函数 */
	g_fn_hash_ndev = fn_hash_create(2); //申请内存空间和初始化链表
	if(g_fn_hash_ndev == NULL)
	{
		printf("fn_hash_create failed!\n");		
		return -2;
	}
	
	fn_hash_register(g_fn_hash_ndev, "AB", fn_ndev_reg);	//节点机签到
	fn_hash_register(g_fn_hash_ndev, "BD", test);	
	
	for(i = 0; i < 2; i++)
	{
		fn_hash_run(g_fn_hash_ndev, val[i]); //指令运行
	}
	
	
	fn_hash_destroy(g_fn_hash_ndev); //内存释放
#endif

	return 0;
}











