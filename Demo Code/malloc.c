#include <stdio.h>
#include <stdlib.h>

void print_chararray( char *c, unsigned long len ){
    printf("Array address: %p; len: %lu; sizeof(c):%lu\n", c, len, sizeof(c) );
    
    for( int i=0; i<=len; i++){
        printf("%d\n", i);
        printf("\tarray[%2d]: '%c' (%d)\n", i, c[i], c[i]);
    }
    printf("\n");
}

int main( int argc, char *argv[] ){
    char * carray1;
    char ** carray2;
    
    //printf("carray1D[0]: %c", carray1[0]);
    
    carray1 = malloc( 4 * sizeof(char) );
    carray1[0] = 'b';
    carray1[1] = 'a';
    carray1[2] = 'r';
    carray1[3] = '\0';
    
    printf("carray1: %p; &carray1: %p\n", carray1, &carray1);
    print_chararray(carray1, 4);
    
    carray2 = malloc( 2 * sizeof(char *));
    carray2[0] = malloc( 3 * sizeof(char));
    carray2[1] = malloc( 3 * sizeof(char));

    carray2[0][0] = 'f';
    carray2[0][1] = 'o';
    carray2[0][2] = 'o';
    carray2[0][3] = '\0'; //error: may or may not trigger

    carray2[1][0] = 'b';
    carray2[1][1] = 'a';
    carray2[1][2] = 'r';
    carray2[1][3] = '\0';  //error: may or may not trigger

    for(int i=0; i<2; i++){
        printf("carray2[%d]: %p; &carray1: %p\n", i, carray2[i], &carray2[i]);
        print_chararray(carray2[i], 3);
    }
    
    return 0;
}
