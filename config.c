#include <stdio.h>
#include <fidoconf/fidoconf.h>
#include "config.h"
#include "log.h"


s_fidoconfig *fidoConfig;
char logFileName[MAXPATH];

void getConfig()
{
    fidoConfig=readConfig(NULL);
    if (NULL == fidoConfig) {
        Log(3, "FidoConfig not found\n");
        exit(-1);
    };
    if (!fidoConfig->logFileDir)
    {
        printf("Keyword 'logFileDir' in fidoconfig not found.\n");
        exit(-1);
    }
    sprintf(logFileName, "%sbsopack.log", fidoConfig->logFileDir);
}
