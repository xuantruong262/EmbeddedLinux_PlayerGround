#include<linux/module.h>
#include<linux/init.h>


int my_init(void){
	printk("This is init CB function\n");
	return 0;
}

void my_exit(void){
        printk("This is exit CB function\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
