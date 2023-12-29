/**
 * \author Maxime Schuybroeck
 */

#include "sbuffer.h"
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include "datamgr.h"
#include "connmgr.h"
#include "sensor_db.h"
#include "config.h"
#include <unistd.h>
#include <time.h>


// global variables
sbuffer_t *buffer;
//FILE *log_file;
int log_pipe[2];
int sequence_num = 0;
//pthread_mutex_t log_mutex;


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
        //close(log_pipe[1]);

        // opening the log_file
        FILE *log_file = fopen("gateway.log", "w");
        if(log_file == NULL){
            printf("Error in opening the log_file because it is EMPTY\n");
            return -1;
        }

        //local variables
        int log_buffer[256];
        ssize_t result_bytes;

        while((result_bytes = read(log_pipe[0], log_buffer, sizeof(log_buffer)))){
            time_t current_time;
            time(&current_time);
            char *date = ctime(&current_time);
            date[strlen(date) - 1] = '\0';
            fprintf(log_file, "%d - %s - ", sequence_num, date);
            fwrite(log_buffer,1,result_bytes,log_file);
            sequence_num++;
        }

        // closing the log_file
        fprintf(log_file,"\n");
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



int main(int argc, char *argv[]) {
    // first checking if the provided arguments are right
    if(argc < 3) {
        printf("Please provide the right arguments: first the port, then the max nb of clients");
        return -1;
    }

    // initialising the buffer
    if(sbuffer_init(&buffer) != SBUFFER_SUCCESS) {
        fprintf(stderr, "Error initialising the buffer\n");
        return -1;
    }

    // starting the logger
    if(create_log_process() != 0){
        fprintf(stderr, "Failed starting the log process\n");
    }

    // setting up the data manager
    datamgr_parse_room_sensor_map();

    // setting up the storage manager
    open_db();

    // creating the threads
    pthread_t tid[3];
    pthread_create(&tid[0], NULL, (void *)start_connmgr, argv);
    pthread_create(&tid[1], NULL, data_manager_thread, NULL);
    pthread_create(&tid[2], NULL, storage_manager_thread, NULL);

    // joining the treads
    for (int i = 0; i < 3; i++) {
        pthread_join(tid[i], NULL);
    }

    // closing CSV file
    close_db();
    // freeing the buffer
    sbuffer_free(&buffer);
    // freeing the list
    //TODO END
    printf("!!!!!!!!!!!!!! hier geraakt\n");
    datamgr_free();
    // closing the logger process
    end_log_process();

}