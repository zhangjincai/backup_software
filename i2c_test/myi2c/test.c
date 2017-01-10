
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

int myprobe(struct i2c_client *cli, const struct i2c_device_id *id)
{
	printk("dev_name:%s,dev_addr:%x\n", cli->name,cli->addr);
	int status,i,j;
	unsigned char tmp,reg_addr,buffer[2];
	char data,buf[20];

	reg_addr  = 0x0;
	buffer[0] = 0x0;
	buffer[1] = 0x43;
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
	mdelay(10);
	//write buffer[1] to reg_addr buffer[0]
	if(2!= i2c_master_send(cli,buffer,2))	
	{
		printk("i2c_master_send failed ...\n");
		return -ENODEV;
	}
	printk("write 0x%x to 0x%x\n",buffer[1],buffer[0]);
	mdelay(10);
	//////////////read from reg_addr/////////////////
	for(j=0;j<18;j++)
	{
		reg_addr+=j;
                if( 1!= i2c_master_send(cli,&reg_addr,1))	
           	{
           		printk("write reg_addr failed...\n");
           		return -ENODEV;
           	}
           	mdelay(10);//wait
           	if(1!= i2c_master_recv(cli,&data,1))
           	{
           		printk("i2c_master_recv failed ...\n");
           		return -ENODEV;
           	}
           	mdelay(10);//wait
           	printk("%x ",data);
		reg_addr = 0x0;
	}
	printk("\n");

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

static const unsigned short normal_i2c[] = {0xa,I2C_CLIENT_END};

struct i2c_driver mydrv = {
	.probe = myprobe,
	.remove = myremove,
	
	.id_table = ids,

	.driver = {
		.name = "dev_driver",
		.owner = THIS_MODULE,
	},	
	
	.address_list = normal_i2c,
};

ssize_t myread(struct file *fl, char __user *buf, size_t len, loff_t *off)
{
	printk("in myread ...\n");
	//copy_to_user(buf,&reg_val,4);
	//return strlen(buf);
	return 0;
}
long myunlocked_ioctl(struct file *fl, unsigned int cmd, unsigned long arg)
{
	printk("in  myunlocked_ioctl....\n");
	return 0;
}
struct file_operations fops = {
	.owner = THIS_MODULE,
	.read  = myread,
	.unlocked_ioctl = myunlocked_ioctl,
};
struct miscdevice mydev = {
	.minor = MISC_DYNAMIC_MINOR,
	.fops  = &fops,
	.name  = "myi2c",
};

static int __init test_init(void)
{
	int ret;
	ret = misc_register(&mydev);
	return i2c_add_driver(&mydrv);
	printk("test init......\n");
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


