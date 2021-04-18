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
	char decrypted[2048];

	char *key = argv[1];
	FILE *inputfile = fopen(argv[2], "r");
	if(inputfile == NULL){
		printf("there is no such encrypted file.\n");
		return 0;
	}

	unsigned char prikey[1000000];
	strcat((char *)prikey, "-----BEGIN RSA PRIVATE KEY-----\n");
	for(int i = 0; i < strlen(key); i+= 64){
		strncat((char *)prikey, key+i, 64);
		strcat((char *)prikey, "\n");
	}
	strcat((char *)prikey, "-----END RSA PRIVATE KEY-----\n\0");
	
	fseek(inputfile, 0, SEEK_END);
	int size = 	ftell(inputfile);
	fseek(inputfile, 0, SEEK_SET);

	int count = fread(encrypted, sizeof(char), size, inputfile);
	encrypted[count] = '\0';

	RSA *rsa = NULL;
	BIO *keybio = BIO_new_mem_buf(prikey, -1);
	if (keybio==NULL){
		printf( "Failed to create key BIO.\n");
		return 0;
	}
	
	rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);
	if(rsa == NULL){
		printf( "Failed to create RSA.\n");
		return 0;
	}

	int res = RSA_private_decrypt(count, encrypted, (unsigned char *)decrypted, rsa, padding);
	
	FILE *outputfile = fopen(argv[3], "w");
	int count2 = fwrite(decrypted, sizeof(char), res, outputfile);


	fclose(outputfile);
	fclose(inputfile);
	printf("\nDecrypted message saved successfully.\nDecrypted msg len = %d\n", res);
	return 0;
}
