
/* Utility header file
 *
 * Declares utility functions that will be used
 */

#pragma once

#include "httpd.h"

#define RESOLVE_FAILED 0

void daemonfunc(const char *cmd);

/*  Starts With
 *
 *  Checks if a char array starts with another char array
 * @pre       string, the string to start with
 * @str       string, the string that should start with pre
 */
bool startsWith(const char *pre, const char *str);

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

char* getIPStr(const struct sockaddr *sa, char *s, size_t maxlen);

/*  Concatenate (AKA SeaCat)
 *
 *  Concatenates two char arrays
 * @a       char array, array a
 * @b       char array, array b
 * @size    size_t, size of both string with terminating null
 * return   char pointer, new array
 */
char* ccat(char *a, char *b, size_t size);
