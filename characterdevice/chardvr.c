/*
 *chrdev legacy
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/cdev.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/ioctl.h>

#define  BUF_LEN    100
#define  WR_VALUE   _IOW('a', 'a', int32_t *)
#define  RD_VALUE   _IOR('a', 'b', int32_t *)

static   dev_t 	    dev = 0;
static   int 	    major;
static   char       chrdev_buf[BUF_LEN];
static   char 	    buf[BUF_LEN];
static   long 	    checksum = 0;
static   struct     proc_dir_entry * ent;
static   struct     class * dev_class;
static   struct     cdev ioctl_cdev;

static ssize_t myread(struct file *file, char __user *ubuf, size_t count, 
		      loff_t *ppos)
{
	int i, len;
	len      = 0;
	checksum = 0;
	
	if (*ppos > 0 || count < BUF_LEN)
		return 0;

	for (i = 0; i < strlen(chrdev_buf); i++)
		checksum = (checksum * checksum) ^ chrdev_buf[i];
	
	len += sprintf(buf, "checksum = %ld\n", checksum);

	if (copy_to_user(ubuf, buf, len))
		return -EFAULT;
	*ppos = len;
	return len;
}

static ssize_t chrdev_read(struct file *filp,
	char __user *buf, size_t count, loff_t *ppos)
{
	int ret;

	if (*ppos + count >= BUF_LEN)
		count = BUF_LEN - *ppos;

	ret = copy_to_user(buf, chrdev_buf + *ppos, count);
	if (ret < 0)
		return ret;

	*ppos += count;
	return count;
}

static ssize_t chrdev_write(struct file *filp, const char __user *buf, size_t count, loff_t *ppos)
{

	int ret;
	if (*ppos + count >= BUF_LEN)
		count = BUF_LEN - *ppos;

	ret = copy_from_user(chrdev_buf + *ppos, buf, count);
	if (ret < 0)
		return ret;
	
	*ppos += count;
	return count;
}

static long ioctl_ioctl(struct file *file, int unsigned cmd, unsigned long arg)
{
	int i;
	switch (cmd) {
	case WR_VALUE:
		//clear the character buffer
		for (i = 0;i < 100;i++)
			chrdev_buf[i]=0;
		break;
	case RD_VALUE:
		copy_to_user((long*) arg, &checksum, sizeof(checksum));
		arg = checksum;
		break;
	}
	return 0;
}

static int chrdev_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int chrdev_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static int ioctl_open(struct inode *inode, struct file *file)
{
	return 0;
}

static int ioctl_release(struct inode *inode, struct file *file)
{
	return 0;
}

struct file_operations chrdev_fops = { 
	.owner 	 	= 	THIS_MODULE,
	.read		= 	chrdev_read,
	.write 		= 	chrdev_write,
	.open 	 	= 	chrdev_open,
	.release 	= 	chrdev_release
};

struct file_operations fops = { 
	.owner          = 	THIS_MODULE,
	.open           = 	ioctl_open,
	.unlocked_ioctl = 	ioctl_ioctl,
	.release 	= 	ioctl_release,
};

static struct file_operations myops = { 
	.owner 		= 	THIS_MODULE,
	.read 		= 	myread,
};

static int __init chrdev_init(void)
{
	int ret;
	ret = register_chrdev(0, "chrdev", &chrdev_fops);

	if (ret < 0) {
		return ret;
	}
	major = ret;
	pr_info("got major %d\n", major);
	ent = proc_create("apertus", 0777, NULL, &myops);
	printk(KERN_ALERT "inside kernel space :)\n");

	//allocating for the IOCTL
	if ((alloc_chrdev_region(&dev, 0, 1, "ioctl_Dev")) < 0) {
		printk(KERN_INFO "Cannot allocate major number\n");
		return -1;
	}

	printk(KERN_INFO " IOCTL : Major = %d Minor = %d \n", MAJOR(dev), MINOR(dev));
	cdev_init(&ioctl_cdev, &fops);
	if ((cdev_add(&ioctl_cdev, dev, 1)) < 0) {
		printk(KERN_INFO "Cannot add the device to the system\n");
		goto r_class;
	}
	
	
	if ((dev_class = class_create(THIS_MODULE, "ioctl_class")) == NULL) {
		printk(KERN_INFO "Cannot create the struct class\n");
		goto r_class;
	}

	if ((device_create(dev_class, NULL, dev, NULL, "ioctl_device")) == NULL) {
		printk(KERN_INFO "Cannot create the Device 1\n");
		goto r_device;
	}

	printk(KERN_INFO "Device Driver Insert...Done!!!\n");
	return 0;

r_device:
	class_destroy(dev_class);
r_class:
	unregister_chrdev_region(dev, 1);
	return -1;

}

static void __exit chrdev_exit(void)
{
	unregister_chrdev(major, "chrdev");
	proc_remove(ent);
	printk(KERN_WARNING "Exiting\n");
	device_destroy(dev_class, dev);
	class_destroy(dev_class);
	cdev_del(&ioctl_cdev);
	unregister_chrdev_region(dev, 1);
	printk(KERN_INFO "Device Driver Remove...Done!!!\n");
}

module_init(chrdev_init);
module_exit(chrdev_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pratyush");
MODULE_DESCRIPTION("chrdev apertus kernel challenge task 1 T884");
