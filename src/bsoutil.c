/* $Id$ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <errno.h>
#include <smapi/compiler.h>

#ifdef HAS_DIRENT_H
  #include <dirent.h>
#endif
#ifdef HAS_UNISTD_H
  #include <unistd.h>
#endif

#ifdef HAS_PROCESS_H
  #include <process.h>
#endif
#ifdef HAS_DIRECT_H
  #include <direct.h>
#endif
#ifdef HAS_IO_H
  #include <io.h>
#endif

#include <smapi/progprot.h>
#include <fidoconf/fidoconf.h>
#include <fidoconf/common.h>
#include <fidoconf/log.h>

#include "config.h"
#include "bsoutil.h"

/* ANSI C knows nothing about this constant */

#ifndef F_OK
#define F_OK 0
#endif

s_fidoconfig *config;

time_t t;
unsigned long serial;
time_t lastt;

const char outext[5]={'i', 'c', 'd', 'o', 'h'};
const char flowext[5]={'i', 'c', 'd', 'f', 'h'};
const char daynames[7][5]={"su","mo","tu","we","th","fr","sa"};


char *createPktName()
{
    unsigned long num;
    static char name[9];

    t=time(NULL);
    if (serial==MAXPATH) {
        if (lastt==t)
            return 0;
        else
            serial=0;
    }
    if(t == lastt)
        serial++;
    else
        serial=0;
    lastt=t;
    num = (t<<8) + serial;
    sprintf(name,"%08lx",num);
    w_log(LL_DEBUG, "generated pkt name: %s", name);
    return name;
}

void createDirIfNEx(char *dir)
{   char *pp;

    if ( *(pp=(char*)(dir + strlen(dir) -1)) == PATH_DELIM )
        *pp = '\0';        /* we can't create "c:\dir\", only "c:\dir" */

/*    if (access(dir, F_OK))*/
    if (!direxist(dir))
    {
        w_log(LL_DEBUG, "creating directory %s...", dir);
        if (mymkdir(dir))
        {
            w_log(LL_CRIT, "Can't create directory %s, errno=%d",
                dir, errno);
            w_log(LL_DEBUG, "can't create, errno=%d. exiting!", errno);
            exit(-1);
        }
    }
}

char *addr2str(s_link *link){
    static char node[24];
    if (link->hisAka.point!=0)
        sprintf(node, "%u:%u/%u.%u", link->hisAka.zone, link->hisAka.net,
                link->hisAka.node, link->hisAka.point);
    else
        sprintf(node, "%u:%u/%u", link->hisAka.zone, link->hisAka.net,
                link->hisAka.node);

    return node;
}

unsigned long getNMSizeForLink(char *outb)
{
    unsigned long NMSize=0;
    struct stat fInfo;
    char *fname=NULL;
    char *p_flavour=NULL;
    int i;

    w_log(LL_DEBUG, "guessing how much netmail this link has...");
    /* /outb/12345678.hut+'\0' = strlen(outb)+4 */
    fname=(char *)smalloc(strlen(outb)+4);
    strcpy(fname, outb);
    p_flavour=fname+strlen(outb);
    strcpy(p_flavour, "?ut");

    for (i=0;i<=4;i++)
    {
        *p_flavour=outext[i];
        if (!stat(fname, &fInfo))
        {
            NMSize+=fInfo.st_size;
            w_log(LL_DEBUG, "found file %s, size=%lu", fname, fInfo.st_size);
        }
    }
    w_log(LL_DEBUG, "found %lu bytes of netmail.", NMSize);
    nfree(fname);
    return NMSize;
}


