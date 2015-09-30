
#include "request.h"

/* Send Line	(Private)
 *
 * Sends one line to a socket
 *
 * @sd          int, socket ID
 * @tmp         char, pointer ro char array to print, have to be terminated with newline
                      \r\n and also nullbyte \0
 * @return      int, -1 if error
 */
int sendLine(int sd, char *tmp)
{
	return send(sd, tmp, strlen(tmp)+2, 0) == -1 ? -1 : 0;
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
    DIE_CON
  }

  // Check if request follows standard

	if (startsWith("GET /index.html", reqBuf)) {
		// Open the file
		int fd = open("../www/index.html", O_RDONLY);
		if (fd == -1) {
			printf("ERROR: Unable to open requested file\n");
			DIE_CON
		}

		// Get the file size
		struct stat stat_buf;
		fstat(fd, &stat_buf);

	 	// HEADER -------------------------------------
		char tmp[128];
		char date[64];
		time_t t = time(NULL);
		memset(tmp, '\0', sizeof(tmp));
		memset(date, '\0', sizeof(date));

		// Status
		if(sendLine(sd, "HTTP/1.0 200 OK\r\n") == -1) {
			DIE_CON
		}

		// Server
		if(sendLine(sd, "Server: webserver\r\n") == -1) {
			DIE_CON
		}

		// Content-length
		sprintf(tmp, "Content-Length: %d\r\n", stat_buf.st_size);
		if(sendLine(sd, tmp) == -1) {
			DIE_CON
		}

		// Content-Type
		strcpy(tmp, "Content-Type: text/html\r\n");
		if(sendLine(sd, tmp) == -1) {
			DIE_CON
		}

		// Content-Encoding
		strcpy(tmp, "Content-Encoding: UTF-8\r\n");
		if(sendLine(sd, tmp) == -1) {
			DIE_CON
		}

		// Last Modified
		strftime(date, sizeof(date), "%a, %d %b %Y %T %z", localtime(&stat_buf.st_ctime));
		sprintf(tmp, "Last-Modified: %s\r\n", date);
		if(sendLine(sd, tmp) == -1) {
			DIE_CON
		}

		// Date
		strftime(date, sizeof(date), "%a, %d %b %Y %T %z", localtime(&t));
		sprintf(tmp, "Date: %s\r\n", date);
		if(sendLine(sd, tmp) == -1) {
			DIE_CON
		}

		// -------------------------------------------------------------------------

		// Send the requested file
		if(sendfile(sd, fd, NULL , stat_buf.st_size) == -1)	{
			printf("ERROR: Unable to send requested file, %s\n", strerror(errno));
			DIE_CON
		}

		// Log
		//log_access(sd, reqBuf, "200", stat_buf.st_size);

	}
	else {
		if (sendLine(sd, "Unable to handle this shit") == -1) {
			DIE_CON
		}
	}

  DIE_CON
}
