/* $Id$ */

#ifndef _BSOUTIL_H_
#define _BSOUTIL_H_

#if !defined MAXPATH
#define MAXPATH 256
#endif

void packNetMailForLink(s_link *link);
char *addr2str(s_link *link);

#endif
