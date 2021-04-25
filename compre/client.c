#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <openssl/pem.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int socket_desc = 0;
int const padding = RSA_PKCS1_PADDING;

void* upload(void* param);

int main(int argc, char *argv[]){

	if(argc != 5){
		printf("Command line argument not proper\n");
		return 0;
	}

	int port;
	char* ip_addr;
	struct sockaddr_in server = {0};

	if((socket_desc = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		printf("Socket can't be created");
		return 1;
	}

	ip_addr = argv[1];
	port = atoi(argv[2]);

	server.sin_addr.s_addr = inet_addr(ip_addr); //Local Host//change
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	int connected = connect(socket_desc,(struct sockaddr *)&server,sizeof(struct sockaddr_in));
	if(connected < 0){
		printf("Either connection rejected or server is unreachable!! Exiting, Bye!\n");
		return 1;
	}
	printf("Sucessfully conected with server\n");

	pthread_t tid;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
	pthread_create(&tid, &attr, upload, argv[4]);

	char msg[4096];
	unsigned char encrypted[10000];

	FILE* decrypt_key = fopen(argv[3], "rb");
	if(decrypt_key == NULL){
		printf("There is no such file for decryption key.\n");
		return 0;
	}

	RSA *rsa = RSA_new();
	rsa = PEM_read_RSAPrivateKey(decrypt_key, &rsa, NULL, NULL);
	if(rsa == NULL){
		printf( "Failed to create RSA.\n");
		return 0;
	}
	
	int count = 0;
	while((count = recv(socket_desc, encrypted, 10000, 0)) > 0){
		encrypted[count] = '\0';
		
		int res = RSA_private_decrypt(count, encrypted, (unsigned char *)msg, rsa, padding);

		if(strcmp(msg, "exit") == 0)
			break;

		printf("ciphertext:%s\nplaintext:%s\n", encrypted, msg);
	}
	close(socket_desc);
	fclose(decrypt_key);
	shutdown(socket_desc, 0);
	shutdown(socket_desc, 1);
	shutdown(socket_desc, 2);
	exit(0);
}


void* upload(void* param){
	char msg[4096];
	unsigned char encrypted[10000];

	FILE* encrypt_key = fopen((char*)param, "rb");
	if(encrypt_key == NULL){
		printf("there is no such file for key.\n");
		return 0;
	}
	
	RSA *rsa = RSA_new();
	rsa = PEM_read_RSA_PUBKEY(keyfile, &rsa, NULL, NULL);
	if(rsa == NULL){
		printf( "Failed to create RSA.\n");
		return 0;
	}

	while(1){
		fgets(msg, 2000, stdin);
		msg[strlen(msg) - 1] = '\0';

		int res = RSA_public_encrypt(strlen(msg), (unsigned char*)msg, encrypted, rsa, padding);

		send(socket_desc, encrypted, strlen(encrypted), 0);

		if(strcmp(msg, "exit") == 0)
			break;
	}

	close(socket_desc);
	fclose(encrypt_key);
	shutdown(socket_desc, 0);
	shutdown(socket_desc, 1);
	shutdown(socket_desc, 2);
	exit(0);
}


