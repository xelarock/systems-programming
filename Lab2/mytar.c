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
void createArchive(Options opts);

int main(int argc, char *argv[]) {
    Options opts = parseArgs(argc, argv);

    if (opts.createArchive == 1){
        createArchive(opts);
    }

    return 0;
}

void createArchive(Options opts){
    char buffer[100];
    int32_t magic2;
    struct dirent *de;
    struct stat file_stat;
    char * fullname;
    int fd;

    FILE *archiveFile = fopen(opts.archiveFile,"w+");
    int32_t magic_number = 0x7261746D;
    fwrite(&magic_number, 4, 1, archiveFile);
//    fclose(archiveFile);

//    archiveFile = fopen(opts.archiveFile, "r");
    fseek(archiveFile, -4, SEEK_CUR);
    fread(buffer, 4, 1, archiveFile);
    printf("hex: %x\n", *((int32_t *) buffer));

    DIR *input_directory = opendir(opts.inputDirectory);
    fullname = (char *)malloc(sizeof(char)*(strlen(opts.inputDirectory)+258));
    for (de = readdir(input_directory); de != NULL; de = readdir(input_directory)) {
//        printf("cmp: %d %d\n", strcmp(de->d_name, "."), strcmp(de->d_name, ".."));
        if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0){
            printf("++++++++++++++++++++++\n");
            printf("%s\n", de->d_name);
            sprintf(fullname, "%s/%s", opts.inputDirectory, de->d_name);
            stat(fullname, &file_stat);

            fwrite(&file_stat.st_ino, 8, 1, archiveFile);                   // inode #
            ino64_t inode_num = 0;
            fseek(archiveFile, -8, SEEK_CUR);
            fread(&inode_num, 8, 1, archiveFile);
            printf("inode: %llu, read_inode: %llu\n", file_stat.st_ino, inode_num);

            int32_t file_name_length = strlen(de->d_name) + 1;
            fwrite(&file_name_length, 4, 1, archiveFile);                   // name length
            file_name_length = 0;
            fseek(archiveFile, -4, SEEK_CUR);
            fread(&file_name_length, 4, 1, archiveFile);
            printf("name length: %lu, read length: %d\n", strlen(de->d_name) + 1, file_name_length);

            fwrite(&(de->d_name), file_name_length, 1, archiveFile);                   // name
            char *file_name;
            fseek(archiveFile, 0-file_name_length, SEEK_CUR);
            fread(file_name, file_name_length, 1, archiveFile);
            printf("name : %s, read name: %s\n", de->d_name, file_name);

            // at this point in the future, check if file is a hard link, if so, break here

            fwrite(&file_stat.st_mode, 4, 1, archiveFile);                   // mode
            mode_t read_mode;
            fseek(archiveFile, -4, SEEK_CUR);
            fread(&read_mode, 4, 1, archiveFile);
            printf("mode : %us, read mode: %us\n", file_stat.st_mode, read_mode);

            fwrite(&file_stat.st_mtimespec.tv_sec, 8, 1, archiveFile);             // modification time
            time_t read_mtime;
            fseek(archiveFile, -8, SEEK_CUR);
            fread(&read_mtime, 8, 1, archiveFile);
            printf("mtime : %ld, read mtime: %ld\n", file_stat.st_mtimespec.tv_sec, read_mtime);

            if (S_ISDIR(file_stat.st_mode)){
                printf("DIRECTORY\n");
            }else {
                fwrite(&file_stat.st_size, 8, 1, archiveFile);             // size
                off_t read_size;
                fseek(archiveFile, -8, SEEK_CUR);
                fread(&read_size, 8, 1, archiveFile);
                printf("size : %llu, read size: %llu\n", file_stat.st_size, read_size);

                fwrite(&file_stat, read_size, 1, archiveFile);             // content
                off_t read_size;
                fseek(archiveFile, -8, SEEK_CUR);
                fread(&read_size, 8, 1, archiveFile);
                printf("size : %llu, read size: %llu\n", file_stat.st_size, read_size);
            }
        }
    }
    fclose(archiveFile);

    for (de = readdir(input_directory); de != NULL; de = readdir(input_directory)) {

    }

}

void read_tar(char * dirname){

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