/* $Id$ */

#ifndef _CONFIG_H
#define _CONFIG_H

#define VERSION "0.2-stable/"UNAME

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