// THIS CODE IS OUR OWN WORK, IT WAS WRITTEN WITHOUT CONSULTING A TUTOR
// OR CODE WRITTEN BY OTHER STUDENTS OUTSIDE OF OUR TEAM. Alex Welsh

//
// Created by Alex Welsh on 9/14/20.
//
#include "my_malloc.h"
#include <stddef.h>
#include <unistd.h>

// function and variable declarations;
FreeListNode head = NULL;
MyErrorNo my_errno = MYNOERROR;
void insert_node(FreeListNode node);
FreeListNode remove_node(unsigned sz);
FreeListNode find_node(unsigned sz);
int min_chunk_size();

// malloc's space given a size_t size and returns the address of the beginning of the malloc'ed space
void *my_malloc(size_t size){
    void *chunk_start = (void *) sbrk(0);                           // set addresses for start of chunk, start of malloc
    void *chunk_end = (void *) sbrk(0);                             // and end of chunk
    void *malloc_start = (void *) sbrk(0);
    int total_chunk_size;
    size += CHUNKHEADERSIZE;                                        // add header size

    if (size < min_chunk_size()){                                   // if size less than 16, set to 16
        size = min_chunk_size();
    } else if (size%8 != 0){                                        // if greater than 16, round up to
        size += 8-size%8;                                           // multiple of 8.
    }

    if (find_node(size) != NULL){                                   // if the free list has a node with enough space
        FreeListNode removed_node = remove_node(size);
        total_chunk_size = removed_node->size;                      // save the size of the free node.
        chunk_start = &(*removed_node);                             // set the start of the new malloc
        *((__int32_t *)chunk_start) = size;                         // set size and magic number in the chunk header
        *((__int32_t *)(chunk_start + sizeof(__int32_t))) = 8888;
        chunk_end = chunk_start + size;                             // calculate the address for the end of the chunk
        malloc_start = chunk_start + CHUNKHEADERSIZE;               // calculate the address for the start of malloc
    } else {                                                        // if no available node, or list is empty
        if (size > 8192) {                                          // if the size is greater 8192, try to sbrk
            total_chunk_size = size;
            long check_error = (long) sbrk(size);                   // if that fails, set the error and return NULL
            if (check_error == -1){
                my_errno = MYENOMEM;
                return NULL;
            }
        }else{                                                      // if its smaller, sbrk and set the total size
            sbrk(8192);
            total_chunk_size = 8192;
        }
        *((__int32_t *)chunk_start) = size;                         // set the malloc size and magic number
        *((__int32_t *)(chunk_start + sizeof(__int32_t))) = 8888;
        chunk_end = chunk_start + size;                             // calculate chunk end address
        malloc_start = chunk_start + CHUNKHEADERSIZE;               // calculate malloc start address
    }
    if (total_chunk_size - size >= min_chunk_size()){               // if the leftover space in the free node is >
        FreeListNode split_node = NULL;                             // min_chunk_size, split the node into allocated
        split_node = (FreeListNode) (chunk_end);                    // node and add smaller free node to list
        split_node->size = total_chunk_size - size;
        split_node->flink = NULL;                                   // set the free node to end of allocated node, set
        insert_node(split_node);                                    // size (total-size) and insert node to free list
    }else{
        *((__int32_t *)chunk_start) = total_chunk_size;             // if leftover space is < min_chunk_size, set the
    }                                                               // size of allocated chuck to total. Small amount
    return malloc_start;                                            // of internal fragmentation
}

// returns the min_chunk_size, either 16 or size of FreeListNode (could be bigger than 16 on certain systems)
int min_chunk_size(){
    if (sizeof(FreeListNode)>16){
        return sizeof(FreeListNode);
    }else {
        return 16;
    }
}

// frees a pointer to malloc'ed space and adds the chunk to the free list
void my_free(void *ptr){
    if (ptr == NULL || *((__int32_t *)(ptr-sizeof(__int32_t))) != 8888){    // sanity check of magic number
        my_errno = MYBADFREEPTR;                                            // if not malloc'ed space, set error
        return;
    }
    FreeListNode n;
    n = (FreeListNode) (ptr - CHUNKHEADERSIZE);         // create a node starting at pointer - header
    n->size = *((__int32_t *)(ptr - CHUNKHEADERSIZE));  // set the size of pointer based on ptr header size
    n->flink = NULL;
    insert_node(n);                                     // insert into free list
    return;
}

