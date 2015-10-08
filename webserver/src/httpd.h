
/* Header file
 *
 * Includes headers important for the project
 */

#pragma once

#define _NAME     "Rhino"
#define _VERSION  "1.0"
#define _HTTP_VER "1.0"

#define _THREAD   0
#define _PREFORK  1
#define _NUMBER_OF_CHILDREN 15

#include <arpa/inet.h>
#include <fcntl.h>
#include <limits.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <pwd.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/resource.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>

// Global lock
pthread_mutex_t thread_lock;

#include "structures.h"
#include "globalConfigVariables.h"
#include "config.h"
#include "pathHandler.h"
#include "log.h"
#include "utility.h"
#include "request.h"
