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

struct i2c_client *client;
int myprobe(struct i2c_client *cli, const struct i2c_device_id *id)
{
	//printk("dev_name:%s,dev_addr:0x%x\n", cli->name,cli->addr);
	int status,i;
	//////////////power on///////////
	for(i=0;i<5;i++)
	{
            status = gpio_request(GPIOG(2),NULL);
            if(status == 0) break;//success
            else //failed
                printk("request GPIOG(2) failed...\n");
	}
	if(i == 5) return -1;//failed
    	sw_gpio_setcfg(GPIOG(2),1);//set as output
    	gpio_set_value(GPIOG(2),1);
	printk("power on success.....\n");

	client = cli;
	return 0;
}
int myremove(struct i2c_client *cli)
{
	printk("in myremove ...\n");
	return 0;
}

struct i2c_device_id ids[] = {
	{"hello"},
	{},
};
struct i2c_driver mydrv = {
	.probe = myprobe,
	.remove = myremove,
	.id_table = ids,
	.driver = {
		.name = "dev_driver",
	},	
};

int myopen(struct inode *ino, struct file *fl)
{
	printk("in myopen....\n");

	char addr = 0,data;
	i2c_master_send(client,&addr,1);
	i2c_master_recv(client,&data,1);

	printk("data is:0x%x\n",data);
	return 0;
}
ssize_t i2c_read(struct file *fl, char __user *buf, size_t len, loff_t *off)
{
	unsigned char data;
	printk("in i2c read ...\n");

	copy_from_user(&data,buf,strlen(buf));//先取出要读的寄存器地址	
	printk("read reg_addr:%x\n",data);
	if(1!= i2c_master_send(client,&data,1))	//写入寄存器地址
	{
		printk("i2c_master_send1 failed ...\n");
		return -ENODEV;
	}
        mdelay(10);//wait
	if(1!= i2c_master_recv(client,&data,1))//读寄存器的值
        {
        	printk("i2c_master_recv failed ...\n");
        	return -ENODEV;
        }
        printk("read data:%x",data);
	copy_to_user(buf,&data,sizeof(data));

	return strlen(buf);
}
ssize_t i2c_write(struct file *fl, const char __user *buf, size_t len, loff_t *off)
{
	unsigned char data[2];

//	copy_from_user(data,buf,strlen(buf));//取出要写入的寄存器地址和值
	copy_from_user(data,buf,sizeof(buf));//取出要写入的寄存器地址和值
	printk("write data[0]:%x data[1]:%x\n",data[0],data[1]);
	if(2!= i2c_master_send(client,data,2))	
	{
		printk("i2c_master_send failed ...\n");
		return -ENODEV;
	}
        mdelay(10);//wait

	return strlen(buf);
}
struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = myopen,
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

	printk("test init.....\n");
	return ret;
}

static void __exit test_exit(void)
{
	gpio_free(GPIOG(2));
	i2c_del_driver(&mydrv);
	misc_deregister(&mydev);

	printk("test exit....\n");
}

module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");


