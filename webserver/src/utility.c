
#include "utility.h"

void daemonfunc(const char *cmd)
{
    int i;
    pid_t pid;
    struct sigaction    sa;

    // Become a session leader to lose controlling TTY.
    if ((pid = fork()) < 0) {
        perror(cmd);
        exit(-1);
    }
    else if (pid != 0) // Exit parent
        exit(0);

    // Set session id so this process is session leader (Untach controlling terminal CTTY)
    if (setsid() < 0) {
      perror("Can't set sid");
      exit(-1);
    }

    // Ensure future opens won't allocate controlling TTYs.
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) < 0) {
        perror("Can't ignore SIGHUP");
        exit(-1);
    }
    //signal(SIGHUP, SIG_IGN); ?

    // Fork again to get rid of session leader
    if ((pid = fork()) < 0){
        perror("Can't fork");
        exit(-1);
    }
    else if (pid != 0) // Exit parent
        exit(0);

    // Clear file creation mask.
    umask(0);

    // Change the current working directory to the root so
    // we won't prevent file systems from being unmounted.
    if (chdir("/") < 0){
        perror("Can't change to /");
        exit(-1);
    }

    // Close all open file descriptors that's inherited from the parent
    // Including stdin, stdout, stderr
    for (i = 0; i < sysconf(_SC_OPEN_MAX); i++)
        close(i);
}

bool startsWith(const char *pre, const char *str)
{
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0;
}

bool isValidIpAddress(char *ipAddress)
{
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ipAddress, &(sa.sin_addr));
    if(result == 0)
      result = inet_pton(AF_INET6, ipAddress, &(sa.sin_addr));
    return result;
}

bool hostnameToIp(char *hostname)
{
    struct hostent *he;
    struct in_addr **addr_list;
    int i;

    // If lookup failed
    if ( (he = gethostbyname( hostname ) ) == NULL)
        return false;


    addr_list = (struct in_addr **) he->h_addr_list;

    for(i = 0; addr_list[i] != NULL; i++)
    {
        //Return the first one
        strcpy(hostname , inet_ntoa(*addr_list[i]) );
        return true;
    }

    return false;
}

void printHelp(void)
{
  printf("\n -h \t Print help text \n \
  -p \t port, Select port to listen to \n \
  -d \t Run webserver as a daemon \n \
  -l \t logfile, Log to logfile \n\t If not specified, logging will be output to syslog \n \
  -s \t (thread|fork), Select request handling method \n \
  -c \t absolute path to config, If not set process will try finding it\n\n");
}

const char *getExt (const char *fspec) {
    char *e = strrchr (fspec, '.');
    if (e == NULL)
        e = "";
    return e;
}
