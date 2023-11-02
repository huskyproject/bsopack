/* $Id$ */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fidoconf/fidoconf.h>
#include <fidoconf/common.h>
#include <huskylib/log.h>
#include "config.h"


s_fidoconfig *config;
char *fidoConfigFile = NULL;
int fidocfg_in_arg = 0;
char *versionStr;

void Usage()
{
    printf("\nUsage: bsopack [-c fidconfig] [options]\n");
    printf("Options:\n");
    printf("\t-h or --help  This help screen\n\n");

    if (fidocfg_in_arg)
    {
        nfree(fidoConfigFile);
    }
}

void getOpts(int argc, char **argv)
{
    int i;

    fidoConfigFile = getenv("FIDOCONFIG");

    for (i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help"))
        {
            Usage();
            exit(0);
        }

        if (!strcmp(argv[i], "-c"))
        {
            if (fidocfg_in_arg)
            {
                fprintf(stderr, "Duplicate command line parameter -c.\n");
                Usage();
                exit(-1);
            }

            if (++i >= argc)
            {
                fprintf(stderr, "Incorrect command line parameter.\n");
                Usage();
                exit(-1);
            }

            fidoConfigFile = (char *)smalloc(strlen(argv[i])+1);
            snprintf(fidoConfigFile, strlen(argv[i])+1, "%s", argv[i]);
            fidocfg_in_arg = 1;

            continue;
        }

        fprintf(stderr, "Incorrect command line parameter \"%s\"\n", argv[i]);
        Usage();
        exit(-1);
    }
}

void getConfig()
{
    config = readConfig(fidoConfigFile);
    if (NULL == config) {
        fprintf(stderr, "FidoConfig not found.\n");
        exit(-1);
    }
    if (!config->logFileDir)
    {
        fprintf(stderr, "Required keyword 'logFileDir' not found in FidoConfig.\n");
        exit(-1);
    }
    if (!config->tempOutbound)
    {
        fprintf(stderr, "Required keyword 'tempOutbound' not found in FidoConfig.\n");
        exit(-1);
    }
    if (!config->screenloglevels)
    {
        fprintf(stderr, "Required keyword 'screenLogLevels' not found in FidoConfig.\n");
        exit(-1);
    }
    if (!config->loglevels)
    {
        fprintf(stderr, "Required keyword 'logLevels' not found in FidoConfig.\n");
        exit(-1);
    }
}

void freeConfig()
{
    w_log(LL_DEBUG, "freeing config...");
    disposeConfig(config);
    if (fidocfg_in_arg)
    {
        nfree(fidoConfigFile);
    }
}
