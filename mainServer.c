#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>

#define TRUE   1
#define FALSE  0
#define MAX_CLIENTS 50

typedef struct Node Node;

typedef struct Node{
  int chunkNum;
  char serverIP[16], serverPort[5];
  Node* next;
};

typedef struct FileData{
  char* fileName[100];
  Node* listHead;
} FileData;

int isFileData(char* buffer){
  if (strstr(buffer, "file server: file name:#") == buffer){
    write(1, "file data recieved.\n", 20);
    return TRUE;
  }
  return FALSE;
}

char* parseClientMessage(char* buffer){
  if (strcmp(buffer, "file server: first connection.\n") == 0)
    return "main server: connection accepted from server.\n";
  else if (isFileData(buffer))
    return "file data added.\n";
  else if (strcmp(buffer, "file server: finished.\n") == 0)
    return "nice job. bye.\n";
  else
    return "that wasn't an acceptable request.\n";
}

int main(int argc , char *argv[]){

  int opt = TRUE;
  int master_socket, new_socket, client_socket[50], activity;
  int addrlen, max_clients = MAX_CLIENTS, i, valread, sd, max_sd;
  struct sockaddr_in address;
  char buffer[1025], response[1025], port[5], ip[16];

  write(1, "Enter main server ip address:\n", 30);
  read(0, ip, sizeof(ip));
  write(1, "Enter main server port:\n", 24);
  read(0, port, sizeof(port));

  fd_set readfds;

  for (i = 0; i < max_clients; i++)
    client_socket[i] = 0;

  if ((master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0){
    write(2, "socket failed", 13);
    exit(EXIT_FAILURE);
  }

  if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 ){
    write(2, "setsockopt error", 16);
    exit(EXIT_FAILURE);
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(atoi("8888"));

  if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0){
    write(2, "bind failed", 11);
    exit(EXIT_FAILURE);
  }

  if (listen(master_socket, 50) < 0){
    write(2, "listen error", 12);
    exit(EXIT_FAILURE);
  }

  addrlen = sizeof(address);
  write(1, "Waiting for connections...\n", 27);

  while(TRUE){
    FD_ZERO(&readfds);

    FD_SET(master_socket, &readfds);
    max_sd = master_socket;

    for ( i = 0 ; i < max_clients ; i++){
      sd = client_socket[i];
      if(sd > 0)
        FD_SET( sd , &readfds);
      if(sd > max_sd)
        max_sd = sd;
    }

    activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
    if ((activity < 0) && (errno!=EINTR))
      write(2, "select error\n", 13);

    if (FD_ISSET(master_socket, &readfds)){
      if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0){
        write(2, "accept error", 12);
        exit(EXIT_FAILURE);
      }

      write(1, "connection accepted.\n", 21);

      for (i = 0; i < max_clients; i++)
        if( client_socket[i] == 0 ){
          client_socket[i] = new_socket;
          break;
        }
    }

    for (i = 0; i < max_clients; i++){
      sd = client_socket[i];
      if (FD_ISSET(sd, &readfds)){
        memset(buffer, '\0', 1024);
        if ((valread = recv(sd ,buffer, 1024, 0)) == 0){
          close( sd );
          client_socket[i] = 0;
        }
        else{
          buffer[valread] = '\0';
          write(1, buffer, strlen(buffer));
          strcpy(response, parseClientMessage(buffer));
          if (send(sd, response, strlen(response), 0) < 0)
            write(2, "sending response to client failed.\n", 35);
        }
      }
    }
  }
  return 0;
}
