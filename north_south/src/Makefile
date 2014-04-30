#! /bin/make

IDIR =../include
CC=gcc
CFLAGS=-I$(IDIR) -Wall -std=gnu99 -g
LFLAGS=-lpthread
LIBS=../libs

ODIR=.
_DEPS = monitor.h merrors.h mconnection.h cconnection.h controller.h log.h interpreter.h mbuffer.h lib.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_MOBJ = monitor.o mconnection.o log.o interpreter.o mbuffer.o lib.o
MOBJ = $(patsubst %,$(ODIR)/%,$(_MOBJ))

_COBJ = controller.o cconnection.o log.o interpreter.o mbuffer.o lib.o
COBJ = $(patsubst %,$(ODIR)/%,$(_COBJ))

_TOBJ = tools.o cconnection.o log.o interpreter.o mbuffer.o lib.o
TOBJ = $(patsubst %,$(ODIR)/%,$(_TOBJ))

all: ns_monitor ns_controller ns_tools

install: ns_monitor ns_controller ns_tools
	mkdir -p /usr/local/bin/
	cp ns_monitor /usr/local/bin/
	cp ns_controller /usr/local/bin/
	cp ns_tools /usr/local/bin/

ns_tools: $(TOBJ)
	gcc -o $@ $^ $(CFLAGS) $(LFLAGS) -L$(LIBS) -lcfu 

ns_monitor: $(MOBJ)
	gcc -o $@ $^ $(CFLAGS) $(LFLAGS) -L$(LIBS) -lcfu

ns_controller: $(COBJ)
	gcc -o $@ $^ $(CFLAGS) $(LFLAGS) -L$(LIBS) -lcfu

$(ODIR)/monitor.o: monitor.c $(DEPS) 
	$(CC) -c $< $(CFLAGS)

$(ODIR)/mconnection.o: mconnection.c $(DEPS)
	$(CC) -c $< $(CFLAGS)

$(ODIR)/cconnection.o: cconnection.c $(DEPS)
	$(CC) -c $< $(CFLAGS)

$(ODIR)/controller.o: controller.c $(DEPS)
	$(CC) -c $< $(CFLAGS)

$(ODIR)/interpreter.o: interpreter.c $(DEPS)
	$(CC) -c $< $(CFLAGS)

$(ODIR)/log.o: log.c $(DEPS)
	$(CC) -c $< $(CFLAGS)

$(ODIR)/mbuffer.o: mbuffer.c $(DEPS)
	$(CC) -c $< $(CFLAGS)

$(ODIR)/lib.o: lib.c $(DEPS)
	$(CC) -c $< $(CFLAGS)

$(ODIR)/tools.o: tools.c $(DEPS)
	$(CC) -c $< $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ monitor controller tools 
