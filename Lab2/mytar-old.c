//
// Created by Alex Welsh on 9/30/20.
//
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include "inodemap.h"

typedef struct {                // defines the options structure to hold the options for the program.
    int createArchive, extractArchive, printContents, yesArchiveFile, yesInputDirectory;
    char *archiveFile;
    char *inputDirectory;
}Options;

Options parseArgs(int argc, char *argv[]);
void checkModes (int numTrans);
void createArchive(FILE *archiveFile, char *inputDirectoryString);
void printContents(char *archiveFileString);
//char ** Map=NULL;

int main(int argc, char *argv[]) {
    Options opts = parseArgs(argc, argv);

    if (opts.createArchive == 1){
        FILE *archiveFile = fopen(opts.archiveFile,"w+");
        int32_t magic_number = 0x7261746D;
        fwrite(&magic_number, 4, 1, archiveFile);
//        DIR *input_directory = opendir(opts.inputDirectory);

        struct stat file_stat;
        lstat(opts.inputDirectory, &file_stat);
        fwrite(&file_stat.st_ino, 8, 1, archiveFile);                   // inode #
        int32_t file_name_length = strlen(opts.inputDirectory) + 1;
        fwrite(&file_name_length, 4, 1, archiveFile);                   // name length
        fwrite(opts.inputDirectory, file_name_length, 1, archiveFile);                  // name
        fwrite(&file_stat.st_mode, 4, 1, archiveFile);                      // mode
        fwrite(&file_stat.st_mtimespec.tv_sec, 8, 1, archiveFile);          // modification time
        printf("end of dir1 \n");
        createArchive(archiveFile, opts.inputDirectory);
        fclose(archiveFile);
    }
    if (opts.printContents == 1){
        printContents(opts.archiveFile);
    }
    if (opts.extractArchive == 1){

    }

    return 0;
}

void printContents(char *archiveFileString){
    FILE *archiveFile = fopen(archiveFileString,"r");
    int32_t magic_number;
    char *fullname = (char *)malloc(sizeof(char)*258);

    fread(&magic_number, 4, 1, archiveFile);
    printf("hex: %x\n", magic_number);

    while(feof(archiveFile) != 1){
        printf("+++++++++++++++++++++++\n");
        ino64_t inode_num = 0;
        fread(&inode_num, 8, 1, archiveFile);
        printf("inode: %llu\n", inode_num);

        int32_t file_name_length;
        fread(&file_name_length, 4, 1, archiveFile);
        printf("name length: %d\n", file_name_length);

        char *file_name = (char *) malloc(file_name_length * sizeof(char));
        fread(file_name, file_name_length, 1, archiveFile);
        printf("name : %s\n", file_name);

//        char *name = (char *) malloc(file_name_length * sizeof(char));
//        strcpy(name, file_name);

        mode_t read_mode;
//        printf("name3 : %s\n", name);
        fread(&read_mode, 4, 1, archiveFile);
        printf("name2 : %s\n", file_name);
        if (!feof(archiveFile))                                             // if the last file in archive is hard link
            fseek(archiveFile, -4, SEEK_CUR);                               // don't rewind 4 bytes after reading mode
//        printf("name1 : %s\n", name);                                                                    // prevents infinite loop
        if(S_ISDIR(read_mode) || S_ISREG(read_mode)){
            fread(&read_mode, 4, 1, archiveFile);
            printf("not a hard link! mode is: %us\n", read_mode);
//            printf("name : %s\n", name);

            time_t read_mtime;
            fread(&read_mtime, 8, 1, archiveFile);
            printf("mtime : %ld\n", read_mtime);

            if (S_ISDIR(read_mode)){
                printf("%s/ -- inode: %llu, mode: %o, mtime: %llu\n", file_name, inode_num, read_mode, (unsigned long long) read_mtime);
            }else {
                off_t read_size;
                fread(&read_size, 8, 1, archiveFile);
                printf("size : %llu\n", read_size);

                char *read_contents = malloc(read_size);
                fread(read_contents, read_size, 1, archiveFile);
                printf("read contents: %s\n", read_contents);

                if (S_IXUSR == 1 || S_IXGRP == 1 || S_IXOTH == 1){
                    printf("%s* -- inode: %llu, mode: %o, mtime: %llu, size: %llu\n", file_name, inode_num, read_mode,
                           (unsigned long long) read_mtime, read_size);
                }else{
                    printf("%s -- inode: %llu, mode: %o, mtime: %llu, size: %llu\n", file_name, inode_num, read_mode,
                           (unsigned long long) read_mtime, read_size);
                }
            }

        }else{
            printf("%s -- inode: %llu\n", file_name, inode_num);
        }
//        free(&*file_name);
    }
}

