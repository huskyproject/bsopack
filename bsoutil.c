#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <errno.h>
#include <fidoconf/fidoconf.h>
#include <fidoconf/common.h>
#include <smapi/compiler.h>

#if defined (UNIX)
  #include <dirent.h>
  #include <unistd.h>
#endif

#if defined (__WATCOMC__)
  #include <process.h>
  #include <direct.h>
  #include <io.h>
#endif

#include "log.h"
#include "config.h"
#include "bsoutil.h"



time_t t;
unsigned long serial;
time_t lastt;

const char outext[5][5]={".iut", ".cut", ".dut", ".out", ".hut"};
const char flowext[5][5]={".ilo", ".clo", ".dlo", ".flo", ".hlo"};
const char daynames[7][5]={"su","mo","tu","we","th","fr","sa"};


int createPktName(char *name)
{
    unsigned long num;
    t=time(NULL);
    if (serial==MAXPATH) { if (lastt==t) return 0; else serial=0; }
    if(t == lastt) serial++;
    else { serial=0; }
    lastt=t;
    num = (t<<8) + serial;
    if (name==NULL)
        name=(char *)smalloc(9);
    else
        name=(char *)srealloc(name, 9);
    sprintf(name,"%08lx",num);
    return 1;
}

void createDirIfNEx(char **dir)
{
    if (access((*dir), F_OK))
    {
        if (mymkdir((*dir)))
        {
            Log('9', "Can't create directory %s , errno=%d\n",
                (*dir), errno);
            exit(-1);
        }
    }
}

void getZoneOutbound(s_link *link, char **zoneOutbound, char *outbound)
{
    sprintf((*zoneOutbound), "%s", outbound);
    if (fidoConfig->addr->zone!=link->hisAka.zone)
    {
        sprintf((*zoneOutbound)+strlen((*zoneOutbound)),".%03x%c",
                link->hisAka.zone, PATH_DELIM);
    }
    createDirIfNEx(zoneOutbound);
}

void getOutboundForLink(s_link *link, char **outb)
{
    char *dir=NULL;
    dir=(char *)smalloc(strlen(fidoConfig->outbound)+21);
    getZoneOutbound(link, &dir, fidoConfig->outbound);
    if ((*outb)==NULL)
        (*outb)=(char *)smalloc(21);
    else
        (*outb)=(char *)srealloc((*outb), 21);
	
    if (link->hisAka.point==0)
        sprintf((*outb), "%04x%04x", link->hisAka.net, link->hisAka.node);
    else
    {
      sprintf((char *)(dir+strlen(dir)), "%04x%04x.pnt", link->hisAka.net, link->hisAka.node);
      createDirIfNEx(&dir);
      sprintf((*outb), "%04x%04x.pnt/%08x", link->hisAka.net, link->hisAka.node,
                link->hisAka.point);
    }
    nfree(dir);
    return;
}

unsigned long getNMSizeForLink(s_link *link)
{
    unsigned long NMSize=0;
    struct stat fInfo;
    char *fname=NULL, *outbForLink=NULL;
    int i;

    for (i=0;i<=4;i++)
    {
        fname=(char *)smalloc(strlen(fidoConfig->outbound)+32);
        getOutboundForLink(link, &outbForLink);
        getZoneOutbound(link, &fname, fidoConfig->outbound);
        sprintf(fname+strlen(fname), "%s%s", outbForLink, outext[i]);
        if (!stat(fname, &fInfo))
            NMSize+=fInfo.st_size;
        nfree(fname);
        nfree(outbForLink);
    }
    return NMSize;
}


