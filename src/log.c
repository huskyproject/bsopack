/* $Id$ */

#include <stdio.h>
#if !defined (__FreeBSD__)
  #include <malloc.h>
#else
  #include <stdlib.h>
#endif
#include <string.h>
#include <time.h>
#include <stdarg.h>

#if defined (UNIX)
#include <unistd.h>
#endif

#if defined (__WATCOMC__) || (OS2) || (_MSC_VER)
#include <process.h>
#endif

#include "config.h"
#include "log.h"

void Log(char level, char *msg,...)
{
    time_t t;
    struct tm *tp;
    va_list args;
    char *params;
    FILE *fp;

    params=(char *)malloc(MAXPATH);
    va_start(args, msg);
    vsprintf(params, msg, args);
    va_end(args);

    t=time(NULL);
    tp=localtime(&t);

    fp=fopen(logFileName, "a+");
    if (fp==NULL)
    {
        fprintf(stderr, "Can't open log %sbsopack.log!\n", fidoConfig->logFileDir);
        return;
    }

    if (fidoConfig->logEchoToScreen && !enable_quiet)
    {
        if(strchr(fidoConfig->screenloglevels, level))
        {
            printf("%02d.%02d.%04d %02d:%02d:%02d [%u]: %s",
                   tp->tm_mday, tp->tm_mon+1, tp->tm_year+1900, tp->tm_hour, tp->tm_min, tp->tm_sec,
                   getpid(), params);
        }
    }

    if (!fidoConfig->loglevels) return;
    if (strchr(fidoConfig->loglevels, level))
    {
        fprintf(fp, "%02d.%02d.%04d %02d:%02d:%02d [%u]: %s",
             tp->tm_mday, tp->tm_mon+1, tp->tm_year+1900, tp->tm_hour, tp->tm_min, tp->tm_sec,
             getpid(), params);
    }

    fclose(fp);
    free(params);
}

void Debug(char *msg,...)
{
    va_list args;
    char *params;

    if (!enable_debug) return;
    params=(char *)malloc(MAXPATH);
    va_start(args, msg);
    vsprintf(params, msg, args);
    va_end(args);

    fprintf(stdout, "[%u]: %s", getpid(), params);
    fflush(stdout);

    free(params);
}
