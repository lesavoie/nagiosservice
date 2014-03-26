#! /bin/make

IDIR =../include
CC=gcc
CFLAGS=-I$(IDIR) -Wall -std=gnu99 -g

ODIR=.
_DEPS = monitor.h merrors.h mconnection.h cconnection.h controller.h log.h interpreter.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_MOBJ = monitor.o mconnection.o log.o interpreter.o 
MOBJ = $(patsubst %,$(ODIR)/%,$(_MOBJ))

_COBJ = controller.o cconnection.o log.o interpreter.o
COBJ = $(patsubst %,$(ODIR)/%,$(_COBJ))

all: monitor controller

monitor: $(MOBJ)
	gcc -o $@ $^ $(CFLAGS)

controller: $(COBJ)
	gcc -o $@ $^ $(CFLAGS)

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


.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ monitor controller 
