#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
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

#if defined (OS2)
  #include <io.h>
  #include <process.h>
  #include <sys/fcntl.h>
#endif

#include "log.h"
#include "config.h"
#include "bsoutil.h"



time_t t;
unsigned long serial;
time_t lastt;

const char outext[5]={'i', 'c', 'd', 'o', 'h'};
const char flowext[5]={'i', 'c', 'd', 'f', 'h'};
const char daynames[7][5]={"su","mo","tu","we","th","fr","sa"};


int createPktName(char **name)
{
    unsigned long num;
    t=time(NULL);
    if (serial==MAXPATH) { if (lastt==t) return 0; else serial=0; }
    if(t == lastt) serial++;
    else { serial=0; }
    lastt=t;
    num = (t<<8) + serial;
    if (*name==NULL)
        *name=(char *)smalloc(9);
    else
        *name=(char *)srealloc(*name, 9);
    sprintf(*name,"%08lx",num);
    Debug("generated pkt name: %s\n", *name);
    return 1;
}

void createDirIfNEx(char *dir)
{

    if ((char) *(dir + strlen(dir) -1) == PATH_DELIM)
        (char) *(dir + strlen(dir) -1) = '\0';        // we can't create "c:\dir\", only "c:\dir"

    if (access(dir, F_OK))
    {
        Debug("creating directory %s...\n", dir);
        if (mymkdir(dir))
        {
            Log('9', "Can't create directory %s, errno=%d\n",
                dir, errno);
            Debug("can't create, errno=%d. exiting!", errno);
            exit(-1);
        }
    }
}

unsigned long getNMSizeForLink(s_link *link, char *outb)
{
    unsigned long NMSize=0;
    struct stat fInfo;
    char *fname=NULL;
    char *p_flavour=NULL;
    int i;

    Debug("guessing how much netmail this link has...\n");
    // /outb/12345678.hut+'\0' = strlen(outb)+4
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
            Debug("found file %s, size=%lu\n", fname, fInfo.st_size);
        }
    }
    Debug("found %lu bytes of netmail.\n", NMSize);
    nfree(fname);
    return NMSize;
}


void getBundleName(s_link *link, int flavour, char *outb)
{
    int i;
    int day=0;
    char *bundleName=NULL;
    time_t t;
    struct tm *tp;
    struct stat fInfo;
    FILE *fp;
    char *flowFile=NULL, *flowLine=NULL, *flowLineTmp=NULL;
    char *sepDir=NULL, *outb_end=NULL;
    int foundOtherFlavour=0;
    char Idx='\0';
    char *IdxPtr;
    int outbLen=0;

    Debug("guessing bundle name...\n");
    time(&t);
    tp=localtime(&t);
    day=tp->tm_wday;
    outbLen=strlen(outb)+30;
    //  /outb.12b/12345678.pnt/12345678.sep/12345678.su0+'\0';
    bundleName=(char *)smalloc(outbLen);
    //  /outb.12b/12345678.pnt/12345678.flo+'\0';
    flowFile=(char *)smalloc(outbLen-13);
    flowLine=(char *)smalloc(256);
    flowLineTmp=flowLine;

    memset(bundleName, 0, outbLen);
    if (fidoConfig->separateBundles)
        sprintf(bundleName, "%ssep%c", outb, PATH_DELIM);
    else
        strcpy(bundleName, outb);
    outb_end=bundleName+strlen(bundleName);

    if (fidoConfig->addr->point || link->hisAka.point)
    {       // /outb.12b/12345678.pnt/12344321.su0
        sprintf(outb_end, "%04x%04x.%2s0",
                (fidoConfig->addr->node - link->hisAka.node) & 0xffff,
                (fidoConfig->addr->point - link->hisAka.point) & 0xffff,
                daynames[day]);
    } else
    {       // /outb/12344321.su0
        sprintf(outb_end, "%04x%04x.%2s0",
                (fidoConfig->addr->net - link->hisAka.net) & 0xffff,
                (fidoConfig->addr->node - link->hisAka.node) & 0xffff,
                daynames[day]);
    }

    IdxPtr=bundleName+strlen(bundleName)-1;   // points to the last symbol before '\0'

    if (fidoConfig->separateBundles)
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
                if (!foundOtherFlavour) break; // this file name is not used for other flavour so we can use it.
            } else continue;
    }

    copyString(bundleName, &(link->packFile));
    Debug("bundle name generated: %s\n", link->packFile);
    nfree(bundleName);
    nfree(flowFile);
    nfree(flowLine);
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

