typedef struct node{
  char chunkNum[3];
  char serverIP[16], serverPort[5];
  struct node* next;
} node;

node* create(char chunkNum[3], char serverIP[16], char serverPort[5], node* next);

node* append(node* head, char chunkNum[3], char serverIP[16], char serverPort[5]);

node* insertion_sort(node* head);
