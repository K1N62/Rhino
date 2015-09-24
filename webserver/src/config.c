

#include "config.h"

FILE* loadConfig(char *path)
{
  FILE *fd;

  if ((fd = fopen(path, "r")) == NULL) {
    // ERROR HANDLING LOG PERHAPS
    perror("Failed to open config file");
    exit(1);
  }
  else
  {
    return fd;
  }
}

int parseConfig()
{
  FILE *config = loadConfig(CONFIG_PATH);


  while( ( line = getline(config) ) != EOF )
      printf("%c",ch);

  return 0;
}
