#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/semaphore.h>
#include <asm/uaccess.h>

//create a struct for fake_device
struct fake_device{
	char data[100];
	struct semaphore sem;
} virtual_device;

//standart declarations to register device
struct cdev *mcdev;
int major_number;
int ret;
dev_t dev_num;
#define DEVICE_NAME "fakedevice"

int device_open(struct inode *inode, struct file *filp)
{
	if(down_interruptible(&virtual_device.sem) != 0){
		printk(KERN_ALERT "fakedevice: could not lock device during open");
	        return -1;
	}
	printk(KERN_INFO "fakedevice: device opened");
	return 0;
}

ssize_t device_read(struct file* filp, char* bufStoreData, size_t bufCount, loff_t* curOffset)
{
	//take data from kernel space to user space
	//copy_to_user (dest., source, sizeToTransfer)
	printk(KERN_INFO "fakedevice: reading from device");
	ret = copy_to_user(bufStoreData, virtual_device.data, bufCount);
	return ret;
}

ssize_t device_write(struct file* filp, const char* bufStoreData, size_t bufCount, loff_t* curOffset)
{
	//write data from user to kernel
	//copy_from_user (dest., source, sizeToTransfer)
	printk(KERN_INFO "fakedevice: writting to device");
	ret = copy_from_user(bufStoreData, virtual_device.data, bufCount);
	return ret;
}

int device_close(struct inode *inode, struct file *filp)
{
	up(&virtual_device.sem);
	printk(KERN_INFO "fakedevice: closed device");
	return 0;
}

struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = device_open,
	.release = device_close,
	.write = device_write,
	.read = device_read
}

static int __init driver_entry(void)
{
	//register device with the system
	ret = alloc_chrdev_region(&dev_num,0,1,DEVICE_NAME);
	if(ret < 0){
		printk(KERN_ALERT "fakedevice: failed allocate a major number");
	        return ret;
	}
	major_number = MAJOR(dev_num);
	printk(KERN_ALERT "fakedevice: major number is %d", dev_num);
	printk(KERN_ALERT "\tuse \"mknod /dev/%s c %d 0\" for device file", DEVICE_NAME, major_number);
	mcdev = cdev_alloc();
	mcdev->ops = &fops;
	mcdev->owner = THIS_MODULE;
	//now cdev created and we need to add it to kernel
	ret = cdev_add(mcdev, dev_num, 1);
	if(ret < 0){
		printk(KERN_ALERT "fakedevice: unable to add cdev to kernel");
		return ret;
	}
	//initialize semaphore
	sama_init(&virtual_device.sem, 1); //initial value of one
	return 0;
}

static void __exit driver_exit(void)
{
	//unregister everything
	cdev_del(mcdev);
	unregister_chrdev_region(dev_num, 1);
	printk(KERN_ALERT "fakedevice: unloaded module");
}

module_init(driver_entry);
module_exit(driver_exit);
