#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <fidoconf/fidoconf.h>
#include <fidoconf/common.h>
#include "config.h"
#include "log.h"


s_fidoconfig *fidoConfig;
char *logFileName=NULL;
int enable_quiet=0;
int enable_debug=0;
char *fidoConfigFile=NULL;
int fidocfg_in_env=0;

void Usage()
{
    printf("\nBSOpack %s\n", VERSION);
    printf("Usage: bsopack [-c fidconfig] [options]\n");
    printf("Options:\n");
    printf("\t-q            Quiet mode\n");
    printf("\t-d            Debug mode\n");
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
            if (strchr(argv[i], 'q')) enable_quiet=1;
            if (strchr(argv[i], 'd')) enable_debug=1;
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
    Debug("reading config file...\n");
    fidoConfig=readConfig(fidoConfigFile);
    if (NULL == fidoConfig) {
        fprintf(stderr, "FidoConfig not found.\n");
        exit(-1);
    };
    Debug("fidoconfig seems to be read successfully. retrieving info...\n");
    if (!fidoConfig->logFileDir)
    {
        fprintf(stderr, "Required keyword 'logFileDir' in fidoconfig not found.\n");
        exit(-1);
    }
    if (!fidoConfig->tempOutbound)
    {
        fprintf(stderr, "Required keyword 'tempOutbound' in fidoconfig not found.\n");
        exit(-1);
    }
    if (!fidoConfig->screenloglevels)
    {
        fprintf(stderr, "Required keyword 'screenLogLevels' in fidoconfig not found.\n");
        exit(-1);
    }
    if (!fidoConfig->loglevels)
    {
        fprintf(stderr, "Required keyword 'logLevels' in fidoconfig not found.\n");
        exit(-1);
    }
    logFileName=(char *)smalloc(strlen(fidoConfig->logFileDir)+11+1);
    sprintf(logFileName, "%sbsopack.log", fidoConfig->logFileDir);
    Debug("looks all tokens found.\n");
}

void freeConfig()
{
    Debug("freeing config...\n");
    disposeConfig(fidoConfig);
    nfree(logFileName);
    if (!fidocfg_in_env)
        nfree(fidoConfigFile);
}
