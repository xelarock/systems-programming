// THIS CODE IS OUR OWN WORK, IT WAS WRITTEN WITHOUT CONSULTING A TUTOR 
// OR CODE WRITTEN BY OTHER STUDENTS OUTSIDE OF OUR TEAM. Alex Welsh

#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include "pbm.h"

typedef struct {
    int convertToPBM, convertToPGM, applySepia, applyMirror, thumbnailScale, tileScale, isolateColor, removeColor;
    long greyScaleValue;
    char *output[60];
}Options;

Options parseArgs(int argc, char *argv[]);
void checkMultipleTransformations (int numTrans);
PBMImage * convertToBitmap (PPMImage *image);
PGMImage * convertToGreyscale (PPMImage *ppm, long greyScaleMax);
PPMImage * applypSepiaTransformation (PPMImage *image);
PPMImage * mirrorImage (PPMImage *image);
PPMImage * createThumbnail (PPMImage *image, int thumbnailScale);
PPMImage * tileImages (PPMImage *image, int tileScale);
PPMImage * isolateColor (PPMImage *image, int isolateColor);
PPMImage * removeColor (PPMImage *image, int removeColor);

int main(int argc, char *argv[]){

    if (argc>6){
        fprintf(stderr, "Error: Multiple transformations specified\n");
        exit(1);
    }

    Options opts = parseArgs(argc, argv);

    PPMImage *inputPic = read_ppmfile(argv[argc-1]);

    if (opts.convertToPBM == 1){
        PBMImage *outputImage = convertToBitmap(inputPic);
        write_pbmfile(outputImage, *opts.output);
    }else if (opts.convertToPGM == 1){
        PGMImage *outputImage = convertToGreyscale(inputPic, opts.greyScaleValue);
        write_pgmfile(outputImage, *opts.output);
    }else if (opts.applySepia == 1){
        PPMImage *outputImage = applypSepiaTransformation(inputPic);
        write_ppmfile(outputImage, *opts.output);
    }else if (opts.applyMirror == 1){
        PPMImage *outputImage = mirrorImage(inputPic);
        write_ppmfile(outputImage, *opts.output);
    }else if (opts.thumbnailScale >= 1){
        PPMImage *outputImage = createThumbnail(inputPic, opts.thumbnailScale);
        write_ppmfile(outputImage, *opts.output);
    }else if (opts.tileScale >= 1){
        PPMImage *outputImage = tileImages(inputPic, opts.tileScale);
        write_ppmfile(outputImage, *opts.output);
    }else if (opts.isolateColor >= 1){
        PPMImage *outputImage = isolateColor(inputPic, opts.isolateColor);
        write_ppmfile(outputImage, *opts.output);
    }else if (opts.removeColor >= 1){
        PPMImage *outputImage = removeColor(inputPic, opts.removeColor);
        write_ppmfile(outputImage, *opts.output);
    }

    //write_ppmfile(inputPic, *opts.output);

    return 0;
}

