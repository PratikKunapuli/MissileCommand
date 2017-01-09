///////////////////////////////////////////////////////////////////////
// Doubly Linked List
//
// This file should contain you implementation of the doubly-linked
// list data structure as described in the project documentation.
// 
// Some functions are provided for you - use these as guides for the
// implementation of the remaining functions.
//
// This code can be tested using the testbench module. See
// testbench.h in the source, and LL_testbench_documentation.pdf in 
// the project documentation for more information.
//
// GEORGIA INSTITUTE OF TECHNOLOGY, FALL 2016
///////////////////////////////////////////////////////////////////////


#include <stdlib.h>
#include <stdio.h>
#include "doubly_linked_list.h"

/**
 * create_llnode
 *
 * Helper function that creates a node by allocating memory for it on the heap,
 * and initializing its previous and next pointers to NULL and its data pointer to the input
 * data pointer
 *
 * @param data A void pointer to data the user is adding to the doublely linked list.
 * @return A pointer to the linked list node
 */
static LLNode* create_llnode(void* data) {
    LLNode* newNode = (LLNode*)malloc(sizeof(LLNode));
    newNode->data = data;
    newNode->previous = NULL;
    newNode->next = NULL;
    return newNode;
}

/**
 * create_dlinkedlist
 *
 * Creates a doublely liked list by allocating memory for it on the heap. Initialize the size to zero,
 * as well as head, current, and tail pointer to NULL
 *
 * @return A pointer to an empty dlinkedlist
 */
DLinkedList* create_dlinkedlist(void) {
    DLinkedList* newList = (DLinkedList*)malloc(sizeof(DLinkedList));
    newList->head = NULL;
    newList->tail = NULL;
    newList->current = NULL;
    newList->size = 0;
    return newList;
}

/**
 * InsertHead
 *
 * Insert the data to the head of the doublely linked list. Update the current pointer of the list only 
 * when it is originally NULL.
 *
 * @param dll A pointer to the doubly linked list
 * @param data A void pointer to data the user is adding to the doublely linked list.
 * 
 */
void insertHead(DLinkedList* dll, void* data){
  LLNode* newNode = create_llnode(data);
  if(dll->head == NULL){ // if there's no current head
    dll->size++; // increment size
    dll->head = newNode; //new head = new node since there's only one node. 
    dll->tail = newNode;
  }else{
    dll->size++;
    newNode->next = dll->head; //make new node new head. 
    (dll->head)->previous = newNode;
    dll->head = newNode;
  }
}

/**
 * insertTail
 *
 * Insert the data to the tail of the doublely linked list. Update the current pointer of the list only 
 * when it is originally NULL
 *
 * @param dll A pointer to the doubly linked list
 * @param data A void pointer to data the user is adding to the doublely linked list.
 * 
 */
void insertTail(DLinkedList* dll, void* data){
    LLNode* newNode = create_llnode(data);
    if(dll->tail == NULL) { // if there's no nodes in the list...
        dll->size++;
        dll->tail = newNode; //new node is head and tail
        dll->head = newNode;
    } else {
        dll->size++;
        newNode->previous = dll->tail; // make new node the new tail. 
        (dll->tail)->next = newNode;
        dll->tail = newNode;
    }   
}

/**
 * insertAfter
 *
 * Insert the new data to the doublely linked list right after the current pointer
 *
 * @param dll A pointer to the doubly linked list
 * @param newData A void pointer to the new data that the user want to add after data
 * @return 1 if insert the new data successfully
 *         0 if the current pointer is NULL
 */
int insertAfter(DLinkedList* dll, void* newData) {
    LLNode* newNode = create_llnode(newData); // make a new node for the data. 
    if (dll->current == NULL || dll == NULL) { //if the current is null, then return 0.
        return 0;
    } else if(dll->current == dll->tail) { //If the current pointer is at the tail, the new node is the new tail. 
        insertTail(dll,newData);
        return 1;
    } else { //if the current is somewhere in the middle, add the node and assign the next/previous accordingly. 
        dll->size++;
        newNode->previous = dll->current;
        newNode->next = (dll->current)->next;
        ((dll->current)->next)->previous = newNode;
        (dll->current)->next = newNode;
        return 1;
    }
}

