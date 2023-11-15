/**
 * \author Maxime Schuybroeck
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "functions.h"
#include <sys/wait.h>

int main() {
    int pipe_fd[2];
    // Create a pipe
    if (create_pipe(pipe_fd) == -1) {
        exit(EXIT_FAILURE);
    }

    // Fork the process
    pid_t child_pid = fork();
    if (child_pid == -1) {
        perror("Error in fork");
        exit(EXIT_FAILURE);
    }
    if (child_pid > 0) {
        // Parent process
        close(pipe_fd[0]); // Close the reading end in the parent process
        // Send a message from the parent to the child
        const char *message = "Hi There";
        communicate_parent_to_child(pipe_fd, message);
        // Wait for the child process to finish
        wait(NULL);
    } else {
        // Child process
        communicate_child_to_parent(pipe_fd);
    }

    return 0;
}
