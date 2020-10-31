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
#include <unistd.h>
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
    Options opts = parseArgs(argc, argv);                                           // parse arguments
    if (opts.createArchive == 1){
        FILE *archiveFile = fopen(opts.archiveFile,"w+");                   // create archive file
        if (archiveFile == NULL) {
            perror("fopen");
            exit(-1);
        }
        int32_t magic_number = 0x7261746D;                                         // write magic number
        if (!fwrite(&magic_number, 4, 1, archiveFile)){
            perror("fwrite");
            exit(-1);
        }
        // write the target directory to the tar file
        struct stat file_stat;                                                      // get stat struct
        if (lstat(opts.inputDirectory, &file_stat) != 0) {
            perror("lstat");
            exit(-1);
        }
        int64_t inode_num = (int64_t) file_stat.st_ino;
        if (!fwrite(&inode_num , 8, 1, archiveFile)) {                   // write inode #
            perror("fwrite");
            exit(-1);
        }
        set_inode(file_stat.st_ino, opts.inputDirectory);
        int32_t file_name_length = strlen(opts.inputDirectory);
        if (!fwrite(&file_name_length, 4, 1, archiveFile)) {                  // write name length
            perror("fwrite");
            exit(-1);
        }
        if (!fwrite(opts.inputDirectory, file_name_length, 1, archiveFile)) {       // write name
            perror("fwrite");
            exit(-1);
        }
        int32_t mode = (int32_t) file_stat.st_mode;
        if (!fwrite(&mode, 4, 1, archiveFile)) {                      // write mode
            perror("fwrite");
            exit(-1);
        }
        int64_t time_num = (int64_t) file_stat.st_mtim.tv_sec;
        if (!fwrite(&time_num, 8, 1, archiveFile)) {          // write modification time
            perror("fwrite");
            exit(-1);
        }
        createArchive(archiveFile, opts.inputDirectory);                // recursively write files/directories to tar
        if (fclose(archiveFile) != 0){                                  // close tar
            perror("fclose");
            exit(-1);
        }
    }
    if (opts.printContents == 1){                                       // print contents of tar
        printContents(opts.archiveFile);
    }
    if (opts.extractArchive == 1){                                      // extract tar
        FILE *archiveFile = fopen(opts.archiveFile,"r");          // open the tar file
        if (archiveFile == NULL) {
            perror("fopen");
            exit(-1);
        }
        int32_t magic_number;
        if (!fread(&magic_number, 4, 1, archiveFile)){
            perror("fread");
            exit(-1);
        }
        if (magic_number != 0x7261746D){                                // read and check magic number
            fprintf(stderr, "Error: Bad magic number (%d), should be: %d.\n", magic_number, 0x7261746D);
            exit(-1);
        }
        extractArchive(archiveFile);                                    // extract the files in the tar
        if (fclose(archiveFile) != 0){                                  // close the tar file
            perror("fclose");
            exit(-1);
        }
    }
    exit(0);
}

