
#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>

struct i2c_board_info info = {"sha204", 0, 0x64};

static int __init test_init(void)
{
	struct i2c_client *client;
	struct i2c_adapter *adapter;
	
	adapter = i2c_get_adapter(3);
	client = i2c_new_device(adapter, &info);
	i2c_put_adapter(client->adapter);//释放i2c总线

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
MODULE_DESCRIPTION("sha204 test driver");
