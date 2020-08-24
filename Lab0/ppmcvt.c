// THIS CODE IS OUR OWN WORK, IT WAS WRITTEN WITHOUT CONSULTING A TUTOR 
// OR CODE WRITTEN BY OTHER STUDENTS OUTSIDE OF OUR TEAM. Alex Welsh

#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    int o;

    for ( int i=0; i<argc; i++ ){
        printf("Argument[%d]: %s\n", i, argv[i] );
    }

    while ((o = getopt(argc, argv, "bg:i:r:smt:n:o:")) != -1) {
        switch (o) {
        case 'b':
            printf("Option 'b' present\n");
            break;
        case 'g':
            printf("Option 'g' present with argument: %s\n", optarg);
            char *ptr;
            long num = strtol(optarg, &ptr, 10);
            if (num < 1 || num > 65535) {
                printf("Error: Invalid max grayscale pixel value: %s\n", optarg);
            }
            break;
        case 'i':
            printf("Option 'i' present with argument: %s\n", optarg);
            if (!(strcmp("red", optarg) == 0 || strcmp("green", optarg) == 0 || strcmp("blue", optarg) == 0)) {
                printf("Error: invalid channel specification: (%s); should be 'red', 'green' or 'blue'\n", optarg);
            }
            break;
        case 'r':
            printf("Option 'r' present with argument: %s\n", optarg);
            if (!(strcmp("red", optarg) == 0 || strcmp("green", optarg) == 0 || strcmp("blue", optarg) == 0)) {
                printf("Error: invalid channel specification: (%s); should be 'red', 'green' or 'blue'\n", optarg);
            }
            break;
        case 's':
            printf("Option 's' present\n");
            break;
        case 'm':
            printf("Option 'm' present\n");
            break;
        case 't':
            printf("Option 't' present with argument: %s\n", optarg);
            break;
        case 'n':
            printf("Option 'n' present with argument: %s\n", optarg);
            break;
        case 'o':
            printf("Option 'o' present with argument: %s\n", optarg);
            break;
        default:
            printf("Usage: ppmcvt [-bgirsmtno] [FILE]\n");
            break;
        }
    }

    return 0;

}