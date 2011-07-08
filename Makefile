# 2.4 Makefile for restart.o

# $Id: Makefile,v 1.1.1.1 2005/08/25 16:49:34 pferguso Exp $



TARGET  := restart
WARN    := -W -Wall -Wstrict-prototypes -Wmissing-prototypes
INCLUDE := -isystem /lib/modules/`uname -r`/build/include
CFLAGS  := -O2 -DMODULE -D__KERNEL__ ${WARN} ${INCLUDE}
CC      := gcc323
	
${TARGET}.o: ${TARGET}.c

.PHONY: clean

clean:
	rm -rf ${TARGET}.o
