
%token SERVERNAME LISTEN ROOTDIR BASEDIR LOGMETHOD ACCESSLOG SERVERLOG FIFO
%token INTEGER NAME PATH

%{
  #include <stdio.h>

  int yylex(void);

%}

%%

//main loop
program:
  program statement '\n'
  | /* empty */
  ;

statement:
  expr                      { printf("%d\n", $1); ans = $1; }
  | VARIABLE '=' expr       { var[$1] = $3; ans = $3;}
  | QUIT                    { return 0; }
  ;

expr:
  /* empty */               { $$ = ans; }
  | ANSWER                  { $$ = ans; }
  | INTEGER                 { $$ = $1; }
  | VARIABLE                { $$ = var[$1]; }
  | expr '+' expr           { $$ = $1 + $3; }
  | expr '-' expr           { $$ = $1 - $3; }
  | expr '*' expr           { $$ = $1 * $3; }
  | expr '/' expr           { $$ = $1 / $3; }
  | expr '^' expr           { $$ = pow($1, $3); }
  | '(' expr ')'            { $$ = $2; }
  ;

%%
