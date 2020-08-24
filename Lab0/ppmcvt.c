// THIS CODE IS OUR OWN WORK, IT WAS WRITTEN WITHOUT CONSULTING A TUTOR 
// OR CODE WRITTEN BY OTHER STUDENTS OUTSIDE OF OUR TEAM. Alex Welsh

#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include "pbm.h"

int main(int argc, char *argv[]){
    int o;
    int convertToPBM = 0;
    int convertToPGM = 0;
    int applySepia = 0;
    int applyMirror = 0;
    int thumbnailScale = 0;
    int tileScale = 0;
    long greyScaleValue = 0;
    int isolateColor = 0;
    int removeColor = 0;
    char *ptr;

    for ( int i=0; i<argc; i++ ){
        printf("Argument[%d]: %s\n", i, argv[i] );
    }

    while ((o = getopt(argc, argv, "bg:i:r:smt:n:o:")) != -1) {
        switch (o) {
        case 'b':
            printf("Option 'b' present\n");
            convertToPBM = 1;
            break;
        case 'g':
            greyScaleValue = strtol(optarg, &ptr, 10);
            if (greyScaleValue < 1 || greyScaleValue > 65535) {
                fprintf(stderr, "Error: Invalid max grayscale pixel value: %s\n", optarg);
                exit(1);
            }
            printf("Option 'g' present with argument: %ld\n", greyScaleValue);
            convertToPGM = 1;
            break;
        case 'i':
            if (!(strcmp("red", optarg) == 0 || strcmp("green", optarg) == 0 || strcmp("blue", optarg) == 0)) {
                fprintf(stderr, "Error: invalid channel specification: (%s); should be 'red', 'green' or 'blue'\n", optarg);
                exit(1);
            }
            if (strcmp("red", optarg) == 0){
                isolateColor = 1;
            }else if (strcmp("green", optarg) == 0){
                isolateColor = 2;
            }else if (strcmp("blue", optarg) == 0){
                isolateColor = 3;
            }
            printf("Option 'i' present with argument: %s or %d\n", optarg, isolateColor);
            break;
        case 'r':
            if (!(strcmp("red", optarg) == 0 || strcmp("green", optarg) == 0 || strcmp("blue", optarg) == 0)) {
                fprintf(stderr, "Error: invalid channel specification: (%s); should be 'red', 'green' or 'blue'\n", optarg);
                exit(1);
            }
            if (strcmp("red", optarg) == 0){
                removeColor = 1;
            }else if (strcmp("green", optarg) == 0){
                removeColor = 2;
            }else if (strcmp("blue", optarg) == 0){
                removeColor = 3;
            }
            printf("Option 'r' present with argument: %s or %d\n", optarg, removeColor);
            break;
        case 's':
            printf("Option 's' present\n");
            applySepia = 1;
            break;
        case 'm':
            printf("Option 'm' present\n");
            applyMirror = 1;
            break;
        case 't':
            thumbnailScale = strtol(optarg, &ptr, 10);
            if (thumbnailScale < 1 || thumbnailScale > 8){
                fprintf(stderr, "Error: Invalid scale factor: %s; must be 1-8\n", optarg);
                exit(1);
            }
            printf("Option 't' present with argument: %d\n", thumbnailScale);
            break;
        case 'n':
            tileScale = strtol(optarg, &ptr, 10);
            if (tileScale < 1 || tileScale > 8){
                fprintf(stderr, "Error: Invalid scale factor: %s; must be 1-8\n", optarg);
                exit(1);
            }
            printf("Option 'n' present with argument: %d\n", tileScale);
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

 //   PPMImage *inputPic = read_ppmfile(argv[argc-1]);

    printf("Input File: %s\n", argv[argc-1]);
    printf("Convert to PBM: %d\nConvert to PGM: %d\nPGM Value: %ld\n", convertToPBM, convertToPGM, greyScaleValue);
    printf("Isolate: %d\nRemove: %d\nSepia: %d\nMirror: %d\n", isolateColor, removeColor, applySepia, applyMirror);
    printf("Thumbnail Scale: %d\nTiling Scale: %d\nOutput: %s\n", thumbnailScale, tileScale, optarg);

    return 0;
}

PPMImage *new_ppmimage(unsigned int width, unsigned int height, unsigned int max){
    PPMImage ppm;
    ppm.width = malloc(width * sizeof(unsigned int));
    ppm.height = malloc(height * sizeof(unsigned int));
    ppm.max = malloc(sizeof(unsigned int));

    ppm.width = width;
    ppm.height = height;
    ppm.max = max;

    return ppm;
}

PGMImage *new_pgmimage( unsigned int width, unsigned int height, unsigned int max){

}

PBMImage *new_pbmimage( unsigned int width, unsigned int height ){

}

void del_ppmimage( PPMImage * ){

}

void del_pgmimage( PGMImage * ){

}

void del_pbmimage( PBMImage * ){

}

PBMImage convertToBitmap (PPMImage image){

}

PGMImage convertToGreyscale (PPMImage image){
    
}

PPMImage applySepiaTransformation (PPMImage image){
    //NewR = 0.393(OldR) + 0.769(OldG) + 0.189(OldB) 
    //NewG = 0.349(OldR) + 0.686(OldG) + 0.168𝑥(OldB) 
    //NewB = 0.272(OldR) + 0.534(OldG) + 0.131𝑥(OldB)
}

PPMImage mirrorImage (PPMImage image){
    
}

PPMImage createThumbnail (PPMImage image){
    
}

PPMImage tileImages (PPMImage image){       // what does this mean? like if n=2 and original is 100*100, then out is 50*100
    
}