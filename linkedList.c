#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedList.h"

#define MAX_TRANSFER_BYTES 1024

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
