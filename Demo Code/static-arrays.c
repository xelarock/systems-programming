#include <stdio.h>

void print_chararray( char c[], unsigned long len ){
    printf("Array address: %p; len: %lu; sizeof(c):%lu\n", c, len, sizeof(c) );
    
    for( int i=0; i<len; i++){
        printf("\tarray[%2d]: '%c' (%d)\n", i, c[i], c[i]);
    }
    printf("\n");
}

int main(int argc, char *argv[] ){
    char carray1[]="Sample initialized character array";
    char carray2[]={0,1,2,3, 32, 33, 34, 35};
    char carray3[4];
    

    //carray3 = "bar";
    carray3[0] = 'b';
    carray3[1] = 'a';
    carray3[2] = 'r';
    carray3[2] = 114;
    carray3[3] = '\0';
    
    printf("carray1: %p; &carray1: %p\n", carray1, &carray1);
    print_chararray(carray1, sizeof(carray1));
    
    printf("carray2: %p; &carray2: %p\n", carray2, &carray2);
    print_chararray(carray2, sizeof(carray2));
    
    printf("carray3: %p; &carray2: %p\n", carray3, &carray3);
    print_chararray(carray3, sizeof(carray3));

    return 0;
}
