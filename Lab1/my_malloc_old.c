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
void coalesce_();
void print_list();
int min_chunk_size();

int main(int argc, char *argv[]){
//    printf("head start: %p", );
    printf("malloc size 16\n");
    int *ptr = my_malloc(16);
    printf("pointer Loc: %p\n", ptr);
//    print_list();
    printf("++++++++++++++++++++++++++++++++++++\n");

    printf("head %p, head next: %p\n", head, head->flink);

    printf("malloc size 1\n");
    int *ptr2 = my_malloc(1);
    printf("++++++++++++++++++++++++++++++++++++\n");

    printf("head %p, head next: %p\n", head, head->flink);
    my_free(ptr2);

    printf("head %p, head next: %p, head next next: %p\n", head, head->flink, head->flink->flink);
//
////    print_list();
    printf("++++++++++++++++++++++++++++++++++++\n");
    printf("malloc size 7\n");
    int *ptr3 = my_malloc(7);
    printf("head %p, head next: %p, head next next: %p\n", head, head->flink, head->flink);
//
////    printf("head %p", head);
//
////    print_list();
////    printf("++++++++++++++++++++++++++++++++++++\n");
//    printf("malloc size 8\n");
//    int *ptr4 = my_malloc(8);
////
////    printf("++++++++++++++++++++++++++++++++++++\n");
//    my_free(ptr3);

//
//    printf("malloc size 9\n");
//    ptr = my_malloc(9);
//
//    printf("malloc size 17\n");
//    ptr = my_malloc(17);
//
//    printf("malloc size 23\n");
//    ptr = my_malloc(23);
//
//    printf("malloc size 24\n");
//    ptr = my_malloc(24);
//
//    printf("malloc size 25\n");
//    ptr = my_malloc(25);
//    printf("malloc size 128\n");
//    ptr = my_malloc(128);

//    printf("malloc size 8098\n");
//    ptr = my_malloc(8098);
//    print_list();
}

void *my_malloc(size_t size){
    printf("heap_end: %p\n", sbrk(0));
    void *chunk_start = sbrk(0);
    size += CHUNKHEADERSIZE;                                        // add header size

    if (size < min_chunk_size()){                                   // if less than 16, set to 16
        size = min_chunk_size();
    } else if (size%8 != 0){                                        // if greater than 16, round up to
        size += 8-size%8;                                           // multiple of 8.
    }
    printf("needed size: %zu\n", size);
    if (free_list_begin() == NULL || find_node(size) == NULL){
        printf("too freaking FAT\n");
        if (size < 8192) {
            sbrk(8192);
            *((int32_t *)chunk_start) = size;
            *((int32_t *)chunk_start + sizeof(int32_t)) = 8888;
            printf("new heap_end: %p\nold end: %p\nstart malloc: %p\n",
                   sbrk(0), chunk_start, (chunk_start + CHUNKHEADERSIZE));
            printf("size: %p, magic#: %p\n", chunk_start, (chunk_start+sizeof(int32_t)));
            printf("size: %d, magic#: %d\n", *((int32_t *)chunk_start),
                   *((int32_t *)chunk_start+sizeof(int32_t)));
            if (8192 - size >= min_chunk_size()){
                FreeListNode node;
                node = (FreeListNode) (chunk_start + size);
                node->size = 8192 - size;
                node->flink = NULL;
                insert_node(node);
            }
            return (chunk_start+CHUNKHEADERSIZE);
        }else{
            sbrk(size);
            *((int32_t *)chunk_start) = size;
            *((int32_t *)chunk_start + sizeof(int32_t)) = 8888;
            printf("new heap_end: %p\nold end: %p\nstart malloc: %p\n",
                   sbrk(0), chunk_start, (chunk_start + CHUNKHEADERSIZE));
            printf("size: %p, magic#: %p\n", chunk_start, (chunk_start+sizeof(int32_t)));
            printf("size: %d, magic#: %d\n", *((int32_t *)chunk_start),
                   *((int32_t *)chunk_start+sizeof(int32_t)));
            return (chunk_start+CHUNKHEADERSIZE);
        }
    }else {
        FreeListNode node = find_node(size);
        if (node == NULL){
            printf("NULL!");
        }
        printf("node add: %p, node size: %zu\n", node, node->size);
        chunk_start = remove_node(size);
        printf("remove add!!: %p, %p\n", head, head->flink);
        printf("new heap_end: %p\nold end: %p\nstart malloc: %p\n",
               sbrk(0), chunk_start, (chunk_start + CHUNKHEADERSIZE));
        *((int32_t *)chunk_start) = size;
        *((int32_t *)chunk_start + sizeof(int32_t)) = 8888;
        printf("size: %p, magic#: %p\n", chunk_start, (chunk_start+sizeof(int32_t)));
        printf("size: %d, magic#: %d\n", *((int32_t *)chunk_start),
               *((int32_t *)chunk_start+sizeof(int32_t)));
//        print_list();
        printf("node size: %zu, %lu\n", node->size, node->size - size);
//        printf("head loc: %p\n", &(*head));
        if (node->size - size >= min_chunk_size()) {
            printf("new node loc: %p\n", chunk_start + size);
            FreeListNode new_node = (FreeListNode) (chunk_start + size);
//            printf("node size2: %zu, %lu\n", node->size, node->size - size);
            new_node->size = node->size - size;
            new_node->flink = NULL;
            insert_node(new_node);
        }
        //print_list();

    }
    printf("node list --> %p", head);
    printf("++++++++++++++++++++++++++++++++++++!!\n");
    printf("what the hell: %p\n", (chunk_start + CHUNKHEADERSIZE));
    printf("EEEEEEEEEEE\n");
    printf("remove add!!: %p, %p\n", head, head->flink);
    return (chunk_start + CHUNKHEADERSIZE);
}

