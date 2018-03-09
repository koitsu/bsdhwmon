#
# SPDX-License-Identifier: BSD-2-Clause-FreeBSD
#

.if defined(DEBUG)
CFLAGS+=	-g3 -ggdb
.endif
CFLAGS+=	-Werror -Wall -Wextra -Wformat=2 -Waggregate-return -Wbad-function-cast -Wcast-align -Wdeclaration-after-statement -Wdisabled-optimization -Wfloat-equal -Winline -Wmissing-declarations -Wmissing-prototypes -Wnested-externs -Wold-style-definition -Wpacked -Wpointer-arith -Wredundant-decls -Wstrict-prototypes -Wunreachable-code -Wwrite-strings

OBJS=	main.o boards.o output.o chip_w83792d.o chip_w83793g.o chip_x6dva.o smbus_io.o

all: bsdhwmon man

bsdhwmon: ${OBJS}
	${CC} ${CFLAGS} -o ${.TARGET} ${.ALLSRC}

${OBJS}: global.h
	${CC} ${CFLAGS} -c ${.PREFIX}.c

man: bsdhwmon.8.txt

bsdhwmon.8.txt: bsdhwmon.8
	troff -Tascii -mman bsdhwmon.8 | grotty -c -b -o -u > ${.TARGET}

clean:
	-rm -f bsdhwmon bsdhwmon.8.txt ${OBJS} *.core

distclean: clean

ports-test:
	@echo "portlint && make stage && make check-plist && make stage-qa && make package"

.PHONY: all man clean distclean ports-test

