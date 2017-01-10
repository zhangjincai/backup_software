#include <stdio.h>
#include <pthread.h>

int main(void)
{
	typedef struct lnt_notify_ptr
	{
		pthread_mutex_t *mutex;
		pthread_cond_t *cond;
		unsigned int notify;		
	}lnt_notify_ptr_t;
	
	lnt_notify_ptr_t *ptr = malloc(sizeof(lnt_notify_ptr_t));
	printf("sizeof(ptr):%d\n",sizeof(*ptr));
	
	ptr->mutex = malloc(sizeof(pthread_mutex_t));
	printf("sizeof(ptr->mutex):%d\n",sizeof(*(ptr->mutex)));
	printf("sizeof(ptr):%d\n",sizeof(*ptr));

	ptr->cond = malloc(sizeof(pthread_cond_t));
	printf("sizeof(ptr->cond):%d\n",sizeof(*(ptr->cond)));
	printf("sizeof(ptr):%d\n",sizeof(*ptr));


	unsigned char id[6] = "test11";
	printf("strlen:%d\n",strlen(&id));

	free(ptr->cond);
	free(ptr->mutex);
	free(ptr);

	return 0;
}
