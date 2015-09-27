
#include "utility.h"

void daemonfunc(const char *cmd)
{
    int i , fd0, fd1, fd2;
    pid_t pid;
    struct rlimit       rl;
    struct sigaction    sa;
    /*
     *      * Clear file creation mask.
     *           */
    umask(0);

    /*
     *      * Get maximum number of file descriptors.
     *           */
   if (getrlimit(RLIMIT_NOFILE, &rl) < 0) {
        perror(cmd);
        exit(1);
    }

    /*
     *      * Become a session leader to lose controlling TTY.
     *           */
    if ((pid = fork()) < 0) {
        perror(cmd);
        exit(1);
    }
    else if (pid != 0) /* parent */
        exit(0);
    setsid();

    /*
     *      * Ensure future opens won't allocate controlling TTYs.
     *           */
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

    /*
     *      * Change the current working directory to the root so
     *           * we won't prevent file systems from being unmounted.
     *                */
    if (chdir("/") < 0){
        perror("Can't change to /");
        exit(1);
    }

    /*
     *      * Close all open file descriptors.
     *           */
   if (rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 1024;
    for (i = 0; i < rl.rlim_max; i++)
        close(i);

    /*
     *      * Attach file descriptors 0, 1, and 2 to /dev/null.
     *           */
    fd0 = open("/dev/null", O_RDWR);
    fd1 = open("/dev/null", O_RDWR);
    fd2 = open("/dev/null", O_RDWR);

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
    return result != 0;
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