Options parseArgs(int argc, char *argv[]){
    Options opts;
    opts.convertToPBM = 0;
    opts.convertToPGM = 0;
    opts.applySepia = 0;
    opts.applyMirror = 0;
    opts.thumbnailScale = 0;
    opts.tileScale = 0;
    opts.isolateColor = 0;
    opts.removeColor = 0;
    opts.greyScaleValue = 0;
    int o;
    char *ptr;
    int moreThanOneTrans = 0;

    for ( int i=0; i<argc; i++ ){
        printf("Argument[%d]: %s\n", i, argv[i] );
    }

    while ((o = getopt(argc, argv, "bg:i:r:smt:n:o:")) != -1) {
        switch (o) {
        case 'b':
            printf("Option 'b' present\n");
            opts.convertToPBM = 1;
            moreThanOneTrans+=1;
            break;
        case 'g':
            checkMultipleTransformations(moreThanOneTrans);
            opts.greyScaleValue = strtol(optarg, &ptr, 10);
            if (opts.greyScaleValue < 1 || opts.greyScaleValue > 65535) {
                fprintf(stderr, "Error: Invalid max grayscale pixel value: %s\n", optarg);
                exit(1);
            }
            printf("Option 'g' present with argument: %ld\n", opts.greyScaleValue);
            opts.convertToPGM = 1;
            moreThanOneTrans+=1;
            break;
        case 'i':
            checkMultipleTransformations(moreThanOneTrans);
            if (!(strcmp("red", optarg) == 0 || strcmp("green", optarg) == 0 || strcmp("blue", optarg) == 0)) {
                fprintf(stderr, "Error: invalid channel specification: (%s); should be 'red', 'green' or 'blue'\n", optarg);
                exit(1);
            }
            if (strcmp("red", optarg) == 0){
                opts.isolateColor = 1;
            }else if (strcmp("green", optarg) == 0){
                opts.isolateColor = 2;
            }else if (strcmp("blue", optarg) == 0){
                opts.isolateColor = 3;
            }
            printf("Option 'i' present with argument: %s or %d\n", optarg, opts.isolateColor);
            moreThanOneTrans+=1;
            break;
        case 'r':
            checkMultipleTransformations(moreThanOneTrans);
            if (!(strcmp("red", optarg) == 0 || strcmp("green", optarg) == 0 || strcmp("blue", optarg) == 0)) {
                fprintf(stderr, "Error: invalid channel specification: (%s); should be 'red', 'green' or 'blue'\n", optarg);
                exit(1);
            }
            if (strcmp("red", optarg) == 0){
                opts.removeColor = 1;
            }else if (strcmp("green", optarg) == 0){
                opts.removeColor = 2;
            }else if (strcmp("blue", optarg) == 0){
                opts.removeColor = 3;
            }
            printf("Option 'r' present with argument: %s or %d\n", optarg, opts.removeColor);
            moreThanOneTrans+=1;
            break;
        case 's':
            checkMultipleTransformations(moreThanOneTrans);
            printf("Option 's' present\n");
            opts.applySepia = 1;
            moreThanOneTrans+=1;
            break;
        case 'm':
            checkMultipleTransformations(moreThanOneTrans);
            printf("Option 'm' present\n");
            opts.applyMirror = 1;
            moreThanOneTrans+=1;
            break;
        case 't':
            checkMultipleTransformations(moreThanOneTrans);
            opts.thumbnailScale = strtol(optarg, &ptr, 10);
            if (opts.thumbnailScale < 1 || opts.thumbnailScale > 8){
                fprintf(stderr, "Error: Invalid scale factor: %s; must be 1-8\n", optarg);
                exit(1);
            }
            printf("Option 't' present with argument: %d\n", opts.thumbnailScale);
            moreThanOneTrans+=1;
            break;
        case 'n':
            checkMultipleTransformations(moreThanOneTrans);
            opts.tileScale = strtol(optarg, &ptr, 10);
            if (opts.tileScale < 1 || opts.tileScale > 8){
                fprintf(stderr, "Error: Invalid scale factor: %s; must be 1-8\n", optarg);
                exit(1);
            }
            printf("Option 'n' present with argument: %d\n", opts.tileScale);
            moreThanOneTrans+=1;
            break;
        case 'o':
            *opts.output = optarg;
            break;
        default:
            fprintf(stderr, "Usage: ppmcvt [-bgirsmtno] [FILE]\n");
            exit(1);
            break;
        }
    }

    if (moreThanOneTrans == 0){
        opts.convertToPBM = 1;
    }

    printf("Input File: %s\n", argv[argc-1]);
    printf("Convert to PBM: %d\nConvert to PGM: %d\nGreyScale Value: %ld\n", opts.convertToPBM, opts.convertToPGM, opts.greyScaleValue);
    printf("Isolate: %d\nRemove: %d\nSepia: %d\nMirror: %d\n", opts.isolateColor, opts.removeColor, opts.applySepia, opts.applyMirror);
    printf("Thumbnail Scale: %d\nTiling Scale: %d\nOutput: %s\n", opts.thumbnailScale, opts.tileScale, *opts.output);

    return opts;
}

void checkMultipleTransformations (int numTrans){
    if (numTrans > 0){
        fprintf(stderr, "Error: Multiple transformations specified\n");
        exit(1);
    }
}

