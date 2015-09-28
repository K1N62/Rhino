
/* Request header file
 *
 * Handles requests
 */

#pragma once

#include "httpd.h"

#define REQ_BUFSIZE 512

void requestHandle(int sd, struct sockaddr_in pin);
