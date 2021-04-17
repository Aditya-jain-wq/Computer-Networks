#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<unistd.h>


int main(){
	struct sockaddr_in server = {0};
	char send_msg[127] = {0};
	char receive_msg[127] = {0};

	int socket_desc, port;
	char ip_addr[20];
	if((socket_desc = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		printf("Socket can't be created");
		return 1;
	}

	printf("Enter Ip address:");
	scanf("%s", ip_addr);
	printf("Enter Port:");
	scanf("%d", &port);

	server.sin_addr.s_addr = inet_addr(ip_addr); //Local Host//change
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	int connected = connect(socket_desc,(struct sockaddr *)&server,sizeof(struct sockaddr_in));
	if(connected < 0){
		printf("Server is unreachable!! Exiting, Bye!\n");
		return 1;
	}
	printf("Sucessfully conected with server\n");

	while(1){
		printf("Enter filename:");
		for(int i = 0; i < 127; i++)
			send_msg[i] = '\0';
		scanf("%s", send_msg);
		
		if(strcmp(send_msg, "exit") == 0)
			break;

		send(socket_desc, send_msg, strlen(send_msg), 0);

		FILE *f = fopen(send_msg, "w");

		recv(socket_desc, receive_msg, 11, 0);
		fprintf(f, "%s", receive_msg);
		fclose(f);
		printf("New file received and created successfully\n");
	}
	close(socket_desc);
	shutdown(socket_desc, 0);
	shutdown(socket_desc, 1);
	shutdown(socket_desc, 2);
	return 0;	
}


