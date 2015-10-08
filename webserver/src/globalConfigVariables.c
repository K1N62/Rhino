
#include "globalConfigVariables.h"

void setDefaultConfig(struct configuration *config)
{
  strcpy(config->servername,  "127.0.0.1");
  strcpy(config->rootDir,     "");
  strcpy(config->basedir,     "../www");
  strcpy(config->errpg,       "errpg");
  strcpy(config->accLogPath,  "log/access.log");
  strcpy(config->srvLogPath,  "log/server.log");
  strcpy(config->configPath,  "config/httpd.conf");
  strcpy(config->fifoPath,    "fifo");
  config->syslog              = true;
}
