
/* Log Header
 *
 *  Declares functions and macros for the log functions
 */

#pragma once

#include "httpd.h"

FILE *_log_access_fd,
     *_log_server_fd;

int log_init(struct configuration *config);

bool logToSyslog;

void log_destroy();

void log_access(const struct sockaddr_in *addr, const struct _rqhd_req *request, const struct _rqhd_header *header);

void log_server(int error, char *errorMessage);
