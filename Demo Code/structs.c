#include <stdio.h>

struct charintchar {
    char c1;
    int i;
    char c2;
};

typedef struct charcharint{
    char c1;
    char c2;
    int i;
} CCI;

int main( int argc, char *argv[] ){
    struct charintchar ic = {'a', 10, 'c'};
    CCI ci = {'b', 'd', 9};

    printf("ic: %c %c %d (size: %lu)\n", ic.c1, ic.c2, ic.i, sizeof(ic) );
    printf("ci: %c %c %d (size: %lu)\n", (&ci)->c1, (&ci)->c2, (&ci)->i, sizeof(ci) );

    return 0;
}
