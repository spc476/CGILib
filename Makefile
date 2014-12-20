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

.PHONY: all clean install tarball

DESTLIB    = /usr/local/lib
DESTHEADER = /usr/local/include/cgilib6
CGIVERSION = $(shell git describe --tag)

CC     = gcc -std=c99 -pedantic -Wall -Wextra
CFLAGS = -g 
AR     = ar rscu

override CFLAGS += -DCGIVERSION='"CGILIB/$(CGIVERSION)"'

all : depend build build/src build/src/Url build/libcgi6.a

build/libcgi6.a : build/src/conf.o		\
		build/src/nodelist.o 		\
		build/src/util.o		\
		build/src/pair.o		\
		build/src/cgi.o			\
		build/src/rfc822.o		\
		build/src/htmltok.o		\
		build/src/mail.o		\
		build/src/chunk.o		\
		build/src/bisearch.o		\
		build/src/crashreport.o		\
		build/src/Url/url.o		\
		build/src/Url/http.o		\
		build/src/Url/file.o
	$(AR) $@ $?

build build/src build/src/Url :
	mkdir -p $@

#---------------------------------------------------------------------
# rules to compile source files
#----------------------------------------------------------------------

build/src/%.o : src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	/bin/rm -rf build depend
	/bin/rm -rf `find . -name '*~'`

install:
	install -d $(DESTLIB)
	install -d $(DESTHEADER)
	install build/libcgi6.a $(DESTLIB)
	install src/*.h $(DESTHEADER)

depend:
	makedepend -f- -pbuild/ -- $(CFLAGS) -- src/*.c src/Url/*.c >depend

tarball:
	(cd .. ; tar czvf /tmp/cgilib.tar.gz -X cgi/.exclude cgi/ )

include depend
