client: client.c server
	gcc -o client client.c -lpthread -lrt -lssl -lcrypto

server: server.c
	gcc -o server server.c -lpthread -lrt


clean:
	rm client server

