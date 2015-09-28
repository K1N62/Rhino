
#include "log.h"

void accesslog(struct configuration *config, const struct sockaddr *addr, char* request, int code, int byte)
{
  int fd;
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  char str[48];

  fd = open(config.accLogPath, O_CREAT | O_RDWR);

  fputs("%s - - [%d/%d/%d:%d:%d:%d +0100] \"%s HTTP/1.0\" %d %d\n", getIPStr(addr, str, sizeof(str)), tm.tm_mday, getMon(tm.tm_mon), tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec, request, code, byte);
  close(fd);
}

void serverlog(struct configuration *config, int error, char* errorMessage)
{
  int fd;
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  char* logLevel;

  fd = open(config.serLogPath, O_CREAT | O_RDWR);

  switch(error){
    case(0):
      logLevel = "LOG_CRITICAL";
      break;
    case(1):
      logLevel = "LOG_ERROR";
      break;
    case(2):
      logLevel = "LOG_WARNING";
      break;
    case(3):
      logLevel = "LOG_INFORMATION";
      break;
    case(4):
      logLevel = "LOG_DEBUG";
      break;

  fputs("%d/%d/%d:%d:%d:%d %s \"%s\"\n", tm.tm_mday, getMon(tm.tm_mon), tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec, logLevel, errorMessage);
  close (fd);
}
