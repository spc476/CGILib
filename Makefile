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

CGIVERSION := $(shell git describe --tag)

CC     = c99 -pedantic -Wall -Wextra
CFLAGS = -g 
AR     = ar rscu

INSTALL         = /usr/bin/install
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA    = $(INSTALL) -m 644

prefix      = /usr/local
includedir  = $(prefix)/include
exec_prefix = $(prefix)
libdir      = $(exec_prefix)/lib

override CFLAGS += -DCGIVERSION='"CGILIB $(CGIVERSION)"'

.PHONY: all clean install tarball depend

all : build               \
	build/src         \
	build/src/Url     \
	build/src/RFC822  \
	build/src/Cgi     \
	build/src/Htmltok \
	build/src/Util	  \
	build/src/Dump    \
	build/src/Pair    \
	build/libcgi6.a

build/libcgi6.a : build/src/conf.o			\
		build/src/nodelist.o 			\
		build/src/Util/ctohex.o			\
		build/src/Util/down_string.o		\
		build/src/Util/empty_string.o		\
		build/src/Util/hextoc.o			\
		build/src/Util/remove_char.o		\
		build/src/Util/trim_lspace.o		\
		build/src/Util/trim_tspace.o		\
		build/src/Util/up_string.o		\
		build/src/Pair/PairCreate.o		\
		build/src/Pair/PairFree.o		\
		build/src/Pair/PairListCreate.o		\
		build/src/Pair/PairListFree.o		\
		build/src/Pair/PairListGetPair.o	\
		build/src/Pair/PairListGetValue.o	\
		build/src/Pair/PairNew.o		\
		build/src/Cgi/UrlEncodeChar.o		\
		build/src/Cgi/UrlEncodeString.o		\
		build/src/Cgi/UrlDecodeChar.o		\
		build/src/Cgi/UrlDecodeString.o		\
		build/src/Cgi/CgiNew.o          	\
		build/src/Cgi/CgiNextValue.o    	\
		build/src/Cgi/CgiListMake.o     	\
		build/src/Cgi/CgiListGetValues.o	\
		build/src/Cgi/CgiListRequired.o		\
		build/src/Cgi/CgiFree.o			\
		build/src/RFC822/RFC822LineRead.o	\
		build/src/RFC822/RFC822HeadersRead.o	\
		build/src/RFC822/RFC822HeaderWrite.o	\
		build/src/RFC822/RFC822HeadersWrite.o	\
		build/src/Htmltok/HtmlParseNew.o	\
		build/src/Htmltok/HtmlParseClone.o	\
		build/src/Htmltok/HtmlParsePrintTag.o	\
		build/src/Htmltok/HtmlParseNext.o	\
		build/src/Htmltok/HtmlParseFree.o	\
		build/src/mail.o			\
		build/src/chunk.o			\
		build/src/bisearch.o			\
		build/src/crashreport.o			\
		build/src/crashreport-posix.o		\
		build/src/tree.o			\
		build/src/Dump/hex.o			\
		build/src/Dump/hexdump_mems.o		\
		build/src/Dump/chardump_mems.o		\
		build/src/Dump/dump_mems.o		\
		build/src/Dump/dump_memorys.o		\
		build/src/Dump/dump_memoryf.o		\
		build/src/Dump/dump_memoryl.o		\
		build/src/Url/url.o			\
		build/src/Url/http.o			\
		build/src/Url/file.o			\
		build/src/Url/gopher.o
	$(AR) $@ $?

build build/src build/src/Url build/src/RFC822 build/src/Cgi build/src/Htmltok build/src/Util build/src/Dump build/src/Pair:
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
	$(INSTALL) -d $(DESTDIR)$(libdir)
	$(INSTALL) -d $(DESTDIR)$(includedir)/cgilib6
	$(INSTALL_PROGRAM) build/libcgi6.a $(DESTDIR)$(libdir)
	$(INSTALL_DATA)    src/*.h $(DESTDIR)$(includedir)/cgilib6

depend:
	makedepend -pbuild/ -Y -- $(CFLAGS) -- src/*.c src/Url/*.c 2>/dev/null

tarball:
	git archive -o /tmp/CGILib-$(CGIVERSION).tar.gz --prefix CGILib $(CGIVERSION)

# DO NOT DELETE

build/src/bisearch.o: src/bisearch.h
build/src/cgi.o: src/util.h src/dump.h src/cgi.h src/nodelist.h src/pair.h
build/src/chunk.o: src/chunk.h
build/src/conf.o: src/conf.h
build/src/crashreport.o: src/dump.h src/crashreport.h
build/src/dump_memoryf.o: src/dump.h
build/src/dump_memoryl.o: src/dump.h
build/src/dump_memorys.o: src/dump.h
build/src/dump_mems.o: src/dump.h
build/src/hex.o: src/dump.h
build/src/hexdump_mems.o: src/dump.h
build/src/htmltok.o: src/nodelist.h src/htmltok.h src/pair.h src/util.h
build/src/htmltok.o: src/dump.h
build/src/mail.o: src/util.h src/dump.h src/rfc822.h src/nodelist.h
build/src/mail.o: src/pair.h src/mail.h
build/src/nodelist.o: src/nodelist.h
build/src/pair.o: src/pair.h src/nodelist.h
build/src/tree.o: src/tree.h
build/src/util.o: src/util.h src/dump.h
build/src/Url/file.o: src/url.h
build/src/Url/gopher.o: src/url.h
build/src/Url/http.o: src/url.h
build/src/Url/url.o: src/url.h
