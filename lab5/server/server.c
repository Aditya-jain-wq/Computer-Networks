#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<unistd.h>


int main(int argc, char *argv[]){
	struct sockaddr_in server = {0};
	struct sockaddr_in client = {0};
	char receive_msg[127] = {0};
	char send_msg[127] = {0};

	int socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_desc < -1){
		printf("Socket can't be created");
		return 1;
	}

	int port;
	printf("Give Port:");
	scanf("%d", &port);
	// server = {0};
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);
	fflush(stdout);
	int binded = bind(socket_desc,(struct sockaddr *)&server,sizeof(server));

	// printf("binded:%d", binded);
	if(binded < 0){
		printf("not binded");
		return 1;
	}

	listen(socket_desc, 3);

	while(1){
		int len = sizeof(struct sockaddr_in);
		int accepted = accept(socket_desc, (struct sockaddr*) &client,(socklen_t*) &len);
		if(accepted < 0){
			printf("not accepting");
			return 1;
		}
		printf("connection established with client.\n");
		int count;
		while((count = recv(accepted, receive_msg, 127, 0)) > 0){
			receive_msg[count] = '\0';
			printf("The filename received is:%s\n", receive_msg);
			if(access( receive_msg , F_OK ) == 0){
				FILE *f = fopen(receive_msg, "r");
				int i;
				for(int i = 0; i < 10; i++){
					send_msg[i] = fgetc(f);
					if(send_msg[i] == EOF){
						send_msg[i] = '\0';
						break;
					}
				}
				send_msg[10] = '\0';
				fclose(f);
				if(send(accepted, send_msg, 11, 0) < 0){
					printf("sending failed! exiting");
					return 1;
				}
			}
			else{
				send_msg[0] = '\0';
				if(send(accepted, send_msg, 11, 0) < 0){
					printf("sending failed! exiting");
					return 1;
				}
			}
		}
		close(accepted);
	}
	return 0;
	
	
	
}