void getBundleName(s_link *link, int flavour, char *outb)
{
    int i;
    int day=0;
    char *bundleName=NULL;
    time_t t;
    struct stat fInfo;
    FILE *fp;
    char *flowFile=NULL, *flowLine=NULL, *flowLineTmp=NULL;
    char *sepDir=NULL, *outb_end=NULL;
    int foundOtherFlavour=0;
    char Idx='\0';
    char *IdxPtr;
    int outbLen=0;

    w_log(LL_DEBUG, "guessing bundle name...");
    time(&t);
    day=localtime(&t)->tm_wday;

    outbLen=strlen(outb)+30;
    /*  /outb.12b/12345678.pnt/12345678.sep/12345678.su0+'\0'; */
    bundleName=(char *)smalloc(outbLen);
    /*  /outb.12b/12345678.pnt/12345678.flo+'\0'; */
    flowFile=(char *)smalloc(outbLen-13);
    flowLine=(char *)smalloc(256);
    flowLineTmp=flowLine;

    memset(bundleName, 0, outbLen);
    if (config->separateBundles)
    {
        sprintf(bundleName, "%ssep%c", outb, PATH_DELIM);
        outb_end=bundleName+strlen(bundleName);
    }
    else
    {
        strcpy(bundleName, outb);
        outb_end=bundleName+strlen(bundleName)-9;
    }

    if (config->addr->point || link->hisAka.point)
    {       /* /outb.12b/12345678.pnt/12344321.su0 */
        sprintf(outb_end, "%04x%04x.%2s0",
                (config->addr->node - link->hisAka.node) & 0xffff,
                (config->addr->point - link->hisAka.point) & 0xffff,
                daynames[day]);
    } else
    {       /* /outb/12344321.su0 */
        sprintf(outb_end, "%04x%04x.%2s0",
                (config->addr->net - link->hisAka.net) & 0xffff,
                (config->addr->node - link->hisAka.node) & 0xffff,
                daynames[day]);
    }

    IdxPtr=bundleName+strlen(bundleName)-1;   /* points to the last symbol before '\0' */

    if (config->separateBundles)
    {
        sepDir=(char *)smalloc(strlen(bundleName)-11);
        memset(sepDir, 0, strlen(bundleName)-11);
        strncpy(sepDir, bundleName, strlen(bundleName)-12);
        createDirIfNEx(sepDir);
        nfree(sepDir);
    }

    for (Idx='0';Idx<='z';Idx++)
    {
        if (Idx==('9'+1))
            Idx='a';
        *IdxPtr=Idx;

        if(stat(bundleName, &fInfo)) break; else
            if((fInfo.st_size <= link->arcmailSize*1024) && (fInfo.st_size!=0))
            {
                foundOtherFlavour=0;
                for(i=0;i<5;i++)
                {
                    if(i==flavour) continue;
                    sprintf(flowFile, "%s%clo", outb, flowext[i]);
                    if (access(flowFile, F_OK)) continue;
                    fp=fopen(flowFile, "r");
                    while(fgets(flowLine, 256, fp)!=NULL)
                    {
                        flowLineTmp=flowLine;
                        if(strlen(flowLineTmp)<2) continue;
                        if (flowLineTmp[0]=='#' || flowLineTmp[0]=='^' || flowLineTmp[0]=='~')
                            flowLineTmp++;
                        if(strncmp(flowLineTmp, bundleName, strlen(bundleName))==0)
                        {
                            foundOtherFlavour=1;
                            break;
                        }
                    }
                    fclose(fp);
                    if(foundOtherFlavour) break;
                }
                if (!foundOtherFlavour) break; /* this file name is not used for other flavour so we can use it. */
            } else continue;
    }

    copyString(bundleName, &(link->packFile));
    w_log(LL_DEBUG, "bundle name generated: %s", link->packFile);
    nfree(bundleName);
    nfree(flowFile);
    nfree(flowLine);
}

