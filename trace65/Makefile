#
#
#

#CFLAGS =  -g -DDEBUG
CFLAGS =  -g 
CC = gcc 

INSTALL_DIR = /home/john/bin
MANPATH=/usr/local/man/man1

SRCS	= trace.c disassembler.c
BINS	= t65
OBJS	= trace.o disassembler.o
MANSRC  = 
MANFILE	= 
HEAD	= 
LIBS	= 

# prg: prg.c makefile $(OBJS) $(HEAD) 
# 	$(CC) -oprg $(CFLAGS) $(OBJS) $(LIBS) 

all: $(BINS)

t65: trace.o disassembler.o 
	$(CC) -o t65 $(CFLAGS) trace.o disassembler.o 
	cp $(BINS) ../bin/

clean:
	rm -rf $(BINS) $(OBJS)	

install:
	cp $(BINS) $(INSTALL_DIR)
#	cp $(MANSRC) $(MANPATH)/$(MANFILE)

uninstall:
	rm $(INSTALL_DIR)/$(BINS)
#	rm $(MANPATH)/$(MANFILE)