void getBundleName(s_link *link, int flavour)
{
    int idx, i;
    int day=0;
    char *bundleName=NULL;
    time_t t;
    struct tm *tp;
    struct stat fInfo;
    FILE *fp;
    char *flowFile=NULL, *flowLine=NULL, *flowLineTmp=NULL, *outbForLink=NULL;
    char *sepDir=NULL;
    int foundOtherFlavour=0;
    char Idx='\0';

    time(&t);
    tp=localtime(&t);
    day=tp->tm_wday;
    bundleName=(char *)smalloc(strlen(fidoConfig->outbound)+4+13+12+13);
    flowFile=(char *)smalloc(strlen(fidoConfig->outbound)+4+13+13);
    flowLine=(char *)smalloc(256);
    flowLineTmp=flowLine;

    for (idx=0;idx<36;idx++)
    {
        if (fidoConfig->addr->point || link->hisAka.point)
        {
            if (idx>9)
                Idx='a'+idx-10;
            else
                Idx='0'+idx;

            getZoneOutbound(link, &bundleName, fidoConfig->outbound);
            sprintf(bundleName+strlen(bundleName), "%04x%04x.pnt", link->hisAka.net, link->hisAka.node);
            createDirIfNEx(&bundleName);
            getZoneOutbound(link, &bundleName, fidoConfig->outbound);
            if (fidoConfig->separateBundles)
               sprintf(bundleName+strlen(bundleName), "%04x%04x.pnt%c%08x.sep%c%04x%04x.%2s%1c",
                       link->hisAka.net, link->hisAka.node, PATH_DELIM,
                       link->hisAka.point, PATH_DELIM,
                       (fidoConfig->addr->node - link->hisAka.node) & 0xffff,
                       (fidoConfig->addr->point - link->hisAka.point) & 0xffff,
                       daynames[day], Idx);
            else
               sprintf(bundleName+strlen(bundleName), "%04x%04x.pnt%c%04x%04x.%2s%1c",
                    link->hisAka.net, link->hisAka.node, PATH_DELIM,
                    (fidoConfig->addr->node - link->hisAka.node) & 0xffff,
                    (fidoConfig->addr->point - link->hisAka.point) & 0xffff,
                    daynames[day], Idx);
        } else
        {
            getZoneOutbound(link, &bundleName, fidoConfig->outbound);
            if (fidoConfig->separateBundles)
                sprintf(bundleName+strlen(bundleName), "%04x%04x.sep%c%04x%04x.%2s%1x",
                        link->hisAka.net, link->hisAka.node, PATH_DELIM,
                        (fidoConfig->addr->net - link->hisAka.net) & 0xffff,
                        (fidoConfig->addr->node - link->hisAka.node) & 0xffff,
                        daynames[day], Idx);
            else
                sprintf(bundleName+strlen(bundleName), "%04x%04x.%2s%1x",
                        (fidoConfig->addr->net - link->hisAka.net) & 0xffff,
                        (fidoConfig->addr->node - link->hisAka.node) & 0xffff,
                        daynames[day], Idx);

        }
        if (fidoConfig->separateBundles)
        {
            sepDir=(char *)smalloc(strlen(bundleName)-11);
	    memset(sepDir, 0, strlen(bundleName)-11);
            strncpy(sepDir, bundleName, strlen(bundleName)-12);
            createDirIfNEx(&sepDir);
            nfree(sepDir);
        }

        if(stat(bundleName, &fInfo)) break; else
            if(fInfo.st_size <= link->arcmailSize*1024)
            {
                foundOtherFlavour=0;
                for(i=0;i<5;i++)
                {
                    if(i==flavour) continue;
                    getOutboundForLink(link, &outbForLink);
                    getZoneOutbound(link, &flowFile, fidoConfig->outbound);
                    sprintf(flowFile+strlen(flowFile), "%s%s", outbForLink, flowext[i]);
                    if (access(flowFile, F_OK)) continue;
                    fp=fopen(flowFile, "r");
                    while(fgets(flowLine, 256, fp)!=NULL)
                    {
                        flowLine=flowLineTmp;
                        if(strlen(flowLine)<2) continue;
                        if (flowLine[0]=='#' || flowLine[0]=='^' || flowLine[0]=='~')
                            flowLine++;
                        if(strncmp(flowLine,bundleName,strlen(bundleName))==0)
                        {
                            foundOtherFlavour=1;
                            break;
                        }
                    }
                    fclose(fp);
                    if(foundOtherFlavour) break;
                }
                if (!foundOtherFlavour && idx<15) break;
            } else continue;
    }
    if (link->packFile==NULL)
        link->packFile=(char *)smalloc(strlen(bundleName)+1);
    else
        link->packFile=(char *)srealloc(link->packFile, strlen(bundleName)+1);
    sprintf(link->packFile, "%s", bundleName);
    nfree(bundleName);
    flowLine=flowLineTmp;
    nfree(flowFile);
    nfree(flowLine);
    nfree(outbForLink);
}

