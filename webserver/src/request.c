
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

	return 0;
}

void *requestHandle(void *context)
{
	// Get the arguments
	struct _rqhd_args *args = context;
	int sd = args->sd;
	struct sockaddr_in pin = args->pin;
	char reqBuf[REQ_BUFSIZE];


  printf("Request from %s:%i\n", inet_ntoa(pin.sin_addr), ntohs(pin.sin_port));

  // Recieve the data, thank you
  if (recv(sd, reqBuf, sizeof(reqBuf), 0) == -1) {
    printf("ERROR: Unable to recieve request\n");
    close(sd); free(args); pthread_exit(NULL);
  }

  // Check if request follows standard

	if (startsWith("GET /index.html", reqBuf)) {
		// Open the file
		FILE *reqFile = fopen("../www/index.html", "r");
		if (reqFile == NULL) {
			printf("ERROR: Unable to open requested file\n");
			DIE_CON
		}

		// Get the file size
		struct stat stat_buf;
		fstat(fileno(reqFile), &stat_buf);

	 	// HEADER -------------------------------------
		char date[64];
		memset(date, '\0', sizeof(date));
		struct _rqhd_header head;

		// Server name
		head.server 	= "SkyNet@Ur.service";
		// Protocol
		head.protocol = "HTTP/1.0";
		// Status code
		head.status 	= "200 OK";
		// Type
		head.type 		= "text/html";
		// Size
		head.size			= (int)stat_buf.st_size;
		// Cache
		head.cache 		= "public";
		// Last-Modified
		strftime(date, sizeof(date), "%a, %d %b %Y %T %z", localtime(&stat_buf.st_ctime));
		head.modified	= date;

		// Send header
		if (sendHeader(sd, &head) == -1) {
			printf("ERROR: Unable to send header\n");
			DIE_CON
		}
		// -------------------------------------------------------------------------

		// Send the requested file
		if(sendfile(sd, fileno(reqFile), NULL , stat_buf.st_size) == -1)	{
			printf("ERROR: Unable to send requested file, %s\n", strerror(errno));
			DIE_CON
		}

		// Log
		log_access(&pin, reqBuf, "200", stat_buf.st_size);

		// Close requested file
		fclose(reqFile);
	}
	else {
		if (sendLine(sd, "Unable to handle this shit") == -1) {
			close(sd); free(args); pthread_exit(NULL);
		}
	}

  close(sd); free(args); pthread_exit(NULL);
}
