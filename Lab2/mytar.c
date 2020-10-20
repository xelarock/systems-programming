//
// Created by Alex Welsh on 9/30/20.
//
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <utime.h>
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
void extractArchive(FILE *archiveFile);

int main(int argc, char *argv[]) {
    Options opts = parseArgs(argc, argv);
//    exit(0);

    if (opts.createArchive == 1){
        FILE *archiveFile = fopen(opts.archiveFile,"w+");
        if (archiveFile == NULL) {
            perror("fopen");
            exit(-1);
        }
        int32_t magic_number = 0x7261746D;
        if (!fwrite(&magic_number, 4, 1, archiveFile)){
            perror("fwrite");
            exit(-1);
        }

        struct stat file_stat;
        if (lstat(opts.inputDirectory, &file_stat) != 0) {
            perror("lstat");
            exit(-1);
        }
        int64_t inode_num = (int64_t) file_stat.st_ino;
        if (!fwrite(&inode_num , 8, 1, archiveFile)) {                   // inode #
            perror("fwrite");
            exit(-1);
        }
        set_inode(file_stat.st_ino, opts.inputDirectory);
        int32_t file_name_length = strlen(opts.inputDirectory);
        if (!fwrite(&file_name_length, 4, 1, archiveFile)) {                  // name length
            perror("fwrite");
            exit(-1);
        }
        if (!fwrite(opts.inputDirectory, file_name_length, 1, archiveFile)) {                 // name
            perror("fwrite");
            exit(-1);
        }
        int32_t mode = (int32_t) file_stat.st_mode;
        if (!fwrite(&mode, 4, 1, archiveFile)) {                      // mode
            perror("fwrite");
            exit(-1);
        }
        int64_t time_num = (int64_t) file_stat.st_mtimespec.tv_sec;
        if (!fwrite(&time_num, 8, 1, archiveFile)) {          // modification time
            perror("fwrite");
            exit(-1);
        }
        printf("end of dir1 \n");
        createArchive(archiveFile, opts.inputDirectory);
        if (fclose(archiveFile) != 0){
            perror("fclose");
            exit(-1);
        }
    }
    if (opts.printContents == 1){
        printContents(opts.archiveFile);
    }
    if (opts.extractArchive == 1){
        FILE *archiveFile = fopen(opts.archiveFile,"r");
        if (archiveFile == NULL) {
            perror("fopen");
            exit(-1);
        }
        int32_t magic_number;
        if (!fread(&magic_number, 4, 1, archiveFile)){
            perror("fread");
            exit(-1);
        }
        if (magic_number != 0x7261746D){
            fprintf(stderr, "Error: Bad magic number (%d), should be: %d.\n", magic_number, 0x7261746D);
            exit(-1);
        }
        extractArchive(archiveFile);
    }
    return 0;
}

void extractArchive(FILE *archiveFile){
    ino_t inode_num = 0;
    int64_t inode_num_int;
    while(fread(&inode_num_int, 8, 1, archiveFile) == 1){
        printf("************************\n");
        inode_num = (ino_t) inode_num_int;
        printf("inode num %llu \n", inode_num);
        int32_t file_name_length;
        if (!fread(&file_name_length, 4, 1, archiveFile)){
            perror("fread");
            exit(-1);
        }
        char *file_name =  malloc(file_name_length+1 * sizeof(char));
        if (file_name == NULL){
            perror("malloc");
            exit(-1);
        }
        if (!fread(file_name, file_name_length, 1, archiveFile)){
            perror("fread");
            exit(-1);
        }
        file_name[file_name_length] = '\0';
        printf("filename: %s\n", file_name);

        if(get_inode(inode_num) != NULL){
            printf("HARD LINK\n");
            char *src_name = (char *) malloc((strlen(get_inode(inode_num)) + 1) * sizeof(char));
            if (src_name == NULL){
                perror("malloc");
                exit(-1);
            }
            if (strcpy(src_name, get_inode(inode_num)) == NULL){
                perror("strcpy");
                exit(-1);
            }
            if (link(src_name, file_name) != 0){
                perror("link");
                exit(-1);
            }
        }else{
            int32_t file_mode;
            if (!fread(&file_mode, 4, 1, archiveFile)){
                perror("fread");
                exit(-1);
            }
            int64_t file_mtime;
            if (!fread(&file_mtime, 8, 1, archiveFile)){
                perror("fread");
                exit(-1);
            }
            if (S_ISDIR((mode_t) file_mode)){
                printf("DIRECTORY\n");
                if (mkdir(file_name, (mode_t) file_mode) != 0){
                    perror("mkdir");
                    exit(-1);
                }
                set_inode(inode_num, file_name);
            }else{
                printf("FILE\n");
                int64_t file_size;
                if (!fread(&file_size, 8, 1, archiveFile)){
                    perror("fread");
                    exit(-1);
                }
                char *file_contents = (char *) malloc(file_size * sizeof(char));
                if (file_contents == NULL){
                    perror("malloc");
                    exit(-1);
                }
                if (!fread(file_contents, file_size, 1, archiveFile)){
                    perror("fread");
                    exit(-1);
                }
                FILE *currentFile = fopen(file_name,"w+");
                if (currentFile == NULL){
                    perror("fopen");
                    exit(-1);
                }
                if (!fwrite(file_contents, file_size, 1, currentFile)){
                    perror("fwrite");
                    exit(-1);
                }
                if (chmod(file_name, (mode_t) file_mode) != 0){
                    perror("fread");
                    exit(-1);
                }
                struct timeval time_val_ac;
                struct timeval time_val_mod;
                if (gettimeofday(&time_val_ac, NULL) != 0){
                    perror("gettimeofday");
                    exit(-1);
                }
                time_val_mod.tv_sec = (time_t) file_mtime;
                time_val_mod.tv_usec = 0;
                struct timeval times[2] = {time_val_ac, time_val_mod};
                printf("time: %lu %lu, %d\n", (time_t) file_mtime, times[1].tv_sec, times[1].tv_usec);
                if (fclose(currentFile) != 0){
                    perror("fclose");
                    exit(-1);
                }
                if (utimes(file_name, times) != 0){
                    perror("utimes");
                    exit(-1);
                }
                set_inode(inode_num, file_name);
            }
        }
    }
}

