
/* Webserver
 *
 * Authors, Jim Ahlstrand & Lukas Landenstad
 * Copyright 2015

 TODO:
  * chroot jail, anti haxxor
  * Root permission så vi kan binda portar under 1024
  * Alt req method
  * syslog
  * Content-Type
*/

#include "httpd.h"

int execute, sd;

// signal Handler functions
void sig_handle_int(int signum) {
  printf("\nI'm dying..\n");
  // Interrupt loop
  execute = false;
  // Shutdown open sockets (Wake up)
  shutdown(sd, SHUT_RD);
}

int main(int argc, char* argv[]) {
  // Set execution to true
  execute = true;

  // Variable declarations
  int i, port, sd_current, addrlen, handlingMethod;
  struct sockaddr_in sin, pin;
  struct configuration config;
  char error[1024];
  pthread_t handler;
  pthread_attr_t att;

  // Set default handling method to thread
  handlingMethod = _THREAD;

  // Get size of pin ..
  addrlen = sizeof(pin);

  // Signal handlers
  signal(SIGPIPE, SIG_IGN);
  signal(SIGINT, sig_handle_int);
  signal(SIGABRT, sig_handle_int);

  // Set default config
  setDefaultConfig(&config);

  // Set root dir to current running directory
  path_init(&config);
  rootDir(argv[0]);

  // Parse config file
  if (parseConfig(&config) == -1) {
    exit(-1);
  }

  // Check arguments
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
          if(strcmp(argv[i], "thread") == 0)
            handlingMethod = _THREAD;
          else if(strcmp(argv[i], "fork") == 0)
            handlingMethod = _FORK;
          else {
            printHelp();
            return 0;
          }
          break;
        case 'c':
          i++;
          if(i >= argc) {
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
  if (log_init(&config) == -1) {
    exit(-1);
  }

  // Check super user
  if (getuid() != 0) {
    perror("You have to be root to run this program");
    exit(-1);
  }

  // Set root directory to document root
  chdir(config.basedir);
  if (chroot(config.basedir) == -1) {
    sprintf(error, "Unable to change root directory, %s", strerror(errno));
    log_server(LOG_ERROR, error);
    execute = false;  // Terminate
  }

  // Drop root privileges
  if (setgid(getgid()) == -1) {
    sprintf(error, "Unable to change user, %s", strerror(errno));
    log_server(LOG_ERROR, error);
    execute = false;  // Terminate
  }
  if (setuid(getuid()) == -1) {
    sprintf(error, "Unable to change user, %s", strerror(errno));
    log_server(LOG_ERROR, error);
    execute = false;  // Terminate
  }

  // Create listening socket
  // Domain -> AF_INET = IPV4
  // Type -> SOCK_STREAM = TCP
  if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
      sprintf(error, "Unable to open socket, %s", strerror(errno));
      log_server(LOG_ERROR, error);
      execute = false;  // Terminate
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
    sprintf(error, "Unable to bind socket, %s", strerror(errno));
    log_server(LOG_ERROR, error);
    execute = false;  // Terminate
	}

  // Start to listen for requests
  if(listen(sd, config.backlog) == -1) {
    sprintf(error, "Too loud unable to listen, %s", strerror(errno));
    log_server(LOG_ERROR, error);
    execute = false;  // Terminate
	}

  // Init thread lock
  pthread_mutex_init(&thread_lock, NULL);

  // If handling method is set to thread
  if(handlingMethod == _THREAD) {

    // Init thread attr
    pthread_attr_init(&att);
    // Set threads to detached state
    pthread_attr_setdetachstate(&att, PTHREAD_CREATE_DETACHED);
    // Set system scope
    pthread_attr_setscope(&att, PTHREAD_SCOPE_SYSTEM);
    // Set RoundRobin scheduling
    pthread_attr_setschedpolicy(&att, SCHED_RR); // Not supported in LINUX pthreads

    // Start accepting requests
    while(execute) {

      // Accept a request from queue, blocking
      if ((sd_current = accept(sd, (struct sockaddr*) &pin, (socklen_t*) &addrlen)) == -1) {
        if (execute) {
          sprintf(error, "Unable to accept request, %s", strerror(errno));
          log_server(LOG_ERROR, error);
        }
    		close(sd_current);
        execute = false;    // Terminate
  	  } else {

        // Shit happens, if server is out of memory just skip the request
        struct _rqhd_args *args = malloc(sizeof(struct _rqhd_args));
        if (args == NULL) {
          sprintf(error, "Unable to allocate memory, %s", strerror(errno));
          log_server(LOG_CRITICAL, error);
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
            close(sd_current);
            execute = false;    // Terminate
          }
        }
      }
    }
  }
  // Else if handling method is set to fork
  else if(handlingMethod == _FORK) {
    // Start accepting requests
    while(execute) {
      // Accept a request from queue, blocking
      if ((sd_current = accept(sd, (struct sockaddr*) &pin, (socklen_t*) &addrlen)) == -1) {
  		  sprintf(error, "Unable to accept request, OS won't share, %s", strerror(errno));
        log_server(LOG_ERROR, error);
  		  close(sd_current);
        execute = false;    // Terminate
  	  } else {

        pid_t pid;

        // Create child process
        if ((pid = fork()) == 0) {

          // Shit happens, if server is out of memory just skip the request
          struct _rqhd_args *args = malloc(sizeof(struct _rqhd_args));
          if (args == NULL) {
            sprintf(error, "Unable to allocate memory, %s", strerror(errno));
            log_server(LOG_CRITICAL, error);
            close(sd_current);
          } else {
            // Set arguments
            args->sd      = sd_current;
            args->pin     = pin;
            args->config  = &config;

            // Call request handler
            requestHandle(args);
          }
        } else if (pid > 1) {
          // Close socket from parent
        } else {
          sprintf(error, "Unable to fork, %s", strerror(errno));
          log_server(LOG_CRITICAL, error);
          close(sd_current);
          execute = false;    // Terminate
        }
      }
    }
  }
  // Else not a valid handling method
  else {
    sprintf(error, "Internal error, no valid handling method is set");
    log_server(LOG_ERROR, error);
  }

printf("Exiting..\n");
  // Clean up
  pthread_attr_destroy(&att);
  pthread_mutex_destroy(&thread_lock);
  close(sd);
  log_destroy();

  return 0;
}