int addToFlow(s_link *link, int flavour, char *outb)
{
    FILE *fp;
    char *line=NULL;
    int foundOldBundle=0;
    unsigned char *buff=NULL;
    struct stat fInfo;

    sprintf(link->floFile,"%s%clo", outb, flowext[flavour]);

    Debug("adding bundle to flow file %s\n", link->floFile);

    if(stat(link->floFile, &fInfo)!=-1)
    {
        Debug("flow file exists.\n");

        fp=fopen(link->floFile, "r+");
        if (fp==NULL)
        {
            Log('9', "Can't open flow file for %u:%u/%u.%u, errno=%d\n",
                link->hisAka.zone, link->hisAka.net,
                link->hisAka.node, link->hisAka.point, errno);
            Debug("can't open flow file, errno=%d\n", errno);
            return 0;
        }

        line=(char *)smalloc(MAXPATH);
        buff=(unsigned char *)smalloc(fInfo.st_size);
        memset(buff, 0, fInfo.st_size);

        while(fgets(line, MAXPATH, fp)!=NULL)
        {
            strcat(buff, line);
            if (strlen(line)<2) continue;
            if (strncmp(line+1, link->packFile, strlen(link->packFile))==0)
            {
                foundOldBundle=1;
                Debug("found old bundle, can't add it again.\n");
                break;
            }
        }

        if (!foundOldBundle)
        {
            Debug("now let's add our bundle.\n");
            rewind(fp);
            fprintf(fp, "^%s\n%s", link->packFile, buff);
            Debug("added our bundle.\n");
        }
        nfree(buff);
    }
    else
    {
        Debug("creating new flow file.\n");
        fp=fopen(link->floFile, "w");
        if (fp==NULL)
        {
            Log('9', "Can't open flow file for %u:%u/%u.%u, errno=%d\n",
                link->hisAka.zone, link->hisAka.net,
                link->hisAka.node, link->hisAka.point, errno);
            Debug("can't create flow file, errno=%d. returning,\n", errno);
            return 0;
        }
        fprintf(fp, "^%s\n", link->packFile);
        Debug("added our bundle.\n");
    }
    fclose(fp);
    return 1;
}

char *initLink(s_link *link)
{
    char *outb;
    Debug("allocating memory for this link...\n");

    /*    /outbound.12b/01234567.pnt/12345678.  +1 = strlen(outb)+5+13+9+1
     and that's enough for nodes too. */

    outb=(char *)scalloc(strlen(fidoConfig->outbound)+28, 1);
    sprintf(outb, "%s", fidoConfig->outbound);
    if (fidoConfig->addr->zone!=link->hisAka.zone)
        sprintf(outb+strlen(outb)-1,".%03x%c",
                link->hisAka.zone, PATH_DELIM);

    if (link->hisAka.point==0)
        sprintf(outb+strlen(outb), "%04x%04x.", link->hisAka.net, link->hisAka.node);
    else
        sprintf(outb+strlen(outb), "%04x%04x.pnt%c%08x.", link->hisAka.net,
                link->hisAka.node, PATH_DELIM, link->hisAka.point);

    link->bsyFile=(char *)smalloc(strlen(outb)+4); // *outb+"bsy"+'\0'
    sprintf(link->bsyFile, "%sbsy", outb);
    link->floFile=(char *)scalloc(strlen(outb)+4, 1);

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
    Debug("arcmailSize for this link is %d kb\n", link->arcmailSize);
    return outb;
}


void releaseLink(s_link *link, char **outb)
{
    Debug("freeing allocated memory for link.\n");
    if (link==NULL) return;
    nfree(link->bsyFile);
    nfree(link->floFile);
    nfree(link->packFile);
    nfree(link->pktFile);
    nfree(*outb);
}

int createBsy(s_link *link, char *outb)
{
    if (!access(link->bsyFile, F_OK))
    {
        Log('6', "Link %u:%u/%u.%u is busy now.\n",
            link->hisAka.zone, link->hisAka.net,
            link->hisAka.node, link->hisAka.point);
        return 0;
    } else
    {
        FILE *fp;
        Debug("trying to create bsy flag for this link...\n");
        fp=fopen(link->bsyFile, "w");
        if (fp == NULL)
        {
            Log('8', "Can't create bsyFile for %u:%u/%u.%u, errno=%d\n",
                link->hisAka.zone, link->hisAka.net,
                link->hisAka.node, link->hisAka.point,
                errno);
            Debug("couldn't create, errno=%d", errno);
            return 0;
        }
        fprintf(fp, "%d", getpid());
        fclose(fp);
        Debug("bsy flag %s created successfully.\n", link->bsyFile);
    }
    return 1;
}