void extractArchive(FILE *archiveFile){
    ino_t inode_num;
    int64_t inode_num_int;
    while(fread(&inode_num_int, 8, 1, archiveFile) == 1){           // read the inode, if cannot read, eof
        inode_num = (ino_t) inode_num_int;                                      // so then exit and done extracting
        int32_t file_name_length;
        if (!fread(&file_name_length, 4, 1, archiveFile)){          // read file name length
            perror("fread");
            exit(-1);
        }
        char *file_name =  malloc(file_name_length+1 * sizeof(char));    // make space for file name
        if (file_name == NULL){
            perror("malloc");
            exit(-1);
        }
        if (!fread(file_name, file_name_length, 1, archiveFile)){       // read file name
            perror("fread");
            exit(-1);
        }
        file_name[file_name_length] = '\0';                                     // append null terminator
        if(get_inode(inode_num) != NULL){                                       // check if hard link
            char *src_name = (char *) malloc((strlen(get_inode(inode_num)) + 1) * sizeof(char));  // space for path
            if (src_name == NULL){
                perror("malloc");
                exit(-1);
            }
            if (strcpy(src_name, get_inode(inode_num)) == NULL){        // copy path in map to src name
                perror("strcpy");
                exit(-1);
            }
            if (link(src_name, file_name) != 0){                        // create hard link
                perror("link");
                exit(-1);
            }
            free(src_name);
        }else{                                                          // if not hard link
            int32_t file_mode;
            if (!fread(&file_mode, 4, 1, archiveFile)){         // read mode
                perror("fread");
                exit(-1);
            }
            int64_t file_mtime;
            if (!fread(&file_mtime, 8, 1, archiveFile)){        // read mtime
                perror("fread");
                exit(-1);
            }
            if (S_ISDIR((mode_t) file_mode)){                           // if mode is dir
                if (mkdir(file_name, (mode_t) file_mode) != 0){             // mkdir
                    perror("mkdir");
                    exit(-1);
                }
                set_inode(inode_num, file_name);                            // add dir to inode
            }else{
                int64_t file_size;
                if (!fread(&file_size, 8, 1, archiveFile)){     // read file size
                    perror("fread");
                    exit(-1);
                }
                FILE *currentFile = fopen(file_name, "w+");             // create a new file
                if (currentFile == NULL) {
                    perror("fopen");
                    exit(-1);
                }
                if (file_size != 0) {                                        // if file size is 0, skip reading contents

                    char *file_contents = (char *) malloc(file_size * sizeof(char));    // make space for contents
                    if (file_contents == NULL) {
                        perror("malloc");
                        exit(-1);
                    }
                    if (!fread(file_contents, file_size, 1, archiveFile)) {             // read contents
                        perror("fread");
                        exit(-1);
                    }
                    if (!fwrite(file_contents, file_size, 1, currentFile)) {            // write contents
                        perror("fwrite");
                        exit(-1);
                    }
                    free(file_contents);
                }
                if (chmod(file_name, (mode_t) file_mode) != 0){             // set the permissions
                    perror("fread");
                    exit(-1);
                }
                struct timeval time_val_ac;                                 // set up timevals
                struct timeval time_val_mod;
                if (gettimeofday(&time_val_ac, NULL) != 0){                 // set access time
                    perror("gettimeofday");
                    exit(-1);
                }
                time_val_mod.tv_sec = (time_t) file_mtime;                  // set mtime
                time_val_mod.tv_usec = 0;
                struct timeval times[2] = {time_val_ac, time_val_mod};
                if (utimes(file_name, times) != 0){                         // set times
                    perror("utimes");
                    exit(-1);
                }
                set_inode(inode_num, file_name);                            // add inode to map

                if (fclose(currentFile) != 0){                              // close the finished file
                    perror("fclose");
                    exit(-1);
                }
            }
        }
    }
}

