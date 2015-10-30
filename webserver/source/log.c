
#include "log.h"

int log_init(configuration *config)
{
  // Open access log
  if ((_log_access_fd = fopen(config->accLogPath, "a+")) == NULL) {
    printf("CRITICAL: Unable to open log %s, %s\n", config->accLogPath, strerror(errno));
    return -1;
  }

  // Open server log
  if ((_log_server_fd = fopen(config->srvLogPath, "a+")) == NULL) {
    printf("CRITICAL: Unable to open log %s, %s\n", config->srvLogPath, strerror(errno));
    return -1;
  }

  // Set logToSyslog if syslog is set
  if(config->syslog)
    logToSyslog = true;

  return 0;
}

void log_destroy()
{
  fclose(_log_access_fd);
  fclose(_log_server_fd);
}

void log_access(const struct sockaddr_in *addr, const _rqhd_req *request, const _rqhd_header *header)
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
  sprintf(req, "\"%s %s %s\" %s %d", request->method, request->uri, request->protocol, status, header->size);

  // Logs to syslog if set
  if(logToSyslog) {
    openlog("accessLog", LOG_NDELAY, LOG_DAEMON | LOG_USER);
    syslog(LOG_INFO, "%s - - [%s] %s\n", ip, date, req);
    closelog();
  }
  else {
    // Set the entry
    sprintf(entry, "%s - - [%s] %s\n", ip, date, req);
    // Get mutex and print to log
    pthread_mutex_lock(&thread_lock);
    fputs(entry, _log_access_fd);
    fflush(_log_access_fd);
    pthread_mutex_unlock(&thread_lock);
  }

}

void log_server(int error, char *errorMessage)
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

  // Logs to syslog if set
  if(logToSyslog) {
    openlog("serverLog", LOG_NDELAY, LOG_DAEMON | LOG_USER);
    syslog(error, "[%s] \"%s %s\"\n", date, logLevel, errorMessage);
    closelog();
  }
  else {
    // Set the entry
    sprintf(entry, "[%s] \"%s %s\"\n", date, logLevel, errorMessage);
    // Get mutex and print to log
    pthread_mutex_lock(&thread_lock);
    fputs(entry, _log_server_fd);
    fflush(_log_server_fd);
    pthread_mutex_unlock(&thread_lock);
  }

}
