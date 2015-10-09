
/*
         _____   _   _   ___   _   _    ____
        |  _ \\ | | | | |_ _| | \ | |  / _ \\
        | |_) | | |_| |  | |  |  \| | | | | |
        |  _ <  |  _  |  | |  | |\  | | |_| |
        |_| \_\ |_| |_| |___| |_| \_| \\___/

         a brilliant HTTP/1.0 Webserver

  Authors, Jim Ahlstrand & Lukas Landenstad
  Copyright 2015

TODO:

BUGS:
  Broken pipe, maybe fixed
  Unique deamon
  corrupted request with concurent connections
*/

#include "httpd.h"

int execute, sd;

// signal Handler functions
void sig_handle_int(int signum) {
  printf("%d: I'm dying..\n", getpid());
  // Interrupt loop
  execute = false;
  // Shutdown open sockets (Wake up)
  shutdown(sd, SHUT_RD);
}

int main(int argc, char* argv[]) {

  // Variable declarations
  int i, port, sd_current, addrlen, handlingMethod, fifo, setval, max_fd;
  struct sockaddr_in sin, pin;
  struct configuration config;
  char error[1024];
  pthread_t handler;
  pthread_attr_t att;
  pid_t pid;
  fd_set rfds;

  // Set execution to true
  execute = true;

  // Clear file creation mask.
  umask(0);

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
        // Help
        case 'h':
          printHelp();
          return 3;
          break;
        // Port
        case 'p':
          i++;
          if(i >= argc) {
            printHelp();
            return 3;
          }
          if(argv[i][0] != '-') {
            if((port = atoi(argv[i])) != 0 && port < 65536) {
              config.listenPort = port;
              printf("Port number: %d\n", port);
            }
            else {
              printHelp();
              return 3;
            }
          }
          else {
            printHelp();
            return 3;
          }
          break;
        // Deamonize
        case 'd':
          // Start daemon if set
          printf("Starting daemon...\n");
          daemonfunc();
          break;
        // Log file
        case 'l':
          i++;
          if(i >= argc) {
            printHelp();
            return 3;
          }
          if(argv[i][0] != '-') {
            strcpy(config.accLogPath, argv[i]);
          }
          else {
            printHelp();
            return 3;
          }
          break;
        // Mode of operation
        case 's':
          i++;
          if(i >= argc) {
            printHelp();
            return 3;
          }
          if(strcmp(argv[i], "thread") == 0)
            handlingMethod = _THREAD;
          else if(strcmp(argv[i], "fork") == 0)
            handlingMethod = _FORK;
          else {
            printHelp();
            return 3;
          }
          break;
        case 'c':
          i++;
          if(i >= argc) {
            printHelp();
            return 3;
          }
          if(argv[i][0] != '-') {
            strcpy(config.configPath, argv[i]);
          }
          else {
            printHelp();
            return 3;
          }
          break;
      }
    }
  }

  // Init logfunctions
  if (log_init(&config) == -1) {
    exit(-1);
  }

  // Create fifo if prefork is set
  if (handlingMethod == _FORK) {
      // Create the named fifo pipe
      mkfifo(config.fifoPath, 0666);
      // Try opening the pipe
      if((fifo = open(config.fifoPath, O_RDWR)) == -1) {
        sprintf(error, "Unable to open FIFO-pipe, %s", strerror(errno));
        log_server(LOG_CRIT, error);
        execute = false;    // Terminate
      }
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
    log_server(LOG_ERR, error);
    execute = false;  // Terminate
  }

  // Drop root privileges
  if (setgid(getgid()) == -1) {
    sprintf(error, "Unable to change user, %s", strerror(errno));
    log_server(LOG_ERR, error);
    execute = false;  // Terminate
  }
  if (setuid(getuid()) == -1) {
    sprintf(error, "Unable to change user, %s", strerror(errno));
    log_server(LOG_ERR, error);
    execute = false;  // Terminate
  }

  // Create listening socket
  // Domain -> AF_INET = IPV4
  // Type -> SOCK_STREAM = TCP
  if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
      sprintf(error, "Unable to open socket, %s", strerror(errno));
      log_server(LOG_ERR, error);
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
    log_server(LOG_ERR, error);
    execute = false;  // Terminate
	}

  // Start to listen for requests
  if(listen(sd, config.backlog) == -1) {
    sprintf(error, "Too loud unable to listen, %s", strerror(errno));
    log_server(LOG_ERR, error);
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
          log_server(LOG_ERR, error);
        }
    		close(sd_current);
        execute = false;    // Terminate
  	  } else {

        // Shit happens, if server is out of memory just skip the request
        struct _rqhd_args *args = malloc(sizeof(struct _rqhd_args));
        if (args == NULL) {
          sprintf(error, "Unable to allocate memory, %s", strerror(errno));
          log_server(LOG_CRIT, error);
      		close(sd_current);
        } else {
          // Set arguments
          args->sd      = sd_current;
          args->pin     = pin;
          args->config  = &config;
        }

        // Create thread
        if(pthread_create(&handler, &att, requestHandle, args) != 0) {
          sprintf(error, "Unable to start thread, %s", strerror(errno));
          log_server(LOG_CRIT, error);
          close(sd_current);
          execute = false;    // Terminate
        }
      }
    }

      // Destroy attributes
      pthread_attr_destroy(&att);
    }
  // Else if handling method is set to fork
  else if(handlingMethod == _FORK) {

    max_fd = sd;
    if (fifo > sd)
      max_fd = fifo;

    // Start accepting requests
    while(execute) {

      FD_ZERO(&rfds);
      FD_SET(sd, &rfds);
      FD_SET(fifo, &rfds);

      // Accept request or handle child
      setval = select(max_fd + 1, &rfds, NULL, NULL, NULL);

      if (FD_ISSET(sd, &rfds)) {
        // Accept a request from queue
        if ((sd_current = accept(sd, (struct sockaddr*) &pin, (socklen_t*) &addrlen)) == -1) {
          if (execute) {
            sprintf(error, "Unable to accept request, %s", strerror(errno));
            log_server(LOG_ERR, error);
          }
      		close(sd_current);
          execute = false;    // Terminate
    	  } else {

          // Fork
          if((pid = fork()) == 0) {
            // CHILD ----------------------------------------------------

            // Shit happens, if server is out of memory just skip the request
            struct _rqhd_args *args = malloc(sizeof(struct _rqhd_args));
            if (args == NULL) {
              sprintf(error, "Unable to allocate memory, %s", strerror(errno));
              log_server(LOG_CRIT, error);
          		close(sd_current);
            } else {
              // Set arguments
              args->sd      = sd_current;
              args->pin     = pin;
              args->config  = &config;

              // Call request handler
              requestHandle(args);
            }
            // Tell parent I'm done
            pid_t id = getpid();
            if (write(fifo, &id, sizeof(pid_t)) == -1) {
                sprintf(error, "Unable to send pid, %s", strerror(errno));
                log_server(LOG_ERR, error);
            }

            // Done
            execute = false;

          } else if(pid > 0) {
            // PARENT ---------------------------------------------------
            // Parent don't handle dirty work
            close(sd_current);
          } else {
            sprintf(error, "Unable to fork, %s", strerror(errno));
            log_server(LOG_CRIT, error);
            close(sd_current);
            execute = false;    // Terminate
          }
        }
      } else if (FD_ISSET(fifo, &rfds)) {
        // Get child pid from fifo and wait for it
        pid_t child;
        if (read(fifo, &child, sizeof(pid_t)) == -1) {
          sprintf(error, "Unable to read pid, %s", strerror(errno));
          log_server(LOG_ERR, error);
        }
        waitpid(child, NULL, 0);
      } else if (setval == -1){
        // Error
        sprintf(error, "Select failed or was interrupted, %s", strerror(errno));
        log_server(LOG_ERR, error);
        execute = false;    // Terminate
      }

    }

    // Close fifo
    close(fifo);
  }
  // Else not a valid handling method
  else {
    sprintf(error, "Invalid handling method is set");
    log_server(LOG_ERR, error);
  }

  // Clean up
  pthread_mutex_destroy(&thread_lock);
  close(sd);
  log_destroy();
  if (pid != 0)
    printf("Cleanup complete, no one will know I was here.\n");

  return 0;
}
