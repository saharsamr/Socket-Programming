#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedList.h"

typedef void (*callback)(node* data);

node* create(char chunkNum[3], char serverIP[16], char serverPort[5], node* next){
    node* new_node = (node*)malloc(sizeof(node));
    if(new_node == NULL){
        printf("Error creating a new node.\n");
        exit(0);
    }
    strcpy(new_node->chunkNum, chunkNum);
    strcpy(new_node->serverIP, serverIP);
    strcpy(new_node->serverPort, serverPort);
    new_node->next = next;
    return new_node;
}

node* append(node* head, char chunkNum[3], char serverIP[16], char serverPort[5]){
    if(head == NULL)
        return NULL;
    node *cursor = head;
    while(cursor->next != NULL)
        cursor = cursor->next;
    node* new_node =  create(chunkNum, serverIP, serverPort, NULL);
    cursor->next = new_node;
    return head;
}

node* prepend(node* head,char chunkNum[3], char serverIP[16], char serverPort[5]) {
    node* new_node = create(chunkNum, serverIP, serverPort, head);
    head = new_node;
    return head;
}

node* insertion_sort(node* head){
    node *x, *y, *e;
    x = head;
    head = NULL;
    while(x != NULL){
        e = x;
        x = x->next;
        if (head != NULL){
            if(atoi(e->chunkNum) > atoi(head->chunkNum)){
                y = head;
                while ((y->next != NULL) && (atoi(e->chunkNum) > atoi(y->next->chunkNum))){
                    y = y->next;
                }
                e->next = y->next;
                y->next = e;
            }
            else{
                e->next = head;
                head = e ;
            }
        }
        else{
            e->next = NULL;
            head = e ;
        }
    }
    return head;
}

void printList(char fileName[], node* head){
  node* temp = head;
  if (head != NULL){
    write(1, "file name: ", 11);
    write(1, fileName, strlen(fileName));
    write(1, "\n", 1);
  }
  while (temp != NULL)
    printf("chunk num:%s, IP:%s, port:%s\n", temp->chunkNum,
              temp->serverIP, temp->serverPort);
    temp = temp->next;
}

char* getListData(node* head){
  node* temp = head;
  char* data = malloc(1024);
  strcpy(data, temp->chunkNum);
  strcat(data, "#");
  strcat(data, temp->serverIP);
  strcat(data, "#");
  strcat(data, temp->serverPort);
  strcat(data, "#");
  temp = head->next;
  while (temp != NULL){
    strcat(data, temp->chunkNum);
    strcat(data, "#");
    strcat(data, temp->serverIP);
    strcat(data, "#");
    strcat(data, temp->serverPort);
    strcat(data, "#");
    temp = temp->next;
  }
  return data;
}
