
/* Webserver
 *
 * Authors, Jim Ahlstrand & Lukas Landenstad
 * Copyright 2015

 TODO:
  * Set base dir, anti haxxor
  * Root permission så vi kan binda portar under 1024
  * Signal handler, dosn't accually exits need SIGINT handler
*/

#include "httpd.h"

int execute;

// signal Handler functions
void sig_handle_int(int signum) {
  printf("\nI'm dying..\n");
  // Interrupt loop
  execute = false;
}

int main(int argc, char* argv[]) {

  // Set execution to true
  execute = true;

  // Variable declarations
  int i, port, sd, sd_current, addrlen;
  struct sockaddr_in sin, pin;
  char error[1024];
  pthread_t handler;
  pthread_attr_t att;

  addrlen = sizeof(pin);

  // signal handlers
  signal(SIGPIPE, SIG_IGN);
  signal(SIGINT, sig_handle_int);

  // Init thread lock
  pthread_mutex_init(&thread_lock, NULL);

  // Set default config
  struct configuration config;
  setDefaultConfig(&config);

  // Parse config file
  path_init(&config);
  rootDir(argv[0]);
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

          break;
        case 'l':
          i++;
          if(i >= argc) {
            printHelp();
            return 0;
          }
          if(argv[i][0] != '-') {
            strcpy(config.accLogPath, argv[i]);
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
            strcpy(config.configPath, argv[i]);
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
  pthread_attr_setschedpolicy(&att, SCHED_RR);

  // Start accepting requests
  while(execute) {
    // Accept a request from queue, blocking
    if ((sd_current = accept(sd, (struct sockaddr*) &pin, (socklen_t*) &addrlen)) == -1) {
  		sprintf(error, "Unable to accept request, OS won't share, %s", strerror(errno));
      log_server(LOG_ERROR, error);
  		DIE_CLEANUP
  	}

    // Create arguments struct
    struct _rqhd_args *args = malloc(sizeof(struct _rqhd_args));
    if (args == NULL) {
      // Shit happens, if server is out of memory just skip the request
      sprintf(error, "Unable to allocate memory skipping request, %s", strerror(errno));
      log_server(LOG_INFO, error);
      close(sd_current);
    } else {
      // Set arguments
      args->sd      = sd_current;
      args->pin     = pin;
      args->config  = &config;

      // Create thread
      if(pthread_create(&handler, &att, requestHandle, args) != 0) {
        sprintf(error, "Unable to start thread, %s", strerror(errno));
        log_server(LOG_CRITICAL, error);
    		DIE_CLEANUP
      }
    }
  }

printf("Exiting..\n");
  // Clean up
  pthread_attr_destroy(&att);printf("Exiting..\n");
  pthread_mutex_destroy(&thread_lock);printf("Exiting..\n");
  close(sd);printf("Exiting..\n");
  log_destroy();printf("Exiting..\n");

  return 0;
}
