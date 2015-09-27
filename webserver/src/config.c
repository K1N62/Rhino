

#include "config.h"

/* Load config (Private)
 *
 * Loads a config file and returns it's filedescriptor
 *
 * @name      string, name of the file default to httpd.conf
 * @return    FILE*, filepointer to the config file
 */
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


void readValue(size_t lineIndex, char *line, char *value, size_t size)
{
  // Set variables
  size_t counter = 0;

  // Ignore spaces
  while (line[lineIndex] == ' ')
    lineIndex++;

  // Ignore quote marks
  if (line[lineIndex] == '"')
    lineIndex++;

  // Get the entered value, stop at end quote, newline or end of buffer
  while (line[lineIndex] != '"' && line[lineIndex] != '\n' && counter < (size - 1))
  {
    value[counter] = line[lineIndex];
    counter++;
    lineIndex++;
  }
  value[counter] = '\0';
}

int parseConfig(struct configuration *config)
{
  FILE *configFile = loadConfig(CONFIG_PATH);
  char *line = NULL;
  char value[64] = {'\0'};
  size_t  lineIndex = 0, len = 64;

  printf("Parsing config file\n" );

  while( getline(&line, &len, configFile) != EOF )
  {
    lineIndex = 0;
    // Ignore spaces
    while (line[lineIndex] == ' ')
      lineIndex++;

    // Ignore commented and empty lines
    if( line[lineIndex] != '#' && line[lineIndex] != '\n' )
    {
      // Parse the line
      if (startsWith("Servername", line + lineIndex))
      {
        // Move pass variable name
        lineIndex += strlen("Servername:");

        readValue(lineIndex, line, value, sizeof(value));

        // Check if value is an valid ip address
        if (isValidIpAddress(value))
        {
          config->servername = value;
          printf("name: %s\n", value );
        }
        // Otherwise check if it's an valid domain name and resolve it
        else if(hostnameToIp(value))
        {
          config->servername = value;
          printf("name: %s\n", value );
        }
        // Else this is not a valid config
        else
          printf("Invalid config, hostname\n");

      }
      else if (startsWith("Listen", line + lineIndex))
      {
        int  port = 0;

        // Move pass variable name
        lineIndex += strlen("Listen:");

        readValue(lineIndex, line, value, sizeof(value));

        // Check if valid port number
        if((port = atoi(value)) != 0 && port < 65536)
        {
          config->listenPort = port;
          printf("Port: %d\n", port);
        }
        // Else invalid
        else
        {
          printf("Invalid config, port\n");
        }
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

  fclose(configFile);
  return 0;
}
