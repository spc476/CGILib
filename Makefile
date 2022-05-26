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

ifeq ($(CGIVERSION),)
  CGIVERSION=6.11.5
endif

CC     = c99 -pedantic -Wall -Wextra -Wwrite-strings
CFLAGS = -g 

INSTALL         = /usr/bin/install
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA    = $(INSTALL) -m 644

prefix      = /usr/local
includedir  = $(prefix)/include
exec_prefix = $(prefix)
libdir      = $(exec_prefix)/lib

override CFLAGS += -DCGIVERSION='"CGILIB $(CGIVERSION)"'

.PHONY    : clean install depend dist

libcgi6.a : $(patsubst %.c,%.o,$(shell find src -name '*.c'))
	$(AR) $(ARFLAGS) $@ $?

#---------------------------------------------------------------------
# rules to compile source files
#----------------------------------------------------------------------

clean:
	$(RM) $(shell find . -name '*~')
	$(RM) $(shell find . -name '*.o')
	$(RM) libcgi6.a Makefile.bak

install: libcgi6.a
	$(INSTALL) -d $(DESTDIR)$(libdir)
	$(INSTALL) -d $(DESTDIR)$(includedir)/cgilib6
	$(INSTALL_DATA) libcgi6.a $(DESTDIR)$(libdir)
	$(INSTALL_DATA) src/*.h   $(DESTDIR)$(includedir)/cgilib6

uninstall:
	$(RM)    $(DESTDIR)$(libdir)/libcgi6.a
	$(RM) -r $(DESTDIR)$(includedir)/cgilib6

depend:
	makedepend -Y -- $(CFLAGS) -- $(shell find src -name '*.c') 2>/dev/null

dist:
	git archive -o /tmp/CGILib-$(CGIVERSION).tar.gz --prefix CGILib/ $(CGIVERSION)

# DO NOT DELETE

src/Cgi/CgiListGetValues.o: src/nodelist.h src/pair.h src/nodelist.h
src/Cgi/CgiListGetValues.o: src/cgi.h src/pair.h
src/Cgi/CgiListMake.o: src/nodelist.h src/cgi.h src/nodelist.h src/pair.h
src/Cgi/UrlDecodeString.o: src/cgi.h src/nodelist.h src/pair.h
src/Cgi/UrlEncodeChar.o: src/util.h src/dump.h src/cgi.h src/nodelist.h
src/Cgi/UrlEncodeChar.o: src/pair.h
src/Cgi/UrlEncodeString.o: src/cgi.h src/nodelist.h src/pair.h
src/Cgi/CgiListRequired.o: src/cgi.h src/nodelist.h src/pair.h
src/Cgi/CgiNextValue.o: src/pair.h src/nodelist.h src/util.h src/dump.h
src/Cgi/CgiNextValue.o: src/cgi.h src/pair.h
src/Cgi/CgiFree.o: src/cgi.h src/nodelist.h src/pair.h src/pair.h
src/Cgi/UrlDecodeChar.o: src/util.h src/dump.h
src/Cgi/CgiNew.o: src/util.h src/dump.h src/cgi.h src/nodelist.h src/pair.h
src/RFC822/RFC822HeadersRead.o: src/nodelist.h src/pair.h src/nodelist.h
src/RFC822/RFC822HeadersRead.o: src/rfc822.h src/util.h src/dump.h
src/RFC822/RFC822HeadersWrite.o: src/pair.h src/nodelist.h src/rfc822.h
src/mail.o: src/util.h src/dump.h src/rfc822.h src/nodelist.h src/pair.h
src/mail.o: src/mail.h
src/Dump/hexdump_mems.o: src/dump.h
src/Dump/dump_memorys.o: src/dump.h
src/Dump/dump_mems.o: src/dump.h
src/Dump/dump_memoryl.o: src/dump.h
src/Dump/hex.o: src/dump.h src/util.h src/dump.h
src/Dump/dump_memoryf.o: src/dump.h
src/conf.o: src/conf.h
src/Htmltok/HtmlParseFree.o: src/htmltok.h src/nodelist.h src/pair.h
src/Htmltok/HtmlParseNew.o: src/nodelist.h src/htmltok.h src/nodelist.h
src/Htmltok/HtmlParseNew.o: src/pair.h
src/Htmltok/HtmlParsePrintTag.o: src/nodelist.h src/htmltok.h src/nodelist.h
src/Htmltok/HtmlParsePrintTag.o: src/pair.h src/util.h src/dump.h
src/Htmltok/HtmlParseNext.o: src/nodelist.h src/htmltok.h src/nodelist.h
src/Htmltok/HtmlParseNext.o: src/pair.h src/util.h src/dump.h
src/Htmltok/HtmlParseClone.o: src/nodelist.h src/htmltok.h src/nodelist.h
src/Htmltok/HtmlParseClone.o: src/pair.h
src/Chunk/ChunkFree.o: src/chunk.h
src/Chunk/ChunkProcess.o: src/chunk.h
src/Chunk/ChunkProcessStream.o: src/chunk.h
src/Chunk/ChunkNew.o: src/chunk.h
src/Pair/PairListCreate.o: src/nodelist.h src/pair.h src/nodelist.h
src/Pair/PairNew.o: src/pair.h src/nodelist.h
src/Pair/PairFree.o: src/pair.h src/nodelist.h
src/Pair/PairCreate.o: src/pair.h src/nodelist.h
src/Pair/PairListFree.o: src/nodelist.h src/pair.h src/nodelist.h
src/Pair/PairListGetValue.o: src/pair.h src/nodelist.h
src/Pair/PairListGetPair.o: src/nodelist.h src/pair.h src/nodelist.h
src/tree.o: src/tree.h
src/bisearch.o: src/bisearch.h
src/Nodelist/ListInit.o: src/nodelist.h
src/Nodelist/ListRemHead.o: src/nodelist.h
src/Nodelist/NodeInsert.o: src/nodelist.h
src/Nodelist/ListRemTail.o: src/nodelist.h
src/Nodelist/NodeRemove.o: src/nodelist.h
src/Url/http.o: src/url.h
src/Url/file.o: src/url.h
src/Url/gopher.o: src/url.h
src/Url/url.o: src/url.h
src/Crashreport/crashreport_core.o: src/crashreport.h
src/Crashreport/crashreport.o: src/dump.h src/crashreport.h
src/Crashreport/crashreport_coresigs.o: src/crashreport.h
src/Crashreport/crashreport_allsigs.o: src/crashreport.h
