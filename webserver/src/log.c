
#include "log.h"

void accesslog(const struct sockaddr *addr, char* request, int code, int byte)
{
  int fd;
  char* accLogPath = logPath + "/accessLog";

  fd = open(accLogPath, O_CREAT | O_RDWR);

  char* logMessage = addr.ai_addr + " - - " + date()

}

void serverlog(int error, char* errorMessage)
{
  int fd;
  char* serLogPath = logPath + "/serverLog";

  fd = open(serLogPath, O_CREAT | O_RDWR);

  char* message = date() + ' ' + (error + '0') ' ' + errorMessage;
  write(fd, message, sizeof(message));
  close (fd);
}
