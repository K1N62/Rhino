
#pragma once

#include "httpd.h"

#define CONFIG_FILENAME   "httpd.conf"
#define CONFIG_PATH       "config/" CONFIG_FILENAME

/* Load config
 *
 * Loads a config file and returns it's filedescriptor
 *
 * @name      string, name of the file default to httpd.conf
 * @return    FILE*, filepointer to the config file
 */
FILE* loadConfig(char *path);

/* Parse log file
 *
 * Parses the log file and sets the global config variables
 *
 * @return    int, error code if ant, 0 if OK
 */
int parseConfig();
