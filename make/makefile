# Generic Makefile for bsopack

include ../../huskymak.cfg

ifeq ($(DEBUG), 1)
  CFLAGS = -I$(INCDIR) $(DEBCFLAGS) $(WARNFLAGS)
  LFLAGS = $(DEBLFLAGS)
else
  CFLAGS = -I$(INCDIR) $(OPTCFLAGS) $(WARNFLAGS)
  LFLAGS = $(OPTLFLAGS)
endif

ifeq ($(SHORTNAME), 1)
  LIBS  = -L$(LIBDIR) -lfidoconf -lsmapi
else
  LIBS  = -L$(LIBDIR) -lfidoconfig -lsmapi
endif

CDEFS=-D$(OSTYPE) -DUNAME=\"$(UNAME)\" $(ADDCDEFS)

SRC_DIR=../src/

OBJS= log.o config.o bsoutil.o bsopack.o

all: bsopack

bsopack: $(OBJS)
		gcc $(OBJS) $(LFLAGS) $(LIBS) -o bsopack


%.o: $(SRC_DIR)%.c
	$(CC) $(CFLAGS) $(CDEFS) -c $<
        

clean:
		rm -f *.o *~

distclean: clean
	rm bsopack

install: bsopack
#	install -s -o uucp -g uucp -m 4750 bsopack /usr/local/bin
	$(INSTALL) bsopack $(BINDIR)
        