int addToFlow(s_link *link, int flavour, char *outb)
{
    FILE *fp;
    char *line=NULL;
    int foundOldBundle=0;
    char *buff=NULL;
    struct stat fInfo;

    sprintf(link->floFile,"%s%clo", outb, flowext[flavour]);

    w_log(LL_DEBUG, "adding bundle to flow file %s", link->floFile);

    if(stat(link->floFile, &fInfo)!=-1)
    {
        w_log(LL_DEBUG, "flow file exists.");

        fp=fopen(link->floFile, "r+");
        if (fp==NULL)
        {
            w_log(LL_CRIT, "Can't open flow file for %2, errno=%d",
                addr2str(link), errno);
            w_log(LL_DEBUG, "can't open flow file, errno=%d", errno);
            return 0;
        }

        line=(char *)smalloc(MAXPATH);
        buff = smalloc(fInfo.st_size);
        memset(buff, 0, fInfo.st_size);

        while(fgets(line, MAXPATH, fp)!=NULL)
        {
            strcat(buff, line);
            if (strlen(line)<2) continue;
            if (strncmp(line+1, link->packFile, strlen(link->packFile))==0)
            {
                foundOldBundle=1;
                w_log(LL_DEBUG, "found old bundle, can't add it again.");
                break;
            }
        }

        if (!foundOldBundle)
        {
            w_log(LL_DEBUG, "now let's add our bundle.");
            rewind(fp);
            fprintf(fp, "^%s\n%s", link->packFile, buff);
            w_log(LL_DEBUG, "added our bundle.");
        }
        nfree(buff);
    }
    else
    {
        w_log(LL_DEBUG, "creating new flow file.");
        fp=fopen(link->floFile, "w");
        if (fp==NULL)
        {
            w_log(LL_CRIT, "Can't open flow file for %s, errno=%d",
                addr2str(link), errno);
            w_log(LL_DEBUG, "can't create flow file, errno=%d. returning.", errno);
            return 0;
        }
        fprintf(fp, "^%s\n", link->packFile);
        w_log(LL_DEBUG, "added our bundle.");
    }
    fclose(fp);
    return 1;
}

char *initLink(s_link *link)
{
    char *outb;
    w_log(LL_DEBUG, "allocating memory for this link...");

    /*    /outbound.12b/01234567.pnt/12345678.  +1 = strlen(outb)+5+13+9+1
     and that's enough for nodes too. */

    outb=(char *)scalloc(strlen(config->outbound)+28, 1);
    sprintf(outb, "%s", config->outbound);
    if (config->addr->zone!=link->hisAka.zone)
        sprintf(outb+strlen(outb)-1,".%03x%c",
                link->hisAka.zone, PATH_DELIM);

    if (link->hisAka.point==0)
        sprintf(outb+strlen(outb), "%04x%04x.", link->hisAka.net, link->hisAka.node);
    else
        sprintf(outb+strlen(outb), "%04x%04x.pnt%c%08x.", link->hisAka.net,
                link->hisAka.node, PATH_DELIM, link->hisAka.point);

    link->bsyFile=(char *)smalloc(strlen(outb)+4); /* *outb+"bsy"+'\0' */
    sprintf(link->bsyFile, "%sbsy", outb);
    link->floFile=(char *)scalloc(strlen(outb)+4, 1);

    if (link->pktFile==NULL)
        link->pktFile=(char *)smalloc(strlen(config->tempOutbound)+13);
    else
        link->pktFile=(char *)srealloc(link->pktFile,
                                       strlen(config->tempOutbound)+13);
    if (!link->arcmailSize)
    {
        if (!config->defarcmailSize)
        {
            link->arcmailSize=100;
        } else
            link->arcmailSize=config->defarcmailSize;
    }
    w_log(LL_DEBUG, "arcmailSize for this link is %d kb", link->arcmailSize);
    return outb;
}


void releaseLink(s_link *link, char **outb)
{
    w_log(LL_DEBUG, "freeing allocated memory for link.");
    if (link==NULL) return;
    nfree(link->bsyFile);
    nfree(link->floFile);
    nfree(link->packFile);
    nfree(link->pktFile);
    nfree(*outb);
}

int createBsy(s_link *link)
{
    if (!access(link->bsyFile, F_OK))
    {
        w_log(LL_EXEC, "Link %s is busy now.", addr2str(link));
        return 0;
    } else
    {
        FILE *fp;
        w_log(LL_DEBUG, "trying to create bsy flag for this link...");
        fp=fopen(link->bsyFile, "w");
        if (fp == NULL)
        {
            w_log(LL_BUSY, "Can't create bsyFile for %s, errno=%d",
                addr2str(link), errno);
            w_log(LL_DEBUG, "couldn't create, errno=%d", errno);
            return 0;
        }
        fprintf(fp, "%d", getpid());
        fclose(fp);
        w_log(LL_DEBUG, "bsy flag %s created successfully.", link->bsyFile);
    }
    return 1;
}

void removeBsy(s_link *link)
{
    w_log(LL_DEBUG, "removing bsy file for this link...");
    if (access(link->bsyFile, F_OK)) {
        w_log(LL_DEBUG, "can't acces bsy file %s, errno=%d. returning.", link->bsyFile, errno);
        return;
    }
    if(remove(link->bsyFile))
    {
       w_log(LL_CRIT, "Can't remove bsyFile for %s, errno=%d",
           addr2str(link), errno);
       w_log(LL_DEBUG, "can't remove, errno=%d.", errno);
    }
}

