/* $Id$ */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fidoconf/fidoconf.h>
#include <fidoconf/common.h>
#include <fidoconf/log.h>
#include "config.h"

s_fidoconfig *config;
char *logFileName=NULL;
char *fidoConfigFile=NULL;
int fidocfg_in_env=0;

void Usage()
{
    printf("Usage: bsopack [-c fidoconfig] [options]\n");
    printf("Options:\n");
    printf("\t-h or --help  This help screen\n\n");
}

void getOpts(int argc, char **argv)
{
    int i;

    fidoConfigFile=getenv("FIDOCONFIG");
    if (fidoConfigFile!=NULL)
        fidocfg_in_env=1;

    for (i=1;i<argc;i++)
    {
        if (argv[i][0]=='-')
        {
            if ((strchr(argv[i], 'h'))||(!strcmp(argv[i]+1, "-help")))
            {
                Usage();
                exit(0);
            }
            if (strchr(argv[i], 'c'))
            {
                if (i+1>=argc)
                {
                    fprintf(stderr, "Incorrect command line parameter.\n");
                    Usage();
                    exit(-1);
                }
                fidoConfigFile=(char *)smalloc(strlen(argv[i+1]+1));
                sprintf(fidoConfigFile, "%s", argv[i+1]);
                fidocfg_in_env=0;
                ++i;
            }
        }
        else
        {
            fprintf(stderr, "Incorrect command line parameter \"%s\"\n", argv[i]);
            Usage();
            exit(-1);
        }
    }
}

void getConfig()
{
    w_log(LL_DEBUG, "reading config file...");
    config=readConfig(fidoConfigFile);
    if (NULL == config) {
        fprintf(stderr, "FidoConfig not found.\n");
        exit(-1);
    };
    w_log(LL_DEBUG, "fidoconfig seems to be read successfully. retrieving info...");
    if (!config->logFileDir)
    {
        fprintf(stderr, "Required keyword 'logFileDir' in fidoconfig not found.\n");
        exit(-1);
    }
    if (!config->tempOutbound)
    {
        fprintf(stderr, "Required keyword 'tempOutbound' in fidoconfig not found.\n");
        exit(-1);
    }
    if (!config->screenloglevels)
    {
        fprintf(stderr, "Required keyword 'screenLogLevels' in fidoconfig not found.\n");
        exit(-1);
    }
    if (!config->loglevels)
    {
        fprintf(stderr, "Required keyword 'logLevels' in fidoconfig not found.\n");
        exit(-1);
    }
    w_log(LL_DEBUG, "looks all tokens found.");
}

void freeConfig()
{
    w_log(LL_DEBUG, "freeing config...");
    disposeConfig(config);
    nfree(logFileName);
    if (!fidocfg_in_env)
        nfree(fidoConfigFile);
}
