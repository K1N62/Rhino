/*
	TODO:
	check for basedir minimum
*/
#include "request.h"

/* Send Line	(Private)
 *
 * Sends one line to a socket
 *
 * @sd          int, socket ID
 * @tmp         char, pointer ro char array to print, have to be terminated with newline
                      \n and also nullbyte \0
 * @return      int, -1 if error
 */
int sendLine(int sd, char *tmp)
{
	return send(sd, tmp, strlen(tmp), MSG_NOSIGNAL) == -1 ? -1 : 0;
}

/* Send Header	(Private)
 *
 * Sends a header with the information from the rqhd header struct
 *
 * @sd				int, socket id
 * @rqhd			struct, see header file for information
 * @return		int, -1 if error
 */
int sendHeader(int sd, const struct _rqhd_header *head)
{
	char tmp[128];
	char date[64];
	time_t t = time(NULL);
	memset(tmp, '\0', sizeof(tmp));
	memset(date, '\0', sizeof(date));

	// Send header start
	if( sendLine(sd, "\r\n") == -1 )
		return -1;

	// Status
	if (head->protocol != NULL && head->status != NULL){
		// Build string
		sprintf(tmp, "%s %s\r\n", head->protocol, head->status);
		// Send it
		if(sendLine(sd, tmp) == -1)
			return -1;
	} else {
		log_server(LOG_INFO, "Status header was not sent! Was this intentional?");
	}
	// Server
	if (head->server != NULL) {
		// Build string
		sprintf(tmp, "Server: %s\r\n", head->server);
		// Send it
		if(sendLine(sd, tmp) == -1)
			return -1;
	} else {
		log_server(LOG_INFO, "Server header was not sent! Was this intentional?");
	}
	// Content-Length
	if (head->size != 0) {
		// Build string
		sprintf(tmp, "Content-Length: %d\r\n", head->size);
		// Send it
		if(sendLine(sd, tmp) == -1)
			return -1;
	} else {
		log_server(LOG_INFO, "Content-Length header was not sent! Was this intentional?");
	}
	// Content-Type
	if (head->type != NULL) {
		// Build string
		sprintf(tmp, "Content-Type: %s\r\n", head->type);
		// Send it
		if(sendLine(sd, tmp) == -1)
			return -1;
	} else {
		log_server(LOG_INFO, "Content-Type header was not sent! Was this intentional?");
	}
	// Cache-Control
	if (head->cache != NULL) {
		// Build string
		sprintf(tmp, "Cache-Control: %s\r\n", head->cache);
		// Send it
		if(sendLine(sd, tmp) == -1)
			return -1;
	} else {
		log_server(LOG_INFO, "Cache-Control header was not sent! Was this intentional?");
	}
	// Last-Modified
	if (head->cache != NULL) {
		// Build string
		sprintf(tmp, "Last-Modified: %s\r\n", head->modified);
		// Send it
		if(sendLine(sd, tmp) == -1)
			return -1;
	} else {
		log_server(LOG_INFO, "Last-Modified header was not sent! Was this intentional?");
	}
	// Date
	// Get date
	strftime(date, sizeof(date), "%a, %d %b %Y %T %z", localtime(&t));
	// Build string
	sprintf(tmp, "Date: %s\r\n", date);
	// Send it
	if(sendLine(sd, tmp) == -1)
		return -1;
	// End of header
	sprintf(tmp, "\r\n");
	if(sendLine(sd, tmp) == -1)
		return -1;
	return 0;
}

