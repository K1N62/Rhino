
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
  char *basedir;
  char *accLogPath;
  char *serLogPath;
  int   listenPort;
};

void setDefaultConfig(struct configuration *config);
