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

#include <string.h>
#include <inttypes.h>
#include "global.h"

/*
 * Function prototypes
 */
static uint32_t	w83793g_rpmconv(const uint16_t);
static uint8_t	w83793g_tempadj(const uint8_t);
int		w83793g_main(int, const uint8_t, struct sensors *);

/*
 * External functions (main.c)
 */
extern void	VERBOSE(const char *, ...);
extern uint8_t	read_byte(int, uint8_t, const char);
extern void	write_byte(int, uint8_t, const char, const char);


/*
 * w83793g_rpmconv(uint16_t count)
 *
 * count = Winbond register: fan speed counter
 *
 * Converts Winbond W83793G fan speed counter register to RPMs.  The
 * conversion formula, according to Winbond, is:
 *
 *   RPM = (1.35 * (10^6)) / (count * (fanpoles/4))
 *
 * "fanpoles" is undocumented, and is completely dependent upon the
 * actual fan model used.  Most fan motors are 4-pole, although some
 * (very) old models could be 2-pole.  "Pole" refers to actual polarity,
 * e.g. north or south.  We have to assume all fans connected are 4-pole
 * models.  There is no way to auto-detect pole count.  For details on
 * how fan motors work and what a "pole" is, see the below URL (which
 * also describes why 4-pole motors are better than 2 or 3-pole):
 *
 * http://electronics.howstuffworks.com/motor6.htm
 *
 * A count value of 0x0fff (4095) means the fan is disconnected.  We also
 * make sure count is non-zero, ensuring we don't divide by zero.
 *
 * Returns the current revolutions-per-minute (RPM) of the fan.  If
 * the fan is disconnected, or count is 0, return 0.
 */
static uint32_t
w83793g_rpmconv(const uint16_t count)
{
	uint32_t r = 0;

	VERBOSE("w83793g_rpmconv(count = 0x%04x)\n", count);

	if (count != 0x0fff && count != 0) {
		r = 1350000 / count;
	}

	VERBOSE("w83793g_rpmconv() returning 0x%08" PRIx32 "\n", r);
	return (r);
}


/*
 * w83793g_tempadj(uint8_t raw)
 *
 * raw = Winbond register: temperature register
 *
 * Winbond W83793G temperatures have 10 bits of granularity: 1 sign bit
 * (defining position or negative temperature), 7 data bits (for the
 * integer portion of the temperature), and 2 remainder bits (for the
 * floating point portion).
 *
 * bsdhwmon makes two assumptions, to keep things simple:
 *
 * 1) We only bother to read the registers used for the integer portion,
 * and the signage.  We're not interested in the floating point part of
 * the temperature.  This allows us to avoid switching banks and other
 * complexities, sticking with a pure "read-only" model,
 *
 * 2) The sign bit/MSB should NEVER be set (a temperature inside of a PC
 * should never be so cold as to have a negative temperature; a processor
 * should not be -72C, for example).  If the MSB is set, bsdhwmon assumes
 * there's no SMBus register tie-ins, or there's no thermistor installed.
 *
 * With regards to #2, Supermicro boards appear to set the MSB when there
 * is no temperature being monitored -- so we rely on that.  The author
 * has personally confirmed this.
 *
 * Returns the value passed to the function, but only if the MSB isn't
 * set.  If the MSB is set, return 0.
 */
static uint8_t
w83793g_tempadj(const uint8_t raw)
{
	uint8_t r = raw;

	VERBOSE("w83793g_tempadj(raw = 0x%02x)\n", raw);

	if ((raw & 0x80) == 0x80) {
		r = 0;
	}

	VERBOSE("w83793g_tempadj() returning 0x%02x\n", r);
	return (r);
}


/*
 * w83793g_main(int fd, uint8_t slave, struct sensors *s)
 *
 *    fd = Descriptor return from open() on a /dev/smbX device
 * slave = SMBus slave address; see boardlist[] in boards.c
 *     s = Pointer to sensors struct; see global.h for a definition
 *
 * Winbond W83793G register reading subroutine.  This does the bulk of
 * the work.  Any board which uses the W83793G will use this routine to
 * read a series of registers (called "CRxx") off the SMBus.
 *
 * The registers we're interested in are CR10 through CR3A in bank 0.
 *
 * For details of what register serves what purpose, refer to the
 * official Winbond W83793G documentation (December 11, 2006; rev 1.0).
 * I've included a decent "map" of what register does what in the
 * below comments, for quick reference/debugging.
 */
