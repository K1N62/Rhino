
#include "log.h"

void log_init(struct configuration *config)
{
  // Open access log
  if ((_log_access_fd = fopen(config->accLogPath, "a+")) == NULL) {
    printf("CRITICAL: Unable to open log %s, %s\n", config->accLogPath, strerror(errno));
    exit(-1);
  }

  // Open server log
  if ((_log_access_fd = fopen(config->srvLogPath, "a+")) == NULL) {
    printf("CRITICAL: Unable to open log %s, %s\n", config->srvLogPath, strerror(errno));
    exit(-1);
  }
}

void log_destroy()
{
  fclose(_log_access_fd);
  fclose(_log_server_fd);
}

void log_access(const struct sockaddr *addr, char *request, char *statusCode, int bytes)
{
  time_t t = time(NULL);
  char entry[265];
  char date[64];
  char str[48];

  // Get the date
  strftime(date, sizeof(date), "%a, %d %b %Y %T %z", localtime(&t));

  // Set the entry
  sprintf(entry, "%s - - [%s] \"%s\" %s %d\n", getIPStr(addr, str, sizeof(str)), date, request, statusCode, bytes);

  // Get mutex and print to log
  pthread_mutex_lock(&thread_lock);
  fputs(entry, _log_access_fd);
  pthread_mutex_unlock(&thread_lock);

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
      logLevel = "[CRITICAL]:";
      break;
    case(1):
      logLevel = "[ERROR]:";
      break;
    case(2):
      logLevel = "[WARNING]:";
      break;
    case(3):
      logLevel = "[INFORMATION]:";
      break;
    case(4):
      logLevel = "[DEBUG]:";
      break;
  }

  // Set the entry
  sprintf(entry, "[%s] \"%s %s\"\n", date, logLevel, errorMessage);

  // Get mutex and print to log
  pthread_mutex_lock(&thread_lock);
  fputs(entry, _log_server_fd);
  pthread_mutex_unlock(&thread_lock);

}
