
#include "log.h"

void log_init(struct configuration *config)
{
  // Open access log
  if ((_log_access_fd = fopen(config->accLogPath, "a+")) == NULL) {
    printf("CRITICAL: Unable to open log %s, %s\n", config->accLogPath, strerror(errno));
    exit(-1);
  }

  // Open server log
  if ((_log_server_fd = fopen(config->srvLogPath, "a+")) == NULL) {
    printf("CRITICAL: Unable to open log %s, %s\n", config->srvLogPath, strerror(errno));
    exit(-1);
  }

  // Get the lenght of doc root
  _log_docRootLen = strlen(config->basedir);
}

void log_destroy()
{
  fclose(_log_access_fd);
  fclose(_log_server_fd);
}

void log_access(const struct sockaddr_in *addr, const struct _rqhd_req *request, const struct _rqhd_header *header, bool syslogBool)
{
  time_t t = time(NULL);
  char  entry[4096],
        date[64],
        ip[INET_ADDRSTRLEN],
        status[4],
        req[1024]; // Safe from overflow, each request value can be maximum BUF_VAL (256)

  // Get the date
  strftime(date, sizeof(date), "%a, %d %b %Y %T %z", localtime(&t));

  // Get the IP
  inet_ntop(AF_INET, &(addr->sin_addr), ip, INET_ADDRSTRLEN);

  // Get the request
  // We only want the numbers
  strncpy(status, header->status, sizeof(status) - 1);
  sprintf(req, "\"%s %s\" %s %d", request->method, request->uri + _log_docRootLen, status, header->size);

  // Set the entry
  sprintf(entry, "%s - - [%s] %s\n", ip, date, req);


  // Get mutex and print to log
  pthread_mutex_lock(&thread_lock);

  // Logs to syslog if set
  if(syslogBool) {
    openlog("webserverLog", LOG_PERROR, LOG_DAEMON | LOG_USER);
    syslog(LOG_INFO, entry);
    closelog();
  }
  else {
    fputs(entry, _log_access_fd);
    fflush(_log_access_fd);
  }
  pthread_mutex_unlock(&thread_lock);

}

void log_server(int error, char *errorMessage, bool syslogBool)
{
  time_t t = time(NULL);
  char entry[265];
  char date[64];
  char *logLevel;

  // Get the date
  strftime(date, sizeof(date), "%a, %d %b %Y %T %z", localtime(&t));

  switch(error){
    case(0):
      logLevel = "[EMERGENCY]:";
      break;
    case(1):
      logLevel = "[ALERT]:";
      break;
    case(2):
      logLevel = "[CRITICAL]:";
      break;
    case(3):
      logLevel = "[ERROR]:";
      break;
    case(4):
      logLevel = "[WARNING]:";
      break;
    case(5):
      logLevel = "[NOTICE]:";
      break;
    case(6):
      logLevel = "[INFORMATION]:";
      break;
    case(7):
      logLevel = "[DEBUG]:";
      break;
  }

  // Set the entry
  sprintf(entry, "[%s] \"%s %s\"\n", date, logLevel, errorMessage);

  // Get mutex and print to log
  pthread_mutex_lock(&thread_lock);

  // Logs to syslog if set
  if(syslogBool) {
    openlog("webserverLog", LOG_PERROR, LOG_DAEMON | LOG_USER);
    syslog(error, entry);
    closelog();
  }
  else {
    fputs(entry, _log_server_fd);
    fflush(_log_server_fd);
  }
  pthread_mutex_unlock(&thread_lock);

}
