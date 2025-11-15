#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/ipc.h>

#define SHM_NAME "/myshm"

int main() {
    int fd;
    char *ptr;

    // Create shared memory object
    // fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    // ftruncate(fd, 1024);

    // Map into memory
    // ptr = mmap(0, 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    // strcpy(ptr, "Hello from POSIX server!");
    // printf("Server wrote: %s\n", ptr);
	// while(1){
		// sleep(20);
	// }
    // munmap(ptr, 1024);
    // close(fd);
    shm_unlink(SHM_NAME);
    return 0;
}