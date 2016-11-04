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
int sendHeader(int sd, const _rqhd_header *head)
{
	char buffert[4096];
	char tmp[256];
	char date[64];
	time_t t = time(NULL);
	memset(tmp, '\0', sizeof(tmp));
	memset(date, '\0', sizeof(date));
	memset(buffert, '\0', sizeof(buffert));

	// Header start
	// Status
	if (head->protocol != NULL && head->status != NULL){
		// Build string
		snprintf(tmp, sizeof(tmp), "%s %s\r\n", head->protocol, head->status);
		strcat(buffert, tmp);
	} else {
		log_server(LOG_INFO, "Status header was not sent! Was this intentional?");
	}
	// Server
	if (head->server != NULL) {
		// Build string
		snprintf(tmp, sizeof(tmp), "Server: %s\r\n", head->server);
		strcat(buffert, tmp);
	} else {
		log_server(LOG_INFO, "Server header was not sent! Was this intentional?");
	}
	// Content-Length
	if (head->size != 0) {
		// Build string
		snprintf(tmp, sizeof(tmp), "Content-Length: %d\r\n", head->size);
		strcat(buffert, tmp);
	} else {
		log_server(LOG_INFO, "Content-Length header was not sent! Was this intentional?");
	}
	// Content-Type
	if (head->type != NULL) {
		// Build string
		snprintf(tmp, sizeof(tmp), "Content-Type: %s\r\n", head->type);
		strcat(buffert, tmp);
	} else {
		log_server(LOG_INFO, "Content-Type header was not sent! Was this intentional?");
	}
	// Cache-Control
	if (head->cache != NULL) {
		// Build string
		snprintf(tmp, sizeof(tmp), "Cache-Control: %s\r\n", head->cache);
		strcat(buffert, tmp);
	} else {
		log_server(LOG_INFO, "Cache-Control header was not sent! Was this intentional?");
	}
	// Last-Modified
	if (head->cache != NULL) {
		// Build string
		snprintf(tmp, sizeof(tmp), "Last-Modified: %s\r\n", head->modified);
		strcat(buffert, tmp);
	} else {
		log_server(LOG_INFO, "Last-Modified header was not sent! Was this intentional?");
	}
	// Date
	// Get date
	strftime(date, sizeof(date), "%a, %d %b %Y %T %z", localtime(&t));
	// Build string
	snprintf(tmp, sizeof(tmp), "Date: %s\r\n", date);
	strcat(buffert, tmp);

	// End of header
	strcat(buffert, "\r\n");

	if(sendLine(sd, buffert) == -1)
		return -1;
	return 0;
}

