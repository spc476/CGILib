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

.PHONY: all clean install tarball depend

DESTLIB    = /usr/local/lib
DESTHEADER = /usr/local/include/cgilib6
CGIVERSION = $(shell git describe --tag)

CC     = gcc -std=c99 -pedantic -Wall -Wextra
CFLAGS = -g 
AR     = ar rscu

override CFLAGS += -DCGIVERSION='"CGILIB/$(CGIVERSION)"'

all : build build/src build/src/Url build/libcgi6.a

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
		build/src/tree.o		\
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
	$(RM) -r build
	$(RM) `find . -name '*~'`
	$(RM) Makefile.bak

install:
	install -d $(DESTLIB)
	install -d $(DESTHEADER)
	install build/libcgi6.a $(DESTLIB)
	install src/*.h $(DESTHEADER)

depend:
	makedepend -pbuild/ -Y -- $(CFLAGS) -- src/*.c src/Url/*.c 2>/dev/null

tarball:
	git archive -o /tmp/CGILib-$(CGIVERSION).tar.gz --prefix CGILib $(CGIVERSION)

# DO NOT DELETE

build/src/bisearch.o: src/bisearch.h
build/src/cgi.o: src/util.h src/cgi.h src/nodelist.h src/pair.h
build/src/chunk.o: src/chunk.h
build/src/conf.o: src/conf.h
build/src/crashreport.o: src/crashreport.h
build/src/htmltok.o: src/nodelist.h src/htmltok.h src/pair.h src/util.h
build/src/mail.o: src/util.h src/rfc822.h src/nodelist.h src/pair.h
build/src/mail.o: src/mail.h
build/src/nodelist.o: src/nodelist.h
build/src/pair.o: src/pair.h src/nodelist.h
build/src/rfc822.o: src/nodelist.h src/util.h src/rfc822.h src/pair.h
build/src/tree.o: src/tree.h
build/src/util.o: src/util.h
build/src/Url/file.o: src/url.h
build/src/Url/http.o: src/url.h
build/src/Url/url.o: src/url.h
