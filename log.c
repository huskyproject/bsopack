#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdarg.h>
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

    if (fidoConfig->logEchoToScreen)
    {
        if (!fidoConfig->screenloglevels) return;
        if(strchr(fidoConfig->screenloglevels, level))
        {
            printf("%02d.%02d.%04d %02d:%02d:%02d [%d]: %s",
                   tp->tm_mday, tp->tm_mon, tp->tm_year+1900, tp->tm_hour, tp->tm_min, tp->tm_sec,
                   getpid(), params);
        }
    }

    if (!fidoConfig->loglevels) return;
    if (strchr(fidoConfig->loglevels, level))
    {
        fprintf(fp, "%02d.%02d.%04d %02d:%02d:%02d [%d]: %s",
             tp->tm_mday, tp->tm_mon, tp->tm_year+1900, tp->tm_hour, tp->tm_min, tp->tm_sec,
             getpid(), params);
    }

    fclose(fp);
    free(params);
}