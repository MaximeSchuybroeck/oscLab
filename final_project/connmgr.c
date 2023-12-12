/**
 * \author Maxime Schuybroeck
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "config.h"
#include "lib/tcpsock.h"
#include "connmgr.h"
#include <pthread.h>
#include <sbuffer.h>

// locale variable
sbuffer_t buffer;


void *thread_runner(void *arg){
    tcpsock_t *client = (tcpsock_t *) arg;
    sensor_data_t data;
    int bytes, result;
    //////////////////////// actual data upload gedeelte
    do {
        /////////// desnoods onderste 2 lijnen weg doen en de rest uncommenten --> werkt zwz (dit is het effecteive data verzamellen dat los staat van de if hieronder
        bytes = sizeof(data);
        result = tcp_receive(client, (void *) &data, &bytes);
        /*
        // read sensor ID
        bytes = sizeof(data.id);
        result = tcp_receive(client, (void *) &data.id, &bytes);
        // read temperature
        bytes = sizeof(data.value);
        result = tcp_receive(client, (void *) &data.value, &bytes);
        // read timestamp
        bytes = sizeof(data.ts);
        result = tcp_receive(client, (void *) &data.ts, &bytes);
         */
        if ((result == TCP_NO_ERROR) && bytes) {
            /////// TODO: een manier vinden om de buffer op te vragen uit de main file
            if(sbuffer_insert(&buffer, &data) != SBUFFER_SUCCESS){
                fprintf(stderr, "Error in inserting the end-of-stream marker into the buffer\n");
            }
            printf("sensor id = %" PRIu16 " - temperature = %g - timestamp = %ld\n", data.id, data.value,
                    (long int) data.ts);
        }
        //////////////////////// actual data upload gedeelte
    } while (result == TCP_NO_ERROR);
    if (result == TCP_CONNECTION_CLOSED)
        printf("Peer has closed connection\n");
    else
        printf("Error occured on connection to peer\n");
    tcp_close(&client);
    pthread_exit(NULL);
}


int start_connmgr(int argc, char *argv[], sbuffer_t* given_buffer) {
    tcpsock_t *server, *client;
    sensor_data_t data;
    int bytes, result;
    int conn_counter = 0;
    buffer = given_buffer;

    if(argc < 3) {
    	printf("Please provide the right arguments: first the port, then the max nb of clients");
    	return -1;
    }
    
    int MAX_CONN = atoi(argv[2]);
    int PORT = atoi(argv[1]);
    pthread_t thread_ident[MAX_CONN];

    printf("Test server is started\n");
    if (tcp_passive_open(&server, PORT) != TCP_NO_ERROR) exit(EXIT_FAILURE);

    while(conn_counter < MAX_CONN){
        if (tcp_wait_for_connection(server, &client) != TCP_NO_ERROR) exit(EXIT_FAILURE);
        printf("Incoming client connection\n");
        conn_counter++;

        // creating a new thread handler for the client
        if(pthread_create(&thread_ident[conn_counter],NULL, thread_runner, (void *)client) != 0){
            printf("failure creating thread %u \n", conn_counter);
            exit(EXIT_FAILURE);
        }else printf("Created thread %u \n", conn_counter);
    }
    conn_counter = 0;
    while(conn_counter < MAX_CONN){
        conn_counter++;
        pthread_join(thread_ident[conn_counter], NULL);
    }

    if (tcp_close(&server) != TCP_NO_ERROR) exit(EXIT_FAILURE);
    printf("Test server is shutting down\n");
    return 0;
}




