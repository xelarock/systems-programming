//
// Created by Alex Welsh on 9/30/20.
//
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

typedef struct {                // defines the options structure to hold the options for the program.
    int createArchive, extractArchive, printContents, yesArchiveFile, yesInputDirectory;
    char *archiveFile;
    char *inputDirectory;
}Options;

Options parseArgs(int argc, char *argv[]);
void checkModes (int numTrans);

int main(int argc, char *argv[]) {
    Options opts = parseArgs(argc, argv);

    return 0;
}

Options parseArgs(int argc, char *argv[]){
    Options opts;
    opts.createArchive = 0;
    opts.extractArchive = 0;
    opts.printContents = 0;
    opts.yesArchiveFile = 0;
    opts.yesInputDirectory = 0;
    int o;
    int numOptions = 0;

    while (optind < argc) {                                              // Iterate through all option arguments
        if ((o = getopt(argc, argv, "cxtf:")) != -1) {
            switch (o) {
            case 'c':
                printf("C!\n");
                numOptions++;
                checkModes(numOptions);
                opts.createArchive = 1;
                break;
            case 'x':
                printf("X!\n");
                numOptions++;
                checkModes(numOptions);
                opts.extractArchive = 1;
                break;
            case 't':
                printf("T!\n");
                numOptions++;
                checkModes(numOptions);
                opts.printContents = 1;
                break;
            case 'f':
                printf("F! %s\n", optarg);
//                if(strstr(optarg, ".tar") == NULL) {
//                    fprintf(stderr, "Error: No tarfile specified.\n");
//                    exit(-1);
//                }
                opts.yesArchiveFile = 1;
                break;
            default:
                printf("default");
            }
        }else{
            printf("input: %s\n", argv[optind]);
            opts.inputDirectory = argv[optind];
            opts.yesInputDirectory = 1;
            optind++;
        }
    }
    if (numOptions == 0){
        fprintf(stderr, "Error: No mode specified.\n");
        exit(-1);
    }
    if (opts.yesInputDirectory != 1){
        fprintf(stderr, "Error: No directory target specified.\n");
        exit(-1);
    }
    if (opts.yesArchiveFile != 1){
        fprintf(stderr, "Error: No tarfile specified.\n");
        exit(-1);
    }
//    if (!opts.createArchive && opts.yesInputDirectory){
//        fprintf(stderr, "Error: Multiple modes specified.\n");
//        exit(1);
//    }
    if (opts.yesInputDirectory == 1){
        printf("opening dir: %s\n", opts.inputDirectory);
        void *dir = opendir(opts.inputDirectory);
        if (dir == NULL){
            fprintf(stderr, "Error: Specified target (\"%s\") does not exist.\n", opts.inputDirectory);
            exit(-1);
        }
        printf("dir: %p\n", dir);
    }
    return opts;
}

void checkModes (int numTrans){
    if (numTrans > 1){
        fprintf(stderr, "Error: Multiple modes specified.\n");
        exit(1);
    }
}