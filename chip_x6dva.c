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
#include "global.h"

/*
 * Function prototypes
 */
int		x6dva_main(int, struct sensors *);

/*
 * External functions (main.c)
 */
extern void	VERBOSE(const char *, ...);

/*
 * External funcions (smbus_io.c)
 */
extern uint8_t	read_byte(int, int, const char);
extern void	write_byte(int, int, const char, const char);

/*
 * External functions (chip_w83792d.c)
 * NOTE: Yes, these are needed!
 */
extern uint8_t	w83792d_divisor(const uint8_t);
extern uint32_t	w83792d_rpmconv(const uint8_t, const uint8_t);


/*
 * x6dva_main(int fd, struct sensors *)
 *
 * fd = Descriptor return from open() on a /dev/smbX device
 *  s = Pointer to sensors struct; see global.h for a definition
 *
 * Supermicro X6DVA / X6DVL / X6DAL register reading subroutine.
 *
 * These Supermicro boards are quite unique -- they use dual Winbond
 * chips (one W83627HF, and one W83792D).  This makes things complex
 * when it comes to reading IC data.
 *
 * We do not want to risk using the standard w83792d_main() routine, since
 * that may read some registers which aren't tied to anything and could
 * put system stability at risk.
 *
 * The SMBus slave addresses are 0x2c for the W83627HF, and 0x2f for the
 * W83792D.  These are assumed/hard-coded in the below function, rather
 * than passed in via boardlist[].
 *
 * The registers we care about are:
 *
 *   Winbond W83627HF registers:
 *     CR20-CR25, CR27
 *   Winbond W83792D registers:
 *     CR20-CR21, CR28-CR2A, CR3E, CR47, CR5B-5C, CRB8-CRBA, CRC0, CRC8
 */
int
x6dva_main(int fd, struct sensors *s)
{
	static u_char regmap[256];
	int w83627hf_slave = 0x2c;
	int w83792d_slave  = 0x2f;
	uint8_t fandiv;

	VERBOSE("x6dva_main(fd = %d, s = %p)\n", fd, s);
	memset(&regmap, 0, sizeof(regmap));

	/*
	 * Winbond W83627HF portion
	 *
	 * Registers     Pin name
	 * -----------   ----------
	 * CR20          VIN0
	 * CR21          VIN1
	 * CR22          VIN2
	 * CR23          VIN3
	 * CR24          12VSEN
	 * CR25          VSEN1
	 * CR27          TD1
	 * -----------   ----------
	 */
	regmap[0x20] = read_byte(fd, w83627hf_slave, 0x20);
	regmap[0x21] = read_byte(fd, w83627hf_slave, 0x21);
	regmap[0x22] = read_byte(fd, w83627hf_slave, 0x22);
	regmap[0x23] = read_byte(fd, w83627hf_slave, 0x23);
	regmap[0x24] = read_byte(fd, w83627hf_slave, 0x24);
	regmap[0x25] = read_byte(fd, w83627hf_slave, 0x25);
	regmap[0x25] = read_byte(fd, w83627hf_slave, 0x25);
	regmap[0x27] = read_byte(fd, w83627hf_slave, 0x27);

	/*
	 * Now calculate voltages and temperatures which use the populated
	 * regmap[] array.  We do this BEFORE the Winbond W83792D reads,
	 * since regmap[] is re-used and overwrites some of the registers
	 * we use here (i.e. 0x20).
	 */
	s->voltages[VOLT_VIN0].value   = regmap[0x20];
	s->voltages[VOLT_VIN1].value   = regmap[0x21];
	s->voltages[VOLT_VIN2].value   = regmap[0x22];
	s->voltages[VOLT_VIN3].value   = regmap[0x23];
	s->voltages[VOLT_12VSEN].value = regmap[0x24];
	s->voltages[VOLT_VSEN1].value  = regmap[0x25];
	s->voltages[VOLT_VSEN1].value  *= -1;		/* VSEN1 is a negative voltage */
	s->temps[TEMP_TD1].value       = regmap[0x27];

	/*
	 * Zero out regmap[] as a precaution, since we do re-use it below.
	 */
	memset(&regmap, 0, sizeof(regmap));

	/*
	 * Winbond W83792D portion
	 *
	 * Registers     Pin name
	 * -----------   ----------
	 * CR20,CR3E     VCOREA
	 * CR20,CR3E     VCOREB
	 * CRC0,CRC1     TD2
	 * CRC8,CRC9     TD3
	 * CR28,CR47     FAN1
	 * CR29,CR47     FAN2
	 * CR2A,CR5B     FAN3
	 * CRB8,CR5B     FAN4
	 * CRB9,CR5C     FAN5
	 * CRBA,CR5C     FAN6
	 * -----------   ----------
	 *
	 * NOTE:
	 * 1. The bits used for CR3E (VCOREB) are undocumented; I assume 3-2.
	 * 2. The calculation formulas for VCOREA and VCOREB are suspect.
	 *    The formulas given in the W83792D data sheet are either
	 *    incorrect or Supermicro chose to use different resistors than
	 *    what the Winbond specification recommends.
	 * 3. The official Winbond W83792D documentation says some temperature
	 *    sensors are 9-bit, where the last bit defines .5 or .0.  It
	 *    doesn't appear that Supermicro wired these up, instead sticking
	 *    to raw 8-bit values.  This is why we don't read/use CRC1 or CRC9.
	 */
	regmap[0x20] = read_byte(fd, w83792d_slave, 0x20);
	regmap[0x21] = read_byte(fd, w83792d_slave, 0x21);
	regmap[0x28] = read_byte(fd, w83792d_slave, 0x28);
	regmap[0x29] = read_byte(fd, w83792d_slave, 0x29);
	regmap[0x2a] = read_byte(fd, w83792d_slave, 0x2a);
	regmap[0x3e] = read_byte(fd, w83792d_slave, 0x3e);
	regmap[0x47] = read_byte(fd, w83792d_slave, 0x47);
	regmap[0x5b] = read_byte(fd, w83792d_slave, 0x5b);
	regmap[0x5c] = read_byte(fd, w83792d_slave, 0x5c);
	regmap[0xb8] = read_byte(fd, w83792d_slave, 0xb8);
	regmap[0xb9] = read_byte(fd, w83792d_slave, 0xb9);
	regmap[0xba] = read_byte(fd, w83792d_slave, 0xba);
	regmap[0xc0] = read_byte(fd, w83792d_slave, 0xc0);
	regmap[0xc8] = read_byte(fd, w83792d_slave, 0xc8);

	s->voltages[VOLT_VCOREA].value = ((regmap[0x20] << 2) + (regmap[0x3e] & 0x03)) * 0.002;
	s->voltages[VOLT_VCOREB].value = ((regmap[0x21] << 2) + ((regmap[0x3e] & 0x0c) >> 2)) * 0.002;

	s->temps[TEMP_TD2].value = regmap[0xc0];
	s->temps[TEMP_TD3].value = regmap[0xc8];

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

	VERBOSE("x6dva_main() returning\n");
	return (0);
}

