#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

const char* pipe1 = "/tmp/fifo_client";
const char* pipe2 = "/tmp/fifo_server";

int main(){
	mkfifo(pipe1, S_IFIFO|0666);
	mkfifo(pipe2, S_IFIFO|0666);
	int c;
	if(c = fork()){
		int fd = open(pipe1, O_CREAT|O_WRONLY);
		char msg[255];
		size_t count;
		while(1){
			scanf("%s", msg);
			if(strcmp(msg, "exit") == 0){
				kill(c, SIGKILL);
				close(fd);
				exit(0);
			}
			write(fd, msg, strlen(msg));
		}
	}
	else{
		int fd = open(pipe2, O_RDONLY);
		char msg[255];
		int count = 0;
		while(1){
			count = 0;
			while(count == 0)
				count = read(fd, msg, sizeof(msg));
			msg[count] = '\0';
			printf("%s\n", msg);
			fflush(stdout);
		}
		close(fd);
	}
	return 0;
}