PBMImage * convertToBitmap (PPMImage *ppm){
    unsigned int height = ppm->height;
    unsigned int width = ppm->width;
    PBMImage *pbm = new_pbmimage(width, height);
    for (int h = 0; h < height; h++){
        for (int w = 0; w < width; w++){
            double average = (double) ((double)(ppm->pixmap[0][h][w] + ppm->pixmap[1][h][w] + ppm->pixmap[2][h][w]) / 3.0);
            double threshold = (double) ((double)ppm->max / 2.0);
            if (average < threshold){
                pbm->pixmap[h][w] = 1;
            }else{
                pbm->pixmap[h][w] = 0;
            }
        }
    }
    return pbm;
}

PGMImage * convertToGreyscale (PPMImage *ppm, long greyScaleMax){
    unsigned int height = ppm->height;
    unsigned int width = ppm->width;
    PGMImage *pgm = new_pgmimage(width, height, greyScaleMax);
    for (int h = 0; h < height; h++){
        for (int w = 0; w < width; w++){
            double average = ((double)(ppm->pixmap[0][h][w] + ppm->pixmap[1][h][w] + ppm->pixmap[2][h][w]) / 3.0);
            double newVal =  (int) (average * ((double) greyScaleMax / (double) ppm->max));
            pgm->pixmap[h][w] = newVal;
        }
    }
    return pgm; 
}

PPMImage * applypSepiaTransformation (PPMImage *image){
    unsigned int height = image->height;
    unsigned int width = image->width;
    unsigned int max = image->max;
    PPMImage *ppm = new_ppmimage(width, height, max);
    for (int h = 0; h < height; h++){
        for (int w = 0; w < width; w++){
            double oldR = (double) image->pixmap[0][h][w];
            double oldG = (double) image->pixmap[1][h][w];
            double oldB = (double) image->pixmap[2][h][w];
            int newR = (int) (0.393*(oldR) + 0.769*(oldG) + 0.189*(oldB)); 
            int newG = (int) (0.349*(oldR) + 0.686*(oldG) + 0.168*(oldB));
            int newB = (int) (0.272*(oldR) + 0.534*(oldG) + 0.131*(oldB));
            if (newR > max)
                newR = max;
            if (newG > max)
                newG = max;
            if (newB > max)
                newB = max;
            ppm->pixmap[0][h][w] = newR;
            ppm->pixmap[1][h][w] = newG;
            ppm->pixmap[2][h][w] = newB;
        }
    }
    return ppm;
}

PPMImage * mirrorImage (PPMImage *image){
    unsigned int height = image->height;
    unsigned int width = image->width;
    unsigned int max = image->max;
    PPMImage *ppm = new_ppmimage(width, height, max);
    for (int h = 0; h < height; h++){
        for (int w = 0; w < width/2 ; w++){
            ppm->pixmap[0][h][width - 1 - w] = image->pixmap[0][h][w];
            ppm->pixmap[1][h][width - 1 - w] = image->pixmap[1][h][w];
            ppm->pixmap[2][h][width - 1 - w] = image->pixmap[2][h][w];
        }
        for (int w = 0; w <= width/2 ; w++){
            ppm->pixmap[0][h][w] = image->pixmap[0][h][w];
            ppm->pixmap[1][h][w] = image->pixmap[1][h][w];
            ppm->pixmap[2][h][w] = image->pixmap[2][h][w];
        }
    }
    return ppm;
}

PPMImage * isolateColor (PPMImage *image, int isolateColor){
    isolateColor = isolateColor - 1;
    for (int h = 0; h < image->height; h++){
        for (int w = 0; w < image->width; w++){
            if (isolateColor == 0){
                image->pixmap[1][h][w] = 0;
                image->pixmap[2][h][w] = 0;
            }else if (isolateColor == 1){
                image->pixmap[0][h][w] = 0;
                image->pixmap[2][h][w] = 0;
            }
            else if (isolateColor == 2){
                image->pixmap[0][h][w] = 0;
                image->pixmap[1][h][w] = 0;
            }
        }
    }
    return image;
}

PPMImage * removeColor (PPMImage *image, int removeColor){
    removeColor = removeColor - 1;
    for (int h = 0; h < image->height; h++){
        for (int w = 0; w < image->width; w++){
            image->pixmap[removeColor][h][w] = 0;
        }
    }
    return image;
}

