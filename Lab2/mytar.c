//
// Created by Alex Welsh on 9/30/20.
//
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
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
void extractArchive(FILE *archiveFile, DIR *currentDirectory);
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
        int64_t inode_num = (int64_t) file_stat.st_ino;
        fwrite(&inode_num , 8, 1, archiveFile);                   // inode #
        set_inode(file_stat.st_ino, opts.inputDirectory);
        int32_t file_name_length = strlen(opts.inputDirectory);
        fwrite(&file_name_length, 4, 1, archiveFile);                   // name length
        fwrite(opts.inputDirectory, file_name_length, 1, archiveFile);                  // name
        int32_t mode = (int32_t) file_stat.st_mode;
        fwrite(&mode, 4, 1, archiveFile);                      // mode
        int64_t time_num = (int64_t) file_stat.st_mtimespec.tv_sec;
        fwrite(&time_num, 8, 1, archiveFile);          // modification time
        printf("end of dir1 \n");
        createArchive(archiveFile, opts.inputDirectory);
        fclose(archiveFile);
    }
    if (opts.printContents == 1){
        printContents(opts.archiveFile);
    }
    if (opts.extractArchive == 1){
        FILE *archiveFile = fopen(opts.archiveFile,"r");
        int32_t magic_number;
        fread(&magic_number, 4, 1, archiveFile);
        if (magic_number != 0x7261746D){
            fprintf(stderr, "Error: Bad magic number (%d), should be: %d.\n", magic_number, 0x7261746D);
            exit(-1);
        }
        int64_t inode_num;
        fread(&inode_num, 8, 1, archiveFile);
        int32_t name_length;
        fread(&name_length, 4, 1, archiveFile);
        char *fullname = (char *)malloc(name_length * sizeof(char));
        fread(fullname, name_length, 1, archiveFile);
        int32_t mode_num;
        fread(&mode_num, 4, 1, archiveFile);
        int64_t time_num;
        fread(&time_num, 8, 1, archiveFile);
//        fwrite(&time_num, 8, 1, archiveFile);
        set_inode((ino64_t) inode_num, fullname);

        mkdir(fullname, (mode_t) mode_num);
        DIR *currentDirectory = opendir(fullname);
        extractArchive(archiveFile, currentDirectory);
    }
    return 0;
}

void extractArchive(FILE *archiveFile, DIR *currentDirectory){
    ino64_t inode_num = 0;
    while(fread(&inode_num, 8, 1, archiveFile) == 1){
        printf("************************\n");
        printf("inode num %llu \n", inode_num);
        int32_t file_name_length;
        fread(&file_name_length, 4, 1, archiveFile);
        char *file_name =  malloc(file_name_length * sizeof(char));
        fread(file_name, file_name_length, 1, archiveFile);
        printf("filename: %s\n", file_name);

        if(get_inode(inode_num) != NULL){
            printf("HARD LINK\n");
            char *src_name = (char *) malloc((strlen(get_inode(inode_num)) + 1) * sizeof(char));
            strcpy(src_name, get_inode(inode_num));
            link(src_name, file_name);
        }else{
            int32_t file_mode;
            fread(&file_mode, 4, 1, archiveFile);
            int64_t file_mtime;
            fread(&file_mtime, 8, 1, archiveFile);
            if (S_ISDIR((mode_t) file_mode)){
                printf("DIRECTORY\n");
                mkdir(file_name, (mode_t) file_mode);
                set_inode(inode_num, file_name);
            }else{
                printf("FILE\n");
                int64_t file_size;
                fread(&file_size, 8, 1, archiveFile);

                char *file_contents = (char *) malloc(file_size * sizeof(char));
                fread(file_contents, file_size, 1, archiveFile);

                FILE *currentFile = fopen(file_name,"w+");
                fwrite(file_contents, file_size, 1, currentFile);
                chmod(file_name, (mode_t) file_mode);
                struct timeval time_val_ac;
                struct timeval time_val_mod;
                gettimeofday(&time_val_ac, NULL);
                time_val_mod.tv_sec = (time_t) file_mtime;
                time_val_mod.tv_usec = 0;
//                time_to_set.modtime = time_val_mod;
//                time_to_set.actime = time_val_ac;
                struct timeval *times = (struct timeval *) malloc(2 * sizeof(struct timeval));
                times[0] = time_val_ac;
                times[1] = time_val_mod;
                printf("time: %lu %lu, %d\n", (time_t) file_mtime, times[1].tv_sec, times[1].tv_usec);
                fclose(currentFile);
                utimes(file_name, times);

                set_inode(inode_num, file_name);
            }
        }
    }

}

