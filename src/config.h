/* $Id$ */

#ifndef _CONFIG_H
#define _CONFIG_H

/*#define VERSION "0.2-stable/"UNAME */

#define  VER_MAJOR    0
#define  VER_MINOR    2
#define  VER_PATCH    2
/* branch is "" for CVS current, "-stable" for the release candiate branch  */
#define  VER_BRANCH  "-stable"


#include <fidoconf/fidoconf.h>

#define MAXPATH 256


extern s_fidoconfig *fidoConfig;
extern char *logFileName;
extern int enable_quiet;
extern int enable_debug;
extern char *fidoConfigFile;

void getConfig();
void freeConfig();
void getOpts(int argc, char **argv);

#endif
