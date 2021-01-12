#include<stdio.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<string.h>

#define PORT 9001
#define CHUNK_SIZE 256


int main()
{
	int socketId, client;

	struct sockaddr_in server;

	int addLen = sizeof(server);

	char fileName_toSend[1024] = {0};

	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = INADDR_ANY;

	//socket(DOMAIN, TYPE, PROTOCOL) and returns socket FD
	socketId = socket(AF_INET, SOCK_STREAM, 0);
	if(socketId == 0){
		perror("Error in socket creation");
		exit(EXIT_FAILURE);
	}

	//Binding port with address
	//Takes socket FD, server structure in generic form (protocol independent) and size of address(server variable in this case)
	int bind_val;
	bind_val = bind(socketId, (struct sockaddr *)&server, sizeof(server));
	if(bind_val < 0){
		perror("Error in binding");
		exit(EXIT_FAILURE);
	}

	//Lets wait for a client(s)
	//listen(socketFD, backlogs)
	//backlogs means maximum number of connections in waiting queue, eg if backlogs = 1, then while serving a connection, if a new connection comes then it will sit in waiting queue if another one comes it will be dropped out,
	//If backlogs = 0, then while serving a connection if another connection comes it will be dropped out
	int listen_val;
	listen_val = listen(socketId, 0);
	if(listen_val < 0){
		perror("Error in listening");
		close(socketId);
		exit(EXIT_FAILURE);
	}
	printf("Server ready\n");

	//Accepting connection
	client = accept(socketId, (struct sockaddr *)&server, (socklen_t *)&addLen);
	if(client < 0){
		perror("Error in accepting connection");
		close(socketId);
		exit(EXIT_FAILURE);
	}

	printf("Client Connected \n");
	read(client, fileName_toSend, 1024);
	printf("Client wants file: %s\n", fileName_toSend);

	FILE *filePointer;
	filePointer = fopen(fileName_toSend, "rb");
	if(filePointer == NULL){
		char flag[] = "N";
		send(client, flag, sizeof(flag), 0);
	}
	else{
		char flag[] = "Y";
		send(client, flag, sizeof(flag), 0);
	}
	if(filePointer == NULL){
		perror("Error in opening file");
		close(socketId);
		exit(EXIT_FAILURE);
	}
	
	size_t bytesRead = 0;
	char buffer[CHUNK_SIZE];
	while((bytesRead = fread(buffer, 1, CHUNK_SIZE, filePointer)) > 0)
	{
		send(client, buffer, bytesRead, 0);
	}

	printf("File sent!\n");

	close(socketId);
}