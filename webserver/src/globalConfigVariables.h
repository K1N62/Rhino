
/* Global Config Variables Header file
 *
 * Contains all the servers configuration Variables
 * These are set to the default values and later overwritten by the config
 *  or parameter settings
 *
 */
#pragma once

#pragma once

char *LISTEN_IP       = "127.0.0.1";
int   LISTEN_PORT     = 80;
char *ROOT_DIR        = "../../www";
char *SERVER_LOG_PATH = "../log/serverlog";
char *ACCESS_LOG_PATH = "../log/accesslog";
