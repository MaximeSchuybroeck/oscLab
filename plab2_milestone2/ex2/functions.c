/**
 * \author Maxime Schuybroeck
 */

#include "functions.h"
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>

int create_pipe(int pipe_fd[2]){
    if(pipe(pipe_fd) == -1){
        perror("Error with creating pipe");
        return -1;
    }
    return 0; // = success
}

void communicate_parent_to_child(int pipe_fd[2], const char *message){
    // closing the reading end in the parent process
    close(pipe_fd[0]);
    // writing the message to the pipe
    write(pipe_fd[1], message, strlen(message) + 1);
    // closing the writing end in the parent process
    close(pipe_fd[1]);
}

void communicate_child_to_parent(int pipe_fd[2]){
    // closing the reading end in the child process
    close(pipe_fd[1]);

    // reading the message
    char buffer[50]; // random value gekozen
    ssize_t bytes_read = read(pipe_fd[0], buffer, sizeof(buffer));
    for (ssize_t i = 0; i < bytes_read; ++i) {
        if (islower(buffer[i])) {
            buffer[i] = toupper(buffer[i]);
        } else if (isupper(buffer[i])) {
            buffer[i] = tolower(buffer[i]);
        }
    }
    // printing result
    printf("Child Process: %s\n", buffer);
    // Close the reading end in the child process
    close(pipe_fd[0]);
}