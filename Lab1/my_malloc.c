//
// Created by Alex Welsh on 9/17/20.
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
int min_chunk_size();

int main(int argc, char *argv[]){
    int *ptr = my_malloc(16);
    print_list();
    printf("+++++++++++++++++++\n");
    int *ptr2 = my_malloc(1);
    print_list();
    printf("+++++++++++++++++++\n");
    my_free(ptr2);
    print_list();
    printf("+++++++++++++++++++\n");
    int *ptr3 = my_malloc(7);
    printf("head: %p, %p", head, head->flink);
    print_list();
    my_free(ptr3);
    print_list();
    printf("+++++++++++++++++++\n");
}

void *my_malloc(size_t size){
    void *chunk_start = sbrk(0);
    void *chunk_end = sbrk(0);
    void *malloc_start = sbrk(0);
    int total_chunk_size;
    size += CHUNKHEADERSIZE;                                        // add header size

    if (size < min_chunk_size()){                                   // if less than 16, set to 16
        size = min_chunk_size();
    } else if (size%8 != 0){                                        // if greater than 16, round up to
        size += 8-size%8;                                           // multiple of 8.
    }

    printf("needed size: %zu\n", size);
    if (find_node(size) != NULL){
        FreeListNode removed_node = remove_node(size);
        printf("helloooo %p\n", head);
        total_chunk_size = removed_node->size;
        printf("loc: %p", removed_node);
        chunk_start = &(*removed_node);
        printf("chunk start %p\n", chunk_start);
        if (head != NULL){
            printf("final head: %p, %p\n", head, head->flink);
        }
        *((int32_t *)chunk_start) = size;
        if (head != NULL){
            printf("final head11: %p, %p\n", head, head->flink);
        }
        *((int32_t *)(chunk_start + sizeof(int32_t))) = 8888;
        if (head != NULL){
            printf("final head1: %p, %p\n", head, head->flink);
        }
        chunk_end = chunk_start + size;
        malloc_start = chunk_start + CHUNKHEADERSIZE;

    } else {
        if (size > 8192) {
            total_chunk_size = size;
            sbrk(size);
        }else{
            sbrk(8192);
            total_chunk_size = 8192;
        }
        *((int32_t *)chunk_start) = size;
        *((int32_t *)chunk_start + sizeof(int32_t)) = 8888;
        chunk_end = chunk_start + size;
        malloc_start = chunk_start + CHUNKHEADERSIZE;
    }
    printf("HELLO!\n");
    if (head != NULL){
        printf("final head: %p, %p\n", head, head->flink);
    }
    if (total_chunk_size - size > min_chunk_size()){
        printf("HELLO IN!\n");
        FreeListNode split_node = NULL;
        split_node = (FreeListNode) (chunk_end);
        split_node->size = total_chunk_size - size;
        split_node->flink = NULL;
        insert_node(split_node);
    }
    if (head != NULL){
        printf("final head: %p, %p\n", head, head->flink);
    }
    return malloc_start;
}

void print_list(){
    FreeListNode temp = head;
    while(temp != NULL){
        printf("%p --> ", temp);
        temp = temp->flink;
    }
    printf("%p\n", temp);
}

int min_chunk_size(){
    if (sizeof(FreeListNode)>16){
        return sizeof(FreeListNode);
    }else {
        return 16;
    }
}

void my_free(void *ptr){
    printf("FREEING NODE******\n");
    FreeListNode n;

    n = (FreeListNode) (ptr - CHUNKHEADERSIZE);
//    printf("free node size %p, %d\n", n, *((int32_t *)(ptr - CHUNKHEADERSIZE)));
    n->size = *((int32_t *)(ptr - CHUNKHEADERSIZE));
    n->flink = NULL;
    insert_node(n);
}

