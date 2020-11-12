//
// Created by Alex Welsh on 10/30/20.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <fcntl.h>
#include "tokens.h"


typedef struct {                // defines the options structure to hold the options for the program.
    int promptType;             // 0 = no prompt, 1 = default, 2 = custom
    char* promptString;
}Options;

typedef struct {
    bool append;
    char *input_file_name, *output_file_name, **cmd;
}Cmd;

typedef struct {
    bool foreground_process; // true is foreground, false is background
    Cmd *cmd_list;
    int num_commands;
}CmdSet;

Options parseArgs(int argc, char *argv[]);
CmdSet parseCommands(char* cmd_line_args);
bool keepWaiting(int *foreground_processes, int num_cmds);
void setForegroundProcess(int *foreground_processes, int pid, int num_cmds);
char** removeOperators(char **tokens, int operator_loc);
int error = 0;

int main(int argc, char **argv){
    char * cmdargv;
    char * fgets_return;
    bool cmds_executed = false;
    char **cmd_tokens;
    int pid, wpid, status;
    Options opts = parseArgs(argc, argv);
    int *foreground_pids;

    while(1){
        printf("%s", opts.promptString);
        cmdargv = malloc(1024);                                    // set space for cmd
        do{                                                             // check if fgets fails, call again if interrupt
            fgets_return = fgets(cmdargv, 1024, stdin);
        }while(fgets_return == NULL && errno == EINTR);

        cmdargv[strlen(cmdargv) - 1] = '\0';                            // remove training newline char
        error = 0;                                                      // error number used to skip over running cmd
        if (strcmp(cmdargv, "") == 0)                                   // and go back to the top of while
            error = 1;                                                  // check for no cmd entered

        CmdSet set_of_commands;                                         // create struct for all cmds
        if (!error){                                                    // if no error, parse the cmd line
            set_of_commands = parseCommands(cmdargv);
            for (int i = 0; i < set_of_commands.num_commands; i++){     // for each cmd given,
                if (set_of_commands.cmd_list[i].cmd[0] == NULL){        // check if a cmd is given, otherwise null error
                    fprintf(stderr, "Error: Invalid null command.\n");
                    error = 1;
                    break;
                }
            }
        }

        if (!error){
            for (int i = 0; i < set_of_commands.num_commands; i++){             // loop through each cmd
                // if the first first cmd has an output file and a pipe, error
                if (set_of_commands.num_commands > 1){
                    if ((i == 0) && (strcmp(set_of_commands.cmd_list[i].output_file_name, "") != 0) && (set_of_commands.num_commands > 1)){
                        fprintf(stderr, "Error: Ambiguous output redirection.\n");
                        error = 1;
                    }else if ((i == set_of_commands.num_commands - 1) && (strcmp(set_of_commands.cmd_list[i].input_file_name, "") != 0)){
                        // if the last cmd has an input file and pipe, error
                        fprintf(stderr, "Error2: Ambiguous input redirection.\n");
                        error = 1;
                    }else if ((i != 0) && (i != set_of_commands.num_commands - 1)){
                        // if a middle cmd has either an input or output file, error
                        if ((strcmp(set_of_commands.cmd_list[i].output_file_name, "") != 0)){
                            fprintf(stderr, "Error: Ambiguous output redirection.\n");
                            error = 1;
                        }else if ((strcmp(set_of_commands.cmd_list[i].input_file_name, "") != 0)){
                            fprintf(stderr, "Error3: Ambiguous input redirection.\n");
                            error = 1;
                        }
                    }
                }
                // if a cmd doesn't have a cmd, i.e. only a redirection, error.
                if (set_of_commands.cmd_list[i].cmd[0] == NULL){
                    fprintf(stderr, "Error: Invalid null command.\n");
                    error = 1;
                    break;
                }
            }
        }

        if (!error){
//            for (int i = 0; i < set_of_commands.num_commands; i++){
//                printf("command %d\n", i);
//                for (int j = 0; set_of_commands.cmd_list[i].cmd[j] != NULL; j++)
//                    printf("Cmd: %s, %s, %s\n", set_of_commands.cmd_list[i].cmd[j],  set_of_commands.cmd_list[i].input_file_name,
//                           set_of_commands.cmd_list[i].output_file_name);
//            }
            // make space to save the foreground pids
            foreground_pids = (int *) malloc(set_of_commands.num_commands * sizeof(int));
            // if the user types exit or control-d, exit shell
            if( fgets_return == NULL || strcmp(set_of_commands.cmd_list[0].cmd[0], "exit") == 0){
                if (errno != EINTR){
                    printf("Goodbye!\n");
                    exit(0);
                }
            }
        }
        // create pipes
        int pipefd[] = {-1, -1}, prevpipefd[] = {-1, -1};
        if (!error) {
            // if no error discovered, go and iterate through each cmd that was parsed for execution
            for (int cmd_index = 0; cmd_index < set_of_commands.num_commands; cmd_index++) {
                // if theres more than 1 cmd, theres a pipe so create a pipe for the middle cmds
                if ((set_of_commands.num_commands > 1) && (cmd_index < set_of_commands.num_commands - 1)) {
                    pipe(pipefd);
                }
                pid = fork();   // fork off child process
                if (pid == 0) {
                    // child process
//                    printf("child process!**************************** pid: %d, ppid: %d\n", getpid(), getppid());
                    int file_descriptor;
//                    printf("Running command with output file: %s\n",
//                           set_of_commands.cmd_list[cmd_index].output_file_name);
//                    printf("right before pipe stuff\n");
                    if (set_of_commands.num_commands > 1) {             // if more than 1 cmd, handle duping pipes
//                        fprintf(stderr, "pipe0: %d, pipe1: %d, prev0: %d, prev1: %d\n", pipefd[0], pipefd[1],
//                                prevpipefd[0], prevpipefd[1]);
                        if (cmd_index == 0) {                   // if this is the first cmd in a cmd with pipes
//                            printf("Command 0!!!!!!!! \n");
                            dup2(pipefd[1], 1);
                            close(pipefd[0]);
                            close(pipefd[1]);
//                            fprintf(stderr, "read: %d, write: %d\n", pipefd[0], pipefd[1]);
                        } else if (cmd_index == set_of_commands.num_commands - 1) {     // if this is the last cmd
//                            printf("command 2!!!!!!\n");
                            dup2(prevpipefd[0], 0);
                            close(prevpipefd[0]);
                        } else {
//                            printf("Command 1!!!!!!!! \n");
                            dup2(prevpipefd[0], 0);             // if its a middle cmd with pipes on both sides
                            dup2(pipefd[1], 1);
                            close(pipefd[0]);
                            close(pipefd[1]);
                            close(prevpipefd[0]);
                        }
                    }
//                    fprintf(stderr, "after pipe stuff: %s\n", set_of_commands.cmd_list[cmd_index].cmd[0]);

                    // if there is an output file, open it with either append or create it and dup
                    if ((strcmp(set_of_commands.cmd_list[cmd_index].output_file_name, "") != 0)) {
//                        printf("output!\n");
                        if (set_of_commands.cmd_list[cmd_index].append == true) {
                            file_descriptor = open(set_of_commands.cmd_list[cmd_index].output_file_name,
                                                   O_CREAT | O_WRONLY | O_APPEND,
                                                   S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
                        } else {
                            file_descriptor = open(set_of_commands.cmd_list[cmd_index].output_file_name,
                                                   O_CREAT | O_WRONLY | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
                        }
                        if (file_descriptor == -1){
//                            printf("error in output\n");
                            perror("open");
                            exit(0);
                        }
                        int dup_out = dup2(file_descriptor, 1);
                        close(file_descriptor);
                    }

                    // if there is an input file then open it and dup the fd
                    if ((strcmp(set_of_commands.cmd_list[cmd_index].input_file_name, "") != 0)) {
                        file_descriptor = open(set_of_commands.cmd_list[cmd_index].input_file_name, O_RDONLY);
//                        printf("input: %d\n", file_descriptor);
                        if (file_descriptor == -1){
//                            printf("error in input\n");
                            perror("open");
                            exit(0);
                        }
//                        fprintf(stderr, "fd for open %d\n", file_descriptor);
                        int dup_out = dup2(file_descriptor, 0);
//                        fprintf(stderr, "fd for input %d\n", dup_out);
                        close(file_descriptor);
                    }
//                    fprintf(stderr, "about to execute cmd %s on pid %d\n", set_of_commands.cmd_list[cmd_index].cmd[0],
//                            getpid());
                    //                    close(pipefd[1]);
                    //                    close(prevpipefd[1]);
                    // execute the cmd and check if it fails
                    if (execvp(set_of_commands.cmd_list[cmd_index].cmd[0], set_of_commands.cmd_list[cmd_index].cmd) ==
                        -1) {
                        perror(set_of_commands.cmd_list[cmd_index].cmd[0]);
                    }
                    exit(-1);
                } else {
                    // parent process
                    //            struct rusage ru;
//                    fprintf(stderr, "parent before wait\n");
                    //            printf("boolean: %d\n", set_of_commands.foreground_process);
                    foreground_pids[cmd_index] = pid;       // add foreground pids to list for tracking

                    // if the cmd is a foreground process and you're at the last cmd, do the waiting
                    if (set_of_commands.foreground_process && (cmd_index == (set_of_commands.num_commands - 1))) {
//                        printf("RUNNING LAST CMD\n");
//                        printf("waiting for: %d, pid: %d, ppid: %d\n", pid, getpid(), getppid());
                        // keep waiting for all the foreground processes
                        while (keepWaiting(foreground_pids, set_of_commands.num_commands)) {
//                            fprintf(stderr, "waiting!\n");
                            // wait for first child process that terminates
                            do{
                                wpid = wait(&status);
                            }while(wpid == -1 && errno == EINTR);
                            // after waiting, set that process to have been waited for
                            setForegroundProcess(foreground_pids, wpid, set_of_commands.num_commands);
//                            fprintf(stderr, "waited again for: %d\n", wpid);
                        }

//                        printf("waited for pid with process id: %d\n", wpid);
                    }
                    // if theres a pipes, close fd's in the parent
                    if (set_of_commands.num_commands > 1) {
                        close(pipefd[1]);
                        close(prevpipefd[0]);
                        prevpipefd[0] = pipefd[0];
                        prevpipefd[1] = pipefd[1];
                    }
//                    printf("parent after wait************************\n");
                }
            }
        }
    }
}

// loop through the list of foreground pids and if that pid was waited for, set it to -1
void setForegroundProcess(int *foreground_processes, int pid, int num_cmds){
    for (int i = 0; i < num_cmds; i++){
//        printf("pid[%d] = %d\n", i, foreground_processes[i]);
        if (foreground_processes[i] == pid){
//            printf("set pid %d to -1\n", pid);
            foreground_processes[i] = -1;
        }
    }
}

// check if you need to wait for more foreground pids. Do it by adding up the values and if
// its all -1, it will be -num_cmds. If all waited for (i.e -1) then return false.
bool keepWaiting(int *foreground_processes, int num_cmds){
    int sum = 0;
    for (int i = 0; i < num_cmds; i++){
//        printf("keep waiting for pid[%d] = %d\n", i, foreground_processes[i]);
        sum += foreground_processes[i];
    }
    if (sum == (-1 * num_cmds)) {
        return 0;
    }
    return 1;
}

// parse the cmd line that was given
CmdSet parseCommands(char* cmd_line_args){
    CmdSet set_of_commands;
//    printf("Command Line: %s\n", cmd_line_args);
    char *background_char;
    background_char = strchr(cmd_line_args, '&');       // look for background operator
//    printf("background!: %s\n", background_char);

    if (strcmp(&cmd_line_args[strlen(cmd_line_args)-1], "&") == 0) {    // if  last character, save it as bkgd process
//        printf("run in background!\n");
        cmd_line_args[strlen(cmd_line_args)-1] = '\0';
        set_of_commands.foreground_process = false;
    }else{                                                              // else it is a foreground process
        set_of_commands.foreground_process = true;
    }

    // if the operator isn't the last character, error
    if ((strchr(cmd_line_args, '&') != NULL) && strcmp(background_char, "&")){
        fprintf(stderr, "Error: \"&\" must be last token on command line\n");
        error = 1;
    }else {
        char **pipe_tokens = get_tokens(cmd_line_args, "|");        // otherwise split cmd by pipe
        int num_cmds;
        int counter = 0;
        while (pipe_tokens[counter] != NULL)                                // number of commands
            counter++;
//        printf("Number of commands!: %d\n", counter);
        set_of_commands.cmd_list = malloc(counter * sizeof(Cmd));   // malloc space for cmds

        for (num_cmds = 0; pipe_tokens[num_cmds] != NULL; num_cmds++) {             // for each cmd
            char **cmd_tokens = get_tokens(pipe_tokens[num_cmds], " \t\n");   // split by args
            Cmd new_cmd;
            new_cmd.append = false;
            new_cmd.input_file_name = malloc(1024 * sizeof(char));             // malloc space for input and output
            new_cmd.output_file_name = malloc(1024 * sizeof(char));
            new_cmd.cmd = cmd_tokens;
//            printf("hello!\n");
            for (int num_args = 0; new_cmd.cmd[num_args] != NULL; num_args++) {     // for each arg in a cmd
//                printf("token cmd %d: %s\n", num_args, cmd_tokens[num_args]);
                // if there is a output redirection check if its valid and then save it
                if ((new_cmd.cmd[num_args] != NULL) && strcmp(new_cmd.cmd[num_args], ">") == 0) {
//                    printf("output name %s\n", new_cmd.output_file_name);
                    if (new_cmd.cmd[num_args + 1] == NULL) {
                        fprintf(stderr, "Error: Missing filename for output redirection.\n");
                        error = 1;
                    } else if (strcmp(new_cmd.output_file_name, "") != 0) {
                        fprintf(stderr, "Error: Ambiguous output redirection.\n");
                        error = 1;
                    } else {
//                        printf("Output File %s\n", new_cmd.cmd[num_args + 1]);
                        new_cmd.output_file_name = new_cmd.cmd[num_args + 1];   // remove the operators from cmd
                        new_cmd.cmd = removeOperators(new_cmd.cmd, num_args);
//                        printf("result: %s %d\n", new_cmd.cmd[0], num_args);
                        if (num_args != 0)  // go back one cause the operator has been removed
                            num_args -= 1;
                    }
                }

//                printf("check here! %d\n", num_args);
                // if there is a input redirection check if its valid and then save it
                if ((new_cmd.cmd[num_args] != NULL) && strcmp(new_cmd.cmd[num_args], "<") == 0) {
//                    printf("input name %s\n", new_cmd.input_file_name);
                    if (new_cmd.cmd[num_args + 1] == NULL) {
                        fprintf(stderr, "Error: Missing filename for input redirection.\n");
                        error = 1;
                    } else if (strcmp(new_cmd.input_file_name, "") != 0) {
                        fprintf(stderr, "Error1: Ambiguous input redirection.\n");
                        error = 1;
                    } else {
//                        printf("input File %s\n", new_cmd.cmd[num_args + 1]);
                        int file_descriptor = open(new_cmd.cmd[num_args + 1], O_RDONLY);
                        // if the file descriptor errors (i.e no file, error)
                        if (file_descriptor == -1){
                            fprintf(stderr, "Error: open(\"%s\"): %s\n", new_cmd.cmd[num_args + 1], strerror(errno));
                            error = 1;
                        }else{
                            close(file_descriptor);
                            new_cmd.input_file_name = new_cmd.cmd[num_args + 1]; // remove the operator and save
                            new_cmd.cmd = removeOperators(new_cmd.cmd, num_args);
                            if (num_args != 0)      // go back one because operator was removed
                                num_args -= 1;
                        }
                    }
                }

//                printf("check before append\n");
                // if there is a input redirection to append check if its valid and then save it
                if ((new_cmd.cmd[num_args] != NULL) && strcmp(new_cmd.cmd[num_args], ">>") == 0) {
//                    printf("output append name %s\n", new_cmd.output_file_name);
                    if (new_cmd.cmd[num_args + 1] == NULL) {
                        fprintf(stderr, "Error: Missing filename for output redirection.\n");
                        error = 1;
                    } else if (strcmp(new_cmd.output_file_name, "") != 0) {
                        fprintf(stderr, "Error: Ambiguous output redirection.\n");
                        error = 1;
                    } else {
                        new_cmd.output_file_name = new_cmd.cmd[num_args + 1];
                        new_cmd.append = true;                              // set append to true and remove operators
//                        printf("Output append File %s, %d\n", new_cmd.output_file_name, new_cmd.append);
                        new_cmd.cmd = removeOperators(new_cmd.cmd, num_args);
                        if (num_args != 0)          // go back one because operator was removed
                            num_args -= 1;
                    }
                }

            }
//            printf("adding cmd!\n");
            set_of_commands.cmd_list[num_cmds] = new_cmd;       // add the final parsed cmd to list
            set_of_commands.num_commands = counter;             // save number of cmds
        }
    }
    return set_of_commands;
}

// removes the <, >, >> operators from the cmd
char** removeOperators(char **tokens, int operator_loc){
    int counter = 0;
    // get number of args in cmd
    while(tokens[counter] != NULL){
        counter++;
    }
//    printf("counter out: %d\n", counter);
    char **cmd_tokens = (char**) malloc((counter - 1) * sizeof(char*)); // malloc space just cmds and NULL end

    int offset = 0;
//    int containsCommand = 0;
    for (int i = 0; tokens[i] != NULL; i++){                                        // iterate through each arg
//        printf("this is i in remove ops: %d and %s\n", i, tokens[i + 1]);
        if ((offset != 2) && (i == operator_loc || (i == operator_loc + 1))) {     // if its the operator or file,remove
//            printf("removed token!: %s\n", tokens[i]);
            offset++;
        }else{                                                                      // otherwise keep it
//            printf("saved token!: %s\n", tokens[i]);
//            containsCommand = 1;
            cmd_tokens[i - offset] = tokens[i];
        }
    }

//    printf("past the seg fault?? %d\n", containsCommand);
//    printf("done with that\n");
    cmd_tokens[counter-offset] = NULL;                                      // set last index to NULL for end of cmd
//    printf("counter - offset %d %d\n", counter, offset);

    return cmd_tokens;
}

// parse the args
Options parseArgs(int argc, char *argv[]){
    Options opts;
    if (argc == 1){                             // if no args, default prompt
//        printf("default shell prompt\n");
        opts.promptType = 1;
        opts.promptString = "mysh:";
        return opts;
    }else if (argc == 2){                      // if  - then no prompt
        if (strcmp(argv[1], "-") == 0){
            opts.promptType = 0;
            opts.promptString = "";
            return opts;
        }else {                                 // if a second prompt then make it the custom one.
            opts.promptType = 2;
            opts.promptString = argv[1];
            return opts;
        }
    }
    fprintf(stderr, "Error: Usage: %s [prompt]\n", "mysh");
    exit(1);
}
