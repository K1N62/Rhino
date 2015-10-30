
#include "pathHandler.h"

void path_init(configuration *config)
{
  _path_config = config;
}

void rootDir(char *path)
{
  char *folder = PATH_BIN;
  char pathBuffert[PATH_MAX];
  size_t size;

  realpath(path, pathBuffert);

  size = strlen(pathBuffert);

  pathBuffert[size - strlen(folder)] = '\0';
  strcpy(_path_config->rootDir, pathBuffert);
}

char* getRelRoot(char *path)
{
  char buffert[PATH_MAX], *retPath;

  // relative the root dir
  sprintf(buffert, "%s/%s", _path_config->rootDir, path);

  // Get the realpath
  retPath = realpath(buffert, NULL);
  if(retPath == NULL){
    printf("Unable to resolve path: %s, %s\n", buffert, strerror(errno));
    return NULL;
  }
  else {
    return retPath;
  }

}
