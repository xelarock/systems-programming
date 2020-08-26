#include <stdio.h>

int main(int argc, char *argv[] ){
    char carray1[][4]={{'f','o','o','\0'},{'b','a','r','\0'}};
    char carray2[2][3]={'f','o','o','b','a','r',};
    
    printf("sizeof(carray1): %lu. addr of carray1: %p\n", sizeof(carray1), carray1 );
    for(int i=0; i<2; i++) {
        printf("carray1[%d]: \"%s\" (%p)\n", i, carray1[i], &carray1[i] );
        for( int j=0; j<4; j++){
            printf("\tcarray1[%d][%d]: '%c' (%p)\n", i, j, carray1[i][j], &carray1[i][j]);
        }
    }

    printf("\nsizeof(carray2): %lu. addr of carray2: %p\n", sizeof(carray2), carray2 );
    for(int i=0; i<2; i++) {
        printf("carray2[%d]: \"%s\" (%p)\n", i, carray2[i], &carray2[i] );
        for( int j=0; j<3; j++){
            printf("\tcarray2[%d][%d]: '%c' (%p)\n", i, j, carray2[i][j], &carray2[i][j]);
        }
    }
    
    return 0;
}
