/*
Copyright (C) 2008-2015 Jeremy Chadwick. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY AUTHOR AND CONTRIBUTORS ``AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL AUTHOR OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.
*/

#include <sys/param.h>
#include <dev/smbus/smb.h>

#include <string.h>
#include <err.h>
#include <sysexits.h>

/*
 * Function prototypes
 */
uint8_t		read_byte(int, int, const char);
void		write_byte(int, int, const char, const char);

/*
 * External functions (main.c)
 */
extern void	VERBOSE(const char *, ...);

/*
 * Global variables
 */
static char	ibuf[SMB_MAXBLOCKSIZE];		/* SMBus data buffer */


/*
 * read_byte(int fd, int slave, const char idxreg)
 *
 *     fd = Descriptor return from open() on a /dev/smbX device
 *  slave = SMBus slave address; see boardlist[] in boards.c
 * idxreg = Index/register to read
 *
 * Reads a byte off off the SMBus via ioctl().  See smb(4) for details.
 *
 * Returns byte read.  On failure, returns -1.
 */
uint8_t
read_byte(int fd, int slave, const char idxreg)
{
	struct smbcmd c;

	memset(&c, 0, sizeof(struct smbcmd));

	c.slave = (u_char) slave & 0xff;
	c.data.byte_ptr = ibuf;
	c.cmd = idxreg;

	if (ioctl(fd, SMB_READB, &c) == -1) {
		err(EX_IOERR, "ioctl(SMB_READB) failed");
	}

	VERBOSE("read_byte(fd = %d, slave = 0x%02x, idxreg = 0x%02x) returned 0x%02x\n",
		fd, slave, idxreg, (u_char) ibuf[0]);

	return ((u_char) ibuf[0]);
}


/*
 * write_byte(int fd, int slave, const char idxreg, const char value)
 *
 *     fd = Descriptor return from open() on a /dev/smbX device
 *  slave = SMBus slave address; see boardlist[] in boards.c
 * idxreg = Index/register to write to
 *  value = Value to write to bus
 *
 * Writes a byte to the SMBus via ioctl().  See smb(4) for details.
 */
void
write_byte(int fd, int slave, const char idxreg, const char value)
{
	struct smbcmd c;

	VERBOSE("write_byte(fd = %d, slave = 0x%02x, idxreg = 0x%02x, value = 0x%02x)\n",
		fd, slave, idxreg, value);

	memset(&c, 0, sizeof(struct smbcmd));

	c.slave = (u_char) slave & 0xff;
	c.data.byte = value;
	c.cmd = idxreg;

	if (ioctl(fd, SMB_WRITEB, &c) == -1) {
		err(EX_IOERR, "ioctl(SMB_WRITEB) failed");
	}

	VERBOSE("write_byte() returning\n");
}

