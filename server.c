#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "/home/davidvid/ourChat/serv_com.c"


int main(int argc, char* argv[])
{
	struct sockaddr_in server_addr, client_addr;
	struct client_info cl;
	socklen_t client_addr_size;
	pthread_t snd_th, rcv_th;
	int server_socket,client_socket,portno,len;
	char msg[500];
	char ip[INET_ADDRSTRLEN];

	if (argc > 2) 
	{
		perror("too many arguments...");
		exit(1);
	}
	
	portno = atoi(argv[1]);
	server_socket = socket(AF_INET, SOCK_STREAM, 0);//cream socket pentru adresa IPv4 pe protocol TCP/IP											
	if (server_socket < 0)																				
	{
		perror("opening channel unsuccessful...");
		exit(1);
	}

	server_addr.sin_family = AF_INET;																	
	server_addr.sin_port = htons(portno);																
	server_addr.sin_addr.s_addr = inet_addr("192.168.0.105");												
	client_addr_size = sizeof(client_addr);

	if (bind(server_socket, (struct sockaddr*) & server_addr, sizeof(server_addr)) != 0)
	{
		perror("binding unsuccessful...");
		exit(1);
	}

	if (listen(server_socket, 5) != 0)																
	{
		perror("listening unsuccessful...");
		exit(1);
	}

	while (1) 
	{
		if ((client_socket = accept(server_socket, (struct sockaddr*) & client_addr, &client_addr_size)) < 0)	// Accepts a connection request - blocking call.
		{
			perror("accept unsuccessful...");
			exit(1);
		}
	
		pthread_mutex_lock(&mutex);
		inet_ntop(AF_INET, (struct sockaddr*) & client_addr, ip, INET_ADDRSTRLEN);
		
		printf("%s connected...\n", ip); //adaugam noul client in vectorul de clienti

		cl.sockno = client_socket; 
		strcpy(cl.ip, ip);
		clients[no_clients] = client_socket;
		no_clients++;
		
		pthread_create(&rcv_th, NULL, recvMsg, &cl); //cream un thread care sa se ocupe de respectivul client
		pthread_mutex_unlock(&mutex);
	}

    close(server_socket);
	return 0;
}

