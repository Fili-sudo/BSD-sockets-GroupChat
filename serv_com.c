#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

struct client_info {
	int sockno;
	char ip[INET_ADDRSTRLEN]; //INET_ADDRSTRLEN spatiu de memorie pentru o adresa IPv4
};

int clients[100];
int no_clients = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; //blocam datele atunci cand un thread acceseaza o variabila comuna

//trimitem mesaje la toti clientii serverului
void sendToAll(char* msg)
{
	int i;
	pthread_mutex_lock(&mutex);
	
	for (i=0;i<no_clients;i++) 
	{
		if (send(clients[i],msg,strlen(msg),0) < 0) 
			{
				perror("sending failure...");
				continue;
			}
	}

	pthread_mutex_unlock(&mutex);
}

// se primeste mesaj de la client.
void* recvMsg(void* socket)
{
	struct client_info cl = *((struct client_info*)socket);
	char buff[500],user[32];
    char *p,*msg;
	int msg_len,i,j;

	
    user[0]='\0';
	while ((msg_len = recv(cl.sockno, buff, 500, 0)) > 0) 
	{
        buff[msg_len] = '\0';
        /* alocam dinamic mesajul primit de la un client ca sa il redirectionam
           catre toti ceilalti clienti conectati la chat*/
        if((msg=(char *)malloc(msg_len*sizeof(char)))==NULL){ 
            perror("allocation error");
            exit(EXIT_FAILURE);
        }
        strcpy(msg,buff);

        if(strlen(user) == 0)
        {
            p=strtok(buff,":");
            strcpy(user,p);
			char welcome[60];
			strcpy(welcome,p);
			strcat(welcome," has entered the chat\n");
			sendToAll(welcome);
        }
		

		sendToAll(msg);
        free(msg);
	}
	
    
    /* odata ajunsi aici stim ca un client s-a deconectat. Scoatem clientul din vectorul de clienti
       si notificam restul clientilor */
    sprintf(buff, "%s left the chat\n", user);
    sendToAll(buff);

	pthread_mutex_lock(&mutex);

	printf("%s disconnected...\n", cl.ip);
   
	for (i=0;i<no_clients;i++) 
	{
		if (clients[i] == cl.sockno) 
		{ 
            for (j=i;j<no_clients-1;j++)
			{
				clients[j]=clients[j+1];
			}
		}
	}

	no_clients--;
	pthread_mutex_unlock(&mutex);
}