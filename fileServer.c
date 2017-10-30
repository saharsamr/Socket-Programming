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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_CLIENT_NUM 50
#define TRUE 1
#define FALSE 0

char fileServerIP[16], fileServerPort[5];

int sendall(int s, char *buf, int *len)
{
    int total = 0;        // how many bytes we've sent
    int bytesleft = *len; // how many we have left to send
    int n;
    int i = 1;
    while(total < *len) {
        n = send(s, buf+total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

    *len = total; // return number actually sent here

    return n==-1?-1:0; // return -1 on failure, 0 on success
}

int main(int argc, char* argv[]){

  int valread, mainServerSocket;
  char buffer[1025], response[1024];

  write(1, "Enter this file server ip address:\n",35);
  read(0, fileServerIP, 18);
  *strstr(fileServerIP, "\n") = '\0';
  write(1, "Enter file server port:\n", 24);
  read(0, fileServerPort, 5);
  *strstr(fileServerPort, "\n") = '\0';

  mainServerSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (mainServerSocket == -1){
    write(1, "File server socket creation for talking to main server failed.\n", 24);
    return 0;
  }

  char* sendTheFile(char* fileName){
    // char* name = strtok(fileName, ":");
    // char* offset = strtok(NULL, ":");
    // printf("%s\n%s\n",name, offset);
    int FILE_FD = open(fileName, O_RDONLY);
    // lseek(name, atoi(offset), 0);
    char *text = malloc(1024*1024 * sizeof(char));
    if (read(FILE_FD, text, 1024*1024) < 0)
      write(1, "couldn't send to client.\n", 25);
    // write(1, text, 1024);
    return text;
  }

  struct sockaddr_in fileServer, client, mainServer;
  mainServer.sin_family = AF_INET;
  mainServer.sin_addr.s_addr = inet_addr("127.0.0.1");
  mainServer.sin_port= htons(atoi("8888"));

  if (connect(mainServerSocket, &mainServer, sizeof(mainServer)) < 0)
    write(1, "Connection error.\n", 16);
//////////////TODO: tabe kon ino :)))
  char fileData[1025], temp[100];
  memset(fileData, '\0', 1025);
  memset(temp, '\0', 100);
  write(1, "Enter file name:\n", 17);
  read(0, temp, 100);
  *strstr(temp, "\n") = '\0';
  strcpy(fileData, "file server#");
  strcat(fileData, temp);
  memset(temp, '\0', 100);
  write(1, "Enter chunk num:\n", 17);
  read(0, temp, 100);
  *strstr(temp, "\n") = '\0';
  strcat(fileData, "#");
  strcat(fileData, temp);
  strcat(fileData, "#");
  strcat(fileData, fileServerIP);
  strcat(fileData, "#");
  strcat(fileData, fileServerPort);
  // write(1, fileData, strlen(fileData));

  if (send(mainServerSocket, fileData, strlen(fileData), 0) < 0)
    write(1, "Sending failed.\n", 17);

    memset(buffer, '\0', 1024);
    if (valread = read(mainServerSocket, buffer, 1024) == 0)
      write(2, "message did not recieve from server.\n", 37);
    else
      write(1, buffer, strlen(buffer));
/////////////////////////////////////////////////////////////////////////

  int opt = TRUE;
  int fileServerSocket, new_socket, client_socket[50], activity;
  int addrlen, sd, max_sd, i, max_clients = MAX_CLIENT_NUM;
  struct sockaddr_in address;
  char port[5], ip[16];

  fd_set readfds;

  for (i = 0; i < max_clients; i++)
    client_socket[i] = 0;

  if ((fileServerSocket = socket(AF_INET , SOCK_STREAM , 0)) == 0){
    write(2, "socket failed", 13);
    exit(EXIT_FAILURE);
  }

  if( setsockopt(fileServerSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 ){
    write(2, "setsockopt error", 16);
    exit(EXIT_FAILURE);
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(atoi(fileServerPort));

  if (bind(fileServerSocket, (struct sockaddr *)&address, sizeof(address)) < 0){
    write(2, "bind failed", 11);
    exit(EXIT_FAILURE);
  }

  if (listen(fileServerSocket, 50) < 0){
    write(2, "listen error", 12);
    exit(EXIT_FAILURE);
  }

  addrlen = sizeof(address);
  write(1, "Waiting for connections...\n", 27);

  while(TRUE){
    FD_ZERO(&readfds);

    FD_SET(fileServerSocket, &readfds);
    max_sd = fileServerSocket;

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

    if (FD_ISSET(fileServerSocket, &readfds)){
      if ((new_socket = accept(fileServerSocket, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0){
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
          memset(response, '\0', 1024);
          buffer[valread] = '\0';
          write(1, buffer, strlen(buffer));
          // strcpy(response, "response\n");
          char* data = sendTheFile(buffer);

          int len = strlen(data);

          sendall(sd, data, &len);
          free(data);
          send(sd, "end of file.", 13, 0);

          // if (send(sd, sendTheFile(buffer), 1024, 0) < 0)
          //   write(2, "sending response to client failed.\n", 35);
        }
      }
    }
  }

  return 0;
}