PPMImage * createThumbnail (PPMImage *image, int thumbnailScale){
    unsigned int height = (unsigned int) (image->height)/thumbnailScale;
    unsigned int width = (unsigned int) (image->width)/thumbnailScale;
    if ((image->width%thumbnailScale) != 0){
        width++;
    }
    if ((image->height%thumbnailScale) != 0){
        height++;
    }
    unsigned int max = image->max;
    PPMImage *ppm = new_ppmimage(width, height, max);
    for (int h = 0; h < height; h++){
        for (int w = 0; w < width ; w++){
            //printf("H: %d, W: %d", h * thumbnailScale, w * thumbnailScale);
            ppm->pixmap[0][h][w] = image->pixmap[0][h * thumbnailScale][w * thumbnailScale];
            ppm->pixmap[1][h][w] = image->pixmap[1][h * thumbnailScale][w * thumbnailScale];
            ppm->pixmap[2][h][w] = image->pixmap[2][h * thumbnailScale][w * thumbnailScale];
        }
    }
    return ppm;
}

PPMImage * tileImages (PPMImage *image, int tileScale){       
    PPMImage *thumbnail = createThumbnail(image, (int) tileScale);
    unsigned int height = image->height;
    unsigned int width = image->width;
    unsigned int max = image->max;
    PPMImage *ppm = new_ppmimage(width, height, max);
    int thumbHeight = 0;
    int thumbWidth = 0;
    for (int h = 0; h < height; h++){
        if (thumbHeight == thumbnail->height)
            thumbHeight = 0;
        for (int w = 0; w < width ; w++){
            if (thumbWidth == thumbnail->width)
                thumbWidth = 0;
            ppm->pixmap[0][h][w] = thumbnail->pixmap[0][thumbHeight][thumbWidth];
            ppm->pixmap[1][h][w] = thumbnail->pixmap[1][thumbHeight][thumbWidth];
            ppm->pixmap[2][h][w] = thumbnail->pixmap[2][thumbHeight][thumbWidth];
            thumbWidth++;
        }
        thumbWidth = 0;
        thumbHeight++;
    }
    return ppm;
}

PPMImage *new_ppmimage(unsigned int width, unsigned int height, unsigned int max){
    PPMImage *ppm;
    ppm = malloc(sizeof(PPMImage));

    for (int i = 0; i < 3; i++){
        ppm->pixmap[i] = malloc(height * sizeof(unsigned int *));
        for (unsigned int h=0; h<height; h++)
            ppm->pixmap[i][h] = malloc(width * sizeof(unsigned int));
    }

    ppm->width = width;
    ppm->height = height;
    ppm->max = max;

    return ppm;
}

PGMImage *new_pgmimage( unsigned int width, unsigned int height, unsigned int max){
    PGMImage *pgm;
    pgm = malloc(sizeof(PGMImage));

    pgm->pixmap = malloc(height * sizeof(unsigned int *));
    for (int h = 0; h < height; h++)
        pgm->pixmap[h] = malloc(width * sizeof(unsigned int));

    pgm->width = width;
    pgm->height = height;
    pgm->max = max;
    return pgm;
}

PBMImage *new_pbmimage( unsigned int width, unsigned int height ){
    PBMImage *pbm;
    pbm = malloc(sizeof(PBMImage));

    pbm->pixmap = malloc(height * sizeof(unsigned int *));
    for (int h = 0; h < height; h++)
        pbm->pixmap[h] = malloc(width * sizeof(unsigned int));

    pbm->width = width;
    pbm->height = height;

    return pbm;
}

void del_ppmimage( PPMImage * ppm){
    for (int i = 0; i<= 2; i++){
        for (int h = 0; h < ppm->height; h++)
            free(ppm->pixmap[i][h]);
    }
    free(ppm->pixmap);
    free(ppm);
}

void del_pgmimage( PGMImage * pgm){
    for (int h = 0; h < pgm->height; h++)
        free(pgm->pixmap[h]);
    free(pgm->pixmap);
    free(pgm);
}

void del_pbmimage( PBMImage * pbm){
    for (int h = 0; h < pbm->height; h++)
        free(pbm->pixmap[h]);
    free(pbm->pixmap);
    free(pbm);
}