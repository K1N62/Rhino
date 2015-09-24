

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
  char *line = NULL;
  size_t len = 64;
  int lineIndex = 0;

  while( getline(&line, &len, config) != EOF )
  {
    lineIndex = 0;
    // Ignore spaces
    while (line[lineIndex] == ' ') {
      lineIndex++;
    }
    // Ignore commented lines
    if( line[lineIndex] != '#' && line[lineIndex] != '\n' )
    {
      printf("%s", line);
    }
  }

  fclose(config);
  return 0;
}