void printContents(char *archiveFileString){
    FILE *archiveFile = fopen(archiveFileString,"r");
    int32_t magic_number;

    fread(&magic_number, 4, 1, archiveFile);
    printf("hex: %x\n", magic_number);

    ino64_t inode_num = 0;
    while(fread(&inode_num, 8, 1, archiveFile) == 1){
        printf("+++++++++++++++++++++++\n");

        int32_t file_name_length;
        fread(&file_name_length, 4, 1, archiveFile);

        char *file_name = (char *) malloc((file_name_length) * sizeof(char));
        fread(file_name, file_name_length, 1, archiveFile);
        file_name[file_name_length] = '\0';

        mode_t read_mode;
        int32_t read_mode_int;
        if (fread(&read_mode_int, 4, 1, archiveFile) == -1)
            perror("fread");
        read_mode = (mode_t) read_mode_int;

        if (!feof(archiveFile))                                             // if the last file in archive is hard link
            fseek(archiveFile, -4, SEEK_CUR);                               // don't rewind 4 bytes after reading mode

        if(S_ISDIR(read_mode) || S_ISREG(read_mode)){
            fread(&read_mode_int, 4, 1, archiveFile);
            read_mode = (mode_t) read_mode_int;

            time_t read_mtime;
            int64_t read_mtime_int;
            fread(&read_mtime_int, 8, 1, archiveFile);
            read_mtime = (time_t) read_mtime_int;

            if (S_ISDIR(read_mode)){
                printf("%s/ -- inode: %llu, mode: %o, mtime: %llu\n", file_name, inode_num, read_mode, (unsigned long long) read_mtime);
            }else {
                off_t read_size;
                int64_t read_size_int;
                fread(&read_size_int, 8, 1, archiveFile);
                read_size = (off_t) read_size_int;

                char *read_contents = (char *) malloc(read_size * sizeof(char));
                fread(read_contents, read_size, 1, archiveFile);

                if ((read_mode & S_IXUSR) || (read_mode & S_IXGRP) || (read_mode & S_IXOTH)){
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
            int64_t inode_num = (int64_t) file_stat.st_ino;
            fwrite(&inode_num, 8, 1, archiveFile);                   // inode #

            int32_t file_name_length = strlen(fullname);
            fwrite(&file_name_length, 4, 1, archiveFile);                   // name length
            fwrite(fullname, file_name_length, 1, archiveFile);                  // name

            if(!get_inode( file_stat.st_ino )) {            //inode not yet seen; add to list and process
                set_inode(file_stat.st_ino, fullname);
                printf("%s path: %s\n", de->d_name, fullname);
                // at this point in the future, check if file is a hard link, if so, break here

                int32_t mode = (int32_t) file_stat.st_mode;
                fwrite(&mode, 4, 1, archiveFile);                      // mode
                int64_t time_num = (int64_t) file_stat.st_mtimespec.tv_sec;
                fwrite(&time_num, 8, 1, archiveFile);          // modification time

                if (S_ISDIR(file_stat.st_mode)){
                    printf("DIRECTORY\n");
                }else {
                    int64_t file_size = (int64_t) file_stat.st_size;
                    fwrite(&file_size, 8, 1, archiveFile);             // size

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