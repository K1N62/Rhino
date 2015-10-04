

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
    exit(-1);
  }
  else
  {
    return fd;
  }
}

/* Read value (Private)
 *
 * Reads a value from a config file line
 *
 * @lineIndex    size_t, the line offset
 * @line         string, the line to search in
 * @value        string, the output array
 * @size         size_t, size of the output array
 * @return       void
 */
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
  FILE *configFile;
  char buffert[PATH_MAX], realPathBuff[PATH_MAX];
  char *line = NULL;
  char value[256] = {'\0'};
  size_t  lineIndex = 0, len = sizeof(value);

  sprintf(buffert, "%s/%s", config->rootDir, config->configPath);
  realpath(buffert, realPathBuff);
printf("%s\n", realPathBuff);
  configFile = loadConfig(realPathBuff);

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
          strcpy(config->servername,  value);
        }
        // Otherwise check if it's an valid domain name and resolve it
        else if(hostnameToIp(value))
        {
          strcpy(config->servername,  value);
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
        }
        // Else invalid
        else
        {
          printf("Invalid config, port\n");
        }
      }
      else if (startsWith("Basedir", line + lineIndex))
      {
        // Move pass variable name
        lineIndex += strlen("Basedir:");

        readValue(lineIndex, line, value, sizeof(value));

        // relative the root dir
        sprintf(buffert, "%s/%s", config->rootDir, value);

        // Get the realpath
        if(realpath(buffert, realPathBuff) == NULL){
          printf("Invalid config, basedirectory: %s, %s\n", realPathBuff, strerror(errno));
          exit(-1);
        }

        // Change basedirectory to the realpath
        strcpy(config->basedir, realPathBuff);
      }
      else if (startsWith("Access_logpath", line + lineIndex))
      {
        // Move pass variable name
        lineIndex += strlen("Access_logpath:");

        readValue(lineIndex, line, value, sizeof(value));

        // relative the root dir
        sprintf(buffert, "%s/%s", config->rootDir, value);

        // Get the realpath
        if(realpath(buffert, realPathBuff) == NULL){
          printf("Invalid config, access logpath: %s, %s\n", realPathBuff, strerror(errno));
          exit(-1);
        }

        // Change accLogPath to the realpath
        strcpy(config->accLogPath, realPathBuff);
      }
      else if (startsWith("Server_logpath", line + lineIndex))
      {
        // Move pass variable name
        lineIndex += strlen("Server_logpath:");

        readValue(lineIndex, line, value, sizeof(value));

        // relative the root dir
        sprintf(buffert, "%s/%s", config->rootDir, value);

        // Get the realpath
        if(realpath(buffert, realPathBuff) == NULL){
          printf("Invalid config, server logpath: %s, %s\n", realPathBuff, strerror(errno));
          exit(-1);
        }

        // Change srvLogPath to the realpath
        strcpy(config->srvLogPath, realPathBuff);
      }
    }
  }

  fclose(configFile);
  return 0;
}
