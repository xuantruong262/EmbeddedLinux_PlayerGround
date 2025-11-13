#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

void signal_handler(int sig) {
    printf("Caught signal %d\n", sig);
}
int main(int num_arg , char**arg){
	signal(SIGINT,signal_handler);
	while(1)
	{
		printf("Running...\n");
		sleep(1);
		signal(SIGINT,SIG_IGN);
	}
	return 0;
}