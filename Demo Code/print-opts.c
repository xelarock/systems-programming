#include <stdio.h>
#include <getopt.h>

int main(int argc, char *argv[]){
    int o;
    
    for( int i=0; i<argc; i++ ){
        printf("Argument[%d]: %s\n", i, argv[i] );
    }
    
    while ((o = getopt(argc, argv, "ab:")) != -1) {
        switch (o) {
        case 'a':
            printf("Option 'a' present\n");
            break;
        case 'b':
            printf("Option 'b' present. Arg = %s\n", optarg);
            break;
        default: /* '?' */
            printf("Unexpected option\n");
            break;
        }
    }

    printf("testing demo branch3\n");

    return 0;
}
