//
// Created by Alex Welsh on 9/14/20.
//

#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>

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

    char buf[4];

    FILE *file1 = fopen("dir1/file1.txt", "r");
    printf("loc: %lu\n", ftell(file1));
    fseek(file1, 0, SEEK_END);
    printf("loc2: %lu\n", ftell(file1));
    fseek(file1, 4, SEEK_SET);
    fread(&buf, 4, 1, file1);
    printf("loc2: %lu, %c\n", ftell(file1), buf[0]);
}
