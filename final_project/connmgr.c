#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "lib/tcpsock.h"
#include <pthread.h>
#include "sbuffer.h"
#include "connmgr.h"
#include "string.h"

// global variable
extern sbuffer_t *buffer;

void *thread_runner(void *arg){
    tcpsock_t *client = (tcpsock_t *) arg;
    sensor_data_t data;
    int bytes, result;
    do {
        // read sensor ID
        bytes = sizeof(data.id);
        result = tcp_receive(client, (void *) &data.id, &bytes);
        // read temperature
        bytes = sizeof(data.value);
        result = tcp_receive(client, (void *) &data.value, &bytes);
        // read timestamp
        bytes = sizeof(data.ts);
        result = tcp_receive(client, (void *) &data.ts, &bytes);

        if ((result == TCP_NO_ERROR) && bytes) {
            sensor_data_t *new_data = malloc(sizeof(sensor_data_t));
            new_data->id = data.id;
            new_data->value = data.value;
            new_data->ts = data.ts;
            if(sbuffer_insert(buffer,new_data)== SBUFFER_SUCCESS){
                //TODO END: printf nog weg en sbuffer_insert uit if functie halen
                printf("sensor id = %" PRIu16 " - temperature = %g - timestamp = %ld\n", data.id, data.value,
                       (long int) data.ts);
            }

        }
    } while (result == TCP_NO_ERROR);
    if (result == TCP_CONNECTION_CLOSED)
        printf("Peer has closed connection\n");
    else
        printf("Error occured on connection to peer\n");
    tcp_close(&client);
    pthread_exit(NULL);
    return NULL;
}


void *start_connmgr(char *argv[]) {
    tcpsock_t *server, *client;
    int conn_counter = 0;

    int MAX_CONN = atoi(argv[2]);
    int PORT = atoi(argv[1]);
    pthread_t threads[MAX_CONN];

    printf("Server is started\n");
    if (tcp_passive_open(&server, PORT) != TCP_NO_ERROR) exit(EXIT_FAILURE);

    do {
        if (tcp_wait_for_connection(server, &client) != TCP_NO_ERROR) exit(EXIT_FAILURE);
        printf("Incoming client connection\n");
        conn_counter++;

        // creating a new thread handler for the client
        if(pthread_create(&threads[conn_counter], NULL, thread_runner, client) != 0){
            printf("failure creating thread %u \n", conn_counter);
            exit(EXIT_FAILURE);
        }else printf("Created thread %u \n", conn_counter);
    } while(conn_counter < MAX_CONN);

    // After last-created thread --> conn_counter = 3 --> while loop stops
    // so the server can shut down because all threads are created
    if(conn_counter > MAX_CONN){
        if (tcp_close(&server) != TCP_NO_ERROR) exit(EXIT_FAILURE);
        printf("Server is shutting down\n");
        //TODO: log msg toevoegen server is shutting down
    }

    // joining the threads
    conn_counter = 0;
    while(conn_counter < MAX_CONN){
        conn_counter++;
        pthread_join(threads[conn_counter], NULL);
    }

    return 0;
}
// oude conmgr.c
//TODO END: oude conmgr wegdoen
/*
void *thread_runner(void *arg) {
    tcpsock_t *client = (tcpsock_t *) arg;
    sensor_data_t *data = (sensor_data_t *) malloc(sizeof(sensor_data_t));
    memset(data, 0, sizeof(sensor_data_t));

    int bytes, result;
    do {
        // getting the id
        bytes = sizeof(data->id);
        result = tcp_receive(client, (void *) &data->id, &bytes);
        // getting the value
        bytes = sizeof(data->value);
        result = tcp_receive(client, (void *) &data->value, &bytes);
        // getting the ts
        bytes = sizeof(data->ts);
        result = tcp_receive(client, (void *) &data->ts, &bytes);

        if ((result == TCP_NO_ERROR) && bytes) {
            if (sbuffer_insert(buffer, data) != SBUFFER_SUCCESS) {
                fprintf(stderr, "Error connmgr in thread_runner: failed inserting the data into the buffer\n");
            }

            char msg[55];
            snprintf(msg, sizeof(msg), "Sensor node %" PRIu16 " has opened a new connection\n", data->id);
            write_to_log_process(msg);
            break;
            //TODO: printf weg doen
            //printf("sensor id = %" PRIu16 " - temperature = %f - timestamp = %ld\n", data->id, data->value,
            //        (long int) data->ts);
        }
    } while (result == TCP_NO_ERROR);

    if (result == TCP_CONNECTION_CLOSED) {
        char msg[55];
        snprintf(msg, sizeof(msg), "Sensor node %" PRIu16 " has closed the connection\n", data->id);
        write_to_log_process(msg);

        printf("Peer has closed connection\n");
    } else {
        //TODO: checken of dit oke is
        printf("Error connmgr in thread_runner: occurred on connection to peer\n");
    }

    result = tcp_close(&client);
    free(data);
    pthread_exit(NULL);
    //return NULL;
}

void *start_connmgr(void *argv[]) {
    int MAX_CONN = atoi((char *) argv[2]);
    int PORT = atoi((char *) argv[1]);

    pthread_t tid[MAX_CONN];
    tcpsock_t *server, *client;
    int conn_counter = 0;

    if (tcp_passive_open(&server, PORT) != TCP_NO_ERROR) exit(EXIT_FAILURE);
    printf("Server is started\n");

    while (conn_counter < MAX_CONN) {
        if (tcp_wait_for_connection(server, &client) != TCP_NO_ERROR) exit(EXIT_FAILURE);
        printf("Incoming client connection\n");

        if (pthread_create(&tid[conn_counter], NULL, thread_runner, client) != 0) {
            printf("Failure creating thread %u \n", conn_counter);
            free(client);
            exit(EXIT_FAILURE);
        } else{
            printf("Created connection thread %u \n", conn_counter);
            conn_counter++;
        }

    }

    for (int i = 0; i < MAX_CONN; i++) {
        pthread_join(tid[i], NULL);
    }

    if (tcp_close(&server) != TCP_NO_ERROR) exit(EXIT_FAILURE);
    printf("Server is shutting down\n");

    sensor_data_t *data = (sensor_data_t *) malloc(sizeof(sensor_data_t));
    data->id = 0;
    if (sbuffer_insert(buffer, data) != SBUFFER_SUCCESS) {
        fprintf(stderr, "Error in inserting the end-of-stream marker into the buffer\n");

        exit(EXIT_FAILURE);
    }
    free(data);
    return NULL;
}
*/