/**
 * insertBefore
 *
 * Insert the new data to the doublely linked list right before the current pointer
 *
 * @param dll A pointer to the doubly linked list
 * @param newData A void pointer to the new data that the user want to add after data
 * @return 1 if insert the new data successfully
 *         0 if the current pointer is NULL
 */
int insertBefore(DLinkedList* dll, void* newData){
    LLNode* newNode = create_llnode(newData);
    if (dll->current == NULL || dll == NULL) { //if the current is null, then return 0.
        return 0;
    } else if(dll->current == dll->head) { //If the current pointer is at the head, the new node is the new head. 
        insertHead(dll, newData);
        return 1;
    } else { //if the current is somewhere in the middle, add the node and assign the next/previous accordingly. 
        dll->size++;
        newNode->next = dll->current;
        newNode->previous = (dll->current)->previous;
        ((dll->current)->previous)->next = newNode;
        (dll->current)->previous = newNode;
        return 1;
    }
}

/**
 * deleteBackward
 *
 * Delete the node the current pointer is pointed at, and move the current pointer backwards.
 * Be aware that deleteBackward will cause problem if the current pointer is pointing 
 * at list head
 *
 * @param dll A pointer to the doubly linked list
 * @param shouldFree Flag. 1 indicates if data should be freed upon deletion of node.
 * @return the data of the new current pointer and NULL if the current pointer is NULL
 */
void* deleteBackward(DLinkedList* dll, int shouldFree){
    LLNode* temp = dll->current; // Store a copy of the node to be deleted. 
    if(dll == NULL || temp == NULL) { //if the current pointer is NULL, then exit. 
        return NULL;
    } else if (dll->size == 1) { //If there is only one node, it's the head AND the tail. 
        if(shouldFree)
            free(temp->data); // delete the data if the flag says to. 
        free(temp); // delete the node. 
        dll->head = NULL; // No nodes left. 
        dll->tail = NULL;
        dll->current = NULL;
        dll->size--;
        return NULL;
    } else if(temp == dll->head) { // if the current pointer is at the head, then it's an edge case. 
        dll->head = temp->next; // move the head pointer. 
        ((temp)->next)->previous = NULL; // set the new head to have no previous node. 
        dll->current = NULL; // delete the current pointer. 
        if(shouldFree) {
            free(temp->data); // free data if needed
        }
        free(temp); // free node. 
        dll->size--;
        return NULL;
    } else if (temp == dll->tail) { //if the current pointer is at the tail, then it's also an edge case. 
        dll->tail = temp->previous; // Move the tail back to the previous node. 
        (dll->tail)->next = NULL; 
        dll->current = dll->tail; // assign the new tail to current. 
        if(shouldFree)
            free(temp->data); //free data if needed. 
        free(temp); //free node. 
        dll->size--;
        return (dll->tail)->data; //return the new data. 
    } else { // somewhere in the middle of the node. 
        (temp->previous)->next = temp->next; //relink around the node to be deleted. 
        (temp->next)->previous = temp->previous;
        dll->current = temp->previous;
        if(shouldFree) {
            free((temp->data)); //free data if needed. 
        }
        free(temp); // free node. 
        dll->size--;
        return (dll->current)->data; // return the new current data. 
    }
}

/**
 * deleteForward
 *
 * Delete the node the current pointer is pointed at, and move the current pointer forwards.
 * Be aware that deleteForward will cause problem if the current pointer is pointing 
 * at list tail
 *
 * @param dll A pointer to the doubly linked list
 * @param shouldFree Flag. 1 indicates if data should be freed upon deletion of node.
 * @return the data of the new current pointer and NULL if the current pointer is NULL
 */
