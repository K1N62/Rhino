%{

#include "configScanner.h"

%}

ws      [ \t]
cmt     \#.*
int     "?[0-9]+"?
name    "?[a-zA-Z]*"?
path    "?[a-zA-Z\\\/. ]*"?

%%

{ws} ;
{cmt} ;

"Servername:"     return SERVERNAME;
"Listen:"         return LISTEN;
"Root_dir:"       return ROOTDIR;
"Base_dir:"       return BASEDIR;
"Log_method:"     return LOGMETHOD;
"Access_logpath:" return ACCESSLOG;
"Server_logpath:" return SERVERLOG;
"Fifo_path:"      return FIFO;

{int} { return INTEGER; }
{name} { return NAME; }
{path} { return PATH; }

%%
