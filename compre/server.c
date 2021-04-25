#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h> 

void* lenadena(void* param);

int main(int argc, char *argv[]){

	if(argc != 2){
		printf("command line argument not proper\n");
		return 0;
	}

	int port = atoi(argv[1]);
	struct sockaddr_in server = {0};
	struct sockaddr_in client = {0};

	int socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_desc < -1){
		printf("Socket can't be created");
		return 1;
	}

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);

	int binded = bind(socket_desc,(struct sockaddr *)&server,sizeof(server));

	if(binded < 0){
		printf("not binded");
		return 1;
	}

	listen(socket_desc, 2);

	int len = sizeof(struct sockaddr_in);
	while(1){
		int accepted1 = accept(socket_desc, (struct sockaddr*) &client,(socklen_t*) &len);
		if(accepted1 < 0){
			printf("not accepting.\n");
			return 1;
		}
		printf("Connection established with first client. Waiting for second...\n");
		
		int accepted2 = accept(socket_desc, (struct sockaddr*) &client,(socklen_t*) &len);
		if(accepted2 < 0){
			printf("not accepting.\n");
			return 1;
		}
		printf("Connection established with both clients.\n");

		int accepted[3] = {accepted1, accepted2, accepted1};

		pthread_t tid1, tid2;
		pthread_attr_t attr; 
		pthread_attr_init(&attr);
		pthread_create(&tid1,&attr,lenadena, accepted);
		pthread_create(&tid2,&attr,lenadena, accepted+1);
		pthread_join(tid1, NULL);
		pthread_join(tid2, NULL);
		printf("Clients Left.\n");
		close(accepted1);
		close(accepted2);
	}
	
	return 0;
}


void* lenadena(void* param){
	int accepted1 = *((int *)param);
	int accepted2 = *(((int *)param)+1);
	char msg[2048] = {0};
	int count = 0;
	while((count = recv(accepted1, msg, 2048, 0)) > 0){
		msg[count] = '\0';
		send(accepted2, msg, count, 0);
	}	
	pthread_exit(0);
}


