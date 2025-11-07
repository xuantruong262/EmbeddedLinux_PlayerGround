obj-m += corss_compile.o

all:
	make -C ${JETSON_KERNEL_HDR} M=$(PWD) modules
clean:
	make -C ${JETSON_KERNEL_HDR} M=$(PWD) clean
