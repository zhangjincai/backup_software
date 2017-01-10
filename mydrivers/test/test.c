
#include <linux/init.h>
#include <linux/module.h>
#include <linux/usb.h>

int myprobe(struct usb_interface *intf,const struct usb_device_id *id)
{
	printk("in myprobe ...\n");
	return 0;
}

void mydisconnect(struct usb_interface *intf)
{
	printk("in mydisconnect ...\n");
}

struct usb_device_id ids[] = {
	//{USB_DEVICE_ID_MATCH_VENDOR|USB_DEVICE_ID_MATCH_PRODUCT, 0x0079, 0x0006},
	{USB_DEVICE(0x0079, 0x0006)},
	{USB_DEVICE(0x0079, 0x0007)},
	{},
};

struct usb_driver mydrv = {
	.name = "myusb_drv",
	.probe = myprobe,
	.disconnect = mydisconnect,

	.id_table = ids,
};

static int __init test_init(void)
{
	printk("test init.............\n");
	return usb_register(&mydrv);
}

static void __exit test_exit(void)
{
	printk("test exit.............\n");
	usb_deregister(&mydrv);
}

module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");
