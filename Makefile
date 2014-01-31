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

.PHONY: clean install tarball

DESTLIB    = /usr/local/lib
DESTHEADER = /usr/local/include/cgilib6

CC     = gcc -std=c99
CFLAGS = -g -Wall -Wextra -pedantic
AR     = ar cr
RANLIB = ranlib

build/libcgi6.a : build/nodelist.o 		\
		 build/util.o			\
		 build/pair.o			\
		 build/cgi.o			\
		 build/rfc822.o			\
		 build/htmltok.o		\
		 build/mail.o			\
		 build/chunk.o			\
		 build/bisearch.o		\
		 build/crashreport.o		\
		 build/url.url.o		\
		 build/url.http.o		\
		 build/url.file.o
	$(AR) $@ $?
	$(RANLIB) $@

#---------------------------------------------------------------------
# rules to compile source files
#----------------------------------------------------------------------

build/url.%.o : src/Url/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

build/%.o : src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

build/nodelist.o    : src/nodelist.c    src/nodelist.h
build/util.o        : src/util.c        src/util.h
build/cgi.o         : src/cgi.c         src/cgi.h     src/util.h
build/pair.o        : src/pair.c        src/pair.h    src/nodelist.h
build/htmltok.o     : src/htmltok.c     src/htmltok.h src/nodelist.h src/util.h src/pair.h
build/rfc822.o      : src/rfc822.c      src/rfc822.h  src/nodelist.h src/util.h src/pair.h
build/mail.o        : src/mail.c        src/mail.h    src/nodelist.h src/util.h src/pair.h src/rfc822.h
build/chunk.o       : src/chunk.c       src/chunk.h
build/bisearch.o    : src/bisearch.c    src/bisearch.h
build/crashreport.o : src/crashreport.c src/crashreport.h
build/url.url.o     : src/Url/url.c     src/url.h
build/url.http.o    : src/Url/http.c    src/url.h
build/url.file.o    : src/Url/file.c    src/url.h

clean:
	/bin/rm -rf build/*.o
	/bin/rm -rf build/*.a
	/bin/rm -rf src/*~
	/bin/rm -rf src/*/*~
	/bin/rm -rf *~

install:
	install -d $(DESTLIB)
	install -d $(DESTHEADER)
	install build/libcgi6.a $(DESTLIB)
	install src/*.h $(DESTHEADER)

tarball:
	(cd .. ; tar czvf /tmp/cgilib.tar.gz -X cgi/.exclude cgi/ )

