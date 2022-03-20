#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

// Receives a message from a client.
void* recvMsg(void* sock)
{
	int my_socket = *((int*)sock);
	char *msg, buff[540];
	int len;

	while ((len = recv(my_socket, buff, 540, 0)) > 0) 
	{
	 	buff[len] = '\0';
        if((msg=(char *)malloc(len*sizeof(char)))==NULL){ 
            perror("allocation error");
            exit(EXIT_FAILURE);
        }
        strcpy(msg,buff);

		fputs(msg, stdout);
		free(msg);
		memset(buff, '\0', sizeof(buff));
	}
	pthread_exit(NULL);
}

int main(int argc, char* argv[])
{
	struct sockaddr_in server_addr;
	pthread_t snd_th, rcv_th;
	int my_socket, server_socket;
	int their_addr_size;
	int portno, usr_len, len;
	char buff[500], username[32];
	char welcome[70];
    char *msg;
	char ip[INET_ADDRSTRLEN];

	if (argc > 3) 
	{
		printf("too many arguments...");
		exit(1);
	}

	portno = atoi(argv[2]);
	strcpy(username, argv[1]);
    usr_len = strlen(username);

	my_socket = socket(AF_INET, SOCK_STREAM, 0); //cream socket pentru adresa IPv4 pe protocol TCP/IP												
	if (my_socket < 0)																	
	{
		perror("opening channel unsuccessful...");
		exit(1);
	}

	memset(server_addr.sin_zero, '\0', sizeof(server_addr.sin_zero));
	server_addr.sin_family = AF_INET;	// Seteaza IPv4.
	server_addr.sin_port = htons(portno);	// Seteaza numarul de port.
	server_addr.sin_addr.s_addr = inet_addr("192.168.0.101");   // Seteaza adresa IP.

	if (connect(my_socket, (struct sockaddr*) & server_addr, sizeof(server_addr)) < 0)			
	{
		perror("connection not esatablished...");
		exit(1);
	}

	inet_ntop(AF_INET, (struct sockaddr*) & server_addr, ip, INET_ADDRSTRLEN);
	printf("connected to %s, start chatting\n", ip);

	pthread_create(&rcv_th, NULL, recvMsg, &my_socket); //se creeaza un thread care sa se ocupe cu primirea mesajelor de la ceilalti clienti
	
	int msg_len;
	strcpy(welcome,username);
	strcat(welcome, " has entered the chat\n");
	msg_len=strlen(welcome);
	len = write(my_socket, &msg_len, sizeof(int));
		if (len < 0) 
		{
			perror("message not sent...");
			exit(1);
		}
	len = write(my_socket, welcome, strlen(welcome));
		if (len < 0) 
		{
			perror("message not sent...");
			exit(1);
		}

	int sw = 0;
	while ((msg_len=read(0, buff, 500)) > 0) 
	{
		if(msg_len == 500){
			if(!sw){
				printf("Your message was too long. Try to express yourself in less than 500 characters at a time\n");

			}
			memset(buff, '\0', sizeof(buff));
			sw=1;
		}
		else if(msg_len < 500 && sw)
		{
			memset(buff, '\0', sizeof(buff));
			sw=0;
		}
		else{
			msg_len = msg_len + usr_len + 3;
			//printf("%d\n", msg_len);

        	if((msg=(char *)malloc((msg_len)*sizeof(char)))==NULL){ 
        	    perror("allocation error");
        	    exit(EXIT_FAILURE);
        	}
			strcpy(msg, username);
			strcat(msg, ": ");
			strcat(msg, buff);

			len = write(my_socket, &msg_len, sizeof(int));
			if (len < 0) 
			{
				perror("message not sent...");
				exit(1);
			}

			len = write(my_socket, msg, strlen(msg));
			if (len < 0) 
			{
				perror("message not sent...");
				exit(1);
			}

			memset(buff, '\0', sizeof(buff));
			free(msg);
		}
		
	}

	pthread_join(rcv_th, NULL);
	close(my_socket);
}