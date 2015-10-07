
/* Request header file
 *
 * Handles requests
 */

#pragma once

#include "httpd.h"

#define BUF_REQ 1024
#define DIE_CON fclose(reqFile); close(sd); free(args); return NULL;
//pthread_exit(NULL); libgcc required oO?

#define _REQ_OK   "200" // OK
#define _REQ_BAD  "400" // Bad Request
#define _REQ_FBN  "403" // Forbidden
#define _REQ_NFD  "404" // Not Found
#define _REQ_ISE  "500" // Internal Server Error
#define _REQ_NIM  "501" // Not Implemented

/* Request Handle
 *
 * Handles http requests
 *
 * @context     void, pointer to a rqhdArgs struct
 * @return      void
 */
void *requestHandle(void *context);
