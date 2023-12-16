/**
 * \author Maxime Schuybroeck
 */

#ifndef _CONNMGR_H_
#define _CONNMGR_H_

#ifndef DTIMEOUT
#define DTIMEOUT 5
#endif
/*
#ifndef MAX_CONN
#define MAX_CONN 3  // max number of connections the server will handle before it will stop
#endif
*/
#include "config.h"

/**
 * runs each thread.
 * @param arg
 * @return
 */
void *thread_runner(void *arg);

/**
 *
 * @param argc
 * @param argv
 * @return
 */
void *start_connmgr(void *argv[]);

#endif  //_CONNMGR_H_
