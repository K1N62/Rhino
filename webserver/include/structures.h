/**
 * Struct definitions
 */

#ifndef _STRUCT_HEADER_
#define _STRUCT_HEADER_
#pragma once

#define BUF_CFG 4096
#define BUF_VAL 256

  //! Config structures
  typedef struct {
    char servername[BUF_CFG];
    char rootDir[BUF_CFG];
    char basedir[BUF_CFG];
    char errpg[BUF_VAL];
    char accLogPath[BUF_CFG];
    char srvLogPath[BUF_CFG];
    char configPath[BUF_CFG];
    char fifoPath[BUF_CFG];
    int  listenPort;
    int  backlog;
    bool syslog;
  } configuration;

  //! Request handler, request arguments
  typedef struct {
    int sd;
    struct sockaddr_in pin;
    configuration *config;
  } _rqhd_args;

  //! Request handler, respons header
  typedef struct {
    char protocol[BUF_VAL];
    char status[BUF_VAL];
    char server[BUF_VAL];
    char type[BUF_VAL];
    char cache[BUF_VAL];
    char modified[BUF_VAL];
    int   size;
  } _rqhd_header;

  //! Request handler, request header
  typedef struct {
    char host[BUF_VAL];
    char userAgent[BUF_VAL];
    char accept[BUF_VAL];
    char referer[BUF_VAL];
  } _rqhd_req_head;

  //! Request handler, request data
  typedef struct {
    char method[BUF_VAL];
    char uri[BUF_VAL];
    char path[BUF_VAL];
    char protocol[BUF_VAL];
    _rqhd_req_head head;
    bool error;
  } _rqhd_req;

  #endif
