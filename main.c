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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <kenv.h>
#include <paths.h>
#include <err.h>
#include <osreldate.h>
#include <sysexits.h>

#include "global.h"

#define DEFAULT_SMBDEV	_PATH_DEV "smb0"

/*
 * Function prototypes
 */
void		VERBOSE(const char *, ...);
static void	USAGE(void);
uint8_t		read_byte(int, uint8_t, const char);
void		write_byte(int, uint8_t, const char, const char);

/*
 * External functions (boards.c)
 */
extern struct board *	board_lookup(const char *, const char *);

/*
 * External functions (output.c)
 */
extern void	sensors_output(struct board *, struct sensors *);
extern void	sensors_output_delim(struct board *, struct sensors *);
extern void	sensors_output_json(struct board *, struct sensors *);
extern void	list_models(struct board *);

/*
 * External functions (chip_XXX.c)
 */
extern int	w83627hf_main(int, uint8_t, struct sensors *);
extern int	w83792d_main(int, uint8_t, struct sensors *);
extern int	w83793g_main(int, uint8_t, struct sensors *);
extern int	x6dva_main(int, struct sensors *);

/*
 * External variables (boards.c)
 */
extern struct board	boardlist;

/*
 * Global variables
 */
static char	ibuf[SMB_MAXBLOCKSIZE];			/* Buffer for SMBus data reads */
static int	smbfd = -1;				/* File descriptor for /dev/smbXXX */
static int	comma_output = 0;			/* Command line flag "-c" */
static int	json_output = 0;			/* Command line flag "-J" */
static char	smbdev[MAXPATHLEN] = DEFAULT_SMBDEV;	/* Command line flag "-f", otherwise /dev/smb0 */
static int	f_verbose = 0;				/* Command line flag "-v" */


/*
 * VERBOSE() is mainly used for debugging.  Use of the "-v" flag will
 * cause underlying functions to call VERBOSE() with all sorts of
 * information.
 */
void
VERBOSE(const char *fmt, ...)
{
	if (f_verbose) {
		va_list ap;

		va_start(ap, fmt);
		printf("==> ");
		vprintf(fmt, ap);
		va_end(ap);
	}
}


static void
USAGE(void)
{
	printf("Usage: bsdhwmon [options]\n");
	printf("\n");
	printf("Options:\n");
	printf("\n");
	printf("  -J            JSON-formatted output\n");
	printf("  -c            comma-delimited output\n");
	printf("  -f DEVICE     use DEVICE as smb(4) device (default: " DEFAULT_SMBDEV ")\n");
	printf("  -l            list supported motherboard ID strings\n");
	printf("  -h            print this message\n");
	printf("  -v            be verbose (show debugging output)\n");
	printf("\n");
	printf("http://bsdhwmon.koitsu.org/\n");
	printf("Report bugs to <jdc@koitsu.org>\n");
	exit(EX_USAGE);
}


/*
 * read_byte(int fd, uint8_t slave, const char idxreg)
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
read_byte(int fd, uint8_t slave, const char idxreg)
{
	struct smbcmd c;

	memset(&c, 0, sizeof(struct smbcmd));

/*
 * The smb(4) driver was modified on 2009/05/15 to require SMBus slave
 * addresses not have their LSB (bit 0) set.  If bit 0 is set to 1,
 * the driver immediately returns EINVAL, resulting in ioctl(2)
 * returning -1 with errno EINVAL.  See commit revision 1.38.2.1 for
 * details:
 *
 * http://www.freebsd.org/cgi/cvsweb.cgi/src/sys/dev/smbus/smb.c
 *
 * __FreeBSD_version was bumped as a result of this change.  The change
 * happened in version 7020101:
 *
 * http://www.freebsd.org/doc/en/books/porters-handbook/freebsd-versions.html
 *
 * The SMBus 2.0 specification states that SMBus slave addresses are 7 bits
 * in width (e.g. 1001001b), and occupy the *upper* 7 bits of an 8-bit
 * address.  Thus, the LSB is always 0.
 */