// given a FreeListNode, inserts it into the linked list
void insert_node(FreeListNode node){
    FreeListNode curr_node = head;
    if (head == NULL ){                                                 // if head is null, set head to node.
        head = node;
    }else if (head >= node) {                                           // if inserting node before head
        curr_node = head;
        node->flink = curr_node;
        head = node;
    }else if (head->flink == NULL && head < node){                      // if inserting node after head,
        node->flink = NULL;                                             // and head->flink is null
        head->flink = node;
    }else if ((head->flink != NULL) && (head->flink >= node)){          // if inserting after head, and head->flink is
        node->flink = head->flink;                                      // !null
        head->flink = node;
    }else{                                                              // all other cases
        curr_node = head->flink;
        while(curr_node->flink != NULL && curr_node->flink < node){     // search for node address that is just smaller
            curr_node = curr_node->flink;                               // than node being inserted
        }
        node->flink = curr_node->flink;                                 // insert node
        curr_node->flink = node;
    }
}

// given a size, search the free list for the first node with a smaller size and remove it from the list
FreeListNode remove_node(unsigned sz){
    FreeListNode curr_node = head;
    if (head == NULL){                                          // if head is null, cannot return anything
        return NULL;
    }else if (head->size >= sz){                                // if head is big enough, remove head and return it
        curr_node = head;
        head = head->flink;
        return curr_node;
    }else if (head->flink != NULL && head->flink->size >= sz) { // if head->flink is big enough, remove and return it
        curr_node = head->flink;
        head->flink = head->flink->flink;
        return curr_node;
    }else{                                                      // in all other cases,
        FreeListNode temp = NULL;
        curr_node = head->flink;
        while(curr_node->flink != NULL && curr_node->flink->size < sz){ // search for node just before node with big
            curr_node = curr_node->flink;                               // enough size
        }
        if (curr_node->flink == NULL || curr_node->flink->size < sz){   // if no node with big enough size or null
            return NULL;                                                // return NULL
        }
        temp = curr_node->flink;                                        // remove node and return it
        curr_node->flink = temp->flink;
        return temp;
    }
}

// similar to remove node, given a size, search list for the first node with a smaller size and return it.
FreeListNode find_node(unsigned sz){
    FreeListNode curr_node = head;
    if (head == NULL){                                  // if head is NULL, return NULL
        return NULL;
    }else if (head->size >= sz){                        // if head is big enough, return head
        curr_node = head;
        return curr_node;
    }else if (head->flink != NULL && head->flink->size >= sz) { // if head->flink is big enough, return it
        curr_node = head->flink;
        return curr_node;
    }else{                                                      // all other cases
        if (head->flink == NULL)                                // if only head and not big enough, return NULL
            return NULL;
        FreeListNode temp = NULL;
        curr_node = head->flink;
        while(curr_node->flink != NULL && curr_node->flink->size < sz){ // search for node just before node with big
            curr_node = curr_node->flink;                               // enough size
        }
        if (curr_node->flink == NULL || curr_node->flink->size < sz){   // if no node with big enough size or null
            return NULL;                                                // return NULL
        }
        temp = curr_node->flink;                                        // get and return the node
        return temp;
    }
    return NULL;                                                        // if no node available, return NULL
}

// iterates through the free list and merges adjacent nodes
void coalesce_free_list(void){
    FreeListNode temp = NULL;
    int counter = 1;
    while( counter != 0){                                       // repeatedly iterates though the list until
        counter = 0;                                            // no merges occur, counter keeps track of merges
        temp = head;
        while(temp != NULL && temp->flink != NULL){             // go through list
            void *new_add = (void *) (temp) + (temp->size);     // for a node, get the address of the next node by
            if (new_add == temp->flink){                        // adding size of node to node address
                FreeListNode temp_next = temp->flink;           // if this new address equals the address of the next
                temp->flink = temp->flink->flink;               // node in the list, then merge the node
                temp->size += temp_next->size;
                counter++;                                      // increase the counter because merge happened
            }
            temp = temp->flink;                                 // go to next node in list and repeat
        }
    }
}

// returns the head of the free list or NULL if list is empty
FreeListNode free_list_begin(void){
    if (head == NULL){
        return NULL;
    }
    return head;
}
