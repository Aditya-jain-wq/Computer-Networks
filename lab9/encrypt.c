#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/err.h>

int main(int argc, char *argv[]){

	if(argc != 4){
		printf("Command line argument not proper\n");
		return 0;
	}

	int padding = RSA_PKCS1_PADDING;
	unsigned char encrypted[1000000];
	char msg[2048];

	char *key = argv[1];
	FILE *inputfile = fopen(argv[2], "r");
	if(inputfile == NULL){
		printf("there is no such inputfile.\n");
		return 0;
	}

	unsigned char pubkey[1000000];
	strcat((char *)pubkey, "-----BEGIN PUBLIC KEY-----\n");
	for(int i = 0; i < strlen(key); i+= 64){
		strncat((char *)pubkey, key+i, 64);
		strcat((char *)pubkey, "\n");
	}
	strcat((char *)pubkey, "-----END PUBLIC KEY-----\n\0");
	
	fseek(inputfile, 0, SEEK_END);
	int size = 	ftell(inputfile);
	fseek(inputfile, 0, SEEK_SET);

	int count = fread(msg, sizeof(char), size, inputfile);
	msg[count] = '\0';

	RSA *rsa = NULL;
	BIO *keybio = BIO_new_mem_buf(pubkey, -1);
	if (keybio==NULL){
		printf( "Failed to create key BIO.\n");
		return 0;
	}
	
	rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa, NULL, NULL);
	if(rsa == NULL){
		printf( "Failed to create RSA.\n");
		return 0;
	}

	int res = RSA_public_encrypt(count, (unsigned char*)msg, encrypted, rsa, padding);

	FILE *outputfile = fopen(argv[3], "w");
	int count2 = fwrite(encrypted, sizeof(char), res, outputfile);


	fclose(outputfile);
	fclose(inputfile);
	printf("\nEncrypted Message saved successfully.\nEncrypted msg len = %d\n", res);
	return 0;
}


// gcc -o decrypt decrypt.c -lssl -lcrypto 