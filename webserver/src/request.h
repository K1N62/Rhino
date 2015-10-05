
/* Request header file
 *
 * Handles requests
 */

#pragma once

#include "httpd.h"

#define BUF_REQ 1024
#define BUF_VAL 256
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
  struct configuration *config;
};

struct _rqhd_header {
  char protocol[BUF_VAL];
  char status[BUF_VAL];
  char server[BUF_VAL];
  char type[BUF_VAL];
  char cache[BUF_VAL];
  char modified[BUF_VAL];
  int   size;
};

struct _rqhd_req_head {
  char host[BUF_VAL];
  char userAgent[BUF_VAL];
  char accept[BUF_VAL];
  char referer[BUF_VAL];
};

struct _rqhd_req {
  char method[BUF_VAL];
  char uri[BUF_VAL];
  char protocol[BUF_VAL];
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
