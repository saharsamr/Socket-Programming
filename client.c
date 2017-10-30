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
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_CLIENT_NUM 50
#define TRUE 1
#define FALSE 0

char clientIP[16], clientPort[5];

int main(int argc, char* argv[]){

  int valread, mainServerSocket;
  char buffer[1025], response[1025];

  write(1, "Enter this client ip address:\n",35);
  read(0, clientIP, 18);
  *strstr(clientIP, "\n") = '\0';
  write(1, "Enter client port:\n", 24);
  read(0, clientPort, 5);
  *strstr(clientPort, "\n") = '\0';

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
  write(1, "Enter file name you want:\n", 26);
  read(0, temp, 100);
  *strstr(temp, "\n") = '\0';
  strcpy(fileData, temp);

  if (send(mainServerSocket, fileData, strlen(fileData), 0) < 0)
    write(1, "Sending failed.\n", 17);

    if (valread = read(mainServerSocket, buffer, 1024) == 0)
      write(2, "message did not recieve from server.\n", 37);
    else{
      write(1, buffer, strlen(buffer));
      char serversData [50][16];
      int numOfServers = 0, i = 0;
      memset(serversData, '\0', 50*16);
      char* chunkNum = strtok(buffer, "#");
      while(chunkNum != NULL){
        strcpy(serversData[i], strtok(NULL, "#"));
        i++;
        strcpy(serversData[i], strtok(NULL, "#"));
        i++;
        chunkNum = strtok(NULL, "#");
        numOfServers++;
      }

      for(int j = 0; j < numOfServers; j++){

        int fileServerSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (fileServerSocket == -1){
          write(1, "File server socket creation for talking to main server failed.\n", 24);
          return 0;
        }

        struct sockaddr_in fileServer;
        fileServer.sin_family = AF_INET;
        fileServer.sin_addr.s_addr = inet_addr("127.0.0.1");
        fileServer.sin_port= htons(atoi(serversData[j*2+1]));

        if (connect(fileServerSocket, &fileServer, sizeof(fileServer)) < 0)
          write(1, "Connection error.\n", 16);

        char* fName[100];
        memset(fName, '\0', 100);
        strcpy(fName, fileData);

        if (send(fileServerSocket, fileData, strlen(fileData), 0) < 0)
          write(1, "Sending failed.\n", 17);

          if (valread = read(fileServerSocket, buffer, 1024) == 0)
            write(2, "message did not recieve from server.\n", 37);
          else{
            char* newFileName[100];
            memset(newFileName, '\0', 100);
            strcat(newFileName, "./");
            strcat(newFileName, clientPort);
            strcat(newFileName, fName);
            int file_fd = open(newFileName, O_CREAT | O_APPEND | O_WRONLY);
            if(file_fd < 0)
              write(1, "error\n", 6);
            write(file_fd, buffer, strlen(buffer));

            while(TRUE){
              memset(buffer, '\0', 1024);
              if (valread = read(fileServerSocket, buffer, 1024) == 0)
                write(2, "message did not recieve from server.\n", 37);
              write(file_fd, buffer, strlen(buffer));
              if(strstr(buffer, "end of file.") == NULL)
                printf("cn\n");

              else{
                printf("done\n");
                break;
              }
            }
            close(file_fd);

        // chunkNum = strtok(NULL, "#");
        // printf("%s\n", chunkNum);
      }
    }
  }
}
