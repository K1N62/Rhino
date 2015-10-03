
/* Webserver
 *
 * Authors, Jim Ahlstrand & Lukas Landenstad
 * Copyright 2015

 TODO:
  * Realpath så i kan förfråga valfri fil
  * Set base dir, anti haxxor
  * Root permission så vi kan binda portar under 1024
  * Signal handler
*/

#include "httpd.h"

int main(int argc, char* argv[]) {
  int i, port, sd, sd_current, addrlen;
  struct sockaddr_in sin, pin;
  pthread_t handler;
  pthread_attr_t att;

  // Init thread lock
  pthread_mutex_init(&thread_lock, NULL);

  // Set default config
  struct configuration config;
  setDefaultConfig(&config);

  // Parse config file
  rootDir(&config, argv[0]); 
  parseConfig(&config);

  if(argc > 1) {
    for(i = 1; i < argc; i++) {
      switch(argv[i][1]) {
        case 'h':
          printHelp();
          return 0;
          break;
        case 'p':
          i++;
          if(i >= argc) {
            printHelp();
            return 0;
          }
          if(argv[i][0] != '-') {
            if((port = atoi(argv[i])) != 0 && port < 65536) {
              config.listenPort = port;
              printf("Port number: %d\n", port);
            }
            else {
              printHelp();
              return 0;
            }
          }
          else {
            printHelp();
            return 0;
          }
          break;
        case 'd':
          // Start daemon if set
          printf("Starting daemon...\n");
          daemonfunc("daemon");
          while(1){}
          break;
        case 'l':
          i++;
          if(i >= argc) {
            printHelp();
            return 0;
          }
          if(argv[i][0] != '-') {
            config.accLogPath = argv[i];
          }
          else {
            printHelp();
            return 0;
          }
          break;
        case 's':
          i++;
          if(i >= argc) {
            printHelp();
            return 0;
          }
          if(argv[i][0] != '-')
            printf("Starting: %s as selected request handling method\n", argv[i]);
          else
          {
            printHelp();
            return 0;
          }
          break;
        case 'c':
          i++;
          if(i >= argc){
            printHelp();
            return 0;
          }
          if(argv[i][0] != '-') {
            config.configPath = argv[i];
          }
          else {
            printHelp();
            return 0;
          }
          break;
      }
    }
  }

  // Init logfunctions
  log_init(&config);

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

  // Init thread attr
  pthread_attr_init(&att);
  // Set threads to detached state
  pthread_attr_setdetachstate(&att, PTHREAD_CREATE_DETACHED);
  // Set system scope
  pthread_attr_setscope(&att, PTHREAD_SCOPE_SYSTEM);
  // Set RoundRobin scheduling
  pthread_attr_setschedpolicy(&att, SCHED_RR); // Not supported in LINUX pthreads

  // Start accepting requests
  addrlen = sizeof(pin);
  while(true) {
    // Accept a request from queue, blocking
    if ((sd_current = accept(sd, (struct sockaddr*) &pin, (socklen_t*) &addrlen)) == -1) {
  		printf("ERROR: Unable to accept request, OS won't share, %s\n", strerror(errno));
  		DIE_CLEANUP
  	}

    // Create arguments struct
    struct _rqhd_args *args = malloc(sizeof(struct _rqhd_args));
    if (args == NULL) {
      printf("CRITICAL: Unable to allocate memory\n");
  		DIE_CLEANUP
    }
    args->sd      = sd_current;
    args->pin     = pin;
    args->config  = &config;

    if(pthread_create(&handler, &att, requestHandle, args) != 0) {
      printf("CRITICAL: Unable to start thread\n");
  		DIE_CLEANUP
    }

  }


  // Clean up
  pthread_attr_destroy(&att);
  pthread_mutex_destroy(&thread_lock);
  close(sd_current);
  close(sd);
  log_destroy();

  return 0;
}
