# Generic Makefile for bsopack

include ../huskymak.cfg

SRCDIR=./

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

CDEFS=-D$(OSTYPE) $(ADDCDEFS)


OBJS= log.o config.o bsoutil.o bsopack.o

all: bsopack

bsopack: $(OBJS)
		gcc $(OBJS) $(LFLAGS) $(LIBS) -o bsopack


%.o: $(SRCDIR)%.c
	$(CC) $(CFLAGS) $(CDEFS) -c $<
        

clean:
		rm -f *.o *~ bsopack config

install: bsopack
#	install -s -o uucp -g uucp -m 4750 bsopack /usr/local/bin
	$(INSTALL) bsopack $(BINDIR)
        
