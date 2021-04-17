#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<unistd.h>
#include<netdb.h>
#include<openssl/ssl.h>
#include<openssl/err.h>

void oldHTTP(char message[], char filename[], int socket_desc);
void newHTTPS(char message[], char filename[], int socket_desc);

int main(int argc, char *argv[]){
	if(argc != 2){
		printf("command line argument not proper\n");
		return 0;
	}

	char *site = argv[1];
	int count = 0, i = (site[4]=='s'?8:7);
	for(; site[i+count] != '/'; count++);
	
	char host[count+1], path[strlen(site)-i-count+10];
	for(int j = 0; j < count; j++)
		host[j] = site[i+j];
	host[count] = '\0';
	
	i += count, count = 0;
	for(int j = 0; site[i+count]; count++, j++)
		path[j] = site[i+count];
	
	path[count] = '\0';
	
	count = 0;
	i = strlen(site)-1;
	for(; site[i-count] != '/'; count++);

	char filename[count+1];
	for(int j = 1; j <= count; j++)
		filename[j-1] = site[i-count+j];
	filename[count] = '\0';

	// -------extracted host, file, path

	char message[4096];

	sprintf(message, "GET %s%s%s%s", path, " HTTP/1.1\r\nHost: ", host, "\r\nConnection: Close\r\n\r\n");
	
	struct sockaddr_in server = {0};

	int socket_desc, port = (site[4]=='s'?443:80); // https:443, http:80

	if((socket_desc = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("Socket can't be created");
		return 1	;
	}
	
	struct hostent *hostent = gethostbyname(host);
	if (hostent == NULL){
		printf("No such url exist. No such host. Exiting, Bye!\n");
		return 1;
	}

	// server.sin_addr.s_addr = inet_addr(ip_addr); //Local Host//change
	memcpy(&server.sin_addr.s_addr,hostent->h_addr,hostent->h_length);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	
	int connected = connect(socket_desc,(struct sockaddr *)&server,sizeof(struct sockaddr_in));
	if(connected < 0){
		printf("Server is unreachable!! Exiting, Bye!\n");
		return 1;
	}
	printf("Sucessfully conected with server\n");
	
	
	site[4]=='s'?newHTTPS(message, filename, socket_desc) : oldHTTP(message, filename, socket_desc);

	close(socket_desc);
	shutdown(socket_desc, 0);
	shutdown(socket_desc, 1);
	shutdown(socket_desc, 2);

	return 0;
	
}

void oldHTTP(char message[], char filename[], int socket_desc){
	
	char receive_msg[100010] = {0};

	if( send(socket_desc , message , strlen(message) , 0) < 0){
		puts("Send failed");
		exit(1);
	}

	remove(filename); // deletes existing file
	FILE *f = fopen(filename, "ab");
	if(f == NULL){
		printf("Error while opening file.\n");
		exit(1);
	}

	int count = 0;
	if((count = recv(socket_desc, receive_msg, 100000, 0)) > 0){
		if(receive_msg[9] != '2' || receive_msg[10] != '0' || receive_msg[11] != '0'){
			printf("Didn't got a 200 OK response. Following is header received. Exiting Bye!!.\n\n");
			printf("%s", receive_msg);
			remove(filename);
			exit(1);
		}
		
		int i = 4;
		while(receive_msg[i-4] != '\r' || receive_msg[i-3] != '\n' || receive_msg[i-2] != '\r' || receive_msg[i-1] != '\n')
			i++;

		fwrite(receive_msg+i , count-i , 1, f);
		receive_msg[i] = '\0';
		printf("HTTP response header:\n\n%s", receive_msg);
	}
	
	while((count = recv(socket_desc, receive_msg, 100000, 0)) > 0){
		fwrite(receive_msg , count , 1, f);
	}

	printf("reply received.\n");
	fclose(f);
	
}

void newHTTPS(char message[], char filename[], int socket_desc){

	SSL* ssl;
	SSL_CTX* ctx;

	char receive_msg[100010] = {0};

	SSL_load_error_strings();
    SSL_library_init();
    OpenSSL_add_all_algorithms();

	ctx = SSL_CTX_new(SSLv23_client_method());
	if (ctx == NULL){
		printf("ctx is null.\n");
		exit(1);
	}
	ssl = SSL_new (ctx);
	if(!ssl){
		printf("Error creating ssl.\n");
		exit(1);
	}

	SSL_set_fd(ssl, socket_desc);

	if(SSL_connect(ssl) <= 0){
		printf("Error while creating ssl connection.\n");
		exit(1);
	}
	printf("SSL connected.\n");


	if(SSL_write(ssl, message, strlen(message)) <= 0){
		puts("Send failed");
		exit(1);
	}

	remove(filename); // deletes existing file
	FILE *f = fopen(filename, "ab");
	if(f == NULL){
		printf("Error while opening file.\n");
		exit(1);
	}

	int count = 0;
	if((count = SSL_read(ssl, receive_msg, 100000)) > 0){
		if(receive_msg[9] != '2' || receive_msg[10] != '0' || receive_msg[11] != '0'){
			printf("Didn't got a 200 OK response. Following is header received. Exiting Bye!!.\n\n");
			printf("%s", receive_msg);
			remove(filename);
			exit(1);
		}
		int i = 4;
		while(receive_msg[i-4] != '\r' || receive_msg[i-3] != '\n' || receive_msg[i-2] != '\r' || receive_msg[i-1] != '\n')
			i++;

		fwrite(receive_msg+i , count-i , 1, f);
		receive_msg[i] = '\0';
		printf("HTTP response header:\n\n%s", receive_msg);
	}

	while((count = SSL_read(ssl, receive_msg, 100000)) > 0){
		fwrite(receive_msg , count , 1, f);
	}
	
	printf("reply received.\n");
	fclose(f);
    SSL_CTX_free(ctx);
}