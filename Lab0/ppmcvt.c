// THIS CODE IS OUR OWN WORK, IT WAS WRITTEN WITHOUT CONSULTING A TUTOR 
// OR CODE WRITTEN BY OTHER STUDENTS OUTSIDE OF OUR TEAM. Alex Welsh

#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include "pbm.h"

typedef struct {                // defines the options structure to hold the options for the program.
    int convertToPBM, convertToPGM, applySepia, applyMirror, thumbnailScale, tileScale, isolateColor, removeColor;
    long greyScaleValue;
    char *output;
    char *input;
}Options;

Options parseArgs(int argc, char *argv[]);                          // method declarations
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

    Options opts = parseArgs(argc, argv);                                   // parse options var and read in input file
    PPMImage *inputPic = read_ppmfile(opts.input);                          // parseArgs check for args errors so at this
                                                                            // point, the transformation info is all set
    if (opts.convertToPBM == 1){                                            // convert input to bitmap, write, and free mem
        PBMImage *outputImage = convertToBitmap(inputPic);
        write_pbmfile(outputImage, opts.output);
        del_ppmimage(inputPic);
        del_pbmimage(outputImage);
        exit(0);
    }else if (opts.convertToPGM == 1){                                      // convert input to greyscale, write, and free mem
        PGMImage *outputImage = convertToGreyscale(inputPic, opts.greyScaleValue); 
        write_pgmfile(outputImage, opts.output);
        del_ppmimage(inputPic);
        del_pgmimage(outputImage);
        exit(0);
    }else if (opts.applySepia == 1){                                        // convert input to sepia, write, and free mem
        PPMImage *outputImage = applypSepiaTransformation(inputPic);
        write_ppmfile(outputImage, opts.output);
        del_ppmimage(inputPic);
        del_ppmimage(outputImage);
        exit(0);
    }else if (opts.applyMirror == 1){                                       // mirror input, write, and free mem
        PPMImage *outputImage = mirrorImage(inputPic);
        write_ppmfile(outputImage, opts.output);
        del_ppmimage(inputPic);
        del_ppmimage(outputImage);
        exit(0);
    }else if (opts.thumbnailScale >= 1){                                    // create thumbnail, write, and free mem
        PPMImage *outputImage = createThumbnail(inputPic, opts.thumbnailScale);
        write_ppmfile(outputImage, opts.output);
        del_ppmimage(inputPic);
        del_ppmimage(outputImage);
        exit(0);
    }else if (opts.tileScale >= 1){                                         // tile image, write, and free mem
        PPMImage *outputImage = tileImages(inputPic, opts.tileScale);
        write_ppmfile(outputImage, opts.output);
        del_ppmimage(inputPic);
        del_ppmimage(outputImage);
        exit(0);
    }else if (opts.isolateColor >= 1){                                      // isolate color channel, write, and free mem
        PPMImage *outputImage = isolateColor(inputPic, opts.isolateColor);
        write_ppmfile(outputImage, opts.output);
        del_ppmimage(inputPic);
        del_ppmimage(outputImage);
        exit(0);    
    }else if (opts.removeColor >= 1){                                       // remove color channel, write, and free mem
        PPMImage *outputImage = removeColor(inputPic, opts.removeColor);
        write_ppmfile(outputImage, opts.output);
        del_ppmimage(inputPic);
        del_ppmimage(outputImage);
        exit(0);
    }
    exit(0);
    return 0;
}

