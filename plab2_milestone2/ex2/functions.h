/**
 * \author Maxime Schuybroeck
 */

#ifndef FUNCTIONS_H
#define FUNCTIONS_H


// functions declarations
int create_pipe(int pipe_fd[2]);
void communicate_parent_to_child(int pipe_fd[2], const char *message);
void communicate_child_to_parent(int pipe_fd[2]);

#endif /* FUNCTIONS_H */