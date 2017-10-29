#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX_CLIENT_NUM 50
#define TRUE 1
#define FALSE 0

char fileServerIP[16], fileServerPort[5];

char* sendPortAndIp(){
  char response[1025];
  strcpy(response, "file server: IP:#");
  strcat(response, fileServerIP);
  strcat(response, " port:#");
  strcat(response, fileServerPort);
  return strcat(response, "\n");
}

int main(int argc, char* argv[]){

  int valread, mainServerSocket;
  char buffer[1025], response[1025];

  write(1, "Enter this file server ip address:\n",35);
  read(0, fileServerIP, 18);
  write(1, "Enter file server port:\n", 24);
  read(0, fileServerPort, 5);

  mainServerSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (mainServerSocket == -1){
    write(1, "File server socket creation for talking to main server failed.\n", 24);
    return 0;
  }

  struct sockaddr_in fileServer, client, mainServer;
  mainServer.sin_family = AF_INET;
  mainServer.sin_addr.s_addr = inet_addr("127.0.0.1");
  mainServer.sin_port= htons(atoi("8888"));

  if (connect(mainServerSocket, &mainServer, sizeof(mainServer)) < 0)
    write(1, "Connection error.\n", 16);

  char fileData[1025], temp[100];
  memset(fileData, 0, 1025);
  memset(temp, 0, 100);
  write(1, "Enter file name:\n", 17);
  read(0, temp, 100);
  strcpy(fileData, "file server: file name:#");
  strcat(fileData, temp);
  memset(temp, 0, 100);
  write(1, "Enter chunk num:\n", 17);
  read(0, temp, 100);
  strcat(fileData, " chunk num:#");
  strcat(fileData, temp);
  strcat(fileData, " IP:#");
  strcat(fileData, fileServerIP);
  strcat(fileData, " port:#");
  strcat(fileData, fileServerPort);
  strcat(fileData, "\n");

  if (send(mainServerSocket, fileData, strlen(fileData), 0) < 0)
    write(1, "Sending failed.\n", 17);

    if (valread = read(mainServerSocket, buffer, 1024) == 0)
      write(2, "message did not recieve from server.\n", 37);
    else
      write(1, buffer, strlen(buffer));

  // while(TRUE){
  //   if (valread = read(mainServerSocket, buffer, 1024) == 0)
  //     write(2, "message did not recieve from server.\n", 37);
  //   else{
  //     write(1, buffer, strlen(buffer));
  //     strcpy(response, parseMainServerMessage(buffer));
  //     if (response == NULL)
  //       break;
  //     if (send(mainServerSocket, response, strlen(response), 0) < 0)
  //       write(2, "sending response to main server failed.\n", 40);
  //   }
  // }

    // fd_set readfds;
    //
    // int clientSockets[MAX_CLIENT_NUM], fileServerSocket, mainServerSocket, i, option = TRUE;
    // for (i = 0; i < MAX_CLIENT_NUM; i++)
    //   clientSockets[i] = 0;

      // if (setsockopt(mainServerSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&option, sizeof(option)) < 0 )
      //   write(1, "Set option process for file server socket failed.\n", 50);

  // if (bind(fileServerSocket, &fileServer, sizeof(fileServer)) < 0)
  //   write(1, "Binding failed.\n", 16);
  //
  // if (send(mainServerSocket, "hello then.\n", 12, 0)){
  //
  // }
  //
  // if (listen(fileServerSocket, 50))
  //   write(1, "Listening process failed for 50 cocurrent clients.\n", 51);
  //
  // write(1,"waiting for incoming cennections...\n", 36);
  //
  // while(TRUE){
  //   char client_message[1025];
  //   int clientSocket;
  //
  //   FD_ZERO(&readfds);
  //   FD_SET(fileServerSocket, &readfds);
  //   int max_sd = fileServerSocket;
  //
  //   for (i = 0; i < MAX_CLIENT_NUM; i++){
  //     int sd = clientSockets[i];
  //     if (sd > 0)
  //       FD_SET(sd, &readfds);
  //     if (sd > max_sd)
  //       max_sd = sd;
  //   }
  //   int connection, read_size;
  //   connection = select(max_sd+1, &readfds, NULL, NULL, NULL);
  //
  //   if(connection < 0)
  //     write(1, "Select error.\n", 14);
  //
  //   if (FD_ISSET(fileServerSocket, &readfds)){
  //     if (clientSocket = accept(fileServerSocket, &fileServer, sizeof(fileServer))< 0)
  //       write(1, "Accept error.\n", 14);
  //     else{
  //       write(1, "New connection...\n", 18);
  //       while (read_size = recv(clientSocket, client_message, strlen(client_message), 0))
  //         write(1, client_message, strlen(client_message));
  //     }
  //   }
  // }
  return 0;
}