#if (__FreeBSD_version >= 702101)
	c.slave = (u_char) slave << 1;
#else
	c.slave = (u_char) slave;
#endif
	c.cmd = idxreg;
	c.data.byte_ptr = ibuf;

	if (ioctl(fd, SMB_READB, &c) == -1) {
		err(EX_IOERR, "ioctl(SMB_READB) failed");
	}

	VERBOSE("read_byte(fd = %d, slave = 0x%02x, idxreg = 0x%02x) returned 0x%02x\n",
		fd, slave, idxreg, (u_char) ibuf[0]);

	return ((u_char) ibuf[0]);
}


/*
 * write_byte(int fd, uint8_t slave, const char idxreg, const char value)
 *
 *     fd = Descriptor return from open() on a /dev/smbX device
 *  slave = SMBus slave address; see boardlist[] in boards.c
 * idxreg = Index/register to write to
 *  value = Value to write to bus
 * 
 * Writes a byte to the SMBus via ioctl().  See smb(4) for details.
 */
void
write_byte(int fd, uint8_t slave, const char idxreg, const char value)
{
	struct smbcmd c;

	VERBOSE("write_byte(fd = %d, slave = 0x%02x, idxreg = 0x%02x, value = 0x%02x)\n",
		fd, slave, idxreg, value);

	memset(&c, 0, sizeof(struct smbcmd));

/*
 * The smb(4) driver was modified on 2009/05/15 to require SMBus slave
 * addresses not have their LSB (bit 0) set.  If bit 0 is set to 1,
 * the driver immediately returns EINVAL, resulting in ioctl(2)
 * returning -1 with errno EINVAL.  See commit revision 1.38.2.1 for
 * details:
 *
 * http://www.freebsd.org/cgi/cvsweb.cgi/src/sys/dev/smbus/smb.c
 *
 * __FreeBSD_version was bumped as a result of this change.  The change
 * happened in version 7020101:
 *
 * http://www.freebsd.org/doc/en/books/porters-handbook/freebsd-versions.html
 *
 * The SMBus 2.0 specification states that SMBus slave addresses are 7 bits
 * in width (e.g. 1001001b), and occupy the *upper* 7 bits of an 8-bit
 * address.  Thus, the LSB is always 0.
 */
#if (__FreeBSD_version >= 702101)
	c.slave = (u_char) slave << 1;
#else
	c.slave = (u_char) slave;
#endif
	c.cmd = idxreg;
	c.data.byte = value;

	if (ioctl(fd, SMB_WRITEB, &c) == -1) {
		err(EX_IOERR, "ioctl(SMB_WRITEB) failed");
	}

	VERBOSE("write_byte() returning\n");
}


