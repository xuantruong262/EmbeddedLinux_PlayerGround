#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
//For device file
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
//C stanard
#include <linux/string.h>
//#include <stdio.h>

static dev_t dev;
static struct cdev my_cdev;
static struct class *cls;

static struct gpio_desc *pin;

#define GPIOE_BASE        0x6000D100
#define GPIO_CNF_OFFSET   0x00
#define GPIO_OE_OFFSET    0x10
#define GPIO_OUT_OFFSET   0x20
#define GPIO_IN_OFFSET    0x30

#define GPIO_BIT          6

static void __iomem *gpio_base;
//File operation interface
//////////////////////Device File interface function////////////////////////
static ssize_t my_write(struct file *f, const char __user *buf, size_t len, loff_t *off) {
  printk("Here is a write function\n");
  
  char kbuf[128];
  memset(kbuf,0,128);
  // Get string data
  copy_from_user(kbuf, buf, len);
  printk("Data: %s\n",kbuf);
  printk("Len: %d\n",len);
  // Convert to integer
  int val;
  kstrtoint(kbuf, 10, &val);
  printk("Number = %d\n", val);
  //val = val > 0 ? 1 : val;

  if(!val){
    printk("Turn off LED\n");
    iowrite32(1 << GPIO_BIT, gpio_base + GPIO_OUT_OFFSET);
  }
  else {
    printk("Turn on LED\n");
    iowrite32(0, gpio_base + GPIO_OUT_OFFSET);
  }

  return len;
}
static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off) {
  printk("Here is a read function\n");
  return 0;
}
//=================================================================/////////
static struct file_operations fops = {
//    .open = my_open,
//    .release = my_close,
    .read = my_read,
    .write = my_write,
};

int my_init(void){
  u32 val;
  
	printk("Initialize kernel module\n");
  // Create device file
  alloc_chrdev_region(&dev, 0, 1, "mydev");
  cdev_init(&my_cdev, &fops);
  cdev_add(&my_cdev, dev, 1);
  
  cls = class_create(THIS_MODULE, "myclass");
  device_create(cls, NULL, dev, NULL, "mydev");
  
  printk(KERN_INFO "Driver loaded. Major=%d Minor=%d\n", MAJOR(dev), MINOR(dev));
  
  // Map GPIO_E registers into virtual address space
  gpio_base = ioremap(GPIOE_BASE, 0x20);
  if (!gpio_base) {
      pr_err("ioremap failed\n");
      return -ENOMEM;
  }
  // Configure bit 6 as output
  val = ioread32(gpio_base + GPIO_CNF_OFFSET);
  val |= (1 << GPIO_BIT);  // 1 = output
  iowrite32(val, gpio_base + GPIO_CNF_OFFSET);
  // Drive bit 6
  val = ioread32(gpio_base + GPIO_OE_OFFSET);
  val |= (1 << GPIO_BIT);
  iowrite32(val, gpio_base + GPIO_OE_OFFSET);
//  
//  // Set low initially
  iowrite32(0, gpio_base + GPIO_OUT_OFFSET);

	return 0;
}

void my_exit(void){
  printk("This is exit CB function\n");
  //Destroy device file
  device_destroy(cls, dev);
  class_destroy(cls);
  cdev_del(&my_cdev);
  unregister_chrdev_region(dev, 1);
  printk(KERN_INFO "Driver unloaded\n");
  
  // Set low initially
//	iowrite32(1 << GPIO_BIT, gpio_base + GPIO_OUT_OFFSET);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
