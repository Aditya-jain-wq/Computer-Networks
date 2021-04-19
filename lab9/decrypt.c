#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/err.h>

int main(int argc, char *argv[]){

	if(argc != 4){
		printf("Command line argument not proper\n");
		return 0;
	}

	int padding = RSA_PKCS1_PADDING;
	unsigned char encrypted[1000000];
	char decrypted[2048];

	FILE *keyfile = fopen(argv[1], "rb");
	if(keyfile == NULL){
		printf("there is no such file for key.\n");
		return 0;
	}
	FILE *inputfile = fopen(argv[2], "r");
	if(inputfile == NULL){
		printf("there is no such encrypted file.\n");
		return 0;
	}

	fseek(inputfile, 0, SEEK_END);
	int size = 	ftell(inputfile);
	fseek(inputfile, 0, SEEK_SET);

	int count = fread(encrypted, sizeof(char), size, inputfile);
	encrypted[count] = '\0';

	RSA *rsa = RSA_new();

	rsa = PEM_read_RSAPrivateKey(keyfile, &rsa, NULL, NULL);
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
