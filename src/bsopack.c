/* $Id$ */

#include <fidoconf/fidoconf.h>
#include "log.h"
#include "config.h"
#include "bsoutil.h"
#include "cvsdate.h"

int main(int argc, char **argv)
{
    unsigned int i;
    getOpts(argc, argv);
    if (enable_quiet) Debug("[command line args] quiet mode\n");
    if (enable_debug) Debug("[command line args] debug mode\n");
    if (fidoConfigFile!=NULL)
        Debug("config file is %s\n", fidoConfigFile);
    else
        Debug("using default fidoconfig.\n");
    getConfig();
    Debug("config read successfully.\n");
    Log('1', "--- BSOpack %s %s started ---\n", VERSION, cvs_date);
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
    Log('1', "--- BSOpack %s %s stopped ---\n\n", VERSION, cvs_date);
    Debug("exiting main code...\n");
    freeConfig();
    return 0;
}
