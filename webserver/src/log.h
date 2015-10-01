
/* Log Header
 *
 *  Declares functions and macros for the log functions
 */

#pragma once

#include "httpd.h"

#define LOG_CRITICAL  0
#define LOG_ERROR     1
#define LOG_WARN      2
#define LOG_INFO      3
#define LOG_DEBUG     4

FILE *_log_access_fd,
     *_log_server_fd;

void log_init(struct configuration *config);

void log_destroy();

void log_access(const struct sockaddr_in *addr, char *request, char *statusCode, int bytes);

void log_server(int error, char *errorMessage);
