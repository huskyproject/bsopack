/* $Id$ */

#include <string.h>
#include <stdlib.h>
#include <fidoconf/fidoconf.h>
#include <fidoconf/common.h>
#include <fidoconf/xstr.h>
#include "log.h"
#include "config.h"
#include "bsoutil.h"

char* versionStr=NULL;

int main(int argc, char **argv)
{
    unsigned int i;

    xscatprintf(&versionStr, "BSOpack %u.%u.%u%s/" UNAME, VER_MAJOR, VER_MINOR, VER_PATCH, VER_BRANCH);

/*    if (strlen(VER_BRANCH)==0) xscatprintf(&versionStr, " %s", cvs_date); */


    getOpts(argc, argv);
    if (enable_quiet) Debug("[command line args] quiet mode\n");
    if (enable_debug) Debug("[command line args] debug mode\n");
    if (fidoConfigFile!=NULL)
        Debug("config file is %s\n", fidoConfigFile);
    else
        Debug("using default fidoconfig.\n");
    getConfig();
    Debug("config read successfully.\n");
    Log('1', "--- %s started ---\n", versionStr);
    Debug("starting main code...\n");
    Debug("found %d links.\n", fidoConfig->linkCount);
    for (i=0; i < fidoConfig->linkCount; i++)
    {
        Debug("processing link #%d: %d:%d/%d.%d\n", i,
              fidoConfig->links[i].hisAka.zone,
              fidoConfig->links[i].hisAka.net,
              fidoConfig->links[i].hisAka.node,
              fidoConfig->links[i].hisAka.point);
        if (fidoConfig->links[i].packNetmail)
            packNetMailForLink(&fidoConfig->links[i]);
        else Debug("packNetmail for this link if off.\n");
    }
    Log('1', "--- %s stopped ---\n\n", versionStr);
    Debug("exiting main code...\n");
    freeConfig();
    nfree(versionStr);
    return 0;
}
