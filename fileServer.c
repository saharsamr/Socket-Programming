#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "serverStructs.h"

int main(int argc, char* argv[]){

  int socketDesc, newSocket, c;
  socketDesc = socket(AF_INET, SOCK_STREAM, 0);
  if (socketDesc == -1)
    write(1, "Socket creation failed.\n", 24);

  struct SocketAddrIn server, client;
  server.sinFamily = AF_INET;
  server.sinAddr.sAddr = inet_addr("127.0.0.1");
  server.sinPort= htons(8000);
  if (bind(socketDesc, &server, sizeof(server)) < 0)
    write(1, "Binding failed.\n", 16);

  listen(socketDesc, 3);
  write(1,"waiting for incoming cennections...\n", 36);
  c = sizeof(struct SocketAddrIn);
  newSocket = accept(socketDesc, &client, &c);
  if(newSocket < 0)
    write(1, "accept failed.\n", 15);

  return 0;
}
