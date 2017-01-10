#include <stdio.h>
#include <string.h>

#define FN_DBG(fmt, args...)	fprintf(stderr, fmt, args)
#define SAE_REG_W					"aa"		//锁桩主动签到和节点机应答

int main(void)
{

	unsigned char id = 2;

	FN_DBG("fn_sae_reg:%s, id:%d\n", SAE_REG_W, id);

	return 0;
}
