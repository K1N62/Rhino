
#include "pathHandler.h"

void rootDir(struct configuration *config, char *path){
  char *folder = "/bin/httpd";
  char pathBuffert[PATH_MAX];
  int size = 0;

  realpath(path, pathBuffert);

  while(pathBuffert[size] != '\0'){size++;}

  pathBuffert[(size - sizeof(folder) - 2)] = '\0';
  config->rootDir = pathBuffert;
}
