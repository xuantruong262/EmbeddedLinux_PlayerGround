#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
int main(int num_arg , char**arg){
  int fd = open("/dev/mydev", O_RDWR);
  printf("Num of arg: %d\n", num_arg);
  if (fd < 0) {
    printf("Fail to open device file\n");
    perror("open");
    return -1;
  }
  if(num_arg > 1){
    if(strcmp(arg[1], "on") == 0){
      printf("Turn on LED\n");
      write(fd, "1", 1);
    }
    else{
      printf("Turn off LED\n");
      write(fd, "0", 1);
    }  
  }
  else{
    while(1){
      printf("Turn on LED\n");
      write(fd, "1", 1);
      usleep(300 * 1000);
      printf("Turn off LED\n");
      write(fd, "0", 1);
      usleep(300 * 1000);
    }
  }

  close(fd);
  return 0;
}