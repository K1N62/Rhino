
/* Request header file
 *
 * Handles requests
 */

/*
Status-Code    = "200"   ; OK
                | "201"   ; Created
                | "202"   ; Accepted
                | "204"   ; No Content
                | "301"   ; Moved Permanently
                | "302"   ; Moved Temporarily
                | "304"   ; Not Modified
                | "400"   ; Bad Request
                | "401"   ; Unauthorized
                | "403"   ; Forbidden
                | "404"   ; Not Found
                | "500"   ; Internal Server Error
                | "501"   ; Not Implemented
                | "502"   ; Bad Gateway
                | "503"   ; Service Unavailable
                | extension-code
*/

#pragma once

#include "httpd.h"

#define REQ_BUFSIZE 512
#define DIE_CON fclose(reqFile); close(sd); free(args); pthread_exit(NULL);

struct rqhdArgs {
  int sd;
  struct sockaddr_in pin;
};

/* Request Handle
 *
 * Handles http requests
 *
 * @context     void, pointer to a rqhdArgs struct
 * @return      void
 */
void *requestHandle(void *context);
