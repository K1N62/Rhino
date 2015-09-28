
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
  if (recv(sd, reqBuf, sizeof(reqBuf), 0) == -1) {
    printf("ERROR: Unable to recieve request\n");
    exit(-1);
  }

  // Check if request follows standard        code 400, 403

  printf("Message: %s\n", reqBuf);

  // Send a response
	if(send(sd, reqBuf, strlen(reqBuf) + 1, 0) == -1) {
		printf("ERROR: Unable to send response\n");
		exit(-1);
	}

  close(sd);
}