void createArchive(FILE *archiveFile, char *inputDirectoryString){
    struct dirent *de;
    struct stat file_stat;
    char * fullname;

    fseek(archiveFile, 0, SEEK_END);
    printf("end1 %lu\n", ftell(archiveFile));

    DIR *input_directory = opendir(inputDirectoryString);

    printf("inputDir: %s\n", inputDirectoryString);
    fullname = (char *)malloc((strlen(inputDirectoryString)+258));
    for (de = readdir(input_directory); de != NULL; de = readdir(input_directory)) {
        sprintf(fullname, "%s/%s", inputDirectoryString, de->d_name);
        lstat(fullname, &file_stat);
        if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0 && !S_ISLNK(file_stat.st_mode)){
            printf("%s path: %s %llu, %lu\n", de->d_name, fullname, file_stat.st_ino, ftell(archiveFile));
            fwrite(&file_stat.st_ino, 8, 1, archiveFile);                   // inode #

            int32_t file_name_length = strlen(fullname) + 1;
            fwrite(&file_name_length, 4, 1, archiveFile);                   // name length
            fwrite(fullname, file_name_length, 1, archiveFile);                  // name

            if(!get_inode( file_stat.st_ino )) {            //inode not yet seen; add to list and process
                set_inode(file_stat.st_ino, fullname);
                printf("%s path: %s\n", de->d_name, fullname);
                // at this point in the future, check if file is a hard link, if so, break here

                fwrite(&file_stat.st_mode, 4, 1, archiveFile);                      // mode
                fwrite(&file_stat.st_mtimespec.tv_sec, 8, 1, archiveFile);          // modification time

                if (S_ISDIR(file_stat.st_mode)){
                    printf("DIRECTORY\n");
                }else {
                    fwrite(&file_stat.st_size, 8, 1, archiveFile);             // size

                    FILE *inputFile = fopen(fullname,"r");
                    char *read_contents = malloc(file_stat.st_size);
                    fread(read_contents, file_stat.st_size, 1, inputFile);

                    fwrite(read_contents, file_stat.st_size, 1, archiveFile);             // content
                }
            }
        }
        printf("end2 %lu,\n", ftell(archiveFile));

        if (S_ISDIR(file_stat.st_mode) && strcmp(de->d_name, ".") !=0 && strcmp(de->d_name, "..") !=0 ) {
            createArchive(archiveFile, fullname);
//            fseek(archiveFile, 0, SEEK_END);
            printf("end of inner loop %lu\n", ftell(archiveFile));
        }
    }
    printf("end3 %lu,\n", ftell(archiveFile));
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
                opts.archiveFile = optarg;
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
//    if (opts.yesInputDirectory != 1){
//        fprintf(stderr, "Error: No directory target specified.\n");
//        exit(-1);
//    }
//    if (opts.yesArchiveFile != 1){
//        fprintf(stderr, "Error: No tarfile specified.\n");
//        exit(-1);
//    }
//    if (!opts.createArchive && opts.yesInputDirectory){
//        fprintf(stderr, "Error: Multiple modes specified.\n");
//        exit(1);
//    }
    if (opts.yesInputDirectory == 1){
        printf("opening dir: %s\n", opts.inputDirectory);
        void *dir = opendir(opts.inputDirectory);
        if (dir == NULL){
            fprintf(stderr, "Error: Specified target (\"%s\") does not exist.\n", opts.inputDirectory);
            closedir(dir);
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