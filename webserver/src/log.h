/* Header file
 *
 * Includes headers important for logging functions
 */

#pragma once

#include "httpd.h"

#define LOG_CRITICAL  0
#define LOG_ERROR     1
#define LOG_WARN      2
#define LOG_INFO      3
#define LOG_DEBUG     4

void accesslog(struct configuration *config, const struct sockaddr *addr, char* request, int code, int byte);
void serverlog(struct configuration *config, int error, char* errorMessage);
