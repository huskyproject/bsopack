/* $Id$ */

#include <fidoconf/fidoconf.h>
#include <fidoconf/log.h>
#include <fidoconf/xstr.h>
#include <fidoconf/version.h>
#include "config.h"
#include "bsoutil.h"
#include "cvsdate.h"

int lock_fd;

#define LOGFILE "bsopack.log"

int main(int argc, char **argv)
{
    s_fidoconfig *config;
    char *versionStr = NULL;
    unsigned int i;

    versionStr = GenVersionStr( "BSOpack", VER_MAJOR, VER_MINOR, VER_PATCH,
                              VER_BRANCH, cvs_date );
    printf("%s\n\n", versionStr);

    setvar("module", "bsopack");
    config = readConfig(NULL);

    if (!config) {
        printf("Could not read fido config\n");
        return 1;
    }

    if (config->lockfile) {
        lock_fd = lockFile(config->lockfile, config->advisoryLock);
        if( lock_fd < 0 )
        {
            disposeConfig(config);
            exit(EX_CANTCREAT);
        }
    }

    getOpts(argc, argv);

    openLog(LOGFILE, versionStr, config);

    if (fidoConfigFile!=NULL)
        w_log(LL_DEBUG, "config file is %s", fidoConfigFile);
    else
        w_log(LL_DEBUG, "using default fidoconfig.");

    w_log(LL_DEBUG, "config read successfully.");

    w_log(LL_START, "Start");

    w_log(LL_DEBUG, "starting main code...");
    w_log(LL_DEBUG, "found %d links.", config->linkCount);

    for (i=0; i < config->linkCount; i++)
    {
        w_log(LL_DEBUG, "processing link #%d: %d:%d/%d.%d", i,
              config->links[i].hisAka.zone,
              config->links[i].hisAka.net,
              config->links[i].hisAka.node,
              config->links[i].hisAka.point);
        if (config->links[i].packNetmail)
            packNetMailForLink(&config->links[i]);
        else w_log(LL_DEBUG, "packNetmail for this link is off.");
    }

    w_log(LL_STOP,"End");
    closeLog();

    if (config->lockfile) {
        FreelockFile(config->lockfile ,lock_fd);
    }
    disposeConfig(config);
    return 0;
}
