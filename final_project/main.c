/**
 * \author Maxime Schuybroeck
 */

#include "sbuffer.h"
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include "datamgr.h"
#include "connmgr.h"
#include <stdbool.h>
#include "sensor_db.h"
#include "config.h"
#include <unistd.h>
#include <time.h>
#include <stdlib.h>


// global variables
sbuffer_t *buffer;
FILE* csv_file;
int log_pipe[2];
FILE *log_file;
int sequence_num = 0;


int write_to_log_process(char *msg){
    ///TODO: log file fixen
    //write(log_pipe[1], msg, strlen(msg));
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
        int buffer[256];
        ssize_t result_bytes;

        while((result_bytes = read(log_pipe[0], buffer, sizeof(buffer)))){
            time_t current_time;
            time(&current_time);
            char *date = ctime(&current_time);
            date[strlen(date) - 1] = '\0';
            fprintf(log_file, "%d - %s - ", sequence_num, date);
            fwrite(buffer,1,result_bytes,log_file);
            sequence_num++;
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
// logging function end



int main(int argc, char *argv[]) {
    // first checking if the provided arguments are right
    if(argc < 3) {
        printf("Please provide the right arguments: first the port, then the max nb of clients\n");
        return -1;
    }

    // initialising the buffer
    if(sbuffer_init(&buffer) != SBUFFER_SUCCESS) {
        fprintf(stderr, "Error initialising the buffer\n");
        return -1;
    }

    // starting the logger
    if(create_log_process() != 0){
        fprintf(stderr, "Failed ending the log process\n");
    }

    // setting up the data manager
    FILE *fp_sensor_map = fopen("room_sensor.map", "r");
    datamgr_parse_room_sensor_map(fp_sensor_map);

    // setting up the storage manager

    csv_file = open_db("data.csv", false);

    // creating the threads
    pthread_t tid[3];
    pthread_create(&tid[0], NULL, (void *)start_connmgr, (void *) argv);
    //pthread_create(&tid[1], NULL, data_manager_thread, NULL);
    //pthread_create(&tid[2], NULL, storage_manager_thread, NULL);

    // joining the treads
    for (int i = 0; i < 1; i++) {
        pthread_join(tid[i], NULL);
    }

    // closing CSV file
    close_db(csv_file);
    // freeing the buffer
    sbuffer_free(&buffer);
    // freeing hte list
    datamgr_free();
    // closing the logger process
    if(end_log_process() != 0){
        fprintf(stderr, "Failed ending the log process\n");
    }

}