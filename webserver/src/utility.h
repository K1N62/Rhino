
/* Utility header file
 *
 * Declares utility functions that will be used
 */

#pragma once

#include "httpd.h"

void daemonfunc(const char *cmd);

/*  Starts With
 *
 *  Checks if a char array starts with another char array
 * @pre       string, the string to start with
 * @str       string, the string that should start with pre
 */
bool startsWith(const char *pre, const char *str);
