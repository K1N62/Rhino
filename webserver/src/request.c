
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

void *requestHandle(void *context)
{
	// Get the arguments
	struct rqhdArgs *args = context;
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
		char tmp[128];
		char date[64];
		time_t t = time(NULL);
		memset(tmp, '\0', sizeof(tmp));
		memset(date, '\0', sizeof(date));

		// Status
		if(sendLine(sd, "\r\nHTTP/1.0 200 OK\r\n") == -1) {
			DIE_CON
		}

		// Server
		if(sendLine(sd, "Server: webserver\r\n") == -1) {
			DIE_CON
		}

		// Content-length
		memset(tmp, '\0', sizeof(tmp));
		sprintf(tmp, "Content-Length: %d\r\n", (int)stat_buf.st_size);
		if(sendLine(sd, tmp) == -1) {
			DIE_CON
		}

		// Content-Type
		memset(tmp, '\0', sizeof(tmp));
		strcpy(tmp, "Content-Type: text/html; charset=UTF-8\r\n");
		if(sendLine(sd, tmp) == -1) {
			DIE_CON
		}

		// Cache-control
		memset(tmp, '\0', sizeof(tmp));
		strcpy(tmp, "Cach-Control: public\r\n");
		if(sendLine(sd, tmp) == -1) {
			DIE_CON
		}

		// Last Modified
		memset(tmp, '\0', sizeof(tmp));
		strftime(date, sizeof(date), "%a, %d %b %Y %T %z", localtime(&stat_buf.st_ctime));
		sprintf(tmp, "Last-Modified: %s\r\n", date);
		if(sendLine(sd, tmp) == -1) {
			DIE_CON
		}

		// Date
		memset(tmp, '\0', sizeof(tmp));
		strftime(date, sizeof(date), "%a, %d %b %Y %T %z", localtime(&t));
		sprintf(tmp, "Date: %s\r\n", date);
		if(sendLine(sd, tmp) == -1) {
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
