#########################################################################
#
# Copyright 2001,2013 by Sean Conner.  All Rights Reserved.
#
# This library is free software; you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; either version 3 of the License, or (at your
# option) any later version.
#
# This library is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
# License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library; if not, see <http://www.gnu.org/licenses/>.
#
# Comments, questions and criticisms can be sent to: sean@conman.org
#
########################################################################

DESTLIB    = /usr/local/lib
DESTHEADER = /usr/local/include/cgilib6

CC     = gcc -std=c99
CFLAGS = -g -Wall -Wextra -pedantic
AR     = ar cr
RANLIB = ranlib

TARGET = build/libcgi6.a

OFILES = build/nodelist.o 		\
	 build/util.o			\
	 build/pair.o			\
	 build/cgi.o			\
	 build/rfc822.o			\
	 build/htmltok.o		\
	 build/mail.o			\
	 build/chunk.o			\
	 build/bisearch.o		\
	 build/crashreport.o		\
	 build/url.o			\
	 build/url.http.o		\
	 build/url.file.o

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

build/bisearch.o : src/bisearch.c src/bisearch.h
	$(CC) $(CFLAGS) -c -o $@ src/bisearch.c

build/crashreport.o : src/crashreport.c src/crashreport.h
	$(CC) $(CFLAGS) -c -o $@ src/crashreport.c

build/url.o : src/Url/url.c src/url.h
	$(CC) $(CFLAGS) -c -o $@ $<
	
build/url.http.o : src/Url/http.c src/url.h
	$(CC) $(CFLAGS) -c -o $@ $<

build/url.file.o : src/Url/file.c src/url.h
	$(CC) $(CFLAGS) -c -o $@ $<

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