void fillCmdStatement(char *cmd, const char *call, const char *archiv, const char *file, const char *path) {
   const char *start, *tmp, *add;

   *cmd = '\0';  start = NULL;
   for (tmp = call; (start = (char *) strchr(tmp, '$')) != NULL; tmp = start + 2) {
      switch(*(start + 1)) {
         case 'a': add = archiv; break;
         case 'p': add = path; break;
         case 'f': add = file; break;
         default:
            strncat(cmd, tmp, (size_t) (start - tmp + 1));
            start--; continue;
      };
      strncat(cmd, tmp, (size_t) (start - tmp));
      strcat(cmd, add);
   };
   strcat(cmd, tmp);
}

int addToFlow(s_link *link, int flavour)
{
    FILE *fp;
    char *line=NULL;
    int foundOldBundle=0;
    char *outbForLink=NULL;
    unsigned char *buff=NULL;
    struct stat fInfo;

    line=(char *)smalloc(MAXPATH);
    getOutboundForLink(link, &outbForLink);
    getZoneOutbound(link, &link->floFile, fidoConfig->outbound);
    sprintf(link->floFile+strlen(link->floFile), "%s%s", outbForLink, flowext[flavour]);
    nfree(outbForLink);

    if(stat(link->floFile, &fInfo)!=-1)
    {
        buff=(unsigned char *)smalloc(fInfo.st_size);

        fp=fopen(link->floFile, "r+");
        if (fp==NULL)
        {
            Log('9', "Can't open flow file for %d:%d/%d.%d!\n",
                link->hisAka.zone, link->hisAka.net, link->hisAka.node, link->hisAka.point);
            return 0;
        }

        while(fgets(line, MAXPATH, fp)!=NULL)
        {
            if (strlen(line)<2) continue;
            if (strncmp(line+1, link->packFile, strlen(link->packFile))==0)
            {
                foundOldBundle=1;
                break;
            }
        }

        if (!foundOldBundle)
        {
            rewind(fp);
            if (fread(buff, 1, fInfo.st_size, fp)!=fInfo.st_size)
            {
                Log('9', "fread() failed, file %s\n", line);
                return 0;
            }
            fclose(fp);
            fp=fopen(link->floFile, "w+");
            if (fp==NULL)
            {
                Log('9', "Can't open flow file for %d:%d/%d.%d!\n",
                    link->hisAka.zone, link->hisAka.net, link->hisAka.node, link->hisAka.point);
                return 0;
            }
            fprintf(fp, "^%s\n", link->packFile);
            if (fwrite(buff, 1, fInfo.st_size, fp)!=fInfo.st_size)
            {
                Log('9', "fwrite() failed, file %s\n", line);
                return 0;
                }

        }
    }
    else
    {
        fp=fopen(link->floFile, "w");
        if (fp==NULL)
        {
            Log('9', "Can't open flow file for %d:%d/%d.%d!\n",
                link->hisAka.zone, link->hisAka.net, link->hisAka.node, link->hisAka.point);
            return 0;
        }
        fprintf(fp, "^%s\n", link->packFile);
    }
    fclose(fp);
    nfree(buff);
    nfree(line);
    return 1;
}

void initLink(s_link *link)
{
    if (link->hisAka.point)
        link->bsyFile=(char *)smalloc(strlen(fidoConfig->outbound)+5+12+1+12+1);
    else
        link->bsyFile=(char *)smalloc(strlen(fidoConfig->outbound)+5+12+1);

    if (link->hisAka.point)
        link->floFile=(char *)smalloc(strlen(fidoConfig->outbound)+5+12+1+12+1);
    else
        link->floFile=(char *)smalloc(strlen(fidoConfig->outbound)+5+12+1);

    if (link->pktFile==NULL)
        link->pktFile=(char *)smalloc(strlen(fidoConfig->tempOutbound)+13);
    else
        link->pktFile=(char *)srealloc(link->pktFile,
                                       strlen(fidoConfig->tempOutbound)+13);
    if (!link->arcmailSize)
    {
        if (!fidoConfig->defarcmailSize)
        {
            link->arcmailSize=100;
        } else
            link->arcmailSize=fidoConfig->defarcmailSize;
    }
}


void releaseLink(s_link *link)
{
    if (link==NULL) return;
    if (link->bsyFile != NULL)
        nfree(link->bsyFile);
    if (link->floFile != NULL)
        nfree(link->floFile);
    if (link->packFile != NULL)
        nfree(link->packFile);
    if (link->pktFile != NULL)
        nfree(link->pktFile);
}

