#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>   
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#define BUFSIZE  100

char buf[BUFSIZE];
char buf2[BUFSIZE];
static int size=0;
static struct proc_dir_entry *ent;
static ssize_t mywrite(struct file *file, const char __user *ubuf,size_t count, loff_t *ppos) 
{
	int c;
	if(*ppos > 0 || count > BUFSIZE)
		return -EFAULT;
	if(copy_from_user(buf2,ubuf,count))
		return -EFAULT;
	c = strlen(buf2);
	printk(KERN_ALERT "strlen === %d",c);
	size=c;
	printk(KERN_ALERT "mywrite-The INPUT==== %s",buf2);
	*ppos = c;
	return c;
}

static ssize_t myread(struct file *file, char __user *ubuf,size_t count, loff_t *ppos) 
{
	int len=0,i;
	long unsigned int checksum = 0;
	printk(KERN_ALERT "myread-entery-The INPUT==== %s",buf2);
	if(*ppos > 0 || count < BUFSIZE)
		return 0;
	for(i=0;i<size;i++){
		checksum=(checksum*checksum)^buf2[i];
	}
	printk(KERN_ALERT "The i=%d",i);
	len += sprintf(buf,"checksum = %ld\n",checksum);	
	if(copy_to_user(ubuf,buf,len))
		return -EFAULT;
	printk(KERN_ALERT "The output==== %s",buf);
	*ppos = len;
	return len;
}

static struct file_operations myops = 
{
	.owner = THIS_MODULE,
	.read = myread,
	.write = mywrite,
};

static int simple_init(void)
{
	ent=proc_create("apertus",0777,NULL,&myops);
	printk(KERN_ALERT "inside kernel space :)\n");
	return 0;
}

static void simple_cleanup(void)
{
	proc_remove(ent);
	printk(KERN_WARNING "Exiting\n");
}
module_init(simple_init);
module_exit(simple_cleanup);
