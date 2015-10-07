/*

  Struct defenitions

  */

#pragma once

#define BUF_CFG 4096
#define BUF_VAL 256

  /* Request structures */
  struct _rqhd_args {
    int sd;
    struct sockaddr_in pin;
    struct configuration *config;
  };

  struct _rqhd_header {
    char protocol[BUF_VAL];
    char status[BUF_VAL];
    char server[BUF_VAL];
    char type[BUF_VAL];
    char cache[BUF_VAL];
    char modified[BUF_VAL];
    int   size;
  };

  struct _rqhd_req_head {
    char host[BUF_VAL];
    char userAgent[BUF_VAL];
    char accept[BUF_VAL];
    char referer[BUF_VAL];
  };

  struct _rqhd_req {
    char method[BUF_VAL];
    char uri[BUF_VAL];
    char protocol[BUF_VAL];
    struct _rqhd_req_head *head;
    bool error;
  };

  /* Config structures */
  struct configuration {
    char servername[BUF_CFG];
    char rootDir[BUF_CFG];
    char basedir[BUF_CFG];
    char accLogPath[BUF_CFG];
    char srvLogPath[BUF_CFG];
    char configPath[BUF_CFG];
    int  listenPort;
    int  backlog;
  };