int
w83793g_main(int fd, const uint8_t slave, struct sensors *s)
{
	static u_char regmap[256];
	uint8_t i;

	VERBOSE("w83793g_main(fd = %d, slave = 0x%02x, s = %p)\n",
		fd, slave, s);

	memset(&regmap, 0, sizeof(regmap));

	for (i = 0x10; i <= 0x3a; ++i) {
		regmap[i] = read_byte(fd, slave, i);
	}

	/*
	 * Winbond pin    Registers used
	 * ------------   ---------------------
	 * VCOREA         0x10, 0x1b (bits 1-0)
	 * VCOREB         0x11, 0x1b (bits 3-2)
	 * VTT            0x12, 0x1b (bits 5-4)
	 * VSEN1          0x14
	 * VSEN2          0x15
	 * 3VSEN          0x16
	 * 12VSEN         0x17
	 * 5VDD           0x18
	 * 5VSB           0x19
	 * VBAT           0x1a
	 * ------------   ---------------------
	 *
	 * The calculation formulas used below are suspect.  The formulas given in
	 * the W83793G data sheet are either incorrect, or Supermicro chose to use
	 * adifferent resistors than what Winbond did.
	 *
	 * Thanks to Jim Perry for helping with some of the calculations.
	 */
	s->voltages[VOLT_VCOREA].value = ((regmap[0x10] << 2) + (regmap[0x1b] & 0x03)) * 0.002;
	s->voltages[VOLT_VCOREB].value = ((regmap[0x11] << 2) + ((regmap[0x1b] & 0x0c) >> 2)) * 0.002;
	s->voltages[VOLT_VTT].value    = ((regmap[0x12] << 2) + ((regmap[0x1b] & 0x30) >> 4)) * 0.002;
	s->voltages[VOLT_VSEN1].value  = regmap[0x14] * 0.032 * 12;
	s->voltages[VOLT_VSEN1].value  *= -1;		/* VSEN1 is a negative voltage */
	s->voltages[VOLT_VSEN2].value  = regmap[0x15] * 0.016;
	s->voltages[VOLT_3VSEN].value  = regmap[0x16] * 0.016;
	s->voltages[VOLT_12VSEN].value = regmap[0x17] * 0.008 * 12;
	s->voltages[VOLT_5VDD].value   = (regmap[0x18] * 0.024) + 0.15;
	s->voltages[VOLT_5VSB].value   = (regmap[0x19] * 0.024) + 0.15;
	s->voltages[VOLT_VBAT].value   = regmap[0x1a] * 0.016;

	/*
	 * Winbond pin    Registers used
	 * ------------   ---------------------
	 * TD1            0x1c, 0x22 (bits 1-0)
	 * TD2            0x1d, 0x22 (bits 3-2)
	 * TD3            0x1e, 0x22 (bits 5-4)
	 * TD4            0x1f, 0x22 (bits 7-6)
	 * TR1            0x20
	 * TR2            0x21
	 * ------------   ---------------------
	 *
	 * TD temperatures are 10 bits: 1 sign bit (MSB), 7 data bits, and 2 decimal
	 * bits.  We're not interested in the decimal portion, but we are interested
	 * in the integer portion.  tempadj() checks the MSB (sign bit) and if it's
	 * set, makes the assumption that there's no wire/tie-in.
	 *
	 * TR temperatures are 1 sign bit (MSB), 7 data bits.
	 */
	s->temps[TEMP_TD1].value = w83793g_tempadj(regmap[0x1c]);
	s->temps[TEMP_TD2].value = w83793g_tempadj(regmap[0x1d]);
	s->temps[TEMP_TD3].value = w83793g_tempadj(regmap[0x1e]);
	s->temps[TEMP_TD4].value = w83793g_tempadj(regmap[0x1f]);
	s->temps[TEMP_TR1].value = regmap[0x20];
	s->temps[TEMP_TR2].value = regmap[0x21];

	/*
	 * See the official W83793G specification sheet for these
	 */
	s->fans[FAN_FAN1].value = w83793g_rpmconv((regmap[0x23] << 8) | regmap[0x24]);
	s->fans[FAN_FAN2].value = w83793g_rpmconv((regmap[0x25] << 8) | regmap[0x26]);
	s->fans[FAN_FAN3].value = w83793g_rpmconv((regmap[0x27] << 8) | regmap[0x28]);
	s->fans[FAN_FAN4].value = w83793g_rpmconv((regmap[0x29] << 8) | regmap[0x2a]);
	s->fans[FAN_FAN5].value = w83793g_rpmconv((regmap[0x2b] << 8) | regmap[0x2c]);
	s->fans[FAN_FAN6].value = w83793g_rpmconv((regmap[0x2d] << 8) | regmap[0x2e]);
	s->fans[FAN_FAN7].value = w83793g_rpmconv((regmap[0x2f] << 8) | regmap[0x30]);
	s->fans[FAN_FAN8].value = w83793g_rpmconv((regmap[0x31] << 8) | regmap[0x32]);
	s->fans[FAN_FAN9].value = w83793g_rpmconv((regmap[0x33] << 8) | regmap[0x34]);
	s->fans[FAN_FAN10].value = w83793g_rpmconv((regmap[0x35] << 8) | regmap[0x36]);
	s->fans[FAN_FAN11].value = w83793g_rpmconv((regmap[0x37] << 8) | regmap[0x38]);
	s->fans[FAN_FAN12].value = w83793g_rpmconv((regmap[0x39] << 8) | regmap[0x3a]);

	VERBOSE("w83793g_main() returning\n");
	return (0);
}

