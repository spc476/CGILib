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
#CFLAGS=-march=pentium3 -O3 -fomit-frame-pointer -DSCREAM
#CFLAGS=-pg -g -DSCREAM -O4
#CFLAGS=-g -DDDT
CFLAGS=-g
#CFLAGS=-g -pg
AR=ar cr
RANLIB=ranlib

TARGET=build/libcgi5.a

OFILES=build/globals.o			\
	build/nodelist.o 			\
	build/ddt.o			\
	build/util.o			\
	build/pair.o			\
	build/cgi.o			\
	build/http.o			\
	build/htmltok.o			\
	build/memory.o			\
		build/mem.memstdc.o	\
	build/url.o			\
		build/url.file.o		\
		build/url.http.o		\
	build/stream.o			\
		build/stream.file.o 	\
		build/stream.memory.o 	\
		build/stream.line.o	\
		build/stream.string.o	\
		build/stream.tee.o		\
		build/stream.entity.o	\
		build/stream.bundle.o	\
	build/sio.o			\
		build/sio.file.o		\
		build/sio.memory.o		\
		build/sio.bundle.o		\
	build/rfc822.o			\
	build/mail.o			\
	build/chunk.o			\
	build/rawfmt.o

$(TARGET) : $(OFILES)
	$(AR) $(TARGET) $(OFILES)
	$(RANLIB) $(TARGET)

#---------------------------------------------------------------------
# rules to compile source files
#----------------------------------------------------------------------

build/ntest : build/ntest.o $(OFILES)
	$(CC) $(CFLAGS) -o build/ntest build/ntest.o $(OFILES)

build/ntest.o : src/ntest.c
	$(CC) $(CFLAGS) -c -o $@ src/ntest.c

build/globals.o : src/globals.c
	$(CC) $(CFLAGS) -c -o $@ src/globals.c

build/nodelist.o : src/nodelist.c src/nodelist.h
	$(CC) $(CFLAGS) -c -o $@ src/nodelist.c

build/memory.o : src/Memory/memory.c src/memory.h
	$(CC) $(CFLAGS) -c -o $@ src/Memory/memory.c

build/mem.memstdc.o : src/Memory/memstdc.c src/memory.h
	$(CC) $(CFLAGS) -c -o $@ src/Memory/memstdc.c

build/util.o : src/util.c src/util.h
	$(CC) $(CFLAGS) -c -o $@ src/util.c

build/ddt.o : src/ddt.c src/ddt.h
	$(CC) $(CFLAGS) -c -o $@ src/ddt.c

build/rawfmt.o : src/rawfmt.c src/rawfmt.h
	$(CC) $(CFLAGS) -c -o $@ src/rawfmt.c

build/stream.o : src/Stream/stream.c src/stream.h
	$(CC) $(CFLAGS) -c -o $@ src/Stream/stream.c

build/stream.file.o : src/Stream/file.c src/stream.h
	$(CC) $(CFLAGS) -c -o $@ src/Stream/file.c

build/sio.o : src/SIO/sio.c src/sio.h
	$(CC) $(CFLAGS) -c -o $@ src/SIO/sio.c

build/sio.file.o : src/SIO/file.c src/sio.h
	$(CC) $(CFLAGS) -c -o $@ src/SIO/file.c

build/sio.memory.o : src/SIO/memory.c src/sio.h
	$(CC) $(CFLAGS) -c -o $@ src/SIO/memory.c

build/sio.bundle.o : src/SIO/bundle.c src/sio.h
	$(CC) $(CFLAGS) -c -o $@ src/SIO/bundle.c

build/stream.memory.o : src/Stream/memory.c src/stream.h
	$(CC) $(CFLAGS) -c -o $@ src/Stream/memory.c

build/stream.line.o : src/Stream/line.c src/stream.h
	$(CC) $(CFLAGS) -c -o $@ src/Stream/line.c

build/stream.string.o : src/Stream/string.c src/stream.h
	$(CC) $(CFLAGS) -c -o $@ src/Stream/string.c

build/stream.entity.o : src/Stream/entity.c src/stream.h
	$(CC) $(CFLAGS) -c -o $@ src/Stream/entity.c

build/stream.tee.o : src/Stream/tee.c src/stream.h
	$(CC) $(CFLAGS) -c -o $@ src/Stream/tee.c

build/stream.bundle.o : src/Stream/bundle.c src/stream.h
	$(CC) $(CFLAGS) -c -o $@ src/Stream/bundle.c

build/url.o : src/Url/url.c src/url.h
	$(CC) $(CFLAGS) -c -o $@ src/Url/url.c

build/url.file.o : src/Url/file.c src/url.h
	$(CC) $(CFLAGS) -c -o $@ src/Url/file.c

build/url.http.o : src/Url/http.c src/url.h
	$(CC) $(CFLAGS) -c -o $@ src/Url/http.c

build/cgi.o : src/Cgi/cgi.c src/cgi.h
	$(CC) $(CFLAGS) -c -o $@ src/Cgi/cgi.c

build/pair.o : src/pair.c src/pair.h
	$(CC) $(CFLAGS) -c -o $@ src/pair.c

build/http.o : src/http.c src/http.h
	$(CC) $(CFLAGS) -c -o $@ src/http.c

build/htmltok.o : src/htmltok.c src/htmltok.h
	$(CC) $(CFLAGS) -c -o $@ src/htmltok.c

build/rfc822.o : src/rfc822.c src/rfc822.h
	$(CC) $(CFLAGS) -c -o $@ src/rfc822.c

build/mail.o : src/mail.c src/mail.h
	$(CC) $(CFLAGS) -c -o $@ src/mail.c

build/chunk.o : src/chunk.c src/chunk.h
	$(CC) $(CFLAGS) -c -o $@ src/chunk.c

debug:
	make -f make.debug

clean:
	/bin/rm build/*.o
	/bin/rm build/*.a
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

