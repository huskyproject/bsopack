/* $Id$ */

#ifndef _CONFIG_H
#define _CONFIG_H

#define VER_MAJOR 1
#define VER_MINOR 4
#define VER_PATCH 0
#define VER_BRANCH BRANCH_STABLE

#include <fidoconf/fidoconf.h>

#define MAXPATH 256

extern char *logFileName;
extern char *fidoConfigFile;

void getConfig();
void freeConfig();
void getOpts(int argc, char **argv);

#endif
