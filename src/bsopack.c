/* $Id$ */

#include <huskylib/compiler.h>
#include <huskylib/huskylib.h>
#include <fidoconf/fidoconf.h>
#include <huskylib/log.h>
#include <huskylib/locking.h>
#include "config.h"
#include "bsoutil.h"
#include "version.h"

#define LOGFILE "bsopack.log"

int lock_fd;

int main(int argc, char **argv)
{
    unsigned int i;

    versionStr = GenVersionStr( "BSOpack", bsopack_VER_MAJOR, bsopack_VER_MINOR, bsopack_VER_PATCH,
                               bsopack_VER_BRANCH, cvs_date );
    printf("%s\n", versionStr);

    setvar("module", "bsopack");

    getOpts(argc, argv);

    getConfig();

    if (config->lockfile) {
        lock_fd = lockFile(config->lockfile, config->advisoryLock);
        if( lock_fd < 0 )
        {
            disposeConfig(config);
            exit(EX_CANTCREAT);
        }
    }

    initLog(config->logFileDir, config->logEchoToScreen, config->loglevels, config->screenloglevels);
    openLog(LOGFILE, versionStr);

    if (fidoConfigFile != NULL)
    {
        w_log(LL_DEBUG, "config file is %s", fidoConfigFile);
    }
    else
    {
        w_log(LL_DEBUG, "using default FidoConfig.");
    }

    w_log(LL_DEBUG, "config read successfully.");

    w_log(LL_START, "Start");

    w_log(LL_DEBUG, "starting main code...");
    w_log(LL_DEBUG, "found %d links.", config->linkCount);

    for (i = 0; i < config->linkCount; i++)
    {
        w_log(LL_DEBUG, "processing link #%d: %d:%d/%d.%d", i,
              config->links[i]->hisAka.zone,
              config->links[i]->hisAka.net,
              config->links[i]->hisAka.node,
              config->links[i]->hisAka.point);

        if (config->links[i]->packNetmail)
        {
            packNetMailForLink(config->links[i]);
        }
        else
        {
            w_log(LL_DEBUG, "packNetmail for this link is off.");
        }
    }

    w_log(LL_STOP,"End");
    closeLog();
    if (config->lockfile)
    {
        FreelockFile(config->lockfile ,lock_fd);
    }
    freeConfig();
    return 0;
}
