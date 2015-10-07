
/* Header file
 *
 * Includes headers important for the project
 */

#pragma once

#define _NAME     "Rhino"
#define _VERSION  "1.0"
#define _HTTP_VER "1.0"

#define DIE_CLEANUP pthread_attr_destroy(&att); pthread_mutex_destroy(&thread_lock); close(sd_current); close(sd); log_destroy(); exit(-1);

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <sys/errno.h>
#include <stdbool.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <limits.h>
#include <time.h>

// Global lock
pthread_mutex_t thread_lock;

#include "structures.h"
#include "globalConfigVariables.h"
#include "config.h"
#include "pathHandler.h"
#include "log.h"
#include "utility.h"
#include "request.h"