int min_chunk_size(){
    if (sizeof(FreeListNode)>16){
        return sizeof(FreeListNode);
    }else {
        return 16;
    }
}

void my_free(void *ptr){
    printf("FREEING NODE\n");
    FreeListNode n;

    n = (FreeListNode) (ptr - CHUNKHEADERSIZE);
    printf("free node size %p, %d\n", n, *((int32_t *)(ptr - CHUNKHEADERSIZE)));
    n->size = *((int32_t *)(ptr - CHUNKHEADERSIZE));
    n->flink = NULL;
    insert_node(n);
}

FreeListNode free_list_begin(void){
    return head;
}

void print_list(){
    printf("node list --> %p", head);
    printf("node list --> %zu", head->size);
    FreeListNode curr_node = head;
    printf("hellooooo: %d", curr_node->flink != NULL);
//    while(curr_node->flink != NULL){
    printf("%p %zu--> ", curr_node, curr_node->size);
    printf("%p %zu--> ", curr_node->flink, curr_node->flink->size);
        curr_node = curr_node->flink;
//    }
//    printf("NULL\n");
}

void insert_node(FreeListNode node) {
    printf("INSERTING NODE!!!***********\n");
    printf("new node size: %zu\n", node->size);
//    printf("true insert?: %d\n", head->flink == NULL);
    printf("head %p\n", head);

    if (head == NULL || ((head->size) >= node->size)) {
        if (head == NULL){
            head = node;
            head->flink = NULL;
        }else{
            node->flink = head;
            head = node;
        }
    } else {
        FreeListNode curr_node = head;
        while (curr_node != NULL && curr_node->flink->size < node->size) {
            curr_node = curr_node->flink;
        }
        node->flink = curr_node->flink;
        curr_node->flink = node;
    }
}
//    if (head == NULL){
//        printf("head is null\n");
//        head = node;
//        printf("head add: %p, %zu\n", head, head->size);
//    }else if (node < head) {
//        printf("head: %p, node: %p\n", head, node);
//        node->flink = head;
//        head = node;
//        printf("head: %p, head->flink: %p\n", head, head->flink);
//    }else{
//        FreeListNode curr_node = head;
//        while(curr_node->flink != NULL){
//            if (node < curr_node->flink && node > curr_node) {
//                node->flink = curr_node->flink;
//                curr_node->flink = node;
//            }else{
//                curr_node = curr_node->flink;
//            }
//        }
//        if (curr_node->flink == NULL) {
//            if (node > curr_node) {
//                curr_node->flink = node;
//                node->flink = NULL;
//            }
//        }
//    }
//}

FreeListNode remove_node(unsigned sz) {
    printf("REMOVING NODE!!!***********\n");
    FreeListNode node = head;
    FreeListNode prev_node;
    printf("true? remove: %d\n", head->flink == NULL);
    printf("size:: %u\n", sz);
    printf("remove add: %p, %zu, %p\n", head, head->size, head->flink);
    printf("remove add!!: %p, %zu, %p\n", node, node->size, node->flink);
    if (node != NULL && node->size >= sz) {
        printf("remove add!!++==: %p, %p, %d\n", head, node->flink, node->flink == NULL);
//        int test = (node->flink == NULL);
//        printf("test %d", test);
        if ((node->flink) == 0){
            printf("return 1 node!!!!");
            head = NULL;
            return node;
        }else if (head->flink != NULL && head->size >= sz){
            printf("!!!!!!!!!!!!!!!!!!!!!!!!!!11");
            printf("remove add!!: %p, %p, %p\n", head, head->flink, head->flink->flink);
            node = head;
            head = head->flink;
            printf("remove add!!: %p, %p, %p\n", head, head->flink, node);
            return node;
        }
    }
    printf("remove add!!++: %p, %p\n", head, node);
    while (node != NULL && node->size < sz) {
        prev_node = node;
        node = node->flink;
    }
    if (node == NULL)
        return NULL;
    prev_node->flink = node->flink;
    return prev_node;
}
//    }else if (head != NULL && head->flink != NULL){
//        printf("helll\n");
//        if (head->size >= sz){
//            FreeListNode removed_node = head;
//            printf("REMO: %p, %p\n", removed_node, head->flink);
//            head = head->flink;
//            printf("HEAD: %p\n", head);
//            print_list();
//            return removed_node;
//        }
//    }else {
//        while (node->flink != NULL) {
//            if (node->size >= sz) {
//                FreeListNode removed_node = node;
//                node = node->flink;
//                return node;
//            }else{
//                node = node->flink;
//            }
//        }
//    }
//    return NULL;
//}

FreeListNode find_node(unsigned sz){
    printf("FINDING NODE!!!***********\n");
    FreeListNode node = head, prev_node;
    printf("size %u\n", sz);
    printf("true?: %d\n", head->flink == NULL);
    if (node != NULL && node->size >= sz) {
//        head = node->flink;
        return node;
    }
    while (node != NULL && node->size < sz) {
        prev_node = node;
        node = node->flink;
    }
    if (node == NULL)
        return NULL;
//    prev_node->flink = node->flink;
    return prev_node;
//    if (head->flink == NULL){
//        if (head->size >= sz) {
//            printf("what\n");
//            return head;
//        }
//    }else {
//        while (node->flink != NULL) {
//            printf("wile\n");
//            if (node->size >= sz) {
//                printf("what\n");
//                return node;
//            } else
//                node = node->flink;
//        }
//    }
//    return NULL;
}

void coalesce(){

}


