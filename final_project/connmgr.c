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


void *thread_runner(void *arg) {
    tcpsock_t *client = (tcpsock_t *) arg;
    sensor_data_t *data = malloc(sizeof(sensor_data_t));
    memset(data, 0, sizeof(sensor_data_t));

    int bytes, result;
    do {
        bytes = sizeof(sensor_data_t);
        result = tcp_receive(client, (void *) data, &bytes);

        if ((result == TCP_NO_ERROR) && bytes) {
            if (sbuffer_insert(buffer, data) != SBUFFER_SUCCESS) {
                fprintf(stderr, "Error connmgr in thread_runner: failed inserting the data into the buffer\n");
            }

            char msg[55];
            snprintf(msg, sizeof(msg), "Sensor node %" PRIu16 " has opened a new connection\n", data->id);
            write_to_log_process(msg);
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
        printf("Error connmgr in thread_runner: occurred on connection to peer\n");
    }

    tcp_close(&client);
    free(data);
    pthread_exit(NULL);
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
        free(data);
        exit(EXIT_FAILURE);
    }

    return NULL;
}
