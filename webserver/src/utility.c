
#include "utility.h"

void daemonfunc(const char *cmd)
{
    int i;
    pid_t pid;
    struct rlimit       rl;
    struct sigaction    sa;

    // Get maximum number of file descriptors.
   if (getrlimit(RLIMIT_NOFILE, &rl) < 0) {
        perror(cmd);
        exit(1);
    }

    // Become a session leader to lose controlling TTY.
    if ((pid = fork()) < 0) {
        perror(cmd);
        exit(1);
    }
    else if (pid != 0) /* parent */
        exit(0);


    // Ensure future opens won't allocate controlling TTYs.
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) < 0) {
        perror("Can't ignore SIGHUP");
        exit(1);
    }
    if ((pid = fork()) < 0){
        perror("Can't fork");
        exit(1);
    }
    else if (pid != 0) /* parent */
        exit(0);

    // Clear file creation mask.
    umask(0);

    // Set session id so this process is session leader
    setsid();

    // Change the current working directory to the root so
    // we won't prevent file systems from being unmounted.
    if (chdir("/") < 0){
        perror("Can't change to /");
        exit(1);
    }

    // Close all open file descriptors.
   if (rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 1024;
    for (i = 0; i < rl.rlim_max; i++)
        close(i);

  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);
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

char *ccat(char *a, char *b, size_t size)
{
  char *tmp = malloc(size);
  strncpy(tmp, a, size);
  strcat(tmp, b);
  return tmp;
}

void printHelp(void)
{
  printf("\n -h \t Print help text \n -p \t port, Select port to listen to \n -d \t Run webserver as a daemon \n -l \t logfile, Log to logfile \n    \t If not specified, logging will be output to syslog \n -s \t |thread|, Select request handling method \n -c \t absolute path to config, If not set process will try finding it\n\n");
}

const char *getExt (const char *fspec) {
    char *e = strrchr (fspec, '.');
    if (e == NULL)
        e = "";
    return e;
}

uid_t name_to_uid(char const *name)
{
  if (!name)
    return -1;
  long const buflen = sysconf(_SC_GETPW_R_SIZE_MAX);
  if (buflen == -1)
    return -1;
  char buf[buflen];
  struct passwd pwbuf, *pwbufp;
  if (0 != getpwnam_r(name, &pwbuf, buf, buflen, &pwbufp)
      || !pwbufp)
    return -1;
  return pwbufp->pw_uid;
}
