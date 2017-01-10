#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/io.h>
#include <mach/gpio.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/fs.h>

int myprobe(struct i2c_client *cli, const struct i2c_device_id *id)
{
	printk("dev_name:%s,dev_addr:%x\n", cli->name,cli->addr);

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

ssize_t i2c_read(struct file *fl, char __user *buf, size_t len, loff_t *off)
{
	unsigned int data[1] = {0x22};
	printk("in i2c read ...\n");
	//copy_to_user(buf,data,sizeof(data));

	return strlen(buf);
}
ssize_t i2c_write(struct file *fl, const char __user *buf, size_t len, loff_t *off)
{
	unsigned int data[10];

	//copy_from_user(data,buf,strlen(buf));

	printk("write data:%x\n",*data);
	return strlen(buf);
}
struct file_operations fops = {
	.owner = THIS_MODULE,
	.read  = i2c_read,
	.write = i2c_write,
};
struct miscdevice mydev = {
	.minor = MISC_DYNAMIC_MINOR,
	.fops  = &fops,
	.name  = "myi2c",
};

static int __init test_init(void)
{
	int ret;
	ret = i2c_add_driver(&mydrv);
	ret = misc_register(&mydev);

	printk("test init......\n");
	return ret;
}

static void __exit test_exit(void)
{
	//gpio_free(GPIOG(2));
	i2c_del_driver(&mydrv);
	misc_deregister(&mydev);

	printk("test exit....\n");
}

module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("zjc706859431@163.com");
MODULE_DESCRIPTION("sha204 driver");

