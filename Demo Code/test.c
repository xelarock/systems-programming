// This is a test

#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    // int array[4][4] = {{2, 2, 1, 1},
    //                    {2, 2, 1, 1},
    //                    {2, 2, 1, 1},
    //                    {2, 2, 1, 1},};
    // int arrayWidth = 4;
    // int arrayHeigth = 4;

    // //printf("%d", arrayWidth / 2);

    // for (int h = 0; h < arrayHeigth; h++){
    //     for (int w = 0; w < arrayWidth ; w++){
    //         printf("%d", array[h][w]);
    //     }
    //     printf("\n");
    // }

    // for (int h = 0; h < arrayHeigth; h++){
    //     for (int w = 0; w < arrayWidth/2 ; w++){
    //         array[h][arrayWidth - 1 - w] = array[h][w];
    //     }
    // }

    // for (int h = 0; h < arrayHeigth; h++){
    //     for (int w = 0; w < arrayWidth ; w++){
    //         printf("%d ", array[h][w]);
    //     }
    //     printf("\n");
    // }

    printf("%d", (int) 10%3);
    int o;
    while (optind < argc){
        if((o = getopt(argc, argv, "bg:")) != -1){
            switch (o) {
            case 'b':
                printf("case b\n");
                break;
            case 'g':
                printf("Case g with arg: %s\n", optarg);
                break;
            default:
                break;
            }
        }else {
            printf("Non-option Arg: %s\n", argv[optind]);
            optind ++;
        }
    }

}
