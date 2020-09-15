//
// Created by Alex Welsh on 9/14/20.
//

#include "my_malloc.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>

FreeListNode head = NULL;
void insert_node(FreeListNode node);
FreeListNode remove_node(unsigned sz);
FreeListNode find_node(unsigned sz);
void coalesce();
void print_list();

int main(int argc, char *argv[]){
    printf("malloc size 16\n");
    int *ptr = my_malloc(16);
//    printf("pointer Loc: %d", *ptr);
    print_list();

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
    print_list();
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
        printf("teting");
        if (size < 8192) {
            sbrk(8192);
            *chunk_start = (int) size;
            *(chunk_start + 4) = 8888;
            printf("new heap_end: %p\nold end: %p\nstart malloc: %p\n",
                   sbrk(0), chunk_start, (chunk_start + CHUNKHEADERSIZE/sizeof(int)));
            printf("size: %d, magic#: %d\n", *chunk_start, *(chunk_start+4));
            if (8192 - size >= 16){
                FreeListNode node;
                node = (FreeListNode) (chunk_start + size/sizeof(int));
                node->size = 8192 - size;
                node->flink = NULL;
                insert_node(node);
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
        printf("HHHHHHHHHH\n");
        FreeListNode node = find_node(size);
        if (node == NULL){
            printf("NULL!");
        }
        printf("node add: %p, node size: %zu", node, node->size);
    }
    printf("++++++++++++++++++++++++++++++++++++\n");
    return chunk_start + CHUNKHEADERSIZE;
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

void print_list(){
    FreeListNode curr_node = head;
//    printf("node list --> %p", head);
    while(curr_node != NULL){
        printf("%p --> ", curr_node);
        curr_node = curr_node->flink;
    }
    printf("NULL\n");
}

void insert_node(FreeListNode node){
    printf("new node size: %zu\n", node->size);
    if (head == NULL){
        printf("head is null\n");
        head = node;
        head->flink = NULL;
    }else{
        FreeListNode curr_node = head;
        while(curr_node->flink != NULL){
            if (node < head){
                node->flink = head;
                head->flink = node;
            } else if (node < curr_node->flink && node > curr_node) {
                node->flink = curr_node->flink;
                curr_node->flink = node;
            }
        }
        if (curr_node->flink == NULL) {
            printf("should go here\n");
            curr_node->flink = node;
            node->flink = NULL;
        }
    }
}

FreeListNode remove_node(unsigned sz){
    FreeListNode node = head;
    printf("size %u\n", sz);
    while (node->flink != NULL){
        if (node->size <= sz)
            return node;
        else
            node = node->flink;
    }
    return NULL;
}

FreeListNode find_node(unsigned sz){
    FreeListNode node = head;
    printf("size %u\n", sz);
    printf("true?: %d\n", head->flink == NULL);
    if (head->flink == NULL){
        if (head->size >= sz) {
            printf("what\n");
            return head;
        }
    }else {
        while (node->flink != NULL) {
            printf("wile\n");
            if (node->size >= sz) {
                printf("what\n");
                return node;
            } else
                node = node->flink;
        }
    }
    return NULL;
}

void coalesce(){

}