int createBsy(s_link *link)
{
    char *outbForLink=NULL;
    
    getOutboundForLink(link, &outbForLink);
    getZoneOutbound(link, &link->bsyFile, fidoConfig->outbound);
    sprintf(link->bsyFile+strlen(link->bsyFile), "%s.bsy", outbForLink);
    nfree(outbForLink);

    if (!access(link->bsyFile, F_OK))
    {
        Log('6', "Link %d:%d/%d.%d is busy now.\n",
            link->hisAka.zone, link->hisAka.net,
            link->hisAka.node, link->hisAka.point);
        return 0;
    } else
    {
        FILE *fp;
        fp=fopen(link->bsyFile, "a+");
        if (fp == NULL)
        {
            Log('8', "Can't create bsyFile for %d:%d/%d.%d, errno=%d\n",
                link->hisAka.zone, link->hisAka.net,
                link->hisAka.node, link->hisAka.point,
                errno);
            return 0;
        }
        fprintf(fp, "%d", getpid());
        fclose(fp);
    }
    return 1;
}

void removeBsy(s_link *link)
{
    if (access(link->bsyFile, F_OK)) return;
    if(remove(link->bsyFile))
       Log('9', "Can't remove bsyFile for %d:%d/%d.%d, errno=%d\n",
                link->hisAka.zone, link->hisAka.net,
                link->hisAka.node, link->hisAka.point,
                errno);
}

void packNetMailForLink(s_link *link)
{
    int flavour=0;
    char *pktname=NULL;
    char *execstr=NULL;
    char *bsoNetMail=NULL;
    char *outbForLink=NULL;
    unsigned long nmSize=0;


    if (link->packerDef == NULL)
    {
        Log('9', "Packer for link %d:%d/%d.%d undefined but 'packNetmail' is on, skipping...\n",
            link->hisAka.zone, link->hisAka.net,
            link->hisAka.node, link->hisAka.point
           );
        return;
    }

    initLink(link);
    if (!createBsy(link)) return;
    pktname=(char *)smalloc(9);
    execstr=(char *)smalloc(MAXPATH);
    bsoNetMail=(char *)smalloc(MAXPATH);
    if ((nmSize=getNMSizeForLink(link))>=link->maxUnpackedNetmail*1024)
    {
	if (nmSize)
          Log('5', "Found %lu bytes of netmail for %d:%d/%d.%d\n", nmSize,
            link->hisAka.zone, link->hisAka.net,
            link->hisAka.node, link->hisAka.point
            );
        for (flavour=0;flavour<5;flavour++)
        {
            getOutboundForLink(link, &outbForLink);
            if (fidoConfig->addr->zone==link->hisAka.zone)
                sprintf(bsoNetMail, "%s%s%s", fidoConfig->outbound, outbForLink, outext[flavour]);
            else
            {
                sprintf(bsoNetMail, "%s", fidoConfig->outbound);
                bsoNetMail[strlen(bsoNetMail)-1]='\0';
                sprintf(bsoNetMail+strlen(bsoNetMail), ".%03x%c%s%s", link->hisAka.zone,
                        PATH_DELIM, outbForLink, outext[flavour]);
            }
            if (!access(bsoNetMail, F_OK))
                if (createPktName(pktname))
                {
                    sprintf(link->pktFile, "%s/%s.pkt", fidoConfig->tempOutbound, pktname);
                    if (rename(bsoNetMail, link->pktFile))
                    {
                        Log('9', "Error renaming %s to %s, errno=%d\n",
                            bsoNetMail, link->pktFile, errno);
                        exit(-1);
                    }
                    getBundleName(link, flavour);
                    fillCmdStatement(execstr, link->packerDef->call, link->packFile, link->pktFile, "");
                    Log('6', "Packing %s -> %s\n", bsoNetMail, link->packFile);
                    if (system(execstr)==-1)
                    {
                        Log('9', "Error executing packer, errno=%d\n", errno);
                        exit(-1);
                    }
                    if (addToFlow(link, flavour))
                        if(remove(link->pktFile)==-1)
                        {
                            Log('9', "Can't remove pktFile %s, errno=%d\n", link->pktFile, errno);
                        }
                }
        }
    }
    else
    	if (nmSize)
          Log('5', "Found %lu bytes of netmail for %d:%d/%d.%d, leaving unpacked\n", nmSize,
            link->hisAka.zone, link->hisAka.net,
            link->hisAka.node, link->hisAka.point
            );
    removeBsy(link);
    releaseLink(link);
    nfree(pktname);
    nfree(execstr);
    nfree(bsoNetMail);
    nfree(outbForLink);
}

