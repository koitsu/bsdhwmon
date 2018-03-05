#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/param.h>
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
extern int	w83627hf_main(int, const int, struct sensors *);
extern int	w83792d_main(int, const int, struct sensors *);
extern int	w83793g_main(int, const int, struct sensors *);
extern int	x6dva_main(int, struct sensors *);

/*
 * External variables (boards.c)
 */
extern struct board	boardlist;

/*
 * Global variables
 */
/*
 * smbfd needs to be pre-initialised to -1 for proper error handling
 * scenarios during start-up (see comment near top of main())
 */
static int	smbfd = -1;				/* File descriptor for /dev/smbXXX */
static int	comma_output = 0;			/* Command line flag "-c" */
static int	json_output = 0;			/* Command line flag "-J" */
static char	smbdev[MAXPATHLEN] = DEFAULT_SMBDEV;	/* Command line flag "-f", otherwise /dev/smb0 */
static int	f_verbose = 0;				/* Command line flag "-v" */


/*
 * VERBOSE() is mainly used for debugging.  Use of the "-v" flag will
 * cause underlying functions to call VERBOSE() with all sorts of
 * information.
 *
 * The format attribute is to safely squelch -Wformat-nonliteral (part
 * of of -Wformat=2) warnings about vprintf() with clang.  Reference:
 * http://clang.llvm.org/docs/AttributeReference.html#format-gnu-format
 */
__attribute__((__format__ (__printf__, 1, 0)))
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
	fprintf(stderr,
		"Usage: bsdhwmon [options]\n"
		"\n"
		"Options:\n"
		"  -J            JSON-formatted output\n"
		"  -c            comma-delimited output\n"
		"  -f DEVICE     use DEVICE as smb(4) device (default: " DEFAULT_SMBDEV ")\n"
		"  -l            list supported motherboard ID strings\n"
		"  -h            print this message\n"
		"  -v            be verbose (show debugging output)\n"
		"\n"
		"https://github.com/koitsu/bsdhwmon\n"
		"Report bugs at https://github.com/koitsu/bsdhwmon/issues\n"
	);
	exit(EX_USAGE);
}


int
main(int argc, char *argv[])
{
	const char kenv_planar_maker[] = "smbios.planar.maker";
	const char kenv_planar_product[] = "smbios.planar.product";
	int ch;
	int ret = 1;
	int exitcode = EX_OK;
	/*
	 * product, maker, and sensors pointers need to be pre-assigned
	 * to NULL.  If any of the "startup" routines fail (e.g. goto
	 * finish), free() will get called on them.  free(NULL) is harmless,
	 * while free() on something that's uninitialised (usually contains
	 * a random value) may result in errors and other problems.
	 */
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
				if (strlcpy(smbdev, optarg, MAXPATHLEN) >= MAXPATHLEN) {
					warnx("Device pathname exceeds %u bytes in length", MAXPATHLEN);
					exitcode = EX_SOFTWARE;
					goto finish;
				}
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
		warnx("Must be run as root, or setuid root");
		exitcode = EX_NOPERM;
		goto finish;
	}

	/*
	 * Do some basic argument conflict checking
	 */
	if (comma_output && json_output) {
		warnx("Please choose only one output format");
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
		warnx("Your motherboard does not appear to be supported.  Please visit\n"
		     "https://github.com/koitsu/bsdhwmon to see if support for your motherboard\n"
		     "and/or system is under development.\n");
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
			warnx("Internal error.  Please report this bug to the author.");
			exitcode = EX_SOFTWARE;
			goto finish;
	}

	/*
	 * Verify that the sensor collection routine was successful (chip
	 * validation passed, etc.).
	 */
	if (ret != 0) {
		warnx("Your motherboard is supported, but H/W chip verification failed.\n"
		     "Please re-run bsdhwmon with the -v flag and send full output + bug\n"
		     "report to the author.");
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

