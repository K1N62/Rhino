
/* Request header file
 *
 * Handles requests
 */

#pragma once

#include "httpd.h"

#define REQ_BUFSIZE 512

struct rqhdArgs {
  int sd;
  struct sockaddr_in pin;
};

void *requestHandle(void *context);
