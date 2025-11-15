#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define SHM_NAME "/myshm"

int main() {
    int fd;
    char *ptr;

    // Open shared memory object
    fd = shm_open(SHM_NAME, O_RDONLY, 0666);

    ptr = mmap(0, 1024, PROT_READ, MAP_SHARED, fd, 0);

    printf("Client read: %s\n", ptr);

    munmap(ptr, 1024);
    close(fd);
    return 0;
}