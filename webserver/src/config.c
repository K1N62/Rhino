

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
  int lineIndex = 0;
  regex_t *r;
  char *regex_servername    = "/^Servername: \"?([0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3})|([a-z]*\.[a-z]{1,6})\"?$/";
  char *regex_listen_port   = "/^Listen: \"?([0-9]{1,5})\"?$/";
  char *regex_root_dir      = "/^Basedir: \"?([a-z.\/]*)\"?$/";

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
      // Parse the line

      printf("%s", line);
    }
  }

  regfree(r);
  fclose(config);
  return 0;
}
