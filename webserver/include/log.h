
/* Log Header
 *
 *  Declares functions and macros for the log functions
 */

#ifndef _LOG_HEADER_
#define _LOG_HEADER_
#pragma once

#include "httpd.h"

FILE *_log_access_fd,
     *_log_server_fd;

int log_init(configuration *config);

bool logToSyslog;

void log_destroy();

void log_access(const struct sockaddr_in *addr, const _rqhd_req *request, const _rqhd_header *header);

void log_server(int error, char *errorMessage);

#endif
