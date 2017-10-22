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
