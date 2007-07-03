#-------------------------------------------------------------------
#
# Common Gateway Interface Library Verion 5.0
# By Sean Conner.
# All Rights Reserved.
#
#-------------------------------------------------------------------

SHELL=/bin/sh
CC=gcc -DCGILIB -DSTDCGI -Wall -pedantic -ansi
#CC=cc -DCGILIB -DSTDCGI 
#CFLAGS=-Wpointer-arith -Wshadow -Wwrite-strings -Wmissing-prototypes -Wcast-qual -Wmissing-declarations -Wnested-externs -Winline -W
#CFLAGS=-Wtraditional -Wpointer-arith -Wshadow -Wwrite-strings -Wstrict-prototypes -Wmissing-prototypes -Wcast-qual -Waggregate-return -Wmissing-declarations -Wnested-externs -Winline -W
#CFLAGS=-O4 -fomit-frame-pointer -DSCREAM
#CFLAGS=-O4 -DSCREAM -pg -g
#CFLAGS=-O4 -fomit-frame-pointer -DSCREAM
#CFLAGS=-O3 -fomit-frame-pointer -DSCREAM
#CFLAGS=-O3 -fomit-frame-pointer
#CFLAGS=-march=pentium3 -O3 -fomit-frame-pointer 
#CFLAGS=-pg -g -DSCREAM -O4
#CFLAGS=-g -DDDT
CFLAGS=-g
#CFLAGS=-g -pg
AR=ar cr
RANLIB=ranlib

TARGET=$(HOSTDIR)/libcgi5.a

OFILES=$(HOSTDIR)/globals.o			\
	$(HOSTDIR)/nodelist.o 			\
	$(HOSTDIR)/ddt.o			\
	$(HOSTDIR)/util.o			\
	$(HOSTDIR)/pair.o			\
	$(HOSTDIR)/cgi.o			\
	$(HOSTDIR)/http.o			\
	$(HOSTDIR)/htmltok.o			\
	$(HOSTDIR)/memory.o			\
		$(HOSTDIR)/mem.memstdc.o	\
	$(HOSTDIR)/url.o			\
		$(HOSTDIR)/url.file.o		\
		$(HOSTDIR)/url.http.o		\
	$(HOSTDIR)/stream.o			\
		$(HOSTDIR)/stream.file.o 	\
		$(HOSTDIR)/stream.memory.o 	\
		$(HOSTDIR)/stream.line.o	\
		$(HOSTDIR)/stream.string.o	\
		$(HOSTDIR)/stream.tee.o		\
		$(HOSTDIR)/stream.entity.o	\
		$(HOSTDIR)/stream.bundle.o	\
	$(HOSTDIR)/rfc822.o			\
	$(HOSTDIR)/mail.o			\
	$(HOSTDIR)/rawfmt.o

$(TARGET) : $(OFILES)
	$(AR) $(TARGET) $(OFILES)
	$(RANLIB) $(TARGET)

#---------------------------------------------------------------------
# rules to compile source files
#----------------------------------------------------------------------

$(HOSTDIR)/ntest : $(HOSTDIR)/ntest.o $(OFILES)
	$(CC) $(CFLAGS) -o $(HOSTDIR)/ntest $(HOSTDIR)/ntest.o $(OFILES)

$(HOSTDIR)/ntest.o : src/ntest.c
	$(CC) $(CFLAGS) -c -o $@ src/ntest.c

$(HOSTDIR)/globals.o : src/globals.c
	$(CC) $(CFLAGS) -c -o $@ src/globals.c

$(HOSTDIR)/nodelist.o : src/nodelist.c src/nodelist.h
	$(CC) $(CFLAGS) -c -o $@ src/nodelist.c

$(HOSTDIR)/memory.o : src/Memory/memory.c src/memory.h
	$(CC) $(CFLAGS) -c -o $@ src/Memory/memory.c

$(HOSTDIR)/mem.memstdc.o : src/Memory/memstdc.c src/memory.h
	$(CC) $(CFLAGS) -c -o $@ src/Memory/memstdc.c

$(HOSTDIR)/util.o : src/util.c src/util.h
	$(CC) $(CFLAGS) -c -o $@ src/util.c

$(HOSTDIR)/ddt.o : src/ddt.c src/ddt.h
	$(CC) $(CFLAGS) -c -o $@ src/ddt.c

$(HOSTDIR)/rawfmt.o : src/rawfmt.c src/rawfmt.h
	$(CC) $(CFLAGS) -c -o $@ src/rawfmt.c

$(HOSTDIR)/stream.o : src/Stream/stream.c src/stream.h
	$(CC) $(CFLAGS) -c -o $@ src/Stream/stream.c

$(HOSTDIR)/stream.file.o : src/Stream/file.c src/stream.h
	$(CC) $(CFLAGS) -c -o $@ src/Stream/file.c

$(HOSTDIR)/stream.memory.o : src/Stream/memory.c src/stream.h
	$(CC) $(CFLAGS) -c -o $@ src/Stream/memory.c

$(HOSTDIR)/stream.line.o : src/Stream/line.c src/stream.h
	$(CC) $(CFLAGS) -c -o $@ src/Stream/line.c

$(HOSTDIR)/stream.string.o : src/Stream/string.c src/stream.h
	$(CC) $(CFLAGS) -c -o $@ src/Stream/string.c

$(HOSTDIR)/stream.entity.o : src/Stream/entity.c src/stream.h
	$(CC) $(CFLAGS) -c -o $@ src/Stream/entity.c

$(HOSTDIR)/stream.tee.o : src/Stream/tee.c src/stream.h
	$(CC) $(CFLAGS) -c -o $@ src/Stream/tee.c

$(HOSTDIR)/stream.bundle.o : src/Stream/bundle.c src/stream.h
	$(CC) $(CFLAGS) -c -o $@ src/Stream/bundle.c

$(HOSTDIR)/url.o : src/Url/url.c src/url.h
	$(CC) $(CFLAGS) -c -o $@ src/Url/url.c

$(HOSTDIR)/url.file.o : src/Url/file.c src/url.h
	$(CC) $(CFLAGS) -c -o $@ src/Url/file.c

$(HOSTDIR)/url.http.o : src/Url/http.c src/url.h
	$(CC) $(CFLAGS) -c -o $@ src/Url/http.c

$(HOSTDIR)/cgi.o : src/Cgi/cgi.c src/cgi.h
	$(CC) $(CFLAGS) -c -o $@ src/Cgi/cgi.c

$(HOSTDIR)/pair.o : src/pair.c src/pair.h
	$(CC) $(CFLAGS) -c -o $@ src/pair.c

$(HOSTDIR)/http.o : src/http.c src/http.h
	$(CC) $(CFLAGS) -c -o $@ src/http.c

$(HOSTDIR)/htmltok.o : src/htmltok.c src/htmltok.h
	$(CC) $(CFLAGS) -c -o $@ src/htmltok.c

$(HOSTDIR)/rfc822.o : src/rfc822.c src/rfc822.h
	$(CC) $(CFLAGS) -c -o $@ src/rfc822.c

$(HOSTDIR)/mail.o : src/mail.c src/mail.h
	$(CC) $(CFLAGS) -c -o $@ src/mail.c

debug:
	make -f make.debug

clean:
	/bin/rm $(HOSTDIR)/*
	/bin/rm src/*~
	/bin/rm src/*/*~
	/bin/rm cgi4.tar.gz

package:
	make -i clean
	tar czvf cgi4.tar.gz makefile library.doc src

