/*
 * SPDX-License-Identifier: BSD-2-Clause-FreeBSD
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <sys/types.h>
#include "global.h"

/*
 * Function prototypes
 */
uint8_t		w83792d_divisor(const uint8_t);
uint32_t	w83792d_rpmconv(const uint8_t, const uint8_t);
int		w83792d_main(int, const int, struct sensors *);

/*
 * External functions (smbus_io.c)
 */
extern uint8_t	read_byte(int, int, const char);
extern void	write_byte(int, int, const char, const char);


/*
 * w83792d_divisor(uint8_t raw)
 *
 * raw = PWM input divisor
 *
 * Converts a Winbond W83792D fan PWM input divisor to an integer value.
 * Supposedly the default value is 2, so we assume that in the case the
 * divisor is > 7.
 *
 * Returns a numeric divisor, which can be used mathematically.
 */
uint8_t
w83792d_divisor(const uint8_t raw)
{
	uint8_t r = 2;

	VERBOSE("w83792d_divisor(raw = 0x%02" PRIx8 ")\n", raw);

	switch (raw) {
		case 0x00: r = 1; break;
		case 0x01: r = 2; break;
		case 0x02: r = 4; break;
		case 0x03: r = 8; break;
		case 0x04: r = 16; break;
		case 0x05: r = 32; break;
		case 0x06: r = 64; break;
		case 0x07: r = 128; break;
	}
	VERBOSE("w83792d_divisor() returning 0x%02" PRIx8 "\n", r);
	return (r);
}


/*
 * w83792d_rpmconv(uint8_t count, uint8_t div)
 *
 * count = Winbond register: fan speed counter
 *   div = Winbond register: fan divisor
 *
 * Converts Winbond W83792D fan speed counter register to RPMs.  The
 * conversion formula, according to Winbond, is:
 *
 *   RPM = (1.35 * (10^6)) / (count * divisor)
 *
 * A count value of 0xff (255) means the fan is disconnected.  We also
 * make sure count and div are non-zero, ensuring we don't divide by zero.
 *
 * Returns the current revolutions-per-minute (RPM) of the fan.  If
 * the fan is disconnected, or either count or div are zero, return 0.
 */
uint32_t
w83792d_rpmconv(const uint8_t count, const uint8_t div)
{
	uint32_t r = 0;

	VERBOSE("w83792d_rpmconv(count = 0x%02" PRIx8 ", div = 0x%02" PRIu8 ")\n",
		count, div);

	if (count != 0xff && count != 0 && div != 0) {
		r = 1350000 / (count * div);
	}

	VERBOSE("w83792d_rpmconv() returning 0x%08" PRIx32 "\n", r);
	return (r);
}


/*
 * w83792d_main(int fd, const int slave, struct sensors *s)
 *
 *    fd = Descriptor return from open() on a /dev/smbX device
 * slave = SMBus slave address; see boardlist[] in boards.c
 *     s = Pointer to sensors struct; see global.h for a definition
 *
 * Winbond W83792D register reading subroutine.  This does the bulk of
 * the work.  Any board which uses the W83792D will use this routine to
 * read a series of registers (called "CRxx") off the SMBus.
 *
 * The registers we're interested in are scattered all over the place,
 * which makes writing a clean-looking function very difficult.  There
 * are probably better ways to do this (such as contain a map of all
 * the registers).  At least they're all in Bank 0!  Here's the list:
 *
 *   CR20-CR2A
 *   CR3E-CR3F
 *   CR47
 *   CR49      (chip revision)
 *   CR58-CR5C
 *   CRB0-CRB1
 *   CRC0
 *   CRC8
 *
 * For details of what register serves what purpose, refer to the
 * official Winbond W83792D documentation (April 26, 2006; rev 0.9).
 * I've included a decent "map" of what register does what in the
 * below comments, for quick reference/debugging.
 */
