

#include "log.h"

void accesslog(const struct sockaddr *addr, char* request, int code, int byte) {

}

void serverlog(int error, char* errorMessage){
  int fd;

  fd = open(SERVER_LOG_PATH, O_CREAT | O_RDWR);
}
