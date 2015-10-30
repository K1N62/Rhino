
/* Request header file
 *
 * Handles requests
 */

#ifndef _REQUEST_HEADER_
#define _REQUEST_HEADER_
#pragma once

#include "httpd.h"

#ifndef BUF_REQ
#define BUF_REQ 1024
#endif

#define DIE_CON fclose(reqFile); \
                close(sd); \
                free(args); \
                return NULL;
//pthread_exit(NULL); libgcc required?

#define HTTP_OK               200
#define HTTP_BAD              400
#define HTTP_FORBIDDEN        403
#define HTTP_NOT_FOUND        404
#define HTTP_INTERNAL_ERROR   500
#define HTTP_NOT_IMPLEMENTED  501

/* Request Handle
 *
 * Handles http requests
 *
 * @context     void, pointer to a dynamic allocated rqhdArgs struct
 * @return      void
 */
void *requestHandle(void *context);

#endif
