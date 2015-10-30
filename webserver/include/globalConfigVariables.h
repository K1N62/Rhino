
/* Global Config Variables Header file
 *
 * Contains all the servers configuration Variables
 * These are set to the default values and later overwritten by the config
 *  or parameter settings
 *
 */

#ifndef _GCV_HEADER_
#define _GCV_HEADER_
#pragma once

#include "httpd.h"

void setDefaultConfig(configuration *config);

#endif
