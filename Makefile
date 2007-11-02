#-------------------------------------------------------------------
#
# Common Gateway Interface Library Verion 5.0
# By Sean Conner.
# All Rights Reserved.
#
#-------------------------------------------------------------------

DESTLIB=/usr/local/lib
DESTHEADER=/usr/local/include/cgilib

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

TARGET=obj/libcgi5.a

OFILES=obj/globals.o			\
	obj/nodelist.o 			\
	obj/ddt.o			\
	obj/util.o			\
	obj/pair.o			\
	obj/cgi.o			\
	obj/http.o			\
	obj/htmltok.o			\
	obj/memory.o			\
		obj/mem.memstdc.o	\
	obj/url.o			\
		obj/url.file.o		\
		obj/url.http.o		\
	obj/stream.o			\
		obj/stream.file.o 	\
		obj/stream.memory.o 	\
		obj/stream.line.o	\
		obj/stream.string.o	\
		obj/stream.tee.o		\
		obj/stream.entity.o	\
		obj/stream.bundle.o	\
	obj/sio.o			\
		obj/sio.file.o		\
		obj/sio.memory.o		\
		obj/sio.bundle.o		\
	obj/rfc822.o			\
	obj/mail.o			\
	obj/chunk.o			\
	obj/rawfmt.o

$(TARGET) : $(OFILES)
	$(AR) $(TARGET) $(OFILES)
	$(RANLIB) $(TARGET)

#---------------------------------------------------------------------
# rules to compile source files
#----------------------------------------------------------------------

obj/ntest : obj/ntest.o $(OFILES)
	$(CC) $(CFLAGS) -o obj/ntest obj/ntest.o $(OFILES)

obj/ntest.o : src/ntest.c
	$(CC) $(CFLAGS) -c -o $@ src/ntest.c

obj/globals.o : src/globals.c
	$(CC) $(CFLAGS) -c -o $@ src/globals.c

obj/nodelist.o : src/nodelist.c src/nodelist.h
	$(CC) $(CFLAGS) -c -o $@ src/nodelist.c

obj/memory.o : src/Memory/memory.c src/memory.h
	$(CC) $(CFLAGS) -c -o $@ src/Memory/memory.c

obj/mem.memstdc.o : src/Memory/memstdc.c src/memory.h
	$(CC) $(CFLAGS) -c -o $@ src/Memory/memstdc.c

obj/util.o : src/util.c src/util.h
	$(CC) $(CFLAGS) -c -o $@ src/util.c

obj/ddt.o : src/ddt.c src/ddt.h
	$(CC) $(CFLAGS) -c -o $@ src/ddt.c

obj/rawfmt.o : src/rawfmt.c src/rawfmt.h
	$(CC) $(CFLAGS) -c -o $@ src/rawfmt.c

obj/stream.o : src/Stream/stream.c src/stream.h
	$(CC) $(CFLAGS) -c -o $@ src/Stream/stream.c

obj/stream.file.o : src/Stream/file.c src/stream.h
	$(CC) $(CFLAGS) -c -o $@ src/Stream/file.c

obj/sio.o : src/SIO/sio.c src/sio.h
	$(CC) $(CFLAGS) -c -o $@ src/SIO/sio.c

obj/sio.file.o : src/SIO/file.c src/sio.h
	$(CC) $(CFLAGS) -c -o $@ src/SIO/file.c

obj/sio.memory.o : src/SIO/memory.c src/sio.h
	$(CC) $(CFLAGS) -c -o $@ src/SIO/memory.c

obj/sio.bundle.o : src/SIO/bundle.c src/sio.h
	$(CC) $(CFLAGS) -c -o $@ src/SIO/bundle.c

obj/stream.memory.o : src/Stream/memory.c src/stream.h
	$(CC) $(CFLAGS) -c -o $@ src/Stream/memory.c

obj/stream.line.o : src/Stream/line.c src/stream.h
	$(CC) $(CFLAGS) -c -o $@ src/Stream/line.c

obj/stream.string.o : src/Stream/string.c src/stream.h
	$(CC) $(CFLAGS) -c -o $@ src/Stream/string.c

obj/stream.entity.o : src/Stream/entity.c src/stream.h
	$(CC) $(CFLAGS) -c -o $@ src/Stream/entity.c

obj/stream.tee.o : src/Stream/tee.c src/stream.h
	$(CC) $(CFLAGS) -c -o $@ src/Stream/tee.c

obj/stream.bundle.o : src/Stream/bundle.c src/stream.h
	$(CC) $(CFLAGS) -c -o $@ src/Stream/bundle.c

obj/url.o : src/Url/url.c src/url.h
	$(CC) $(CFLAGS) -c -o $@ src/Url/url.c

obj/url.file.o : src/Url/file.c src/url.h
	$(CC) $(CFLAGS) -c -o $@ src/Url/file.c

obj/url.http.o : src/Url/http.c src/url.h
	$(CC) $(CFLAGS) -c -o $@ src/Url/http.c

obj/cgi.o : src/Cgi/cgi.c src/cgi.h
	$(CC) $(CFLAGS) -c -o $@ src/Cgi/cgi.c

obj/pair.o : src/pair.c src/pair.h
	$(CC) $(CFLAGS) -c -o $@ src/pair.c

obj/http.o : src/http.c src/http.h
	$(CC) $(CFLAGS) -c -o $@ src/http.c

obj/htmltok.o : src/htmltok.c src/htmltok.h
	$(CC) $(CFLAGS) -c -o $@ src/htmltok.c

obj/rfc822.o : src/rfc822.c src/rfc822.h
	$(CC) $(CFLAGS) -c -o $@ src/rfc822.c

obj/mail.o : src/mail.c src/mail.h
	$(CC) $(CFLAGS) -c -o $@ src/mail.c

obj/chunk.o : src/chunk.c src/chunk.h
	$(CC) $(CFLAGS) -c -o $@ src/chunk.c

debug:
	make -f make.debug

clean:
	/bin/rm obj/*
	/bin/rm src/*~
	/bin/rm src/*/*~
	/bin/rm cgi4.tar.gz

install:
	install -d $(DESTLIB)
	install -d $(DESTHEADER)
	install $(TARGET) $(DESTLIB)
	install src/*.h $(DESTHEADER)

tarball:
	(cd .. ; tar czvf /tmp/cgilib.tar.gz -X cgi/.exclude cgi/ )

