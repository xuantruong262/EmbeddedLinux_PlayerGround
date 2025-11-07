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
//For interupt
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/irqdesc.h>
#include <linux/gpio.h>
// Device file var
static dev_t dev;
static struct cdev my_cdev;
static struct class *cls;
// GPIO output var
static struct gpio_desc *pin;
// Interupt var
static unsigned int irq_number;
static unsigned int irq_count = 0;

// GPIO pin macro
#define GPIOE_BASE        0x6000D100
#define GPIOZ_BASE        0x6000D640
#define PZ0_PINMUX_ADDR   0x700008D4

#define GPIO_CNF_OFFSET   0x00
#define GPIO_OE_OFFSET    0x10
#define GPIO_OUT_OFFSET   0x20
#define GPIO_IN_OFFSET    0x30

#define GPIO_OUPUT_BIT          6
#define GPIO_PZ6_NUM          200

static void __iomem *gpio_base;
static void __iomem *gpio_ip_base;
void __iomem *reg;
// ---------- Device File interface function ----------
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
    iowrite32(1 << GPIO_OUPUT_BIT, gpio_base + GPIO_OUT_OFFSET);
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

static struct file_operations fops = {
//    .open = my_open,
//    .release = my_close,
    .read = my_read,
    .write = my_write,
};


// ---------- Interrupt handler (Top Half) ----------
static irqreturn_t gpio_irq_handler(int irq, void *dev_id)
{
    irq_count++;
    printk(KERN_INFO "gpio_irq_demo: Interrupt! count=%u\n", irq_count);
//    iowrite32(0x0, gpio_ip_base + GPIO_OUT_OFFSET);
//    iowrite32(0x0, gpio_ip_base + GPIO_IN_OFFSET);
    return IRQ_HANDLED;
}

// ---------- Kernel module initial function ----------
int my_init(void){
  u32 val;
  int ret;
	printk("Initialize kernel module\n");
//// Create device file ////
  alloc_chrdev_region(&dev, 0, 1, "mydev");
  cdev_init(&my_cdev, &fops);
  cdev_add(&my_cdev, dev, 1);
  
  cls = class_create(THIS_MODULE, "myclass");
  device_create(cls, NULL, dev, NULL, "mydev");
  
  printk(KERN_INFO "Driver loaded. Major=%d Minor=%d\n", MAJOR(dev), MINOR(dev));
//// Config GPIO PE6 as a output pin ////
  // Map GPIO_E registers into virtual address space
  gpio_base = ioremap(GPIOE_BASE, 0x20);
  if (!gpio_base) {
      pr_err("ioremap failed\n");
      return -ENOMEM;
  }
  // Configure bit PE6 as output
  val = ioread32(gpio_base + GPIO_CNF_OFFSET);
  val |= (1 << GPIO_OUPUT_BIT);  // 1 = output
  iowrite32(val, gpio_base + GPIO_CNF_OFFSET);
  // Drive bit PE6
  val = ioread32(gpio_base + GPIO_OE_OFFSET);
  val |= (1 << GPIO_OUPUT_BIT);
  iowrite32(val, gpio_base + GPIO_OE_OFFSET);
 
  // Set low initially
  iowrite32(0, gpio_base + GPIO_OUT_OFFSET);
//// Config PZ0 as a input interupt pin ////
  // Request GPIO
  ret = gpio_request(GPIO_PZ6_NUM, "gpio_irq_input");
  if (ret) {
    printk(KERN_ERR "gpio_irq_demo: Failed to request GPIO\n");
    return ret;
  }

  // Configure as input
  gpio_ip_base = ioremap(GPIOZ_BASE, 0x20);
  // Drive input to pulldown as default
  gpio_direction_input(GPIO_PZ6_NUM);
  iowrite32(0x0, gpio_ip_base + GPIO_OUT_OFFSET);
  
  irq_number = gpio_to_irq(GPIO_PZ6_NUM);
  printk(KERN_INFO "gpio_irq_demo: Mapped GPIO %d to IRQ %d\n", GPIO_PZ6_NUM, irq_number);
  ret = request_irq(irq_number, gpio_irq_handler, IRQF_TRIGGER_RISING/* | IRQF_TRIGGER_FALLING*/, "gpio_irq_demo", NULL);
  if (ret) {
    printk(KERN_ERR "gpio_irq_demo: Cannot register IRQ\n");
    gpio_free(GPIO_PZ6_NUM);
    return ret;
  }
      
  reg = ioremap(PZ0_PINMUX_ADDR, 4);
  if (!reg)
      return -ENOMEM;

  val = readl(reg);

  // clear pull bits [1:0] and set to pull-down (01)
  val &= ~0x3;
  val |= 0x1;        // pull-down

  // enable input (bit 4)
  val |= (1 << 4);

  writel(val, reg);
  iounmap(reg);

  pr_info("PZ0 pull-down and input enabled\n");
	return 0;
}
// ---------- Kernel module exit function ----------
void my_exit(void){
  printk("This is exit CB function\n");
  //Destroy device file
  device_destroy(cls, dev);
  class_destroy(cls);
  cdev_del(&my_cdev);
  unregister_chrdev_region(dev, 1);
  printk(KERN_INFO "Driver unloaded\n");
  // Destroy interupt
  free_irq(irq_number, NULL);
  gpio_free(GPIO_PZ6_NUM);
  
  // Set low initially
//	iowrite32(1 << GPIO_OUPUT_BIT, gpio_base + GPIO_OUT_OFFSET);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
