/**
 * \author Maxime Schuybroeck
 */

#ifndef _CONNMGR_H_
#define _CONNMGR_H_

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
int start_connmgr(int argc, char *argv[]);

#endif  //_CONNMGR_H_
