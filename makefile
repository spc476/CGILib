#-------------------------------------------------------------------
#
# Common Gateway Interface Library Verion 4.0
# By Sean Conner.
#
#-------------------------------------------------------------------

SHELL=/bin/sh
CC=cc -DCGILIB -DSTDCGI -Wall -pedantic -ansi
#CC=cc -DCGILIB -DSTDCGI 
#CFLAGS=-Wtraditional -Wpointer-arith -Wshadow -Wwrite-strings -Wstrict-prototypes -Wmissing-prototypes -Wcast-qual -Waggregate-return -Wmissing-declarations -Wnested-externs -Winline -W
#CFLAGS=-O4 -fomit-frame-pointer -DSCREAM -DNOSTATS
#CFLAGS=-O4 -DSCREAM -DNOSTATS -pg -g
#CFLAGS=-O4 -fomit-frame-pointer -DSCREAM
#CFLAGS=-O3 -fomit-frame-pointer
#CFLAGS=-march=pentium3 -O3 -fomit-frame-pointer 
#CFLAGS=-pg -g -DNOSTATS -DSCREAM -O4
#CFLAGS=-pg -g -DSCREAM -O4
#CFLAGS=-g -DDDT
CFLAGS=-g
#CFLAGS=-g -pg
AR=ar cr
RANLIB=ranlib

TARGET=$(HOSTDIR)/libcgi4.a

OFILES=$(HOSTDIR)/globals.o			\
	$(HOSTDIR)/nodelist.o 			\
	$(HOSTDIR)/ddt.o			\
	$(HOSTDIR)/util.o			\
	$(HOSTDIR)/sio.o			\
	$(HOSTDIR)/pair.o			\
	$(HOSTDIR)/cgi.o			\
	$(HOSTDIR)/http.o			\
	$(HOSTDIR)/htmltok.o			\
	$(HOSTDIR)/match.o			\
	$(HOSTDIR)/memory.o			\
		$(HOSTDIR)/mem.memmine.o	\
		$(HOSTDIR)/mem.memstdc.o	\
	$(HOSTDIR)/url.o			\
		$(HOSTDIR)/url.file.o		\
		$(HOSTDIR)/url.http.o		\
	$(HOSTDIR)/buffer.o			\
		$(HOSTDIR)/buf.dynamic.o	\
		$(HOSTDIR)/buf.null.o		\
		$(HOSTDIR)/buf.ufile.o		\
		$(HOSTDIR)/buf.line.o		\
		$(HOSTDIR)/buf.memory.o		\
		$(HOSTDIR)/buf.entityout.o	\
	$(HOSTDIR)/errors.o			\
	$(HOSTDIR)/clean.o			\
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

$(HOSTDIR)/mem.memmine.o : src/Memory/memmine.c src/memory.h
	$(CC) $(CFLAGS) -c -o $@ src/Memory/memmine.c

$(HOSTDIR)/mem.memstdc.o : src/Memory/memstdc.c src/memory.h
	$(CC) $(CFLAGS) -c -o $@ src/Memory/memstdc.c

$(HOSTDIR)/util.o : src/util.c src/util.h
	$(CC) $(CFLAGS) -c -o $@ src/util.c

$(HOSTDIR)/ddt.o : src/ddt.c src/ddt.h
	$(CC) $(CFLAGS) -c -o $@ src/ddt.c

$(HOSTDIR)/errors.o : src/errors.c src/errors.h
	$(CC) $(CFLAGS) -c -o $@ src/errors.c

$(HOSTDIR)/clean.o : src/clean.c src/clean.h
	$(CC) $(CFLAGS) -c -o $@ src/clean.c

$(HOSTDIR)/rawfmt.o : src/rawfmt.c src/rawfmt.h
	$(CC) $(CFLAGS) -c -o $@ src/rawfmt.c

$(HOSTDIR)/buffer.o : src/Buffer/buffer.c src/buffer.h
	$(CC) $(CFLAGS) -c -o $@ src/Buffer/buffer.c

$(HOSTDIR)/buf.dynamic.o : src/Buffer/dynamic.c src/buffer.h
	$(CC) $(CFLAGS) -c -o $@ src/Buffer/dynamic.c

$(HOSTDIR)/buf.memory.o : src/Buffer/memory.c src/buffer.h
	$(CC) $(CFLAGS) -c -o $@ src/Buffer/memory.c

$(HOSTDIR)/buf.null.o : src/Buffer/null.c src/buffer.h
	$(CC) $(CFLAGS) -c -o $@ src/Buffer/null.c

$(HOSTDIR)/buf.ufile.o : src/Buffer/ufile.c src/buffer.h
	$(CC) $(CFLAGS) -c -o $@ src/Buffer/ufile.c

$(HOSTDIR)/buf.line.o : src/Buffer/line.c src/buffer.h
	$(CC) $(CFLAGS) -c -o $@ src/Buffer/line.c

$(HOSTDIR)/buf.entityout.o : src/Buffer/entityout.c src/buffer.h
	$(CC) $(CFLAGS) -c -o $@ src/Buffer/entityout.c

$(HOSTDIR)/stream.o : src/Stream/stream.c src/stream.h
	$(CC) $(CFLAGS) -c -o $@ src/Stream/stream.c

$(HOSTDIR)/stream.file.o : src/Stream/file.c src/stream.h
	$(CC) $(CFLAGS) -c -o $@ src/Stream/file.c

$(HOSTDIR)/stream.memory.o : src/Stream/memory.c src/stream.h
	$(CC) $(CFLAGS) -c -o $@ src/Stream/memory.c

$(HOSTDIR)/stream.line.o : src/Stream/line.c src/stream.h
	$(CC) $(CFLAGS) -c -o $@ src/Stream/line.c

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

$(HOSTDIR)/sio.o : src/sio.c src/sio.h
	$(CC) $(CFLAGS) -c -o $@ src/sio.c

$(HOSTDIR)/http.o : src/http.c src/http.h
	$(CC) $(CFLAGS) -c -o $@ src/http.c

$(HOSTDIR)/htmltok.o : src/htmltok.c src/htmltok.h
	$(CC) $(CFLAGS) -c -o $@ src/htmltok.c

$(HOSTDIR)/rfc822.o : src/rfc822.c src/rfc822.h
	$(CC) $(CFLAGS) -c -o $@ src/rfc822.c

$(HOSTDIR)/match.o : src/match.c src/match.h
	$(CC) $(CFLAGS) -c -o $@ src/match.c


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

