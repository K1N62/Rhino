
/* Utility header file
 *
 * Declares utility functions that will be used
 */

#pragma once

#include "httpd.h"

#define RESOLVE_FAILED 0

/* Deamon function
 *
 * Daemonizes the process
 *
 * @return    void
 */
void daemonfunc();

/*  Starts With
 *
 *  Checks if a char array starts with another char array
 * @pre       string, the string to start with
 * @str       string, the string that should start with pre
 */
bool startsWith(const char *pre, const char *str);

/*
  Get Extension

  Checks the extension of a filename

  @fspec      string, name of the file
  @return     pointer to the position of the extension
*/
const char *getExt (const char *fspec);

/*  Is Valid Ip Address
 *
 *  Checks if a string is a vlid ip address
 *  Does not work with IPv6 addresses!
 * @ipAddress       string, the ip address to Checks
 * @return          bool, true if it's valid
 */
bool isValidIpAddress(char *ipAddress);

/*  Hostname To IP
 *
 *  Resolves an hostname to an ip address
 *  Overwrites hostname with the ip!
 * @hostname       string, the hostname to resolve
 * @return         bool, false if unsuccessfull
 */
bool hostnameToIp(char *hostname);

/*  Print help
 *
 *
 *  Print help to user
 */
void printHelp(void);
