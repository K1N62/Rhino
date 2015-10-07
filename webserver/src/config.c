

#include "config.h"

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

  if ((configFile = fopen(realPathBuff, "r")) == NULL) {
    printf("CRITICAL: Unable to open config file %s, %s\n", config->accLogPath, strerror(errno));
    return -1;
  }

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
          strncpy(config->servername,  value, BUF_CFG);
        }
        // Otherwise check if it's an valid domain name and resolve it
        else if(hostnameToIp(value))
        {
          strncpy(config->servername,  value, BUF_CFG);
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
        strncpy(config->basedir, realPathBuff, BUF_CFG);
      }
      else if (startsWith("Access_logpath", line + lineIndex))
      {
        // Move pass variable name
        lineIndex += strlen("Access_logpath:");

        readValue(lineIndex, line, value, sizeof(value));

        // relative the root dir
        sprintf(buffert, "%s/%s", config->rootDir, value);

        // Get the realpath
        realpath(buffert, realPathBuff);

        // Change accLogPath to the realpath
        strncpy(config->accLogPath, realPathBuff, BUF_CFG);
      }
      else if (startsWith("Server_logpath", line + lineIndex))
      {
        // Move pass variable name
        lineIndex += strlen("Server_logpath:");

        readValue(lineIndex, line, value, sizeof(value));

        // relative the root dir
        sprintf(buffert, "%s/%s", config->rootDir, value);

        // Get the realpath
        realpath(buffert, realPathBuff);

        // Change srvLogPath to the realpath
        strncpy(config->srvLogPath, realPathBuff, BUF_CFG);
      }
    }
  }

  fclose(configFile);
  return 0;
}