void *requestHandle(void *context)
{
	// Get the arguments
	char 	buffert[PATH_MAX],
				reqBuf[BUF_REQ],
				date[64],
				error[1024],
				*req_line,
				*req_token;
	struct _rqhd_header head;
	struct _rqhd_args *args = (struct _rqhd_args*) context;
	struct _rqhd_req req;
	struct sockaddr_in pin = args->pin;
	struct stat stat_buf;
	int 	sd = args->sd;
	FILE *reqFile;

	// Init variables
	head.protocol[0] 	= '\0';
	head.status[0] 		= '\0';
	head.server[0] 		= '\0';
	head.type[0] 			= '\0';
	head.cache[0] 		= '\0';
	head.modified[0] 	= '\0';
	head.size 				= 0;
	req.method[0] 		= '\0';
	req.uri[0] 				= '\0';
	req.protocol[0] 	= '\0';
	req.error					= false;

  // Recieve the data, thank you
  if (recv(sd, reqBuf, sizeof(reqBuf), 0) == -1) {
		sprintf(error, "Unable to recieve request, %s", strerror(errno));
		log_server(LOG_ERROR, error);
		// Cleanup
    close(sd);
		free(args);
		pthread_exit(NULL);
  }

	// PARSE REQUEST -------------------------------------
	// First line is the actual request
	req_line = strtok(reqBuf, "\r\n");

	// Check if GET or HEAD is set
	req_token = strtok(req_line, " ");
	if (req_token != NULL) {
		if (strcmp(req_token, "GET") == 0 || strcmp(req_token, "HEAD") == 0) {
			strncpy(req.method, req_token, BUF_VAL);

			// Get uri
			req_token = strtok(NULL, " ");
			if (req_token != NULL) {
				strncpy(req.uri, req_token, BUF_VAL);

				// Get Protocol
				req_token = strtok(NULL, " ");
				if (req_token != NULL) {
					// Don't bother with the protocol
					strncpy(req.protocol, req_token, BUF_VAL);

				} else {
					// If no protocol set 400 Bad Request
					strncpy(head.status, "400 Bad Request", BUF_VAL);
					req.error = true;
					strncpy(req.path, "/errpg/400.html", BUF_VAL);
				}
			} else {
				// If no uri set 400 Bad Request
				strncpy(head.status, "400 Bad Request", BUF_VAL);
				req.error = true;
				strncpy(req.path, "/errpg/400.html", BUF_VAL);
			}
		} else {
			// If invalid method set 501 Not Implemented
			strncpy(head.status, "501 Not Implemented", BUF_VAL);
			req.error = true;
			strncpy(req.path, "/errpg/501.html", BUF_VAL);
		}
	} else {
		// If no method set 400 Bad Request
		strncpy(head.status, "400 Bad Request", BUF_VAL);
		req.error = true;
		strncpy(req.path, "/errpg/400.html", BUF_VAL);
	}

	// If '/' was only character in uri set index
	if (strcmp(req.uri, "/") == 0) {
		strncpy(req.path, "/index.html", BUF_VAL);
	}
	// Else set the requested path unless the request is bad
	else if(!req.error) {
		strncpy(req.path, req.uri, BUF_VAL);
	}

	// Check if file exists with realpath
	if (realpath(req.path, buffert) == NULL) {
		// If file does not exists
		sprintf(error, "%s was not found, sending error page instead", req.path);
		log_server(LOG_INFO, error);
		strncpy(head.status, "404 Not Found", BUF_VAL);
		req.error = true;
		// Load error page instead
		strncpy(req.path, "/errpg/404.html", BUF_VAL);
	}
	// If file exists and there was no error set status to 200
	else if(!req.error) {
		strncpy(head.status, "200 OK", BUF_VAL);
	}

	// Open the file
	reqFile = fopen(buffert, "r");

	// Get the file size
	fstat(fileno(reqFile), &stat_buf);

	// HEADER -------------------------------------
	// Server name
	strncpy(head.server, _NAME" "_VERSION, BUF_VAL);
	// Protocol
	strncpy(head.protocol, "HTTP/"_HTTP_VER, BUF_VAL);
	// Type
	if (strcmp(getExt(req.uri), ".png") == 0) {
		strncpy(head.type, "image/png", BUF_VAL);
	} else if (strcmp(getExt(req.uri), ".jpg") == 0) {
		strncpy(head.type, "image/jpg", BUF_VAL);
	} else if (strcmp(getExt(req.uri), ".css") == 0) {
		strncpy(head.type, "text/css", BUF_VAL);
	} else if (strcmp(getExt(req.uri), ".js") == 0) {
		strncpy(head.type, "application/javascript", BUF_VAL);
	} else {
		strncpy(head.type, "text/html", BUF_VAL);
	}
	// Size
	head.size	= (int)stat_buf.st_size;
	// Cache
	strncpy(head.cache, "public", BUF_VAL);
	// Last-Modified
	strftime(date, sizeof(date), "%a, %d %b %Y %T %z", localtime(&stat_buf.st_ctime));
	strncpy(head.modified, date, BUF_VAL);

	// Send header
	if (sendHeader(sd, &head) == -1) {
		sprintf(error, "Unable to send header, %s", strerror(errno));
		log_server(LOG_ERROR, error);
		DIE_CON
	}
	// -------------------------------------------------------------------------

	// Send the requested file if method is GET
	if (strcmp(req.method, "GET") == 0) {
		if (sendfile(sd, fileno(reqFile), NULL, stat_buf.st_size) == -1)	{
      sprintf(error, "Unable to send requested file, %s", strerror(errno));
      log_server(LOG_ERROR, error);
			DIE_CON
		}
	}

	// Log
	log_access(&pin, &req, &head);

	// Cleanup
	DIE_CON
}
