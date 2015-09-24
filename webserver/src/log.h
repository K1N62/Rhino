/* Header file
 *
 * Includes headers important for logging functions
 */

 #pragma once

void accesslog(const struct sockaddr *addr, char* request, int code, int byte);
void serverlog(int error, char* errorMessage);
