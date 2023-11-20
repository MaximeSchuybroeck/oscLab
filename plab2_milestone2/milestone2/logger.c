/**
 * \author Maxime Schuybroeck
 */

#include "logger.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>

int log_pipe[2];
FILE *log_file;

// log_pipe[0] = reading
// log_pipe[1] = writing

int write_to_log_process(char *msg){
    write(log_pipe[1], msg, strlen(msg));
    return 0; // = success
}

int create_log_process(){
    // opening the pipe
    if(pipe(log_pipe) == -1){
        printf("Error in opening/creating the pipe\n");
        return -1;
    }

    // forking
    pid_t pid = fork();
    if(pid == -1){
        printf("Error in forking log\n");
        return -1;
    } else if(pid == 0){    // = child process
        // Closing the writing process
        close(log_pipe[1]);

        // opening the log_file
        log_file = fopen("gateway.log", "a"); //append mode
        if(log_file == NULL){
            printf("Error in opening the log_file because it is EMPTY\n");
            return -1;
        }

        //local variables
        int buffer[50];
        ssize_t result_bytes;

        // reading and directly writing to the log log_file
        while((result_bytes = read(log_pipe[0], buffer, sizeof(buffer)))){
            fwrite(buffer,1,result_bytes,log_file);
        }


        // closing the log log_file
        fclose(log_file);

    }
    // parent process
    close(log_pipe[0]);
    return 0;
}

int end_log_process(){
    // closing the write process
    close(log_pipe[1]);
    return 0;
}