void printContents(char *archiveFileString){
    FILE *archiveFile = fopen(archiveFileString,"r");                           // open tar
    if (archiveFile == NULL){
        perror("fopen");
        exit(-1);
    }
    int32_t magic_number;
    if (!fread(&magic_number, 4, 1, archiveFile)){                          // read magic number
        perror("fread");
        exit(-1);
    }
    if (magic_number != 0x7261746D){                                                    // check magic number
        fprintf(stderr, "Error: Bad magic number (%d), should be: %d.\n", magic_number, 0x7261746D);
        exit(-1);
    }
    ino_t inode_num = 0;
    int64_t inode_num_int;
    while(fread(&inode_num_int, 8, 1, archiveFile) == 1){       // loop through reading inode. if cannot
        inode_num = (ino_t) inode_num_int;                                  // read, then eof and done reading tar
        int32_t file_name_length;
        if (!fread(&file_name_length, 4, 1, archiveFile)){      // read file length
            perror("fread");
            exit(-1);
        }
        char *file_name = (char *) malloc((file_name_length) * sizeof(char));   // make space for file name
        if (file_name == NULL){
            perror("malloc");
            exit(-1);
        }
        if (!fread(file_name, file_name_length, 1, archiveFile)){               // read file name
            perror("fread");
            exit(-1);
        }
        file_name[file_name_length] = '\0';                                             // add null terminator
        if (get_inode(inode_num) != NULL){
            printf("%s -- inode: %lu\n", file_name, inode_num);                        // print if hard link
        }else{
            mode_t read_mode;
            int32_t read_mode_int;
            if (fread(&read_mode_int, 4, 1, archiveFile) == -1) {           // else, read mode
                perror("fread");
                exit(-1);
            }
            read_mode = (mode_t) read_mode_int;
            time_t read_mtime;
            int64_t read_mtime_int;
            if (!fread(&read_mtime_int, 8, 1, archiveFile)){                // read m time
                perror("fread");
                exit(-1);
            }
            read_mtime = (time_t) read_mtime_int;
            set_inode(inode_num, file_name);                                            // add to inode map
            if (S_ISDIR(read_mode)){                                                    // if directory print
                printf("%s/ -- inode: %lu, mode: %o, mtime: %llu\n", file_name, inode_num, read_mode, (unsigned long long) read_mtime);
            }else {
                off_t read_size;                                                        // read file size
                int64_t read_size_int;
                if (!fread(&read_size_int, 8, 1, archiveFile)){
                    perror("fread");
                    exit(-1);
                }
                read_size = (off_t) read_size_int;
                if (read_size != 0) {                                                   // if file size is not 0
                    char *read_contents = (char *) malloc(read_size * sizeof(char)); // make space for contents
                    if (read_contents == NULL) {
                        perror("malloc");
                        exit(-1);
                    }
                    if (!fread(read_contents, read_size, 1, archiveFile)) {         // read contents
                        perror("fread");
                        exit(-1);
                    }
                }
                if ((read_mode & S_IXUSR) || (read_mode & S_IXGRP) || (read_mode & S_IXOTH)){      // if executable
                    printf("%s* -- inode: %lu, mode: %o, mtime: %llu, size: %lu\n", file_name, inode_num, read_mode,
                           (unsigned long long) read_mtime, read_size);
                }else{                                                                              // otherwise normal
                    printf("%s -- inode: %lu, mode: %o, mtime: %llu, size: %lu\n", file_name, inode_num, read_mode,
                           (unsigned long long) read_mtime, read_size);
                }
            }
        }
    }
    if (fclose(archiveFile) != 0){          // close archive file, done
        perror("fclose");
        exit(-1);
    }
}

