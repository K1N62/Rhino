
/* Webserver
 *
 * Authors, Jim Ahlstrand & Lukas Landenstad
 * Copyright 2015
 */

#include "httpd.h"

int main(int argc, char* argv[]) {

  int i, port;

  if(argc > 1){
    for(i = 1; i < argc; i++){
      switch(argv[i][1]){
        case 'h':
          printf("Help not implemented.. yet\n");
          break;
        case 'p':
          i++;
          if(i >= argc){
            printf("ERROR: WRONG USAGE\n");
            return 0;
          }
          if(argv[i][0] != '-'){
            if((port = atoi(argv[i])) != 0)
              printf("Port number: %d\n", port);
            else{
              printf("ERROR: WRONG USAGE\n");
              return 0;
            }
          }
          else{
            printf("ERROR: WRONG USAGE\n");
            return 0;
          }
          break;
        case 'd':
          printf("Starting daemon...\n");
          daemonfunc("daemon");
          break;
        case 'l':
          printf("Starting logging..\n");
          break;
        case 's':
          i++;
          if(i >= argc){
            printf("ERROR: WRONG USAGE\n");
            return 0;
          }
          if(argv[i][0] != '-')
            printf("Starting: %s as selected request handling method\n", argv[i]);
          else{
            printf("ERROR: WRONG USAGE\n");
            return 0;
          }
          break;
      }
    }
  }

  // Parse config file
  parseConfig();
  
// Start daemon if set
// Create listening socket
// Start while-loop
// Accept requests
// Check if request follows standard        code 400, 403
// Use selected request handling method
//

  return 0;
}
