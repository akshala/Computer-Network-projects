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
	int sock = 0;
	struct sockaddr_in server;

	//Specify which protocols to used 
	server.sin_family = AF_INET;     //For IPV4
	server.sin_port = htons(PORT);   //Host to Network Short
	server.sin_addr.s_addr = INADDR_ANY;   //Remote host IP

	//For using on different machines 
	//server.sin_addr.s_addr = inet_addr("192.168.43.112"); 

	//Create socket FD
	//Returns -ve if error >= 0 otherwise known as socket FD
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0){
		perror("Error in socket creation");
		close(sock);
		exit(EXIT_FAILURE);
	}
	/*
	SOCK_STREAM : Data delivery guaranteed, ordered always
	           Uses TCP, if cannot deliver then receive error
	           Slow
	           
	SOCK_DGRAM  : Opposite that of SOCK_STREAM, fast
	*/
	//Plug in to the socket(Connect to the server)
	int connect_val;
	connect(sock, (struct sockaddr * )&server, sizeof(server));
	if(connect_val < 0){
		perror("Error in connection");
		close(sock);
		exit(EXIT_FAILURE);
	}

	printf("You are now connected to server\n");

	printf("Enter file name: ");
	char fileName_toSend[1024];
	scanf("%s", fileName_toSend);

	send(sock, fileName_toSend, strlen(fileName_toSend), 0);

	printf("File name sent!\nNow Reading \n");

	char flag[2];
	int flag_size = read(sock, flag, 2);
	// flag[flag_size] = 0;
	if(strcmp(flag, "N") == 0){
		printf("No such file exists on server\n");
		close(sock);
		exit(0);
	}

	FILE *filePointer;
	char fileName[] = "downloaded_";
	strcat(fileName, fileName_toSend);
	filePointer = fopen(fileName, "wb");
	if(filePointer == NULL){
		perror("Error in opening file");
		close(sock);
		exit(EXIT_FAILURE);
	}

	char buffer[CHUNK_SIZE];
	size_t bytesRead = 0;
	while((bytesRead = read(sock, buffer, CHUNK_SIZE)) > 0){
		fwrite(buffer, bytesRead, 1, filePointer);
	}

	printf("Now Closing \n");

	//Close the socket so that port can be reused by other process
	//It is not 
	close(sock);
}
