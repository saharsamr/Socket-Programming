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
#include "linkedList.h"

#define TRUE   1
#define FALSE  0
#define MAX_CLIENTS 50
#define MAX_TRANSFER_BYTES 1024
#define MAX_IP_LEN 16
#define MAX_PORT_LEN 5

typedef struct FileData{
  char fileName[100];
  node* listHead;
} FileData;

int findFile(char* fileName, FileData* filesdata){
  int i;
  for (i = 0; i < MAX_CLIENTS; i++){
    if (&filesdata[i] != NULL && strcmp(fileName, filesdata[i].fileName) == 0)
      return i;
  }
  return -1;
}

int isFileData(char* buffer, FileData* filesdata, int* filesDataCount){
  char* token; int index;
  char* chunkNum = (char*)malloc(3*sizeof(char)); char* IP = (char*)malloc(16*sizeof(char)); char* port = (char*)malloc(5*sizeof(char));
  FileData* newFile = (FileData*)malloc(sizeof(FileData));
  if (strcmp(token = strtok(buffer, "#"), "file server") == 0){
      memset(newFile->fileName, '\0', 100);
      strcpy(newFile->fileName, strtok(NULL, "#"));
      memset(chunkNum, '\0', 3);
      memset(IP, '\0', MAX_IP_LEN);
      memset(port, '\0', MAX_PORT_LEN);
    chunkNum = strtok(NULL, "#"); IP = strtok(NULL, "#"); port = strtok(NULL, "#");
    if (index = findFile(newFile->fileName, filesdata) < 0){
      node* newNode = (node*)malloc(sizeof(node));
      memset(newNode->chunkNum, '\0', 3);
      memset(newNode->serverIP, '\0', MAX_IP_LEN);
      memset(newNode->serverPort, '\0', MAX_PORT_LEN);
      strcpy(newNode->chunkNum, chunkNum); strcpy(newNode->serverIP, IP); strcpy(newNode->serverPort, port);

      newFile->listHead = newNode;
      filesdata[*filesDataCount] = *newFile;
      (*filesDataCount) += 1;
    }
    else{
      filesdata[index].listHead = append(filesdata[index].listHead, chunkNum, IP, port);
      filesdata[index].listHead = insertion_sort(filesdata[index].listHead);
    }
    write(1, "file data recieved.\n", 20);
    return TRUE;
  }
  return FALSE;
}

char* isClientReq(char* buffer, FileData* filesdata){
  int index = findFile(buffer, filesdata);
  node* temp = filesdata[index].listHead;
  char* data = malloc(MAX_TRANSFER_BYTES);
  memset(data, '\0', MAX_TRANSFER_BYTES);
  while (temp != NULL){
    strcat(data, "#");
    strcat(data, temp->chunkNum);
    strcat(data, "#");
    strcat(data, temp->serverIP);
    strcat(data, "#");
    strcat(data, temp->serverPort);
    temp = temp->next;
  }
  return data;
}

char* parseClientMessage(char* buffer, FileData* filesdata, int* filesDataCount){
  char* response = malloc(MAX_TRANSFER_BYTES);
  if (isFileData(buffer, filesdata, filesDataCount))
    return "file data added.\n";
  else
    return isClientReq(buffer, filesdata);
}

int main(int argc , char *argv[]){

  int opt = TRUE;
  int master_socket, new_socket, client_socket[MAX_CLIENTS], activity;
  int addrlen, i, valread, sd, max_sd;
  struct sockaddr_in address;
  char buffer[MAX_TRANSFER_BYTES], response[MAX_TRANSFER_BYTES], port[MAX_PORT_LEN], ip[MAX_IP_LEN];

  write(1, "Enter main server ip address:\n", 30);
  read(0, ip, sizeof(ip));
  *strstr(ip, "\n") = '\0';
  write(1, "Enter main server port:\n", 24);
  read(0, port, sizeof(port));
  *strstr(port, "\n") = '\0';

  fd_set readfds;

  for (i = 0; i < MAX_CLIENTS; i++)
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
  address.sin_port = htons(atoi(port));

  if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0){
    write(2, "bind failed", 11);
    exit(EXIT_FAILURE);
  }

  if (listen(master_socket, MAX_CLIENTS) < 0){
    write(2, "listen error", 12);
    exit(EXIT_FAILURE);
  }

  addrlen = sizeof(address);
  write(1, "Waiting for connections...\n", 27);

  FileData* filesInServers =(FileData*) malloc(MAX_CLIENTS*sizeof(FileData));
  int filesDataCount = 0;

  while(TRUE){
    printf("files number: %d\n", filesDataCount);

    FD_ZERO(&readfds);

    FD_SET(master_socket, &readfds);
    max_sd = master_socket;

    for ( i = 0 ; i < MAX_CLIENTS ; i++){
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

      for (i = 0; i < MAX_CLIENTS; i++)
        if( client_socket[i] == 0 ){
          client_socket[i] = new_socket;
          break;
        }
    }

    for (i = 0; i < MAX_CLIENTS; i++){
      sd = client_socket[i];
      if (FD_ISSET(sd, &readfds)){
        memset(buffer, '\0', MAX_TRANSFER_BYTES);
        if ((valread = recv(sd ,buffer, MAX_TRANSFER_BYTES, 0)) == 0){
          close( sd );
          client_socket[i] = 0;
        }
        else{
          buffer[valread] = '\0';
          write(1, buffer, strlen(buffer));
          strcpy(response, parseClientMessage(buffer, filesInServers, &filesDataCount));
          if (send(sd, response, strlen(response), 0) < 0)
            write(2, "sending response to client failed.\n", 35);
        }
      }
    }
  }
  return 0;
}
