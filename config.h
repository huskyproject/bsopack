#ifndef _CONFIG_H
#define _CONFIG_H

#include <fidoconf/fidoconf.h>

#define MAXPATH 256


extern s_fidoconfig *fidoConfig;
extern char logFileName[MAXPATH];
extern int CHK;

void getConfig();

#endif