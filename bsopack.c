#define VERSION "0.01/LNX"

#include <fidoconf/fidoconf.h>
#include "log.h"
#include "config.h"
#include "bsoutil.h"

int main(int argc, char **argv)
{
    unsigned int i;
    getOpts(argc, argv);
    getConfig();
    Log('1', "--- BSOpack %s started ---\n", VERSION);
    for (i=0; i < fidoConfig->linkCount; i++)
    {
        if (fidoConfig->links[i].packNetmail)
            packNetMailForLink(&fidoConfig->links[i]);
    }
    Log('1', "--- BSOpack %s stopped ---\n\n", VERSION);
    freeConfig();
    return 0;
}