int
main(int argc, char *argv[])
{
	const char kenv_planar_maker[] = "smbios.planar.maker";
	const char kenv_planar_product[] = "smbios.planar.product";
	int ch;
	int ret = 1;
	int exitcode = EX_OK;
	char *product = NULL;
	char *maker = NULL;
	struct sensors *sdata = NULL;
	struct board *mb;

	while ((ch = getopt(argc, argv, "Jcf:lvh?")) != -1) {
		switch (ch) {
			case 'J':
				json_output = 1;
				break;
			case 'c':
				comma_output = 1;
				break;
			case 'f':
				strncpy(smbdev, optarg, MAXPATHLEN);
				break;
			case 'l':
				list_models(&boardlist);
				break;
			case 'v':
				f_verbose = 1;
				break;
			case 'h':
			case '?':
			default:
				USAGE();
		}
	}

	argc -= optind;
	argv += optind;

	/*
	 * bsdhwmon requires root access due to opening /dev/smbX
	 */
	if (geteuid() != 0) {
		warn("Must be run as root, or setuid root");
		exitcode = EX_NOPERM;
		goto finish;
	}

	/*
	 * Do some basic argument conflict checking
	 */
	if (comma_output && json_output) {
		warn("Please choose only one output format");
		exitcode = EX_USAGE;
		goto finish;
	}

	/*
	 * Allocate memory for the maker and product strings, then attempt to
	 * use kenv(2) to look up smbios.planar.maker and smbios.planar.product
	 * values and return them; these are taken directly from SMBIOS.  If
	 * SMBIOS strings aren't available then the user is out of luck.
	 *
	 * Both strings must match something in the board structure (mb),
	 * otherwise bsdhwmon doesn't support the motherboard in question.
	 */
	if ((maker = calloc(1, KENV_MVALLEN)) == NULL) {
		warn("calloc() for maker failed");
		exitcode = EX_OSERR;
		goto finish;
	}

	if ((product = calloc(1, KENV_MVALLEN)) == NULL) {
		warn("calloc() for product failed");
		exitcode = EX_OSERR;
		goto finish;
	}

	if (kenv(KENV_GET, kenv_planar_maker, maker, KENV_MVALLEN) == -1) {
		warn("kenv() for %s failed", kenv_planar_maker);
		exitcode = EX_OSERR;
		goto finish;
	}

	if (kenv(KENV_GET, kenv_planar_product, product, KENV_MVALLEN) == -1) {
		warn("kenv() for %s failed", kenv_planar_product);
		exitcode = EX_OSERR;
		goto finish;
	}

	if ((mb = board_lookup(maker, product)) == NULL) {
		warn("Your motherboard does not appear to be supported.  You can visit\n"
		     "http://bsdhwmon.koitsu.org/ to see if support for your motherboard\n"
		     "and/or system is under development.");
		exitcode = EX_DATAERR;
		goto finish;
	}

	if ((sdata = calloc(1, sizeof(struct sensors))) == NULL) {
		warn("calloc() for sdata failed");
		exitcode = EX_DATAERR;
		goto finish;
	}

	/*
	 * Open the device with read/write access and an exclusive lock.  The
	 * lock is a safety mechanism in the case two bsdhwmon processes somehow
	 * run simultaneously (one should block/wait indefinitely until the
	 * other has closed the fd).
	 *
	 * I simply don't know if the smb(4) framework would handle two
	 * programs simultaneously reading/writing to /dev/smbX.
	 */
	if ((smbfd = open(smbdev, O_RDWR|O_EXLOCK)) < 0) {
		warn("open() on %s failed", smbdev);
		exitcode = EX_NOINPUT;
		goto finish;
	}

	/*
	 * Collect sensor data.
	 */
	switch (mb->chip) {
		case CUSTOM_X6DVA:
			ret = x6dva_main(smbfd, sdata);
			break;
		case WINBOND_W83792D:
			ret = w83792d_main(smbfd, mb->slave, sdata);
			break;
		case WINBOND_W83793G:
			ret = w83793g_main(smbfd, mb->slave, sdata);
			break;
		default:
			warn("Internal bsdhwmon coding error.  Please report this bug\n"
			     "to the bsdhwmon author.");
			exitcode = EX_SOFTWARE;
			goto finish;
	}

	/*
	 * Verify that the sensor collection routine was successful (chip
	 * validation passed, etc.).
	 */
	if (ret != 0) {
		warn("Your motherboard is supported, but H/W chip verification failed.\n"
		     "Please re-run bsdhwmon with the -v flag and send full output + bug\n"
		     "report to the bsdhwmon author.");
		exitcode = EX_SOFTWARE;
		goto finish;
	}

	/*
	 * Output collected sensor data to user.
	 */
	if (json_output) {
		sensors_output_json(mb, sdata);
	} else if (comma_output) {
		sensors_output_delim(mb, sdata);
	} else {
		sensors_output(mb, sdata);
	}

finish:
	/*
	 * Clean up and exit.
	 */
	if (smbfd != -1) {
		close(smbfd);
	}
	free(sdata);
	free(product);
	free(maker);
	exit(exitcode);
}

