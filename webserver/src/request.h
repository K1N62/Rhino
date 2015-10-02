
/* Request header file
 *
 * Handles requests
 */

#pragma once

#include "httpd.h"

#define REQ_BUFSIZE 512
#define DIE_CON fclose(reqFile); close(sd); free(args); pthread_exit(NULL);

#define _REQ_OK   "200" // OK
#define _REQ_BAD  "400" // Bad Request
#define _REQ_FBN  "403" // Forbidden
#define _REQ_NFD  "404" // Not Found
#define _REQ_ISE  "500" // Internal Server Error
#define _REQ_NIM  "501" // Not Implemented

struct _rqhd_args {
  int sd;
  struct sockaddr_in pin;
};

struct _rqhd_header {
  char *protocol;
  char *status;
  char *server;
  char *type;
  char *cache;
  char *modified;
  int   size;
};

struct _rqhd_req_head {
  char *host;
  char *userAgent;
  char *accept;
  char *referer;
};

struct _rqhd_req {
  char *method;
  char *uri;
  char *protocol;
  struct _rqhd_req_head *head;
};

/* Request Handle
 *
 * Handles http requests
 *
 * @context     void, pointer to a rqhdArgs struct
 * @return      void
 */
void *requestHandle(void *context);
