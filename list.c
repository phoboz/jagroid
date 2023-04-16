#include <stdlib.h> 
#include "list.h"

#ifdef DEBUG
#include <stdio.h> 
extern FILE *fp;
#endif

/* function to swap data of two nodes a and b*/
static void swap(struct Node *a, struct Node *b) 
{
    int temp_key = a->key; 
    a->key = b->key; 
    b->key = temp_key; 

    void *temp_data = a->data; 
    a->data = b->data; 
    b->data = temp_data; 
} 

/* Function to insert a node at the beginning of a linked list */
void insertList(struct Node **start_ref, int key, void *data) 
{ 
    struct Node *ptr1 = (struct Node*)malloc(sizeof(struct Node)); 
    ptr1->key = key;
    ptr1->data = data; 
    ptr1->next = *start_ref; 
    *start_ref = ptr1; 
}

/* Bubble sort the given linked list */
void sortList(struct Node *start) 
{ 
    int swapped; 
    struct Node *ptr1; 
    struct Node *lptr = NULL; 
  
    /* Checking for empty list */
    if (start == NULL) 
        return; 
  
    do
    { 
        swapped = 0; 
        ptr1 = start; 
  
        while (ptr1->next != lptr) 
        { 
            if (ptr1->key > ptr1->next->key) 
            { 
                swap(ptr1, ptr1->next); 
                swapped = 1; 
            } 
            ptr1 = ptr1->next; 
        } 
        lptr = ptr1; 
    } 
    while (swapped); 
} 

#ifdef DEBUG
/* Function to print nodes in a given linked list */
void printList(struct Node *start) 
{ 
    struct Node *temp = start; 
    while (temp!=NULL) 
    { 
        fprintf(fp, "%d ", temp->key); 
        temp = temp->next; 
    } 
}
#endif
