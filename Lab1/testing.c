//
// Created by Alex Welsh on 9/14/20.
//

#include "my_malloc.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>

int main(int argc, char *argv[]){
    printf("out: %d\n", 17%8);
    printf("out: %d\n", 2 == 1);
    unsigned size = 1000000000000;
    size_t head = 8168;
    printf("out: %u, %zu, %d\n", size, head, head >= size);
}
