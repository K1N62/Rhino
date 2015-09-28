
#include "globalConfigVariables.h"

void setDefaultConfig(struct configuration *config)
{
  config->servername    = "127.0.0.1";
  config->basedir       = "../../www";
  config->accLogPat     = "../log/accessLog.txt";
  config->serLogPath    = "../log/serverLog.txt";
  config->listenPort    = 80;
  config->backlog        = 1000;
}
