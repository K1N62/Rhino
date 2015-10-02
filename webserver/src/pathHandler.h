
/* Path handler header file
 *
 * Declares functions that will be used for handling paths
 */

#pragma once

#include "httpd.h"

struct pathHandler {
  char *path;
  char *filename;
};

void setRootDir(struct configuration *config);