void printContents(char *archiveFileString){
    FILE *archiveFile = fopen(archiveFileString,"r");
    if (archiveFile == NULL){
        perror("fopen");
        exit(-1);
    }
    int32_t magic_number;

    if (!fread(&magic_number, 4, 1, archiveFile)){
        perror("fread");
        exit(-1);
    }
    printf("hex: %x\n", magic_number);
    if (magic_number != 0x7261746D){
        fprintf(stderr, "Error: Bad magic number (%d), should be: %d.\n", magic_number, 0x7261746D);
        exit(-1);
    }

    ino_t inode_num = 0;
    int64_t inode_num_int;
    while(fread(&inode_num_int, 8, 1, archiveFile) == 1){
        printf("+++++++++++++++++++++++\n");

        inode_num = (ino_t) inode_num_int;
        int32_t file_name_length;
        if (!fread(&file_name_length, 4, 1, archiveFile)){
            perror("fread");
            exit(-1);
        }

        char *file_name = (char *) malloc((file_name_length) * sizeof(char));
        if (file_name == NULL){
            perror("malloc");
            exit(-1);
        }
        if (!fread(file_name, file_name_length, 1, archiveFile)){
            perror("fread");
            exit(-1);
        }
        printf("file name: %s\n", file_name);
        file_name[file_name_length] = '\0';

        if (get_inode(inode_num) != NULL){
            printf("%s -- inode: %llu\n", file_name, inode_num);
        }else{
            mode_t read_mode;
            int32_t read_mode_int;
            if (fread(&read_mode_int, 4, 1, archiveFile) == -1) {
                perror("fread");
                exit(-1);
            }
//            fread(&read_mode_int, 4, 1, archiveFile);
            read_mode = (mode_t) read_mode_int;

            time_t read_mtime;
            int64_t read_mtime_int;
            if (!fread(&read_mtime_int, 8, 1, archiveFile)){
                perror("fread");
                exit(-1);
            }
            read_mtime = (time_t) read_mtime_int;
            set_inode(inode_num, file_name);

            if (S_ISDIR(read_mode)){
                printf("%s/ -- inode: %llu, mode: %o, mtime: %llu\n", file_name, inode_num, read_mode, (unsigned long long) read_mtime);
//                set_inode(inode_num, file_name);
            }else {
                off_t read_size;
                int64_t read_size_int;
                if (!fread(&read_size_int, 8, 1, archiveFile)){
                    perror("fread");
                    exit(-1);
                }
                read_size = (off_t) read_size_int;

                char *read_contents = (char *) malloc(read_size * sizeof(char));
                if (read_contents == NULL){
                    perror("malloc");
                    exit(-1);
                }
                if (!fread(read_contents, read_size, 1, archiveFile)){
                    perror("fread");
                    exit(-1);
                }

                if ((read_mode & S_IXUSR) || (read_mode & S_IXGRP) || (read_mode & S_IXOTH)){
                    printf("%s* -- inode: %llu, mode: %o, mtime: %llu, size: %llu\n", file_name, inode_num, read_mode,
                           (unsigned long long) read_mtime, read_size);
                }else{
                    printf("%s -- inode: %llu, mode: %o, mtime: %llu, size: %llu\n", file_name, inode_num, read_mode,
                           (unsigned long long) read_mtime, read_size);
                }
            }
        }
    }
}

