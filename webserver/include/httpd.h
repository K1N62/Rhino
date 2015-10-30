
/* Header file
 *
 * Includes headers important for the project
 */

#ifndef _HTTPD_HEADER_
#define _HTTPD_HEADER_
#pragma once

#define _SERVER_NAME     "Rhino"
#define _SERVER_VERSION  "1.1"
#define _SERVER_HTTP_VER "1.0"

#ifndef _THREAD
#define _THREAD   0
#endif

#ifndef _FORK
#define _FORK     1
#endif

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

#endif