void *requestHandle(void *context)
{

	// Get the arguments
	char 	buffert[PATH_MAX] = {0},
				reqBuf[BUF_REQ] = {0},
				date[64] = {0},
				error[1024] = {0},
				*req_line = NULL,
				*req_token = NULL;

	_rqhd_header 	head;
	_rqhd_req 		req;
	_rqhd_args 		*args = (_rqhd_args*) context;

	struct 	sockaddr_in pin = args->pin;
	struct 	stat stat_buf = {0};
	int		sd = args->sd;
	FILE 	*reqFile = NULL;

	// Init variables
	head.protocol[0] 	= '\0';
	head.status[0] 		= '\0';
	head.server[0] 		= '\0';
	head.type[0] 		= '\0';
	head.cache[0] 		= '\0';
	head.modified[0] 	= '\0';
	head.size 			= 0;
	req.method[0] 		= '\0';
	req.uri[0] 			= '\0';
	req.protocol[0] 	= '\0';
	req.error			= false;

  // Recieve the data, thank you
  if (recv(sd, reqBuf, sizeof(reqBuf), 0) == -1) {
		snprintf(error, sizeof(error), "Unable to recieve request, %s", strerror(errno));
		log_server(LOG_ERR, error);
		// Cleanup
    close(sd);
		free(args);
		return NULL;
  }

	// PARSE REQUEST -------------------------------------
	// First line is the actual request
	req_line = strtok(reqBuf, "\r\n");

	// Check if GET or HEAD is set
	req_token = strtok(req_line, " ");
	if (req_token != NULL) {
		if (strncmp(req_token, "GET", 3) == 0 || strncmp(req_token, "HEAD", 4) == 0) {
			strncpy(req.method, req_token, BUF_VAL);

			// Get uri
			req_token = strtok(NULL, " ");
			if (req_token != NULL) {
				strncpy(req.uri, req_token, BUF_VAL);

				/* Dosent matter
				// Get Protocol
				req_token = strtok(NULL, " ");
				if (req_token != NULL) {
					// Don't bother with the protocol
					strncpy(req.protocol, req_token, BUF_VAL);

				} else {
					// If no protocol set 400 Bad Request
					strncpy(head.status, "400 Bad Request", sizeof(head.status));
					req.error = true;
					strncpy(req.path, "/errpg/400.html", sizeof(req.path));
				}*/
				strncpy(req.protocol, "HTTP/1.0", BUF_VAL);
			} else {
				// If no uri set 400 Bad Request
				strncpy(head.status, "400 Bad Request", sizeof(head.status));
				req.error = true;
				strncpy(req.path, "/errpg/400.html", sizeof(req.path));
			}
		} else {
			// If invalid method set 501 Not Implemented
			strncpy(head.status, "501 Not Implemented", sizeof(head.status));
			req.error = true;
			strncpy(req.path, "/errpg/501.html", sizeof(req.path));
		}
	} else {
		// If no method set 400 Bad Request
		strncpy(head.status, "400 Bad Request", sizeof(head.status));
		req.error = true;
		strncpy(req.path, "/errpg/400.html", sizeof(req.path));
	}

	// If '/' was only character in uri set index
	if (strncmp(req.uri, "/", 1) == 0) {
		strncpy(req.path, "/index.html", sizeof(req.path));
	}
	// Else set the requested path unless the request is bad
	else if(!req.error) {
		strncpy(req.path, req.uri, sizeof(req.path));
	}

	// Check if file exists with realpath
	if (realpath(req.path, buffert) == NULL) {
		// If file does not exists
		snprintf(error, sizeof(error), "%s was not found, sending error page instead", req.path);
		log_server(LOG_INFO, error);
		strncpy(head.status, "404 Not Found", sizeof(head.status));
		req.error = true;
		// Load error page instead
		strncpy(req.path, "/errpg/404.html", sizeof(req.path));
		realpath(req.path, buffert);
	}
	// If file exists and there was no error set status to 200
	else if(!req.error) {
		strncpy(head.status, "200 OK", sizeof(head.status));
	}

	// Open the file
	if ((reqFile = fopen(buffert, "r")) == NULL) {
		// If we can't open the file send 500
		req.error = true;
		strncpy(head.status, "500 Internal Server Error", sizeof(head.status));
		strncpy(head.protocol, "HEAD", sizeof(head.protocol));
	}

	// Get the file size
	if (reqFile != NULL) {
		fstat(fileno(reqFile), &stat_buf);
	}

	// HEADER -------------------------------------
	// Server name
	strncpy(head.server, _SERVER_NAME" "_SERVER_VERSION, sizeof(head.server));
	// Protocol
	strncpy(head.protocol, "HTTP/"_SERVER_HTTP_VER, sizeof(head.protocol));
	// Type
	if (strncmp(getExt(req.uri), ".png", 4) == 0) {
		strncpy(head.type, "image/png", sizeof(head.type));
	} else if (strncmp(getExt(req.uri), ".jpg", 4) == 0) {
		strncpy(head.type, "image/jpg", sizeof(head.type));
	} else if (strncmp(getExt(req.uri), ".gif", 4) == 0) {
		strncpy(head.type, "image/gif", sizeof(head.type));
	} else if (strncmp(getExt(req.uri), ".css", 4) == 0) {
		strncpy(head.type, "text/css", sizeof(head.type));
	} else if (strncmp(getExt(req.uri), ".js", 3) == 0) {
		strncpy(head.type, "application/javascript", sizeof(head.type));
	} else if (strncmp(getExt(req.uri), ".html", 5) == 0) {
		strncpy(head.type, "text/html", sizeof(head.type));
	}
	// Size
	head.size	= (int)stat_buf.st_size;
	// Cache
	strncpy(head.cache, "public", sizeof(head.cache));
	// Last-Modified
	strftime(date, sizeof(date), "%a, %d %b %Y %T %z", localtime(&stat_buf.st_ctime));
	strncpy(head.modified, date, sizeof(head.modified));

	// Send header
	if (sendHeader(sd, &head) == -1) {
		snprintf(error, sizeof(error), "Unable to send header, %s. Aborting", strerror(errno));
		log_server(LOG_WARNING, error);
		DIE_CON
	}
	// -------------------------------------------------------------------------

	// Send the requested file if method is GET
	if (strncmp(req.method, "GET", 3) == 0) {
		if (sendfile(sd, fileno(reqFile), NULL, stat_buf.st_size) == -1)	{
      snprintf(error, sizeof(error), "Unable to send requested file, %s. Aborting", strerror(errno));
      log_server(LOG_WARNING, error);
			DIE_CON
		}
	}

	// Log
	log_access(&pin, &req, &head);

	// Cleanup
	DIE_CON
}