void removeBsy(s_link *link)
{
    Debug("removing bsy file for this link...\n");
    if (access(link->bsyFile, F_OK)) {
        Debug("can't acces bsy file %s, errno=%d. returning.\n", link->bsyFile, errno);
        return;
    }
    if(remove(link->bsyFile))
    {
       Log('9', "Can't remove bsyFile for %u:%u/%u.%u, errno=%d\n",
                link->hisAka.zone, link->hisAka.net,
                link->hisAka.node, link->hisAka.point,
           errno);
       Debug("can't remove, errno=%d.\n", errno);
    }
}

void packNetMailForLink(s_link *link)
{
    int flavour=0;
    int retval;
    char *pktname=NULL;
    char *execstr=NULL;
    char *bsoNetMail=NULL;   // /outb.12b/12345678.pnt/12345678.hut
    char *outbForLink=NULL; // /outb.12b/12345678.pnt/12345678.
    char *dir=NULL;          // /outb.12b/12345678.pnt
    unsigned long nmSize=0;


    Debug("packNetmail for this link is on, trying to pack...\n");
    if (link->packerDef == NULL)
    {
        Log('9', "Packer for link %u:%u/%u.%u undefined but 'packNetmail' is on, skipping...\n",
            link->hisAka.zone, link->hisAka.net,
            link->hisAka.node, link->hisAka.point
           );
        Debug("packer for this link is undefined, returning.\n");
        return;
    }

    outbForLink=initLink(link);
    dir=(char *)smalloc(strlen(outbForLink)-9); // strlen(outb)-10 + '\0';
    memset(dir, 0, strlen(outbForLink)-9);
    strncpy(dir, outbForLink, strlen(outbForLink)-10);
    if (access(dir, F_OK))
    {
        Debug("directory %s doesn't exist, skipping...\n", dir);
        nfree(dir);
        releaseLink(link, &outbForLink);
        return;
    }
    nfree(dir);

    if (!createBsy(link, outbForLink)) {
        releaseLink(link, &outbForLink);
        return;
    }

    pktname=(char *)smalloc(9);
    execstr=(char *)smalloc(MAXPATH);
    bsoNetMail=(char *)smalloc(strlen(outbForLink)+4);

    nmSize=getNMSizeForLink(link, outbForLink);

    if ((nmSize!=0) && (nmSize >= (link->maxUnpackedNetmail*1024)))
    {
        Log('5', "Found %lu bytes of netmail for %u:%u/%u.%u\n",
            nmSize, link->hisAka.zone, link->hisAka.net,
            link->hisAka.node, link->hisAka.point
           );

        for (flavour=0;flavour<5;flavour++)
        {
            sprintf(bsoNetMail, "%s%cut", outbForLink, outext[flavour]);

            if (!access(bsoNetMail, F_OK))
                if (createPktName(&pktname))
                {
                    sprintf(link->pktFile, "%s%s.pkt", 
		    fidoConfig->tempOutbound, pktname);

                    Debug("found netmail packet %s, moving to %s\n",
                          bsoNetMail, link->pktFile);

                    if (rename(bsoNetMail, link->pktFile))
                    {
                        Log('9', "Error renaming %s to %s\n", bsoNetMail, link->pktFile);
                        Debug("can't move, errno=%d. exiting!\n", errno);
                        releaseLink(link, &outbForLink);
                        exit(-1);
                    }

                    getBundleName(link, flavour, outbForLink); // calculating bundleName;
                    fillCmdStatement(execstr, link->packerDef->call,
                                     link->packFile, link->pktFile, "");
                    Log('6', "Packing %s -> %s\n", bsoNetMail, link->packFile);
                    Debug("executing packer: %s\n", execstr);
                    if ((retval=system(execstr))!=0)
                    {
                        Log('9', "Error executing packer, errorlevel %d\n", retval);
                        Debug("packer returned errorlevel %d, errno=%d\n", retval, errno);
                        releaseLink(link, &outbForLink);
                        exit(-1);
                    }

                    if (addToFlow(link, flavour, outbForLink))
                        if(remove(link->pktFile)==-1)
                            Log('9', "Can't remove pktFile %s, errno=%d\n",
                                link->pktFile, errno);
                } // if (createPktName(pktname))
        } // for()
    } // if (nmSize...)
    else
    {
        if (nmSize)
            Log('5', "Found %lu bytes of netmail for %u:%u/%u.%u\n",
                nmSize, link->hisAka.zone, link->hisAka.net,
                link->hisAka.node, link->hisAka.point
               );
    }


    removeBsy(link);
    releaseLink(link, &outbForLink);
    nfree(pktname);
    nfree(execstr);
    nfree(bsoNetMail);
}