void createArchive(FILE *archiveFile, char *inputDirectoryString){
    struct dirent *de;
    struct stat file_stat;
    char * fullname;

    DIR *input_directory = opendir(inputDirectoryString);           // open the input directory
    if (input_directory == NULL){
        perror("opendir");
        exit(-1);
    }
    fullname = (char *) malloc((strlen(inputDirectoryString)+258));      // make space for full name
    if (fullname == NULL){
        perror("malloc");
        exit(-1);
    }
    for (de = readdir(input_directory); de != NULL; de = readdir(input_directory)) {     // iterate through files in dir
        if (!sprintf(fullname, "%s/%s", inputDirectoryString, de->d_name)){              // set up relative path
            perror("sprintf");
            exit(-1);
        }
        if (lstat(fullname, &file_stat) != 0){                                           // lstat for info
            perror("lstat");
            exit(-1);
        }
        if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0 && !S_ISLNK(file_stat.st_mode)){
            int64_t inode_num = (int64_t) file_stat.st_ino;             // ignore ".." and "." and soft links
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
            if(!get_inode( file_stat.st_ino )) {            //inode not yet seen; add to list, aka not a hard link
                set_inode(file_stat.st_ino, fullname);
                int32_t mode = (int32_t) file_stat.st_mode;
                if (!fwrite(&mode, 4, 1, archiveFile)) {                      // mode
                    perror("fwrite");
                    exit(-1);
                }
                int64_t time_num = (int64_t) file_stat.st_mtim.tv_sec;
                if (!fwrite(&time_num, 8, 1, archiveFile)) {          // modification time
                    perror("fwrite");
                    exit(-1);
                }
                if (!S_ISDIR(file_stat.st_mode)){
                    int64_t file_size = (int64_t) file_stat.st_size;
                    if (!fwrite(&file_size, 8, 1, archiveFile)) {             // size
                        perror("fwrite");
                        exit(-1);
                    }
                    if (file_size != 0) {                                                   // if size is not 0
                        FILE *inputFile = fopen(fullname, "r");                       // read file to tar
                        if (inputFile == NULL) {
                            perror("fopen");
                            exit(-1);
                        }
                        char *read_contents = malloc(file_stat.st_size * sizeof(char));     // read contents
                        if (read_contents == NULL) {
                            perror("malloc");
                            exit(-1);
                        }
                        if (!fread(read_contents, file_stat.st_size, 1, inputFile)) {     // read contents
                            perror("fread");
                            exit(-1);
                        }
                        if (!fwrite(read_contents, file_stat.st_size, 1, archiveFile)) {    // write contents
                            perror("fwrite");
                            exit(-1);
                        }
                        if (fclose(inputFile) != 0){                                                // close file to tar
                            perror("fclose");
                            exit(-1);
                        }
                    }
                }
            }
        }
        if (S_ISDIR(file_stat.st_mode) && strcmp(de->d_name, ".") !=0 && strcmp(de->d_name, "..") !=0 ) {
            createArchive(archiveFile, fullname);               // recurse if a directory
        }
    }
    if (closedir(input_directory) != 0){        // close input directory
        perror("closedir");
        exit(-1);
    }
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

    while ((o = getopt(argc, argv, "cxtf:")) != -1) {                                              // Iterate through all option arguments
            switch (o) {
            case 'c':                                   // c case
                numOptions++;
                checkModes(numOptions);
                opts.createArchive = 1;
                break;
            case 'x':                                   // x case
                numOptions++;
                checkModes(numOptions);
                opts.extractArchive = 1;
                break;
            case 't':                                   // t case
                numOptions++;
                checkModes(numOptions);
                opts.printContents = 1;
                break;
            case 'f':                                   // f case
                opts.archiveFile = optarg;
                opts.yesArchiveFile = 1;
                break;
            default:
                fprintf(stderr, "Error: No tarfile specified.\n");
                    exit(-1);
            }
    }
    if (optind <  argc){
        for(; optind < argc; optind++){
            if (opts.createArchive) {
                opts.inputDirectory = argv[optind];     // parse the input directory
                opts.yesInputDirectory = 1;
            }
        }
    }

    if (numOptions == 0){                                   // if no option made
        fprintf(stderr, "Error: No mode specified.\n");
        exit(-1);
    }
    if (opts.createArchive && opts.inputDirectory == NULL){             // if no directory target
        fprintf(stderr, "Error: No directory target specified.\n");
        exit(-1);
    }
    if (opts.archiveFile == NULL){                              // no tarfile
        fprintf(stderr, "Error: No tarfile specified.\n");
        exit(-1);
    }
    if (opts.createArchive && opts.inputDirectory != NULL){     // make archive and has input directory
        struct stat file_stat;
        if (lstat(opts.inputDirectory, &file_stat) != 0) {      // check if it exists
            fprintf(stderr, "Error: Specified target (\"%s\") does not exist.\n", opts.inputDirectory);
            exit(-1);
        }else{
            if (!S_ISDIR(file_stat.st_mode)) {              // if it does, check if directory
                fprintf(stderr, "Error: Specified target (\"%s\") is not a directory.\n", opts.inputDirectory);
                exit(-1);
            }
        }
    }
    return opts;
}

// check if multiple modes specified.
void checkModes (int numTrans){
    if (numTrans > 1){
        fprintf(stderr, "Error: Multiple modes specified.\n");
        exit(1);
    }
}
