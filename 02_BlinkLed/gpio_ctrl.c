#include<linux/module.h>
#include<linux/init.h>
#include<linux/gpio/consumer.h>

static struct gpio_desc *pin, *led;

#define IO_PIN 38
#define IO_BUTTON 0x1

#define IO_OFFSET 0

int my_init(void){
	int status = 0;
	printk("This is init CB function\n");
	pin = gpio_to_desc(IO_PIN + IO_OFFSET);
	if(!pin){
		printk("Error desc Led pin\n");
		return -ENODEV;
	}

	status = gpiod_direction_output(pin, 0);
	if(status){
    printk("Fail to direct to led pin\n");
    return status;
  }
	return 0;
}

void my_exit(void){
        printk("This is exit CB function\n");
	gpiod_set_value(pin,1);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
