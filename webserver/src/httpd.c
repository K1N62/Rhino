
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
          break;
        case 's':
          break;
      }
    }
  }


  testPrint();

  return 0;
}
