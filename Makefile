#
# SPDX-License-Identifier: BSD-2-Clause-FreeBSD
#

.if defined(DEBUG)
CFLAGS+=	-g3 -ggdb
.endif

CFLAGS+=	-Werror -Wall -Wextra -Wformat=2 -Wbad-function-cast -Wcast-align -Wdeclaration-after-statement -Wdisabled-optimization -Wfloat-equal -Winline -Wmissing-declarations -Wmissing-prototypes -Wnested-externs -Wold-style-definition -Wpacked -Wpointer-arith -Wredundant-decls -Wstrict-prototypes -Wunreachable-code -Wwrite-strings

SRCS=	main.c boards.c output.c chip_w83792d.c chip_w83793g.c chip_x6dva.c smbus_io.c
OBJS=	${SRCS:.c=.o}

all: depend bsdhwmon man

bsdhwmon: ${OBJS}
	${CC} -o ${.TARGET} ${.ALLSRC}

# BSD make will read the .depend file automatically on invocation, which
# tracks allOBJS targets, and their associated source and #include
# dependencies.  However, in the case an #include is removed from a .c
# file, this can result in a "stale dependency" warning.  To alleviate
# that, we explicitly use .dinclude to ignore warnings, errors, and
# stale dependency warnings.

depend:
	${CC} -E -MM ${SRCS} > .depend

.dinclude ".depend"

man: bsdhwmon.8.txt

bsdhwmon.8.txt: bsdhwmon.8
	mandoc -Tascii bsdhwmon.8 | col -bx > ${.TARGET}

clean:
	-rm -f bsdhwmon bsdhwmon.8.txt ${OBJS} .depend *.core

distclean: clean

ports-test:
	@echo "portlint && make stage && make check-plist && make stage-qa && make package"

.PHONY: all man clean depend distclean ports-test

