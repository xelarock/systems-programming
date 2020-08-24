// THIS CODE IS OUR OWN WORK, IT WAS WRITTEN WITHOUT CONSULTING A TUTOR 
// OR CODE WRITTEN BY OTHER STUDENTS OUTSIDE OF OUR TEAM. Alex Welsh

#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    int o;
    int convert = 0;
    long num;
    char *isolateColor;
    char *removeColor;

    for ( int i=0; i<argc; i++ ){
        printf("Argument[%d]: %s\n", i, argv[i] );
    }

    while ((o = getopt(argc, argv, "bg:i:r:smt:n:o:")) != -1) {
        switch (o) {
        case 'b':
            printf("Option 'b' present\n");
            convert = 0;
            break;
        case 'g':
            printf("Option 'g' present with argument: %s\n", optarg);
            char *ptr;
            num = strtol(optarg, &ptr, 10);
            if (num < 1 || num > 65535) {
                fprintf(stderr, "Error: Invalid max grayscale pixel value: %s\n", optarg);
                exit(1);
            }
            convert = 1;
            break;
        case 'i':
            printf("Option 'i' present with argument: %s\n", optarg);
            if (!(strcmp("red", optarg) == 0 || strcmp("green", optarg) == 0 || strcmp("blue", optarg) == 0)) {
                fprintf(stderr, "Error: invalid channel specification: (%s); should be 'red', 'green' or 'blue'\n", optarg);
                exit(1);
            }
            //isolateColor = (char *) malloc(strlen(optarg) + 1);
            //isolateColor = optarg;
            //printf("Color Isolated: %s", isolateColor);
            break;
        case 'r':
            printf("Option 'r' present with argument: %s\n", optarg);
            if (!(strcmp("red", optarg) == 0 || strcmp("green", optarg) == 0 || strcmp("blue", optarg) == 0)) {
                fprintf(stderr, "Error: invalid channel specification: (%s); should be 'red', 'green' or 'blue'\n", optarg);
                exit(1);
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
            fprintf(stderr, "Usage: ppmcvt [-bgirsmtno] [FILE]\n");
            exit(1);
            break;
        }
    }


    printf("Input File: %s\n", argv[argc-1]);

    return 0;

}