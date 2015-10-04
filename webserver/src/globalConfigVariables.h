
/* Global Config Variables Header file
 *
 * Contains all the servers configuration Variables
 * These are set to the default values and later overwritten by the config
 *  or parameter settings
 *
 */

#pragma once

#include <string.h>

#define BUF_VAR 4096

struct configuration {
  char servername[BUF_VAR];
  char rootDir[BUF_VAR];
  char basedir[BUF_VAR];
  char accLogPath[BUF_VAR];
  char srvLogPath[BUF_VAR];
  char configPath[BUF_VAR];
  int   listenPort;
  int   backlog;
};

void setDefaultConfig(struct configuration *config);