void packNetMailForLink(s_link *link)
{
    int flavour=0;
    int retval;
    char *pktname=NULL;
    char *execstr=NULL;
    char *bsoNetMail=NULL;   /* /outb.12b/12345678.pnt/12345678.hut */
    char *outbForLink=NULL;  /* /outb.12b/12345678.pnt/12345678. */
    char *dir=NULL;          /* /outb.12b/12345678.pnt */
    unsigned long nmSize=0;


    w_log(LL_DEBUG, "packNetmail for this link is on, trying to pack...");
    if (link->packerDef == NULL)
    {
        w_log(LL_CRIT, "Packer for link %s undefined but 'packNetmail' is on, skipping...",
            addr2str(link));
        w_log(LL_DEBUG, "packer for this link is undefined, returning.");
        return;
    }

    outbForLink=initLink(link);
    dir=(char *)smalloc(strlen(outbForLink)-9); /* strlen(outb)-10 + '\0'; */
    memset(dir, 0, strlen(outbForLink)-9);
    strncpy(dir, outbForLink, strlen(outbForLink)-10);
    if (access(dir, F_OK))
    {
        w_log(LL_DEBUG, "directory %s doesn't exist, skipping...", dir);
        nfree(dir);
        releaseLink(link, &outbForLink);
        return;
    }
    nfree(dir);

    if (!createBsy(link)) {
        releaseLink(link, &outbForLink);
        return;
    }

    execstr=(char *)smalloc(MAXPATH);
    bsoNetMail=(char *)smalloc(strlen(outbForLink)+4);

    nmSize=getNMSizeForLink(outbForLink);

    if ((nmSize!=0) && (nmSize >= (link->maxUnpackedNetmail*1024)))
    {
        w_log(LL_POSTING, "Found %lu bytes of netmail for %s", nmSize, addr2str(link));

        for (flavour=0;flavour<5;flavour++)
        {
            sprintf(bsoNetMail, "%s%cut", outbForLink, outext[flavour]);

            if (!access(bsoNetMail, F_OK))
                if ((pktname=(char *)createPktName())!=NULL)
                {
                    sprintf(link->pktFile, "%s%s.pkt",
		    config->tempOutbound, pktname);

                    w_log(LL_DEBUG, "found netmail packet %s, moving to %s",
                          bsoNetMail, link->pktFile);

                    if (rename(bsoNetMail, link->pktFile))
                    {
                        w_log(LL_CRIT, "Error renaming %s to %s", bsoNetMail, link->pktFile);
                        w_log(LL_DEBUG, "can't move, errno=%d. exiting!", errno);
                        releaseLink(link, &outbForLink);
                        exit(-1);
                    }

                    getBundleName(link, flavour, outbForLink); /* calculating bundleName; */
                    fillCmdStatement(execstr, link->packerDef->call,
                                     link->packFile, link->pktFile, "");
                    w_log(LL_EXEC, "Packing %s -> %s\n", bsoNetMail, link->packFile);
                    w_log(LL_DEBUG, "executing packer: %s", execstr);
                    if ((retval=system(execstr))!=0)
                    {
                        w_log(LL_CRIT, "Error executing packer, errorlevel %d", retval);
                        w_log(LL_DEBUG, "packer returned errorlevel %d, errno=%d", retval, errno);
                        releaseLink(link, &outbForLink);
                        exit(-1);
                    }

                    if (addToFlow(link, flavour, outbForLink))
                        if(remove(link->pktFile)==-1)
                            w_log(LL_CRIT, "Can't remove pktFile %s, errno=%d",
                                link->pktFile, errno);
                } /* if (createPktName(pktname)) */
        } /* for() */
    } /* if (nmSize...) */
    else
    {
        if (nmSize)
            w_log(LL_POSTING, "Found %lu bytes of netmail for %s", nmSize, addr2str(link));
    }


    removeBsy(link);
    releaseLink(link, &outbForLink);
    nfree(execstr);
    nfree(bsoNetMail);
}

