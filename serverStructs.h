struct in_addr {
  unsigned long sAddr;
};

struct sockaddr_in {
  short sinFamily;
  unsigned short sinPort;
  struct in_addr sinAddr;
  char sinZero[8];
};

struct  sockaddr {
  unsigned short saFamily;
  char saData[14];
};
