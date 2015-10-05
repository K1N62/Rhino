
/* Global Config Variables Header file
 *
 * Contains all the servers configuration Variables
 * These are set to the default values and later overwritten by the config
 *  or parameter settings
 *
 */

#pragma once

#include <string.h>

#define BUF_CFG 4096

struct configuration {
  char servername[BUF_CFG];
  char rootDir[BUF_CFG];
  char basedir[BUF_CFG];
  char accLogPath[BUF_CFG];
  char srvLogPath[BUF_CFG];
  char configPath[BUF_CFG];
  int  listenPort;
  int  backlog;
};

void setDefaultConfig(struct configuration *config);
