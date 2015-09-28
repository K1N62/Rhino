
#include "globalConfigVariables.h"

void setDefaultConfig(struct configuration *config)
{
  config->servername     = "127.0.0.1";
  config->basedir        = "../../www";
  config->logPath        = "../log";
  config->listenPort     = 80;
  config->backlog        = 1000;
}