void createArchive(FILE *archiveFile, char *inputDirectoryString){
    struct dirent *de;
    struct stat file_stat;
    char * fullname;

    if (fseek(archiveFile, 0, SEEK_END) != 0){
        perror("fseek");
        exit(-1);
    }
//    printf("end1 %lu\n", ftell(archiveFile));

    DIR *input_directory = opendir(inputDirectoryString);
    if (input_directory == NULL){
        perror("opendir");
        exit(-1);
    }

    printf("inputDir: %s\n", inputDirectoryString);
    fullname = (char *) malloc((strlen(inputDirectoryString)+258));
    if (fullname == NULL){
        perror("malloc");
        exit(-1);
    }
    for (de = readdir(input_directory); de != NULL; de = readdir(input_directory)) {
        if (!sprintf(fullname, "%s/%s", inputDirectoryString, de->d_name)){
            perror("sprintf");
            exit(-1);
        }
        if (lstat(fullname, &file_stat) != 0){
            perror("lstat");
            exit(-1);
        }
        if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0 && !S_ISLNK(file_stat.st_mode)){
            printf("%s path: %s %llu, %lu\n", de->d_name, fullname, file_stat.st_ino, ftell(archiveFile));
            int64_t inode_num = (int64_t) file_stat.st_ino;
            if (!fwrite(&inode_num, 8, 1, archiveFile)) {                // inode #
                perror("fwrite");
                exit(-1);
            }

            int32_t file_name_length = strlen(fullname);
            if (!fwrite(&file_name_length, 4, 1, archiveFile)) {                   // name length
                perror("fwrite");
                exit(-1);
            }
            if (!fwrite(fullname, file_name_length, 1, archiveFile)) {                  // name
                perror("fwrite");
                exit(-1);
            }

            if(!get_inode( file_stat.st_ino )) {            //inode not yet seen; add to list and process
                set_inode(file_stat.st_ino, fullname);
                printf("%s path: %s\n", de->d_name, fullname);
                // at this point in the future, check if file is a hard link, if so, break here

                int32_t mode = (int32_t) file_stat.st_mode;
                if (!fwrite(&mode, 4, 1, archiveFile)) {                      // mode
                    perror("fwrite");
                    exit(-1);
                }
                int64_t time_num = (int64_t) file_stat.st_mtimespec.tv_sec;
                if (!fwrite(&time_num, 8, 1, archiveFile)) {          // modification time
                    perror("fwrite");
                    exit(-1);
                }

                if (S_ISDIR(file_stat.st_mode)){
                    printf("DIRECTORY\n");
                }else {
                    int64_t file_size = (int64_t) file_stat.st_size;
                    if (!fwrite(&file_size, 8, 1, archiveFile)) {             // size
                        perror("fwrite");
                        exit(-1);
                    }

                    FILE *inputFile = fopen(fullname,"r");
                    if (inputFile == NULL){
                        perror("fopen");
                        exit(-1);
                    }
                    char *read_contents = malloc(file_stat.st_size);
                    if (read_contents == NULL){
                        perror("malloc");
                        exit(-1);
                    }
                    if (!fread(read_contents, file_stat.st_size, 1, inputFile)){
                        perror("fread");
                        exit(-1);
                    }

                    if (!fwrite(read_contents, file_stat.st_size, 1, archiveFile)) {             // content
                        perror("fwrite");
                        exit(-1);
                    }
                }
            }
        }
//        printf("end2 %lu,\n", ftell(archiveFile));

        if (S_ISDIR(file_stat.st_mode) && strcmp(de->d_name, ".") !=0 && strcmp(de->d_name, "..") !=0 ) {
            createArchive(archiveFile, fullname);
//            printf("end of inner loop %lu\n", ftell(archiveFile));
        }
    }
//    printf("end3 %lu,\n", ftell(archiveFile));
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
                fprintf(stderr, "Error: No tarfile specified.\n");
                    exit(-1);
            }
        }else {
//            printf("input: %s\n", argv[optind]);
            opts.inputDirectory = argv[optind];
            opts.yesInputDirectory = 1;
//            printf("opening dir: %s\n", opts.inputDirectory);
            struct stat file_stat;
            if (lstat(opts.inputDirectory, &file_stat) != 0) {
                fprintf(stderr, "Error: Specified target (\"%s\") does not exist.\n", opts.inputDirectory);
                exit(-1);
            }else{
                if (!S_ISDIR(file_stat.st_mode)) {
                    fprintf(stderr, "Error: Specified target (\"%s\") is not a directory.\n", opts.inputDirectory);
                    exit(-1);
                }
            }
            optind++;
        }
    }
    if (numOptions == 0){
        fprintf(stderr, "Error: No mode specified.\n");
        exit(-1);
    }
    if (opts.createArchive && !opts.yesInputDirectory){
        fprintf(stderr, "Error: No directory target specified.\n");
        exit(-1);
    }
    if (opts.yesArchiveFile != 1 && opts.archiveFile != NULL){
        fprintf(stderr, "Error: No tarfile specified.\n");
        exit(-1);
    }
//    if (!opts.createArchive && opts.yesInputDirectory){
//        fprintf(stderr, "Error: Multiple modes specified.\n");
//        exit(1);
//    }
//    if (opts.yesInputDirectory == 1){
//        printf("opening dir: %s\n", opts.inputDirectory);
//        void *dir = opendir(opts.inputDirectory);
//        if (dir == NULL){
//            fprintf(stderr, "Error: Specified target (\"%s\") does not exist.\n", opts.inputDirectory);
//            exit(-1);
//        }
//        printf("dir: %p\n", dir);
//    }
    return opts;
}

void checkModes (int numTrans){
    if (numTrans > 1){
        fprintf(stderr, "Error: Multiple modes specified.\n");
        exit(1);
    }
}