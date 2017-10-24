#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "serverStructs.h"

// struct filesdata{
//   int partNumber;
//   struct InAddr clientIP;
//   unsigned short clientPort;
// };

int main(int argc, char* argv[]){

  int socketDesc;
  socketDesc = socket(AF_INET, SOCK_STREAM, 0);
  if (socketDesc == -1)
    write(1,"Socket creation failed.\n", 24);

  struct SocketAddrIn server;
  server.sinAddr.sAddr = inet_addr("127.0.0.1");
  server.sinFamily = AF_INET;
  server.sinPort = htons(8000);

  if (connect(socketDesc, &server, sizeof(server)) < 0)
    write(1, "Connection refused.\n", 20);

  return 0;
}
