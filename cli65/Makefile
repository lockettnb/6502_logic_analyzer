#
#  makefile for 6502 In-Circuit Emulation 
#
#

#CFLAGS =  -g -DDEBUG
CFLAGS =  -g 
CC = gcc 

INSTALL_DIR = /home/john/bin
# MANPATH=/usr/local/man/man1

SRCS	= teensy_cli65.c serial.c 
BINS	= cli65 
OBJS	= teensy_cli65.o serial.o 
MANSRC  = 
MANFILE	= 
HEAD	= 
LIBS	= 

# prg: prg.c makefile $(OBJS) $(HEAD) 
# 	$(CC) -oprg $(CFLAGS) $(OBJS) $(LIBS) 

all: $(BINS)

cli65: teensy_cli65.c teensy_cli65.o serial.c serial.h serial.o
	$(CC) -o cli65 $(CFLAGS) teensy_cli65.o serial.o -lreadline
	cp  $(BINS) ../bin

clean:
	rm -rf $(BINS) $(OBJS)	

install:
	cp $(BINS) $(INSTALL_DIR)
#	cp $(MANSRC) $(MANPATH)/$(MANFILE)

uninstall:
	rm $(INSTALL_DIR)/$(BINS)
#	rm $(MANPATH)/$(MANFILE)