void insert_node(FreeListNode node){
    FreeListNode curr_node = head;
    printf("node: %p, head: %p, curr_node: %p\n", node, head, curr_node);
    if (head != NULL)
        printf("%p, %d\n", head->flink, (head->flink != NULL));
    if (head == NULL ){                                                 // head is null
        printf("head is NULL\n");
        head = node;
//        printf("head: %p, head.flink: %p\n", head, head->flink);
    }else if (head >= node) {                                           // insert before head
        printf("insert before head\n");
        curr_node = head;
        node->flink = curr_node;
        head = node;
        printf("node: %p, head: %p, curr_node: %p\n", head, head->flink, head->flink->flink);
    }else if (head->flink == NULL && head < node){                      // insert after head, and head->flink is null
        printf("insert after head, and head->flink is null\n");
        node->flink = NULL;
        head->flink = node;
    }else if ((head->flink != NULL) && (head->flink >= node)){          // insert after head, and head->flink is !null
        printf("insert after head, and head->flink is !null\n");
        node->flink = head->flink;
        head->flink = node;
    }else{                                                              // all other cases
        printf("LAST RESORT\n");
        curr_node = head->flink;
        while(curr_node->flink != NULL && curr_node->flink < node){
            curr_node = curr_node->flink;
        }
        node->flink = curr_node->flink;
        curr_node->flink = node;
    }
}

FreeListNode remove_node(unsigned sz){
    printf("REMOVING NODE\n");
    FreeListNode curr_node = head;
    print_list();
    printf("head size %p, %p\n", head, head->flink);
    if (head->flink != NULL){
        printf("kkep going: %p\n", head->flink->flink);
    }
    if (head == NULL){
        return NULL;
    }else if (head->size >= sz){                        // if head is big enough
        printf("if head is big enough %p, %p \n", head, head->flink);
        curr_node = head;
        if (head->flink != NULL){
            printf("not null %p, %p\n", head, head->flink);
            if (head->flink->flink == NULL){
                printf("not null %p, %p, %p\n", head, head->flink, head->flink->flink);
            }
        }
        head = head->flink;
        printf("if head is big enough!!, %p\n", head);
        if (head != NULL && head->flink == NULL){
            printf("not null %p, %p\n", head, head->flink);
        }
        return curr_node;
    }else if (head->flink->size >= sz) {                // if head->flink is big enough
        printf("if head->flink is big enough\n");
        curr_node = head->flink;
        head->flink = head->flink->flink;
        printf("if head->flink is big enough!! %p help\n", head);
        return curr_node;
    }else{
        printf("REMOVING NOT HEAD\n");
        FreeListNode temp = NULL;
        curr_node = head->flink;
        while(curr_node->flink != NULL && curr_node->flink->size < sz){
            curr_node = curr_node->flink;
        }
        printf("curr node: %p\n", curr_node);
        if (curr_node->flink == NULL || curr_node->flink->size < sz){
            return NULL;
        }
        temp = curr_node->flink;
        curr_node->flink = temp->flink;
        return temp;
    }
}

FreeListNode find_node(unsigned sz){
    printf("FINDING NODE\n");
    FreeListNode curr_node = head;
    if (head == NULL){
        return NULL;
    }else if (head->size >= sz){                        // if head is big enough
        printf("if head is big enough\n");
        curr_node = head;
//        head = head->flink;
        return curr_node;
    }else if (head->flink->size >= sz) {                // if head->flink is big enough
        printf("if head->flink is big enough\n");
        curr_node = head->flink;
//        head->flink = head->flink->flink;
        return curr_node;
    }else{
        printf("FINDING NOT HEAD\n");
        FreeListNode temp = NULL;
        curr_node = head->flink;
        while(curr_node->flink != NULL && curr_node->flink->size < sz){
            curr_node = curr_node->flink;
        }
        printf("curr node: %p\n", curr_node);
        if (curr_node->flink == NULL || curr_node->flink->size < sz){
            return NULL;
        }
        temp = curr_node->flink;
//        curr_node->flink = temp->flink;
        return temp;
    }
}

FreeListNode free_list_begin(void){
    return head;
}

void coalesce_free_list(void){

}
