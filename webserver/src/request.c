
#include "request.h"

void *requestHandle(void *context)
{
	// Get the arguments
	struct rqhdArgs *args = context;
	int sd = args->sd;
	struct sockaddr_in pin = args->pin;
	char reqBuf[REQ_BUFSIZE];

  printf("Request from %s:%i\n", inet_ntoa(pin.sin_addr), ntohs(pin.sin_port));

  // Recieve the data, thank you
  if (recv(sd, reqBuf, sizeof(reqBuf), 0) == -1)
	{
    printf("ERROR: Unable to recieve request\n");
    DIE_CON
  }

  // Check if request follows standard        code 400, 403

	if (startsWith("GET /index.html", reqBuf))
	{
		// Open the file
		int fd = open("../www/index.html", O_RDONLY);
		if (fd == -1)
		{
			printf("ERROR: Unable to open requested file\n");
			DIE_CON
		}

		// Get the file size
		struct stat stat_buf;
		fstat(fd, &stat_buf);

		// Send the file
		if(sendfile(sd, fd, NULL , stat_buf.st_size) == -1) {
			printf("ERROR: Unable to send requested file, %s\n", strerror(errno));
			DIE_CON
		}
	}
	else
	{
		if(send(sd, "500 Unable", 11, 0) == -1) {
			printf("ERROR: Unable to send response\n");
			DIE_CON
		}
	}

  DIE_CON
}
