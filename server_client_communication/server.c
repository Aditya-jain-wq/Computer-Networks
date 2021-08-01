#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<unistd.h>


int main(int argc, char *argv[]){

	if(argc != 2){
		printf("command line argument not proper\n");
		return 0;
	}

	int port = atoi(argv[1]);
	struct sockaddr_in server = {0};
	struct sockaddr_in client = {0};
	char receive_msg[2048] = {0};
	char send_msg[2048] = {0};


	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);
	while(1){
		int socket_desc = socket(AF_INET, SOCK_STREAM, 0);
		if(socket_desc < -1){
			printf("Socket can't be created");
			return 1;
		}
		setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

		int binded = bind(socket_desc,(struct sockaddr *)&server,sizeof(server));

		// printf("binded:%d", binded);
		if(binded < 0){
			printf("not binded\n");
			return 1;
		}

		int listening = listen(socket_desc, 1);

		int len = sizeof(struct sockaddr_in);
		int accepted = accept(socket_desc, (struct sockaddr*) &client,(socklen_t*) &len);
		close(socket_desc);
		
		if(accepted < 0){
			printf("not accepting\n");
			return 1;
		}
		printf("connection established with client.\n");
		int count = recv(accepted, receive_msg, 2000, 0);
		receive_msg[count] = '\0';
		char temp;
		for(int i = 0; i < count/2; i++){
			temp = receive_msg[i];
			receive_msg[i] = receive_msg[count-1-i];
			receive_msg[count-1-i] = temp;
		}
		printf("%s\n", receive_msg);
		
		for(int i = 0; i < 2048; i++)
			send_msg[i] = '\0';

		printf("Enter line:");
		fgets(send_msg, 2000, stdin);
		send_msg[strlen(send_msg) - 1] = '\0';

		send(accepted, send_msg, strlen(send_msg), 0);
		
		close(accepted);
		shutdown(socket_desc, 0);
		shutdown(socket_desc, 1);
		shutdown(socket_desc, 2);
	}

	return 0;
}

