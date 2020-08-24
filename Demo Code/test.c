// This is a test

#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    char str[] = "65535";
    char *ptr;
    long num;

    printf("len = %lu", strlen(str) + 1);
    num = strtol(str, &ptr, 10);
    printf("Num = %ld", num);
}
