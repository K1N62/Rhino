
#include "pathHandler.h"

void rootDir(struct configuration *config, char *path){
  char *folder = "/bin/httpd";
  char pathBuffert[PATH_MAX];
  size_t size;

  realpath(path, pathBuffert);

  size = strlen(pathBuffert);

  pathBuffert[size - strlen(folder)] = '\0';
  config->rootDir = pathBuffert;
}
