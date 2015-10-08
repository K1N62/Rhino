
#pragma once

#include "httpd.h"

/* Parse log file
 *
 * Parses the log file and sets the global config variables
 *
 * @return    int, error code if ant, 0 if OK
 */
int parseConfig(struct configuration *config);
