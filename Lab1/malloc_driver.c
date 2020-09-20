#include "my_malloc.h"
#include <stddef.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]){
    int *ptr = my_malloc(16);

    int *ptr22 = my_malloc(17);

    int *ptr10 = my_malloc(16);
    *ptr22 = 5;

    int *ptr4 = my_malloc(100000000000);

    my_free(ptr10);
    my_free(ptr10);

    printf("herll\n");
    my_free(ptr4);

    int row = 2, col = 3;
    int *arr = (int *) my_malloc(row * col * sizeof(int));
    int i, j;
    for (i = 0; i < row; i++)
        for (j = 0; j < col; j++)
            *(arr + i*col + j) = i + j;
    printf("The matrix elements are:\n");
    for (i = 0; i < row; i++) {
        for (j = 0; j < col; j++) {
            printf("%d ", *(arr + i*col + j));
        }
        printf("\n");
    }
    my_free(arr);

    int* ptr31 = my_malloc(sizeof(int));
    *ptr31 = 5;
    printf("address: %p, value: %d\n",ptr31, *ptr31);
    my_free(ptr31);

    for(i = 0; i < 5; i++){
        int* p = my_malloc(sizeof(int));
        *p = i;
        printf("address: %p, value: %d\n",p, *p);
    }

    my_free(NULL);

    int x;
    my_free(&x);

    void* p = my_malloc(1);
    my_free(p);
    printf("p has been freed\n");
    my_free(p + 1);

    printf("HHHHHHHHH\n");
    p = my_malloc(1);
    my_free(p);
    printf("HHHHHHHHH\n");
    my_free(p);

    void* ptr29 = my_malloc(sizeof(int));
    *((int*)ptr29) = 5;
    printf("integer: address: %p, value: %d\n", ptr29, *((int*)ptr29));
    my_free(ptr29);
    char* d = "Hello World";
    ptr29 = my_malloc(sizeof(char) * (strlen(d) + 1));
    memcpy(ptr29, d, strlen(d));
    printf("String: address: %p, value: %s\n", ptr29, (char*)ptr29);
    my_free(ptr29);
    ptr29 = my_malloc(sizeof(float));
    *((float*)ptr29) = 1.2345;
    printf("float: address: %p, value: %f\n", ptr29, *((float*)ptr29));
    my_free(ptr29);

//    void* ptr47;
//    for(i = 1; (ptr47 = my_malloc(i*1000000)) != NULL; i++){
//        printf("%d bytes allocated\n", i*1000000);
//        if (i%3 != 0)
//            my_free(ptr47);
//    }
    int *q = my_malloc(10);
    my_free(q);

//    int i = 5;
//    free(&i);
//    my_free(&i);

    int *ptr0 = my_malloc(16);

    int *ptr27 = my_malloc(16);

    int *ptr39 = my_malloc(16);

    my_free(ptr27);
    coalesce_free_list();
    my_free(ptr0);
    coalesce_free_list();

    int r = 3, c = 4, count;

    int **arr1 = (int **)my_malloc(r * sizeof(int *));
    for (i=0; i<r; i++)
        arr1[i] = (int *)my_malloc(c * sizeof(int));

    // Note that arr[i][j] is same as *(*(arr+i)+j)
    count = 0;
    for (i = 0; i <  r; i++)
        for (j = 0; j < c; j++)
            arr1[i][j] = ++count;  // OR *(*(arr+i)+j) = ++count

    for (i = 0; i <  r; i++)
        for (j = 0; j < c; j++)
            printf("%d ", arr1[i][j]);

    for (i=0; i<r; i++)
        my_free(arr1[i]);
    my_free(arr1);

}

