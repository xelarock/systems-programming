//
// Created by Alex Welsh on 9/14/20.
//

#include "my_malloc.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>

struct foo{
    char c;
    int i;
}; // what is happenning??????

int main(int argc, char *argv[]){
//    char *a;
//    a = (char *) malloc(4*sizeof(char));
//    printf("%p, %s, %p\n", &a, a, &(*a));
//    printf("%c %c %c %c %c %c %s\n", a[0], a[1], a[2], a[3], a[4], a[5], a);

//    struct foo * f; //this line is not the problem
//    f = malloc(sizeof(struct foo));
//    f->c = 'a';
//    f->i = 27;
//
//    printf("%c, %d\n", f->c, f->i);

    int *i;
    printf("%p, %p\n", i, (void *)i+2);
    short *j;
    printf("%p, %p\n", j, j+2);
}