void* deleteForward(DLinkedList* dll, int shouldFree){
    LLNode* temp = dll->current; //copy of the current node. 
    
    if(dll == NULL || temp == NULL) {
        return NULL;
    } else if(dll->size == 1) { // there is only one node, head = tail = current. Delete it. 
            if(shouldFree)
                free(temp->data);
            free(temp);
            dll->head = NULL;
            dll->tail = NULL;
            dll->current = NULL;
            dll->size--;
            return NULL;
    } else if (temp == dll->tail) {//if the pointer is the tail. 
        dll->tail = temp->previous; // basically do a DeleteBackwards. 
        (dll->tail)->next = NULL;
        dll->current = NULL; // set the current to NULL, beasue we're trying to move forwards. 
        if(shouldFree) {
            free(temp->data);
        }
        free(temp);
        dll->size--;
        return NULL;
    } else if (temp == dll->head) { // if the pointer is at the head, it's an edge case. 
        dll->head = temp->next; // need to point to the new head. 
        (dll->head)->previous = NULL;
        dll->current = dll->head;
        if(shouldFree)
            free(temp->data);
        free(temp);
        dll->size--;
        return getHead(dll);
    }else { //somewhere in the middle. 
        (temp->previous)->next = temp->next; //relink around deleted node. 
        (temp->next)->previous = temp->previous;
        dll->current = temp->next;
        if(shouldFree) {
            free(temp->data); // free the data if needed. 
        }
        free(temp); //free the node. 
        dll->size--;
        return (dll->current)->data; //return the new data. 
    }
}

/**
 * destroyList
 *
 * Destroy the doublely linked list. Everything in the linked list including list structure,
 * nodes and data are all freed from the heap
 *
 * @param dll A pointer to the doubly linked list
 * @param shouldFree Flag. 1 indicates if data should be freed upon deletion of node.
 *
 */
void destroyList(DLinkedList* dll, int shouldFree){
  if(dll->head != NULL){
    getHead(dll);
    while(deleteForward(dll,shouldFree)){};
  }
  printf("*****List Destroyed******\n\r");
  free(dll);
}

/**
 * getHead
 *
 * Return the data contained in the head of the doublely linked list, and set the list current pointer
 * to head
 *
 * @param dll A pointer to the doubly linked list
 * @return the head data or  NULL if head == NULL
 */
void* getHead(DLinkedList* dll){
  if(dll->head != NULL){
    dll->current = dll->head;
    return (dll->head)->data;
  }else{
    return NULL;
  }
}

/**
 * getTail
 *
 * Return the data contained in the tail of the doublely linked list, and set the current pointer
 * to tail
 *
 * @param dll A pointer to the doubly linked list
 * @return the tail data or  NULL if tail == NULL
 */
void* getTail(DLinkedList* dll){
    if(dll->tail != NULL) { //if the tail exists. 
        dll->current = dll->tail; // return the tail data and move the current pointer there. 
        return (dll->tail)->data;
    }
    return NULL; //otherwise, return null. 
}

/**
 * getCurrent
 *
 * Return the data the current pointer is pointing at
 *
 * @param dll A pointer to the doubly linked list
 * @return the current data or NULL if current == NULL
 */
void* getCurrent(DLinkedList* dll){
    if(dll->current != NULL) {
        return (dll->current)->data;
    }
    return NULL;
}

/**
 * getNext
 *
 * Return the next data the current pointer is pointing at, and move the current pointer to next node
 *
 * @param dll A pointer to the doubly linked list
 * @return the next data or NULL if current == NULL
 */
void* getNext(DLinkedList* dll){
    if(dll->current != NULL) {
        LLNode* temp = (dll->current)->next;
        dll->current = temp;
        if(temp != NULL) //if temp isn't after the tail, then return the data. 
            return temp->data;
        return NULL; //If it is after the tail (i.e. current previously pointed to tail), then return NULL because the data could be anything. 
    }
    
    return NULL;
}

/**
 * getPrevious
 *
 * Return the previous data the current pointer is pointing at, and move the current pointer to 
 * previous node
 *
 * @param dll A pointer to the doubly linked list
 * @return the previous data or NULL if current == NULL
 */
void* getPrevious(DLinkedList* dll){
    if(dll->current != NULL) {
        LLNode* temp = (dll->current)->previous;
        dll->current = temp;
        if(temp != NULL) //if temp isn't before the head, then return the data. 
            return temp->data;
        return NULL; //If it is before the head (i.e. current previously pointed to head), then return NULL because the data could be anything. 
    }
    
    return NULL;
}

/**
 * getSize
 *
 * Return the size of the doubly linked list
 *
 * @param dll A pointer to the doubly linked list
 * @return  the size
 */
int getSize(DLinkedList* dll){
    return dll->size;
}


