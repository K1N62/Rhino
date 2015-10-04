
#include "globalConfigVariables.h"

void setDefaultConfig(struct configuration *config)
{
  strcpy(config->servername,  "127.0.0.1");
  strcpy(config->rootDir,     "");
  strcpy(config->basedir,     "../www");
  strcpy(config->accLogPath,  "log/access.log");
  strcpy(config->srvLogPath,  "log/server.log");
  strcpy(config->configPath,  "config/httpd.conf");
  /*config->servername    = "127.0.0.1";
  config->rootDir       = "/home/student/Dokument/GitHub/server/webserver";
  config->basedir       = "../www";
  config->accLogPath    = "log/access.log";
  config->srvLogPath    = "log/server.log";
  config->configPath    = "config/httpd.conf";
  config->listenPort    = 80;
  config->backlog       = 1000;*/
}
