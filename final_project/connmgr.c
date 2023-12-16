/**
 * \author Maxime Schuybroeck
 */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "lib/tcpsock.h"
#include "lib/dplist.h"
#include <pthread.h>
#include "sbuffer.h"
#include "connmgr.h"



// locale variable
extern sbuffer_t *buffer;


void *thread_runner(void *arg){
    tcpsock_t *client = (tcpsock_t *) arg;
    sensor_data_t *data = malloc(sizeof(sensor_data_t));

    int bytes, result;
    do {
        bytes = sizeof(data);
        result = tcp_receive(client, (void *) &data, &bytes);

        if ((result == TCP_NO_ERROR) && bytes) {
            if(sbuffer_insert(buffer, data) != SBUFFER_SUCCESS){
                fprintf(stderr, "Error connmgr in thread_runner: failed inserting the data into the buffer\n");
            }
            printf("sensor id = %" PRIu16 " - temperature = %g - timestamp = %ld\n", data->id, data->value,
                    (long int) data->ts);
        }
    } while (result == TCP_NO_ERROR);
    if (result == TCP_CONNECTION_CLOSED)
        printf("Peer has closed connection\n");
    else
        printf("Error connmgr in thread_runner: occured on connection to peer\n");
    tcp_close(&client);
    free(data);
    pthread_exit(NULL);
}


void *start_connmgr(void *argv[]) {
    // processing method arguments
    int MAX_CONN = atoi((char *)argv[2]);
    int PORT = atoi((char *)argv[1]);

    //TODO: array gaat ni werken dus iets anders vinden
    pthread_t tid[MAX_CONN];

    // initialising server variables
    tcpsock_t *server, *client;
    int conn_counter = 0;

    // opening the TCP connection
    if (tcp_passive_open(&server, PORT) != TCP_NO_ERROR) exit(EXIT_FAILURE);
    printf("Test server is started\n");

    while(conn_counter < MAX_CONN){
        if (tcp_wait_for_connection(server, &client) != TCP_NO_ERROR) exit(EXIT_FAILURE);
        printf("Incoming client connection\n");

        // creating a new thread handler for the client
        if(pthread_create(&tid[conn_counter], NULL, thread_runner, client) != 0){
            printf("failure creating thread %u \n", conn_counter);
            exit(EXIT_FAILURE);
        }else printf("Created thread %u \n", conn_counter);
        conn_counter++;
    }

    // joining the treads
    for (int i = 0; i < MAX_CONN; i++) {
        pthread_join(tid[i], NULL);
    }

    // closing the TCP connection
    if (tcp_close(&server) != TCP_NO_ERROR) exit(EXIT_FAILURE);
    printf("Server is shutting down\n");

    // inserting an end-of-stream marker in the buffer
    sensor_data_t *data = (sensor_data_t *)malloc(sizeof(sensor_data_t));
    //TODO: vragen als deze end-of-stream marker correct is gedaan --> data.id shit
    data->id = 0;    // making an indication
    if(sbuffer_insert(buffer, data) != SBUFFER_SUCCESS){
        fprintf(stderr, "Error in inserting the end-of-stream marker into the buffer\n");
    }
    return NULL;
}