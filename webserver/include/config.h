
#ifndef _CONFIG_HEADER_
#define _CONFIG_HEADER_
#pragma once

#include "httpd.h"
#include "configScanner.h"

/* Parse log file
 *
 * Parses the log file and sets the global config variables
 *
 * @return    int, error code if ant, 0 if OK
 */
int parseConfig(configuration *config);

#endif