// parseArgs take in the number of arguments and the array of arguments and saves them into an Options struct. It also
// check for errors in the arguments given and will exit(1) if there are errors. 

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
    int containsOutput = 0;
    int containsInput = 0;
    int o;
    char *ptr;
    int moreThanOneTrans = 0;   // Counts the number of transformations, prevents more than one transformation from happening

    while (optind < argc){                                              // Iterate through all option arguments
        if ((o = getopt(argc, argv, "bg:i:r:smt:n:o:")) != -1) {        // If there is an option agument, parse it
            switch (o) {
            case 'b':                                                   // if bitmap is specified, save to Options
                opts.convertToPBM = 1;
                moreThanOneTrans+=1;
                break;
            case 'g':
                checkMultipleTransformations(moreThanOneTrans);         // check if more than one transformation is on
                opts.greyScaleValue = strtol(optarg, &ptr, 10);         // read in greyscale value and check if valid
                if (opts.greyScaleValue < 1 || opts.greyScaleValue > 65535) {
                    fprintf(stderr, "Error: Invalid max grayscale pixel value: %s; must be less than 65,536\n", optarg);
                    exit(1);
                }
                opts.convertToPGM = 1;                                  // turn on greyscale option
                moreThanOneTrans+=1;
                break;
            case 'i':                                           // check if red, green, or blue is specifed to be isolated.
                checkMultipleTransformations(moreThanOneTrans);
                if (!(strcmp("red", optarg) == 0 || strcmp("green", optarg) == 0 || strcmp("blue", optarg) == 0)) {
                    fprintf(stderr, "Error: invalid channel specification: (%s); should be 'red', 'green' or 'blue'\n", optarg);
                    exit(1);
                }                                               // opts.isolateColor initallized at 0 (option is OFF)
                if (strcmp("red", optarg) == 0){                // red is saved at one
                    opts.isolateColor = 1;
                }else if (strcmp("green", optarg) == 0){        // green saved as two
                    opts.isolateColor = 2;
                }else if (strcmp("blue", optarg) == 0){         // blue saved as three
                    opts.isolateColor = 3;
                }
                moreThanOneTrans+=1;                            // if opts.isolateColor != 0, then option is ON
                break;
            case 'r':                                           // check if red, green, or blue is specifed to be removed.
                checkMultipleTransformations(moreThanOneTrans);
                if (!(strcmp("red", optarg) == 0 || strcmp("green", optarg) == 0 || strcmp("blue", optarg) == 0)) {
                    fprintf(stderr, "Error: invalid channel specification: (%s); should be 'red', 'green' or 'blue'\n", optarg);
                    exit(1);
                }                                               // opts.removeColor initallized at 0 (option is OFF)
                if (strcmp("red", optarg) == 0){                // red is saved at one
                    opts.removeColor = 1;
                }else if (strcmp("green", optarg) == 0){        // green is saved at two
                    opts.removeColor = 2;
                }else if (strcmp("blue", optarg) == 0){         // blue is saved at three
                    opts.removeColor = 3;
                }
                moreThanOneTrans+=1;                            // if opts.removeColor != 0, then option is ON
                break;
            case 's':                                           // turns on the sepia transformation
                checkMultipleTransformations(moreThanOneTrans);
                opts.applySepia = 1;
                moreThanOneTrans+=1;
                break;
            case 'm':                                           // turns on the mirror transformation
                checkMultipleTransformations(moreThanOneTrans);
                opts.applyMirror = 1;
                moreThanOneTrans+=1;
                break;
            case 't':                                           // turns on thumbnail option with given scaling
                checkMultipleTransformations(moreThanOneTrans);
                opts.thumbnailScale = strtol(optarg, &ptr, 10);
                if (opts.thumbnailScale < 1 || opts.thumbnailScale > 8){    // checks if the scaling factor is allowed
                    fprintf(stderr, "Error: Invalid scale factor: %d; must be 1-8\n", opts.thumbnailScale);
                    exit(1);
                }
                moreThanOneTrans+=1;
                break;
            case 'n':                                           // turns on tiling option with given scaling
                checkMultipleTransformations(moreThanOneTrans);
                opts.tileScale = strtol(optarg, &ptr, 10);
                if (opts.tileScale < 1 || opts.tileScale > 8){  // checks if the scaling factor is allowed.
                    fprintf(stderr, "Error: Invalid scale factor: %s; must be 1-8\n", optarg);
                    exit(1);
                }
                moreThanOneTrans+=1;
                break;
            case 'o':                                           // saves the output file location to options
                opts.output = optarg;
                containsOutput = 1;
                break;
            default:                                            // if unallowed option, show usage
                fprintf(stderr, "Usage: ppmcvt [-bgirsmtno] [FILE]\n");
                exit(1);
                break;
            }
        }else{                                                  // parses non-options args, aka input file location
            if (containsInput > 0){                             // if more than one input has been specified, show usage
                fprintf(stderr, "Usage: ppmcvt [-bgirsmtno] [FILE]\n");
                exit(1);
            }
            opts.input = argv[optind];                          // set input file location and move to the next argument to 
            optind ++;                                          // parse. Indicate that input file has been given.
            containsInput = 1;  
        }
    }

    if (containsOutput == 0){                                   // check if output file given
        fprintf(stderr, "Error: No output file specified\n");
        exit(1);
    }
    if (containsInput == 0){                                    // check if input file given
        fprintf(stderr, "Error: No input file specified\n");
        exit(1);
    }
    if (moreThanOneTrans == 0){                                 // if no transformation is specified, set  
        opts.convertToPBM = 1;                                  // default to convert to bitmap
    }
    return opts;                                                // return all of the specified options
}

