
/* Global Config Variables Header file
 *
 * Contains all the servers configuration Variables
 * These are set to the default values and later overwritten by the config
 *  or parameter settings
 *
 */

#pragma once

struct configuration {
  char *servername;
  char *rootDir;
  char *basedir;
  char *accLogPath;
  char *srvLogPath;
  int   listenPort;
  int   backlog;
};

void setDefaultConfig(struct configuration *config);
