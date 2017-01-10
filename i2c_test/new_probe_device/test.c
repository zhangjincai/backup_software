
#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>

//probe for device presence in a list of possible addresses


unsigned short addrs[] = {0x64, 0x1a, 0x33, 0x77, 0x22, 0x38, I2C_CLIENT_END};

static int __init test_init(void)
{
	struct i2c_client *client;
	struct i2c_adapter *adapter;
	struct i2c_board_info info;	

	adapter = i2c_get_adapter(3);
	
	memset(&info, 0, sizeof(info));
	strcpy(info.type, "test");

	i2c_new_probed_device(adapter, &info, addrs, NULL);
	
	printk("%x\n", info.addr);
	
	return 0;
}

static void __exit test_exit(void)
{
	printk("test exit....\n");
}

module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("zjc706859431@163.com");
MODULE_DESCRIPTION("i2c test driver");

