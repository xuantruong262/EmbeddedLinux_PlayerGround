#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

static struct gpio_desc *pin, *led;

#define GPIOE_BASE        0x6000D100
#define GPIO_CNF_OFFSET   0x00
#define GPIO_OE_OFFSET    0x10
#define GPIO_OUT_OFFSET   0x20
#define GPIO_IN_OFFSET    0x30

#define GPIO_BIT          6

static void __iomem *gpio_base;

int my_init(void){
  u32 val;
  
	printk("This is init CB function\n");
  
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
  
  // Set low initially
  iowrite32(0, gpio_base + GPIO_OUT_OFFSET);
//  iowrite32(1 << GPIO_BIT, gpio_base + GPIO_IN_OFFSET);
  
	return 0;
}

void my_exit(void){
  printk("This is exit CB function\n");
  // Set low initially
	iowrite32(1 << GPIO_BIT, gpio_base + GPIO_OUT_OFFSET);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
