/* $Id$ */

#ifndef _CONFIG_H
#define _CONFIG_H

#include <fidoconf/fidoconf.h>

#define MAXPATH 256


extern s_fidoconfig *config;
extern char *logFileName;
extern char *fidoConfigFile;

void getConfig();
void freeConfig();
void getOpts(int argc, char **argv);

#endif
