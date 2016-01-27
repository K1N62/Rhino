
%token SERVERNAME LISTEN ROOTDIR BASEDIR LOGMETHOD ACCESSLOG SERVERLOG FIFO
%token INTEGER NAME PATH

%{
  #include <stdio.h>

  int yylex(void);

%}

%%



%%
