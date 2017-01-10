
#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/io.h>
#include <mach/gpio.h>
#include <linux/gpio.h>
#include <linux/delay.h>

int myprobe(struct i2c_client *cli, const struct i2c_device_id *id)
{
	printk("in myprobe ...%s, %x\n", cli->name, cli->addr);
	int status,i;
	unsigned char reg_addr,buffer[2];
	char data,*buf;

	reg_addr  = 0x10;
	buffer[0] = 0x10;
	buffer[1] = 0x53;

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
//	gpio_direction_output(GPIOG(2),1);//powr on
	mdelay(10);
	//write
	if( 2!= i2c_master_send(cli,buffer,2))	
	{
		printk("i2c_master_send1 failed ...\n");
		return -ENODEV;
	}
	mdelay(10);
	//////////////read
	if( 1!= i2c_master_send(cli,&reg_addr,1))	
	{
		printk("i2c_master_send2 failed ...\n");
		return -ENODEV;
	}
	mdelay(10);
	if( 1!= i2c_master_recv(cli,&data,1))
	{
		printk("i2c_master_recv failed ...\n");
		return -ENODEV;
	}
	printk("data:%x\n",data);
//	sprintf(buf,"%x\n",data);	

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

static int __init test_init(void)
{
	return i2c_add_driver(&mydrv);
	printk("test init......\n");
}

static void __exit test_exit(void)
{
	gpio_free(GPIOG(2));
	i2c_del_driver(&mydrv);
	printk("test exit....\n");
}

module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");


