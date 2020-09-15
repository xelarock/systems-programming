//
// Created by Alex Welsh on 9/14/20.
//

#include "my_malloc.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>

FreeListNode head;
void insert_node(FreeListNode node);
FreeListNode remove_node(unsigned sz);
FreeListNode find_node(unsigned sz);
void coalesce();

int main(int argc, char *argv[]){
    printf("malloc size 16\n");
    int *ptr = my_malloc(16);
//    printf("pointer Loc: %d", *ptr);

    printf("malloc size 1\n");
    ptr = my_malloc(1);

    printf("malloc size 7\n");
    ptr = my_malloc(7);

    printf("malloc size 8\n");
    ptr = my_malloc(8);

    printf("malloc size 9\n");
    ptr = my_malloc(9);

    printf("malloc size 17\n");
    ptr = my_malloc(17);

    printf("malloc size 23\n");
    ptr = my_malloc(23);

    printf("malloc size 24\n");
    ptr = my_malloc(24);

    printf("malloc size 25\n");
    ptr = my_malloc(25);
}

void *my_malloc(size_t size){
    printf("heap_end: %p\n", sbrk(0));
    int *chunk_start = sbrk(0);
    size += CHUNKHEADERSIZE;                                        // add header size
    if (size < 16){                                                 // if less than 16, set to 16
        size = 16;
    } else if (size%8 != 0){                                        // if greater than 16, round up to
        size += 8-size%8;                                           // multiple of 8.
    }
    printf("needed size: %zu\n", size);
    if (free_list_begin() == NULL){
        if (size < 8192) {
            sbrk(8192);
            *chunk_start = size;
            *(chunk_start + 4) = 8888;
            printf("new heap_end: %p\nold end: %p\nstart malloc: %p\n",
                   sbrk(0), chunk_start, chunk_start + CHUNKHEADERSIZE);
            printf("size: %d, magic#: %d", *chunk_start, *(chunk_start+4));
            if (8192 - size >= 16){

            }
        }else{
            sbrk(size);
            *chunk_start = size;
            *(chunk_start + 4) = 8888;
            printf("new heap_end: %p\nold end: %p\nstart malloc: %p\n",
                   sbrk(0), chunk_start, chunk_start + CHUNKHEADERSIZE);
            printf("size: %d, magic#: %d", *chunk_start, *(chunk_start+4));
        }
    }else {
        FreeListNode node = find_node(size);

    }
    printf("++++++++++++++++++++++++++++++++++++\n");
    return NULL;
}

void my_free(void *ptr){
//    FreeListNode n;

//    n = (FreeListNode) ptr;
//    n->size = *ptr
}

FreeListNode free_list_begin(void){
    if (head == NULL)
        return NULL;
    else
        return head;
}

void insert_node(FreeListNode node){

}

FreeListNode remove_node(unsigned sz){
    return NULL;
}

FreeListNode find_node(unsigned sz){
    return NULL;
}

void coalesce(){

}


