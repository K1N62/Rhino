
/* Webserver
 *
 * Authors, Jim Ahlstrand & Lukas Landenstad
 * Copyright 2015
 */

#include "httpd.h"

int main(int argc, char* argv[]) {

  int i, port, sd, sd_current, addrlen;
  struct sockaddr_in sin, pin;
	char reqBuf[512];

  // Set default config
  struct configuration config;
  setDefaultConfig(&config);

  // Parse config file
  parseConfig(&config);

  if(argc > 1)
  {
    for(i = 1; i < argc; i++)
    {
      switch(argv[i][1])
      {
        case 'h':
          printf("Help not implemented.. yet\n");
          break;
        case 'p':
          i++; &sin, sizeof(sin)) == -1) {
		printf("ERROR: Unable to bind socket\n");
		exit(-1);
          if(i >= argc)
          {
            printf("ERROR: WRONG USAGE\n");
            return 0;
          }
          if(argv[i][0] != '-')
          {
            if((port = atoi(argv[i])) != 0 && port < 65536)
            {
              config.listenPort = port;
              printf("Port number: %d\n", port);
            }
            else
            {
              printf("ERROR: WRONG USAGE\n");
              return 0;
            }
          }
          else
          {
            printf("ERROR: WRONG USAGE\n");
            return 0;
          }
          break;
        case 'd':
          // Start daemon if set
          printf("Starting daemon...\n");
          daemonfunc("daemon");
          break;
        case 'l':
          printf("Starting logging..\n");
          break;
        case 's':
          i++;
          if(i >= argc)
          {
            printf("ERROR: WRONG USAGE\n");
            return 0;
          }
          if(argv[i][0] != '-')
            printf("Starting: %s as selected request handling method\n", argv[i]);
          else
          {
            printf("ERROR: WRONG USAGE\n");
            return 0;
          }
          break;
      }
    }
  }

  // Create listening socket
  // Domain -> AF_INET = IPV4
  // Type -> SOCK_STREAM = TCP
  if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
  		printf("ERROR: Unable to open socket\n");
  		exit(-1);
  }

  // Zeroize sin
  memset(&sin, 0, sizeof(sin));
  // Set domain
  sin.sin_family = AF_INET;
  // Set any in address
  sin.sin_addr.s_addr = INADDR_ANY;
  // Set port, hton converts byteorder
  sin.sin_port = htons(config.listenPort);

  // Try binding the socket
	if(bind(sd, (struct sockaddr*) &sin, sizeof(sin)) == -1) {
		printf("ERROR: Unable to bind socket\n");
		exit(-1);
	}

  // Start to listen for requests
  if(listen(sd, config.backlog) == -1) {
		printf("ERROR: Too loud unable to listen\n");
		exit(-1);
	}

  // Start accepting requests
  addrlen = sizeof(pin);
  while(true)
  {
    // Accept a request from queue, blocking
    if ((sd_current = accept(sd, (struct sockaddr*) &pin, (socklen_t*) &addrlen)) == -1) {
  		printf("ERROR: Unable to accept request, OS to greedy\n");
  		exit(-1);
  	}
    // Recieve the data, thank you
    if (recv(sd_current, reqBuf, sizeof(reqBuf), 0) == -1) {
  		printf("ERROR: Unable to recieve request, I'm too humble\n");
  		exit(-1);
  	}

  	printf("Request from %s:%i\n", inet_ntoa(pin.sin_addr), ntohs(pin.sin_port));
  	printf("Message: %s\n", reqBuf);

  }

  close(sd_current);
	close(sd);

// Accept requests
// Check if request follows standard        code 400, 403
// Use selected request handling method
//

  return 0;
}
