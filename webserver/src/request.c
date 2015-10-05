/*
	TODO:
	Header errors
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
	return send(sd, tmp, strlen(tmp), 0) == -1 ? -1 : 0;
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
				*req_line,
				*req_token;
	struct _rqhd_header head;
	struct _rqhd_args *args = (struct _rqhd_args*) context;
	struct _rqhd_req req;
	struct configuration *config = args->config;
	struct sockaddr_in pin = args->pin;
	struct stat stat_buf;
	int 	sd = args->sd;
	memset(date, '\0', sizeof(date));
	FILE *reqFile;

  // Recieve the data, thank you
  if (recv(sd, reqBuf, sizeof(reqBuf), 0) == -1) {
    printf("ERROR: Unable to recieve request\n");
    close(sd); free(args); pthread_exit(NULL);
  }

	// PARSE REQUEST -------------------------------------
	// First line is the actual request
	req_line = strtok(reqBuf, "\r\n");
	req_token = strtok(req_line, " ");

	// Check if GET or HEAD is set
	if (req_token != NULL) {
		if (strcmp(req_token, "GET") || strcmp(req_token, "HEAD")) {
			strncpy(req.method, req_token, BUF_VAL);

			// Get uri
			req_token = strtok(NULL, " ");
			if (req_token != NULL) {
				strncpy(req.uri, req_token, BUF_VAL);

				// Get Protocol
				req_token = strtok(NULL, " ");
				if (req_token != NULL) {
					if (strcmp(req_token, "HTTP/"_HTTP_VER)) {
						strncpy(req.protocol, req_token, BUF_VAL);

					} else {
						// If invalid protocol set 400 Bad Request
						strncpy(head.status, "400 Bad Request", BUF_VAL);
						sprintf(buffert, "%s/%s", config->rootDir, "errpg/400.html");
						strncpy(req.uri, buffert, BUF_VAL);
					}
				} else {
					// If no protocol set 400 Bad Request
					strncpy(head.status, "400 Bad Request", BUF_VAL);
					sprintf(buffert, "%s/%s", config->rootDir, "errpg/400.html");
					strncpy(req.uri, buffert, BUF_VAL);
				}
			} else {
				// If no uri set 400 Bad Request
				strncpy(head.status, "400 Bad Request", BUF_VAL);
				sprintf(buffert, "%s/%s", config->rootDir, "errpg/400.html");
				strncpy(req.uri, buffert, BUF_VAL);
			}
		} else {
			// If invalid method set 501 Not Implemented
			strncpy(head.status, "501 Not Implemented", BUF_VAL);
			sprintf(buffert, "%s/%s", config->rootDir, "errpg/400.html");
			strncpy(req.uri, buffert, BUF_VAL);
		}
	} else {
		// If no method set 400 Bad Request
		strncpy(head.status, "400 Bad Request", BUF_VAL);
		sprintf(buffert, "%s/%s", config->rootDir, "errpg/400.html");
		strncpy(req.uri, buffert, BUF_VAL);
	}

	// JAIL HERE
	// Use realpath on uri
	// If '/' was only character in uri set index
	if (strlen(req.uri) == 1) {
		sprintf(buffert, "%s%s", config->basedir, "/index.html");
		strncpy(req.uri, buffert, BUF_VAL);
	}
	// Else set the requested file
	else {
		sprintf(buffert, "%s%s", config->basedir, req.uri);
		strncpy(req.uri, buffert, BUF_VAL);
	}

	// Get realpath
	/*if (realpath(req.uri, buffert) == NULL) {
		// If invalid path set 403 Forbidden
		head.status 	= "403 Forbidden";
		req.uri				= "errpg/403.html";
	}*/

	// Open the file
	if ((reqFile = fopen(buffert, "r")) == NULL) {
		log_server(LOG_INFO, "The requested pages was not found");
		strncpy(head.status, "404 Not Found", BUF_VAL);
		strncpy(req.uri, "errpg/404.html", BUF_VAL);
		sprintf(buffert, "%s/%s", config->rootDir, req.uri);
		if ((reqFile = fopen(buffert, "r")) == NULL) {
			log_server(LOG_WARN, "The error page could not be found");
		}
	}
	else {
		strncpy(head.status, "200 OK", BUF_VAL);
	}

	// Get the file size
	fstat(fileno(reqFile), &stat_buf);

	// HEADER -------------------------------------
	// Server name
	strncpy(head.server, _NAME" "_VERSION, BUF_VAL);
	// Protocol
	strncpy(head.protocol, "HTTP/"_HTTP_VER, BUF_VAL);
	// Type
	strncpy(head.type, "text/html", BUF_VAL);
	// Size
	head.size	= (int)stat_buf.st_size;
	// Cache
	strncpy(head.cache, "public", BUF_VAL);
	// Last-Modified
	strftime(date, sizeof(date), "%a, %d %b %Y %T %z", localtime(&stat_buf.st_ctime));
	strncpy(head.modified, date, BUF_VAL);

	// Send header
	if (sendHeader(sd, &head) == -1) {
		printf("ERROR: Unable to send header\n");
		DIE_CON
	}
	// -------------------------------------------------------------------------

	// Send the requested file if method is GET
	if (strcmp(req.method, "GET") == 0) {
		if (sendfile(sd, fileno(reqFile), NULL, stat_buf.st_size) == -1)	{
			printf("ERROR: Unable to send requested file, %s\n", strerror(errno));
			DIE_CON
		}
	}

	// Log
	log_access(&pin, &req, &head);

	// Cleanup
	fclose(reqFile);
  close(sd);
	free(args);
	pthread_exit(NULL);
}
