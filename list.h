#ifndef _SORT_H
#define _SORT_H

/* structure for a node */
struct Node 
{ 
    int key; 
    void *data;
    struct Node *next; 
}; 
  
/* Function to insert a node at the beginning of a linked list */
void insertList(struct Node **start_ref, int key, void *data); 
  
/* Function to bubble sort the given linked list */
void sortList(struct Node *start); 

#ifdef DEBUG
/* Function to print nodes in a given linked list */
void printList(struct Node *start);
#endif

#endif

