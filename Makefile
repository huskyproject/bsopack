# Generic Makefile for bsopack

ifeq ($(DEBIAN), 1)
# Every Debian-Source-Paket has one included.
include /usr/share/husky/huskymak.cfg
else
include ../huskymak.cfg
endif

ifeq ($(DEBUG), 1)
  CFLAGS = -I$(INCDIR) -Ih $(DEBCFLAGS) $(WARNFLAGS)
  LFLAGS = $(DEBLFLAGS)
else
  CFLAGS = -I$(INCDIR) -Ih $(OPTCFLAGS) $(WARNFLAGS)
  LFLAGS = $(OPTLFLAGS)
endif

LIBS  = -L$(LIBDIR) -lfidoconfig -lhusky

ifeq ($(USE_HPTZIP), 1)
  LIBS+= -lhptzip -lz
  CFLAGS += -DUSE_HPTZIP
endif

CDEFS=-D$(OSTYPE) -DUNAME=\"$(UNAME)\" $(ADDCDEFS)

SRC_DIR=./src/

OBJS= config.o bsoutil.o bsopack.o

bsopack: $(OBJS)
		$(CC) $(OBJS) $(LFLAGS) $(LIBS) -o bsopack


%.o: $(SRC_DIR)%.c
		$(CC) $(CFLAGS) $(CDEFS) -c $<
     
info:
	makeinfo --no-split bsopack.texi

html:
	export LC_ALL=C; makeinfo --html --no-split bsopack.texi

docs: info html

FORCE:

man: FORCE
	gzip -9c man/bsopack.1 > bsopack.1.gz

clean:
		rm -f *.o *~ src/*.o src/*~

distclean: clean
	-$(RM) $(RMOPT) bsopack
	-$(RM) $(RMOPT) bsopack.info
	-$(RM) $(RMOPT) bsopack.html
	-$(RM) $(RMOPT) bsopack.1.gz

all: bsopack docs man

install: all
	$(INSTALL) $(IBOPT) bsopack $(DESTDIR)$(BINDIR)
ifdef INFODIR
	-$(MKDIR) $(MKDIROPT) $(DESTDIR)$(INFODIR)
	$(INSTALL) $(IMOPT) bsopack.info $(DESTDIR)$(INFODIR)
	-install-info --info-dir=$(INFODIR)  $(DESTDIR)$(INFODIR)$(DIRSEP)bsopack.info
endif
ifdef HTMLDIR
	-$(MKDIR) $(MKDIROPT) $(DESTDIR)$(HTMLDIR)
	$(INSTALL) $(IMOPT) bsopack*html $(DESTDIR)$(HTMLDIR)
endif
ifdef MANDIR
	-$(MKDIR) $(MKDIROPT) $(DESTDIR)$(MANDIR)$(DIRSEP)man1
	$(INSTALL) $(IMOPT) bsopack.1.gz $(DESTDIR)$(MANDIR)$(DIRSEP)man1
endif

uninstall:
	$(RM) $(RMOPT) $(BINDIR)$(DIRSEP)bsopack$(EXE)
ifdef INFODIR
	$(RM) $(RMOPT) $(INFODIR)$(DIRSEP)bsopack.info
endif
ifdef HTMLDIR
	$(RM) $(RMOPT) $(HTMLDIR)$(DIRSEP)bsopack.html
endif
ifdef MANDIR
	$(RM) $(RMOPT) $(MANDIR)$(DIRSEP)man1$(DIRSEP)bsopack.1.gz
endif

