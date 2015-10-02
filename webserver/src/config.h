
#pragma once

#include "httpd.h"

#define CONFIG_FILENAME   "httpd.conf"
#define CONFIG_PATH       "../../config/" CONFIG_FILENAME

/* Parse log file
 *
 * Parses the log file and sets the global config variables
 *
 * @return    int, error code if ant, 0 if OK
 */
int parseConfig(struct configuration *config);