// checks if mutliple transformations have been specified. If the count is more than one, print error and quit
void checkMultipleTransformations (int numTrans){
    if (numTrans > 0){
        fprintf(stderr, "Error: Multiple transformations specified\n");
        exit(1);
    }
}

// converts input PPMImage to PBMImage
PBMImage * convertToBitmap (PPMImage *ppm){
    unsigned int height = ppm->height;
    unsigned int width = ppm->width;
    PBMImage *pbm = new_pbmimage(width, height);        // allocate space for new pbm image
    for (int h = 0; h < height; h++){                   // iterate across each pixel and apply the transformation. If average
        for (int w = 0; w < width; w++){                // is less than threshold, black (1), else white (0)
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

// converts PPMImage to PGMImage
PGMImage * convertToGreyscale (PPMImage *ppm, long greyScaleMax){
    unsigned int height = ppm->height;
    unsigned int width = ppm->width;
    PGMImage *pgm = new_pgmimage(width, height, greyScaleMax);      // allocate space for new pgm image
    for (int h = 0; h < height; h++){                               // for each pixel apply the mathematical transformation 
        for (int w = 0; w < width; w++){                            // and set the new image pixel to the greyscale value
            double average = ((double)(ppm->pixmap[0][h][w] + ppm->pixmap[1][h][w] + ppm->pixmap[2][h][w]) / 3.0);
            double newVal =  (int) (average * ((double) greyScaleMax / (double) ppm->max));
            pgm->pixmap[h][w] = newVal;
        }
    }
    return pgm; 
}

// apply the sepia transformation to a PPMImage
PPMImage * applypSepiaTransformation (PPMImage *image){
    unsigned int height = image->height;
    unsigned int width = image->width;
    unsigned int max = image->max;
    PPMImage *ppm = new_ppmimage(width, height, max);               // allocate space for a ppm image
    for (int h = 0; h < height; h++){                               // for each pixel RBG, apply the RGB transformation
        for (int w = 0; w < width; w++){                            
            double oldR = (double) image->pixmap[0][h][w];
            double oldG = (double) image->pixmap[1][h][w];
            double oldB = (double) image->pixmap[2][h][w];
            int newR = (int) (0.393*(oldR) + 0.769*(oldG) + 0.189*(oldB)); 
            int newG = (int) (0.349*(oldR) + 0.686*(oldG) + 0.168*(oldB));
            int newB = (int) (0.272*(oldR) + 0.534*(oldG) + 0.131*(oldB));
            if (newR > max)                                         // if any of the new values exceed the max, set the 
                newR = max;                                         // new value to the indicated max
            if (newG > max)
                newG = max;
            if (newB > max)
                newB = max;
            ppm->pixmap[0][h][w] = newR;                            // set each new RBG value for each pixel
            ppm->pixmap[1][h][w] = newG;
            ppm->pixmap[2][h][w] = newB;
        }
    }
    return ppm;
}

// Mirror a PPMImage across the vertical axis
PPMImage * mirrorImage (PPMImage *image){
    unsigned int height = image->height;
    unsigned int width = image->width;
    unsigned int max = image->max;
    PPMImage *ppm = new_ppmimage(width, height, max);                   // allocate space for the ppm image
    for (int h = 0; h < height; h++){                                   // for each row
        for (int w = 0; w < width/2 ; w++){                             // for each column until width/2
            ppm->pixmap[0][h][width - 1 - w] = image->pixmap[0][h][w];  // mirror the pixels from the left half of the 
            ppm->pixmap[1][h][width - 1 - w] = image->pixmap[1][h][w];  // original image to the right side of the new image
            ppm->pixmap[2][h][width - 1 - w] = image->pixmap[2][h][w];
        }
        for (int w = 0; w <= width/2 ; w++){                            // copy the left half of the original image to the 
            ppm->pixmap[0][h][w] = image->pixmap[0][h][w];              // left half of the new image
            ppm->pixmap[1][h][w] = image->pixmap[1][h][w];
            ppm->pixmap[2][h][w] = image->pixmap[2][h][w];
        }
    }
    return ppm;
}

// isolates a certain color channel of a PPMImage
PPMImage * isolateColor (PPMImage *image, int isolateColor){
    unsigned int height = image->height;
    unsigned int width = image->width;
    unsigned int max = image->max;
    PPMImage *ppm = new_ppmimage(width, height, max);                   // allocate space for ppm image 
    isolateColor = isolateColor - 1;                                    // reduce isolate color by 1. Red is 1->0, Green is 2->1
    for (int h = 0; h < height; h++){                                   // Blue is 3->2 which corresponds to the array indexes
        for (int w = 0; w < width; w++){                                // for each color.
            if (isolateColor == 0){                                     // Based on the color to isolate, keep that one and set
                ppm->pixmap[0][h][w] = image->pixmap[0][h][w];          // the others to 0
                ppm->pixmap[1][h][w] = 0;
                ppm->pixmap[2][h][w] = 0;
            }else if (isolateColor == 1){
                ppm->pixmap[0][h][w] = 0;
                ppm->pixmap[1][h][w] = image->pixmap[0][h][w];
                ppm->pixmap[2][h][w] = 0;
            }
            else if (isolateColor == 2){
                ppm->pixmap[0][h][w] = 0;
                ppm->pixmap[1][h][w] = 0;
                ppm->pixmap[2][h][w] = image->pixmap[0][h][w];
            }
        }
    }
    return ppm;
}

// removes a certain color channel of a PPMImage
PPMImage * removeColor (PPMImage *image, int removeColor){
    unsigned int height = image->height;
    unsigned int width = image->width;
    unsigned int max = image->max;
    PPMImage *ppm = new_ppmimage(width, height, max);                   // allocated space for a ppm image
    removeColor = removeColor - 1;                                      // reduce remove color by 1. See isolate color above 
    for (int h = 0; h < height; h++){                                   // for reasoning. R=1->0, G=2->1, B=3->2
        for (int w = 0; w < width; w++){
            ppm->pixmap[0][h][w] = image->pixmap[0][h][w];              // copy the image over
            ppm->pixmap[1][h][w] = image->pixmap[1][h][w];
            ppm->pixmap[2][h][w] = image->pixmap[2][h][w];
            ppm->pixmap[removeColor][h][w] = 0;                         // remove the channle that isn't needed
        }
    }
    return ppm;
}

// creates a thumbnail PPMImage based on the scaling factor
PPMImage * createThumbnail (PPMImage *image, int thumbnailScale){
    unsigned int height = (unsigned int) (image->height)/thumbnailScale;// set new height and width to smaller size based on 
    unsigned int width = (unsigned int) (image->width)/thumbnailScale;  // scaling factor 
    if ((image->width%thumbnailScale) != 0){                            // if the width or height don't divide evenly,
        width++;                                                        // round the width and/or height up one 
    }
    if ((image->height%thumbnailScale) != 0){
        height++;
    }
    unsigned int max = image->max;
    PPMImage *ppm = new_ppmimage(width, height, max);                   // allocate space for thumbnail ppm image
    for (int h = 0; h < height; h++){
        for (int w = 0; w < width ; w++){                               // get every n-th pixel value for thumbnail
            ppm->pixmap[0][h][w] = image->pixmap[0][h * thumbnailScale][w * thumbnailScale];
            ppm->pixmap[1][h][w] = image->pixmap[1][h * thumbnailScale][w * thumbnailScale];
            ppm->pixmap[2][h][w] = image->pixmap[2][h * thumbnailScale][w * thumbnailScale];
        }
    }
    return ppm;
}

// tile thumbnail images in a PPMImage
PPMImage * tileImages (PPMImage *image, int tileScale){       
    PPMImage *thumbnail = createThumbnail(image, (int) tileScale);      // create the thumbnail image to be tiled
    unsigned int height = image->height;
    unsigned int width = image->width;
    unsigned int max = image->max;
    PPMImage *ppm = new_ppmimage(width, height, max);                   // allocate space for new ppm image
    int thumbHeight = 0;
    int thumbWidth = 0;
    for (int h = 0; h < height; h++){                                   // for each row
        if (thumbHeight == thumbnail->height)                           // if you get to the end of a column in the thumbnail
            thumbHeight = 0;                                            // go back to the start tiling again.
        for (int w = 0; w < width ; w++){
            if (thumbWidth == thumbnail->width)                         // if you get to the end of a row in the thumbnail
                thumbWidth = 0;                                         // go back to the start tiling again.
            ppm->pixmap[0][h][w] = thumbnail->pixmap[0][thumbHeight][thumbWidth];    // copy thumbnail image to the new image
            ppm->pixmap[1][h][w] = thumbnail->pixmap[1][thumbHeight][thumbWidth];
            ppm->pixmap[2][h][w] = thumbnail->pixmap[2][thumbHeight][thumbWidth];
            thumbWidth++;
        }
        thumbWidth = 0;                                                 // at the end of a row in the new image, reset 
        thumbHeight++;                                                  // getting thumbnail pixels to 0 and go to the next row
    }                                   
    del_ppmimage(thumbnail);                                            // free up the intermediate thumbnail photo
    return ppm;
}

// allocate space for a PPMImage
PPMImage *new_ppmimage(unsigned int width, unsigned int height, unsigned int max){
    PPMImage *ppm;
    ppm = malloc(sizeof(PPMImage));                                     // allocate space for the image struct

    for (int i = 0; i < 3; i++){                                        // for each colorc channel (RGB)
        ppm->pixmap[i] = malloc(height * sizeof(unsigned int *));       // allocate space for the pointers to each row            
        for (unsigned int h=0; h<height; h++)
            ppm->pixmap[i][h] = malloc(width * sizeof(unsigned int));   // allocate spcace for the ints in the col of each row
    }

    ppm->width = width;                                                 // set image height, width, max
    ppm->height = height;
    ppm->max = max;

    return ppm;
}

// allocate space for a PGMImage
PGMImage *new_pgmimage( unsigned int width, unsigned int height, unsigned int max){
    PGMImage *pgm;
    pgm = malloc(sizeof(PGMImage));                                     // allocate space for the image struct

    pgm->pixmap = malloc(height * sizeof(unsigned int *));              // allocate space for the pointers to each row
    for (int h = 0; h < height; h++)
        pgm->pixmap[h] = malloc(width * sizeof(unsigned int));          // allocate spcace for the ints in the col of each row

    pgm->width = width;                                                 // set image height, width, max
    pgm->height = height;
    pgm->max = max;
    return pgm;
}

// allocate space for a PBMImage
PBMImage *new_pbmimage( unsigned int width, unsigned int height ){
    PBMImage *pbm;
    pbm = malloc(sizeof(PBMImage));                                     // allocate space for the image struct

    pbm->pixmap = malloc(height * sizeof(unsigned int *));              // allocate space for the pointers to each row
    for (int h = 0; h < height; h++)
        pbm->pixmap[h] = malloc(width * sizeof(unsigned int));          // allocate spcace for the ints in the col of each row

    pbm->width = width;                                                 // set image height, width
    pbm->height = height;

    return pbm;
}

// free up space allocated for a PPMImage
void del_ppmimage( PPMImage * ppm){                                 
    for (int i = 0; i<= 2; i++){
        for (int h = 0; h < ppm->height; h++)
            free(ppm->pixmap[i][h]);                                    // free up space for each row
        free(ppm->pixmap[i]);                                           // free space for pointers to each row
    }
    free(ppm);                                                          // free up space for ppm image
}

// free up space allocated for a PGMImage
void del_pgmimage( PGMImage * pgm){
    for (int h = 0; h < pgm->height; h++)
        free(pgm->pixmap[h]);                                           // free up space for each row
    free(pgm->pixmap);                                                  // free space for pointers to each row
    free(pgm);                                                          // free up space for pgm image
}

// free up space allocated for a PBMImage
void del_pbmimage( PBMImage * pbm){
    for (int h = 0; h < pbm->height; h++)
        free(pbm->pixmap[h]);                                           // free up space for each row
    free(pbm->pixmap);                                                  // free space for pointers to each row
    free(pbm);                                                          // free up space for pbm image
}