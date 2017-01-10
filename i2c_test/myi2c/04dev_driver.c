
#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/io.h>
#include <mach/gpio.h>
#include <linux/gpio.h>

int myprobe(struct i2c_client *cli, const struct i2c_device_id *id)
{
	printk("in myprobe ... %s, %x\n", cli->name, cli->addr);
	
	int status,i,ret = -1;
	struct i2c_msg msgs[2];
	char reg_addr = 0x00;	
	char data = 0, rbuf[4] = {0xff,0xff,0xff,0xff};

	msgs[0].addr = cli->addr;//i2c设备的地址
	msgs[0].flags = 0; //write
	msgs[0].len = 1; //要发出的数据长度，多少字节
	msgs[0].buf = &data;
	
	ret = i2c_transfer(cli->adapter, &msgs[0], 1);
	printk("ret = %d\n,\n",ret);
	//if (i2c_transfer(cli->adapter, &msgs[0], 1) != 1)
	{
		printk("i2c_transfer0 failed ...\n");
		return -ENODEV;
	}
#if 0
	msgs[1].addr = cli->addr;//i2c设备的地址
	msgs[1].flags = 0; //write
	msgs[1].len = 1; //要发出的数据长度，多少字节
	msgs[1].buf = &data;
	if (i2c_transfer(cli->adapter, &msgs[1], 1) != 1)
	{
		printk("i2c_transfer1 failed ...\n");
		return -ENODEV;
	}
#endif
#if 0
  	// read reg_addr 0x30
	msgs[1].addr = cli->addr;
	msgs[1].flags = I2C_M_RD;
	msgs[1].len = 4;
	msgs[1].buf = &rbuf; 
	if (i2c_transfer(cli->adapter,  &msgs[1], 4) < 0)
	{
		printk("i2c_transfer1 failed ...\n");
		return -ENODEV;
	}
	
	for(i = 0; i < 4, i++)
	{
		printk("%02x ",rbuf[i]);
	}
	printk("\n");
#endif

	return 0;
}

int myremove(struct i2c_client *cli)
{
	printk("in myremove ...\n");
	return 0;
}


struct i2c_device_id ids[] = {
	{"sha204"},
	{},
};

static const unsigned short normal_i2c[] = {0x64,I2C_CLIENT_END};

struct i2c_driver mydrv = {
	.probe = myprobe,
	.remove = myremove,
	
	.id_table = ids,

	.driver = {
		.name = "sha204 driver",
		.owner = THIS_MODULE,
	},	
	
	.address_list = normal_i2c,
};

static int __init test_init(void)
{
	return i2c_add_driver(&mydrv);
	printk("test init......\n");
}

static void __exit test_exit(void)
{
	i2c_del_driver(&mydrv);
	printk("test exit....\n");
}

module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("zjc706859431@163.com");
MODULE_DESCRIPTION("sha204 driver");

