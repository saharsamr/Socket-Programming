#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>

struct InAddr {
  unsigned long sAddr;
};

struct SocketAddrIn {
  short sinFamily;
  unsigned short sinPort;
  struct InAddr sinAddr;
  char sinZero[8];
};

struct socketAddr {
  unsigned short saFamily;
  char saData[14];
};

int main(int argc, char* argv[]){

  int socketDesc;
  socketDesc = socket(AF_INET, SOCK_STREAM, 0);
  if (socketDesc == -1)
    write(1, "Socket creation failed.\n", 24);

  struct SocketAddrIn server;
  server.sinFamily = AF_INET;
  server.sinAddr.sAddr = inet_addr("127.0.0.1");
  server.sinPort= htons(8000);
  if (bind(socketDesc, &server, sizeof(server)) < 0)
    write(1, "Binding failed.\n", 16);

  listen(socketDesc, 3);

  return 0;
}
