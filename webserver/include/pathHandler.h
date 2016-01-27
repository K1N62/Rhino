/**
 * Path handler header file
 *
 * Declares functions that will be used for handling paths
 */

#ifndef _PATH_HANDLER_HEADER_
#define _PATH_HANDLER_HEADER_
#pragma once

#include "httpd.h"

#ifndef PATH_BIN
#define PATH_BIN "/bin/httpd"
#endif

configuration *_path_config;

void path_init(configuration *config);

void rootDir(char *path);

char* getRelRoot(char *path);

#endif
