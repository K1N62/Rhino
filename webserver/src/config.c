

#include "config.h"

FILE* loadConfig(char *path)
{
  FILE *fd;

  if ((fd = fopen(path, "r")) == NULL) {
    // Unable to open config gile
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

  printf("Parsing config file\n" );

  int lineIndex = 0;
  while( getline(&line, &len, config) != EOF )
  {
    lineIndex = 0;
    // Ignore spaces
    while (line[lineIndex] == ' ') {
      lineIndex++;
    }
    // Ignore commented and empty lines
    if( line[lineIndex] != '#' && line[lineIndex] != '\n' )
    {
      // Parse the line
      if (startsWith("Servername", line + lineIndex))
      {
          printf("Name: %s", line + lineIndex + strlen("Servername: ") );
      }
      else if (startsWith("Listen", line + lineIndex))
      {
        printf("Port: %s", line + lineIndex + strlen("Listen: ") );
      }
      else if (startsWith("Basedir", line + lineIndex))
      {
        printf("Dir: %s", line + lineIndex + strlen("Basedir: ") );
      }
      else if (startsWith("Logpath", line + lineIndex))
      {
        printf("Log: %s", line + lineIndex + strlen("Logpath: ") );
      }
      else
      {
        printf("Invalid configuration");
      }
    }
  }

  fclose(config);
  return 0;
}