int
w83792d_main(int fd, const int slave, struct sensors *s)
{
	static u_char regmap[256];
	uint8_t i;
	uint8_t fandiv;

	VERBOSE("w83792d_main(f = %d, slave = 0x%02x, s = %p)\n",
		fd, slave, s);

	memset(&regmap, 0, sizeof(regmap));

	for (i = 0x20; i <= 0x2a; ++i) {
		regmap[i] = read_byte(fd, slave, i);
	}

	regmap[0x3e] = read_byte(fd, slave, 0x3e);
	regmap[0x3f] = read_byte(fd, slave, 0x3f);
	regmap[0x47] = read_byte(fd, slave, 0x47);
	regmap[0x49] = read_byte(fd, slave, 0x49);

	for (i = 0x58; i <= 0x5c; ++i) {
		regmap[i] = read_byte(fd, slave, i);
	}

	regmap[0xb0] = read_byte(fd, slave, 0xb0);
	regmap[0xb1] = read_byte(fd, slave, 0xb1);
	regmap[0xc0] = read_byte(fd, slave, 0xc0);
	regmap[0xc8] = read_byte(fd, slave, 0xc8);

	/*
	 * Winbond pin    Indexes used
	 * ------------   ---------------------
	 * VCOREA         0x20, 0x3e (bits 1-0)
	 * VCOREB         0x21, 0x3e (bits 3-2)
	 * VIN0           0x22, 0x3e (bits 5-4)
	 * VIN1           0x23, 0x3e (bits 7-6)
	 * VIN2           0x24, 0x3f (bits 1-0)
	 * VIN3           0x25, 0x3f (bits 3-2)
	 * 5VCC           0x26, 0x3f (bits 5-4) XXX - Calculation formula wrong?
	 * 5VSB           0xb0
	 * VBAT           0xb1
	 * ------------   ---------------------
	 *
	 * The bits used for CR3E (VCOREB) are undocumented; I assume 3-2.
	 *
	 * The calculation formulas used below are suspect.  The formulas given in
	 * the W83792D data sheet are either incorrect, or Supermicro chose to use
	 * different resistors than what Winbond did.
	 *
	 * The problematic one appears to be VIN1, which on my systems shows up as
	 * 3.040V or so, which definitely is wrong.
	 *
	 * Additionally, the documentation Supermicro provided does not state
	 * anything about 5VSB or VBAT, but they're shown in the BIOS, and testing
	 * shows they're connected.
	 */
	s->voltages[VOLT_VCOREA].value = ((regmap[0x20] << 2) + (regmap[0x3e] & 0x03)) * 0.002;
	s->voltages[VOLT_VCOREB].value = ((regmap[0x21] << 2) + ((regmap[0x3e] & 0x0c) >> 2)) * 0.002;
	s->voltages[VOLT_VIN0].value = ((regmap[0x22] << 2) + ((regmap[0x3e] & 0x30) >> 4)) * 0.004;
	s->voltages[VOLT_VIN1].value = ((regmap[0x23] << 2) + ((regmap[0x3e] & 0xc0) >> 6)) * 0.004;
	s->voltages[VOLT_VIN2].value = ((regmap[0x24] << 2) + (regmap[0x3f] & 0x03)) * 0.008 * 12;
	s->voltages[VOLT_VIN2].value *= -1;		/* VIN2 is a negative voltage */
	s->voltages[VOLT_VIN3].value = ((regmap[0x25] << 2) + ((regmap[0x3f] & 0x0c) >> 2)) * 0.016;
	s->voltages[VOLT_5VCC].value = ((regmap[0x26] << 2) + ((regmap[0x3f] & 0x30) >> 4)) * 0.006;
	s->voltages[VOLT_5VSB].value = regmap[0xb0] * 0.024;
	s->voltages[VOLT_VBAT].value = regmap[0xb1] * 0.016;

	/*
	 * Winbond pin    Indexes used
	 * ------------   ---------------------
	 * TD1            0x27
	 * TD2            0xc0, 0xc1 (bit 7)
	 * TD3            0xc8, 0xc9 (bit 7)
	 * ------------   ---------------------
	 *
	 * The official Winbond W83792D documentation says some of the temperature sensors
	 * are 9-bit, where the last bit defines .5 or .0.  However, Supermicro didn't
	 * bother to follow any of this, and instead use and instead made all the values
	 * raw 8-bit.  This is why we don't read CRC1 or CRC9.
	 */
	s->temps[TEMP_TD1].value = regmap[0x27];
	s->temps[TEMP_TD2].value = regmap[0xc0];
	s->temps[TEMP_TD3].value = regmap[0xc8];

	/*
	 * Winbond pin    Indexes used
	 * ------------   ---------------------
	 * FAN1           0x28  (divisor at 0x47, bits 2-0)
	 * FAN2           0x29  (divisor at 0x47, bits 6-4)
	 * FAN3           0x2a  (divisor at 0x5b, bits 2-0)
	 * FAN4           0xb8  (divisor at 0x5b, bits 6-4)
	 * FAN5           0xb9  (divisor at 0x5c, bits 2-0)
	 * FAN6           0xba  (divisor at 0x5c, bits 6-4)
	 * FAN7           0xbe  (divisor at 0x9e, bits 2-0)
	 * ------------   ---------------------
	 */
	fandiv = w83792d_divisor(regmap[0x47] & 0x07);
	s->fans[FAN_FAN1].value = w83792d_rpmconv(regmap[0x28], fandiv);

	fandiv = w83792d_divisor((regmap[0x47] & 0x70) >> 4);
	s->fans[FAN_FAN2].value = w83792d_rpmconv(regmap[0x29], fandiv);

	fandiv = w83792d_divisor(regmap[0x5b] & 0x07);
	s->fans[FAN_FAN3].value = w83792d_rpmconv(regmap[0x2a], fandiv);

	fandiv = w83792d_divisor((regmap[0x5b] & 0x70) >> 4);
	s->fans[FAN_FAN4].value = w83792d_rpmconv(regmap[0xb8], fandiv);

	fandiv = w83792d_divisor(regmap[0x5c] & 0x07);
	s->fans[FAN_FAN5].value = w83792d_rpmconv(regmap[0xb9], fandiv);

	fandiv = w83792d_divisor((regmap[0x5c] & 0x70) >> 4);
	s->fans[FAN_FAN6].value = w83792d_rpmconv(regmap[0xba], fandiv);

	fandiv = w83792d_divisor(regmap[0x9e] & 0x07);
	s->fans[FAN_FAN7].value = w83792d_rpmconv(regmap[0xbe], fandiv);

	VERBOSE("w83792d_main() returning\n");
	return (0);
}

