/* Path handler header file
 *
 * Declares functions that will be used for handling paths
 */

#pragma once

#include "httpd.h"

#define PATH_BIN "/bin/httpd"

struct configuration *_path_config;

void path_init(struct configuration *config);

void rootDir(char *path);

char* getRelRoot(char *path);
