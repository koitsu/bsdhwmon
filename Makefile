#
# Copyright (C) 2008-2015 Jeremy Chadwick. All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY AUTHOR AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL AUTHOR OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.

.if defined(DEBUG)
CFLAGS=		-g3 -ggdb -Werror -Wall -Wformat-security -Waggregate-return -Wbad-function-cast -Wcast-align -Wdeclaration-after-statement -Wdisabled-optimization -Wfloat-equal -Winline -Wmissing-declarations -Wmissing-prototypes -Wnested-externs -Wold-style-definition -Wpacked -Wpointer-arith -Wredundant-decls -Wsign-compare -Wstrict-prototypes -Wunreachable-code -Wwrite-strings
.else
CFLAGS+=	-Werror -Wall -Wformat-security -fno-inline
.endif

OBJS=	main.o boards.o output.o chip_w83792d.o chip_w83793g.o chip_x6dva.o

all: bsdhwmon bsdhwmon.8.txt

bsdhwmon: ${OBJS}
	${CC} ${CFLAGS} -o ${.TARGET} ${.ALLSRC}

${OBJS}: global.h
	${CC} ${CFLAGS} -c ${.PREFIX}.c

bsdhwmon.8.txt: bsdhwmon.8
	troff -Tascii -mman bsdhwmon.8 | grotty -c -b -o -u > ${.TARGET}

clean:
	rm -f bsdhwmon *.o *.core

# Assign YYYYMMDD to $NOW variable
NOW!=	/bin/date +%Y%m%d

release:
	@echo cvs tag bsdhwmon-${NOW}
	@echo cd /tmp
	@echo cvs export -d bsdhwmon-${NOW} -r bsdhwmon-${NOW} bsdhwmon
	@echo tar -pcf bsdhwmon-${NOW}.tar bsdhwmon-${NOW}
	@echo gzip -9 bsdhwmon-${NOW}.tar
	@echo chmod 0644 bsdhwmon-${NOW}.tar.gz
	@echo rm -fr bsdhwmon-${NOW}
	@echo ls -l /tmp/bsdhwmon-${NOW}.tar.gz

