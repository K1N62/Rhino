
/* Header file
 *
 * Includes headers important for the project
 */

#pragma once

#define DIE_CLEANUP pthread_attr_destroy(&att); close(sd_current); close(sd); exit(-1);

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <sys/errno.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <time.h>

#include "globalConfigVariables.h"
#include "config.h"
#include "utility.h"
#include "request.h"
