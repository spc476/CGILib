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
CC=gcc -Wall -pedantic -std=c99
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

TARGET=build/libcgi6.a

OFILES=build/nodelist.o 		\
	build/util.o			\
	build/pair.o			\
	build/cgi.o			\
	build/rfc822.o			\
	build/htmltok.o			\
	build/mail.o			\
	build/url.o			\
	build/url.file.o		\
	build/url.http.o		\
	build/chunk.o

$(TARGET) : $(OFILES)
	$(AR) $(TARGET) $(OFILES)
	$(RANLIB) $(TARGET)

#---------------------------------------------------------------------
# rules to compile source files
#----------------------------------------------------------------------

build/nodelist.o : src/nodelist.c src/nodelist.h
	$(CC) $(CFLAGS) -c -o $@ src/nodelist.c

build/util.o : src/util.c src/util.h
	$(CC) $(CFLAGS) -c -o $@ src/util.c

build/url.o : src/Url/url.c src/url.h
	$(CC) $(CFLAGS) -c -o $@ src/Url/url.c

build/url.file.o : src/Url/file.c src/url.h
	$(CC) $(CFLAGS) -c -o $@ src/Url/file.c

build/url.http.o : src/Url/http.c src/url.h
	$(CC) $(CFLAGS) -c -o $@ src/Url/http.c

build/cgi.o : src/cgi.c src/cgi.h
	$(CC) $(CFLAGS) -c -o $@ src/cgi.c

build/pair.o : src/pair.c src/pair.h
	$(CC) $(CFLAGS) -c -o $@ src/pair.c

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
	/bin/rm -rf build/*.o
	/bin/rm -rf build/*.a
	/bin/rm -rf src/*~
	/bin/rm -rf src/*/*~
	/bin/rm -rf *~

install:
	install -d $(DESTLIB)
	install -d $(DESTHEADER)
	install $(TARGET) $(DESTLIB)
	install src/*.h $(DESTHEADER)

tarball:
	(cd .. ; tar czvf /tmp/cgilib.tar.gz -X cgi/.exclude cgi/ )

