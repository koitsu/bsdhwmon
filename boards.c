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

#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <kenv.h>

#include "global.h"

const struct pinmap volts_type00[] = {
	{ VOLT_VCOREA,	"Processor Vcore(V)"	},
	{ VOLT_VIN0,	"3.3V Vcc(V)"		},
	{ VOLT_VIN1,	"5V Vcc(V)"		},
	{ VOLT_VIN2,	"-12V Vcc(V)"		},
	{ VOLT_VIN3,	"12V Vcc(V)"		},
	{ VOLT_5VSB,	"5VSB"			},
	{ VOLT_VBAT,	"VBAT"			},
	{ 0,		NULL			}
};
const struct pinmap temps_type00[] = {
	{ TEMP_TD1,	"CPU Temperature"	},
	{ TEMP_TD2,	"System Temperature"	},
	{ 0,		NULL			}
};
const struct pinmap fans_type00[] = {
	{ FAN_FAN1,	"FAN1"		},
	{ FAN_FAN2,	"FAN2"		},
	{ FAN_FAN3,	"FAN3"		},
	{ FAN_FAN4,	"FAN4"		},
	{ FAN_FAN5,	"FAN5"		},
	{ 0,		NULL		}
};


const struct pinmap volts_type01[] = {
	{ VOLT_VCOREA,	"VcoreA"	},
	{ VOLT_VCOREB,	"VcoreB"	},
	{ VOLT_VIN0,	"P3V3"		},
	{ VOLT_VIN1,	"P5V"		},
	{ VOLT_VIN2,	"N12V"		},
	{ VOLT_VIN3,	"P12V"		},
	{ VOLT_5VCC,	"VDD"		},	/* Not a typo! */
	{ VOLT_5VSB,	"P5Vsb"		},
	{ 0,		NULL		}
};
const struct pinmap temps_type01[] = {
	{ TEMP_TD1,	"CPU1 Temperature"	},
	{ TEMP_TD2,	"CPU2 Temperature"	},
	{ TEMP_TD3,	"System Temperature"	},
	{ 0,		NULL			}
};


const struct pinmap volts_type02[] = {
	{ VOLT_VCOREA,	"VcoreA"	},
	{ VOLT_VCOREB,	"VcoreB"	},
	{ VOLT_VSEN1,	"-12V"		},
	{ VOLT_VSEN2,	"P1V5"		},
	{ VOLT_3VSEN,	"+3.3V"		},
	{ VOLT_12VSEN,	"+12V"		},
	{ VOLT_5VSB,	"5Vsb"		},
	{ VOLT_5VDD,	"5VDD"		},
	{ VOLT_VTT,	"P_VTT"		},
	{ VOLT_VBAT,	"VBat"		},
	{ 0,		NULL		}
};
const struct pinmap temps_type02[] = {
	{ TEMP_TD1,	"CPU1 Temperature"	},
	{ TEMP_TD2,	"CPU1 Second Core"	},
	{ TEMP_TD3,	"CPU2 Temperature"	},
	{ TEMP_TD4,	"CPU2 Second Core"	},
	{ TEMP_TR1,	"System Temperature"	},
	{ 0,		NULL			}
};
const struct pinmap fans_type02[] = {
	{ FAN_FAN1,	"FAN1"		},
	{ FAN_FAN2,	"FAN2"		},
	{ FAN_FAN3,	"FAN3"		},
	{ FAN_FAN4,	"FAN4"		},
	{ FAN_FAN5,	"FAN5"		},
	{ FAN_FAN6,	"FAN6"		},
	{ FAN_FAN7,	"FAN7"		},
	{ FAN_FAN8,	"FAN8"		},
	{ FAN_FAN9,	"FAN9"		},
	{ FAN_FAN10,	"FAN10"		},
	{ 0,		NULL		}
};


const struct pinmap volts_type03[] = {
	{ VOLT_VCOREA,	"Vcore"		},
	{ VOLT_VCOREB,	"+1.5V"		},
	{ VOLT_VSEN1,	"-12V"		},
	{ VOLT_VSEN2,	"Vdimm"		},
	{ VOLT_3VSEN,	"+3.3V"		},
	{ VOLT_12VSEN,	"+12V"		},
	{ VOLT_5VSB,	"5Vsb"		},
	{ VOLT_5VDD,	"5VDD"		},
	{ VOLT_VTT,	"P_VTT"		},
	{ VOLT_VBAT,	"Vbat"		},
	{ 0,		NULL		}
};
const struct pinmap temps_type03[] = {
	{ TEMP_TD1,	"CPU Temperature"	},
	{ TEMP_TR1,	"System Temperature"	},
	{ 0,		NULL			}
};
const struct pinmap fans_type03[] = {
	{ FAN_FAN1,	"FAN1"		},
	{ FAN_FAN2,	"FAN2"		},
	{ FAN_FAN3,	"FAN3"		},
	{ FAN_FAN4,	"FAN4"		},
	{ FAN_FAN5,	"FAN5"		},
	{ FAN_FAN7,	"FAN6"		},	/* Not a typo! */
	{ 0,		NULL		}
};


const struct pinmap volts_type04[] = {
	{ VOLT_VCOREA,	"CPU Core"	},
	{ VOLT_VCOREB,	"+1.5V"		},
	{ VOLT_VIN0,	"+3.3V"		},
	{ VOLT_VIN1,	"+5V"		},
	{ VOLT_VIN2,	"-12V"		},
	{ VOLT_VIN3,	"+12V"		},
	{ VOLT_VBAT,	"3.3Vsb"	},
	{ 0,		NULL		}
};
const struct pinmap temps_type04[] = {
	{ TEMP_TD1,	"System Temperature"	},
	{ TEMP_TD2,	"CPU Temperature"	},
	{ 0,		NULL			}
};
const struct pinmap fans_type04[] = {
	{ FAN_FAN1,	"FAN1"		},
	{ FAN_FAN2,	"FAN2"		},
	{ FAN_FAN3,	"FAN3"		},
	{ FAN_FAN4,	"FAN4"		},
	{ FAN_FAN5,	"FAN5"		},
	{ FAN_FAN6,	"FAN6"		},
	{ 0,		NULL		}
};


const struct pinmap volts_type05[] = {
	{ VOLT_VCOREA,	"VcoreA"	},
	{ VOLT_VSEN1,	"-12V"		},
	{ VOLT_VSEN2,	"V_DIMM"	},
	{ VOLT_3VSEN,	"+3.3V"		},
	{ VOLT_12VSEN,	"+12V"		},
	{ VOLT_5VSB,	"5Vsb"		},
	{ VOLT_5VDD,	"5VDD"		},
	{ VOLT_VBAT,	"Vbat"		},
	{ 0,		NULL		}
};
const struct pinmap temps_type05[] = {
	{ TEMP_TD1,	"CPU Temperature"	},
	{ TEMP_TR2,	"System Temperature"	},	/* Not a typo! */
	{ 0,		NULL			}
};
const struct pinmap fans_type05[] = {
	{ FAN_FAN2,	"FAN1"		},	/* Not a typo! */
	{ FAN_FAN1,	"FAN2"		},	/* Not a typo! */
	{ FAN_FAN3,	"FAN3"		},
	{ FAN_FAN4,	"FAN4"		},
	{ FAN_FAN5,	"FAN5"		},
	{ FAN_FAN7,	"FAN6"		},	/* Not a typo! */
	{ 0,		NULL		}
};


const struct pinmap volts_type06[] = {
	{ VOLT_VCOREA,	"VcoreA"	},
	{ VOLT_VCOREB,	"MCH Core"	},	/* Undocumented */
	{ VOLT_VSEN1,	"-12V"		},
	{ VOLT_VSEN2,	"V_DIMM"	},
	{ VOLT_3VSEN,	"+3.3V"		},
	{ VOLT_12VSEN,	"+12V"		},
	{ VOLT_5VSB,	"5Vsb"		},
	{ VOLT_5VDD,	"5VDD"		},
	{ VOLT_VTT,	"P_VTT"		},	/* Undocumented */
	{ VOLT_VBAT,	"Vbat"		},
	{ 0,		NULL		}
};
const struct pinmap temps_type06[] = {
	{ TEMP_TD1,	"CPU1 Temperature"	},
	{ TEMP_TR2,	"System Temperature"	},	/* Not a typo! */
	{ 0,		NULL			}
};
const struct pinmap fans_type06[] = {
	{ FAN_FAN7,	"FAN1"		},	/* Not a typo! */
	{ FAN_FAN5,	"FAN2"		},	/* Not a typo! */
	{ FAN_FAN1,	"FAN3"		},	/* Not a typo! */
	{ FAN_FAN2,	"FAN4"		},	/* Not a typo! */
	{ FAN_FAN3,	"FAN5"		},	/* Not a typo! */
	{ FAN_FAN4,	"FAN6"		},	/* Not a typo! */
	{ 0,		NULL		}
};


const struct pinmap temps_type07[] = {
	{ TEMP_TD1,	"PECI Agent 1"		},
	{ TEMP_TD2,	"PECI Agent 2"		},
	{ TEMP_TR1,	"System Temperature"	},
	{ 0,		NULL			}
};
const struct pinmap fans_type07[] = {
	{ FAN_FAN1,	"FAN1"		},
	{ FAN_FAN2,	"FAN2"		},
	{ FAN_FAN3,	"FAN3"		},
	{ FAN_FAN4,	"FAN4"		},
	{ FAN_FAN5,	"FAN5"		},
	{ FAN_FAN6,	"FAN6"		},
	{ FAN_FAN7,	"FAN7"		},
	{ FAN_FAN8,	"FAN8"		},
	{ 0,		NULL		}
};


const struct pinmap volts_type08[] = {
	{ VOLT_VCOREA,	"CPU1 Vcore"	},
	{ VOLT_VCOREB,	"CPU2 Vcore"	},
	{ VOLT_3VSEN,	"+3.3V"		},
	{ VOLT_5VDD,	"+5V"		},
	{ VOLT_VSEN1,	"-12V"		},
	{ VOLT_VSEN2,	"+1.5V"		},
	{ VOLT_5VSB,	"5VSB"		},
	{ VOLT_VBAT,	"VBAT"		},
	{ VOLT_12VSEN,	"+12V"		},
	{ VOLT_VTT,	"P_VTT"		},
	{ 0,		NULL		}
};
const struct pinmap temps_type08[] = {
	{ TEMP_TD1,	"CPU Temp 1"	},
	{ TEMP_TD2,	"CPU Temp 2"	},
	{ TEMP_TD3,	"CPU Temp 3"	},
	{ TEMP_TD4,	"CPU Temp 4"	},
	{ TEMP_TR1,	"Sys Temp"	},
	{ 0,		NULL		}
};


const struct pinmap volts_type09[] = {
	{ VOLT_VCOREA,	"CPU1 Vcore"	},
	{ VOLT_VCOREB,	"CPU2 Vcore"	},
	{ VOLT_VIN0,	"+1.5V"		},
	{ VOLT_VIN2,	"+3.3V"		},
	{ VOLT_VIN1,	"+3.3VSB"	},
	{ VOLT_VIN3,	"+5V"		},
	{ VOLT_12VSEN,	"+12V"		},
	{ VOLT_VSEN1,	"-12V"		},
	{ 0,		NULL		}
};
const struct pinmap temps_type09[] = {
	{ TEMP_TD2,	"CPU Temp 1"	},
	{ TEMP_TD3,	"CPU Temp 2"	},
	{ TEMP_TD1,	"Sys Temp"	},
	{ 0,		NULL		}
};


/*
 * Supermicro X7DVL & X7DCL
 */
const struct pinmap temps_type10[] = {
	{ TEMP_TD1,	"PECI Agent 1"	},
	{ TEMP_TD2,	"PECI Agent 2"	},
	{ TEMP_TD3,	"PECI Agent 3"	},
	{ TEMP_TD4,	"PECI Agent 4"	},
	{ TEMP_TR1,	"Sys Temp"	},
	{ 0,		NULL		}
};


/*
 * The product strings in the below table are what are returned by
 * kenv(2), listed under smbios.planar.product.
 */
struct board boardlist[] = {
  /* maker		product		chip ID			slave	voltages	temperatures	fans		*/
  { "Supermicro",	"P8SC8",	WINBOND_W83792D,	0x2f,	volts_type00,	temps_type00,	fans_type00	},
  { "Supermicro",	"P8SCT",	WINBOND_W83792D,	0x2f,	volts_type00,	temps_type00,	fans_type00	},
  { "Supermicro",	"PDSMA+",	WINBOND_W83793G,	0x2f,	volts_type03,	temps_type03,	fans_type03	},
  { "Supermicro",	"PDSMi+",	WINBOND_W83793G,	0x2f,	volts_type03,	temps_type03,	fans_type03	},
  { "Supermicro",	"PDSMU",	WINBOND_W83793G,	0x2f,	volts_type03,	temps_type03,	fans_type04	},
  { "Supermicro",	"X6DHR-8G2/X6DHR-TG", WINBOND_W83792D,	0x2f,	volts_type01,	temps_type01,	fans_type00	},
  { "Supermicro",	"X6DVA",	CUSTOM_X6DVA,		-1,	volts_type09,	temps_type09,	fans_type04	},
  { "Supermicro",	"X7DB8",	WINBOND_W83793G,	0x2f,	volts_type02,	temps_type07,	fans_type07	},
  { "Supermicro",	"X7DBP",	WINBOND_W83793G,	0x2f,	volts_type02,	temps_type02,	fans_type02	},
  { "Supermicro",	"X7DBT",	WINBOND_W83793G,	0x2f,	volts_type08,	temps_type08,	fans_type07	},
  { "Supermicro",	"X7SB4/E",	WINBOND_W83793G,	0x2f,	volts_type05,	temps_type05,	fans_type05	},
  { "Supermicro",	"X7SBA",	WINBOND_W83793G,	0x2f,	volts_type06,	temps_type06,	fans_type06	},
  { "Supermicro",	"X7SBL",	WINBOND_W83793G,	0x2f,	volts_type06,	temps_type06,	fans_type06	},
  { "Supermicro",	"X7SBi",	WINBOND_W83793G,	0x2f,	volts_type06,	temps_type06,	fans_type06	},
  { "Supermicro",	"X7DVL-3",	WINBOND_W83793G,	0x2f,	volts_type03,	temps_type10,	fans_type05	},
  { "Supermicro",	"X7DVL",	WINBOND_W83793G,	0x2f,	volts_type03,	temps_type10,	fans_type05	},
  { "Supermicro",	"X7DCL",	WINBOND_W83793G,	0x2f,	volts_type03,	temps_type10,	fans_type05	},
  { NULL,		NULL,		0,			0,	NULL,		NULL,		NULL		}

/*
 * Chips under development...
 */
/*
  { "Supermicro",	"C2G41",	ITE_IT8720F_HX,		0x2d,	XXX,		XXX,		XXX		},
---
  { "Supermicro",	"C2SEA",	WINBOND_W83627DHG,	0x2d,	XXX,		XXX,		XXX		},
  { "Supermicro",	"C2SEE",	WINBOND_W83627DHG,	0x2d,	XXX,		XXX,		XXX		},
---
  { "Supermicro",	"X7SB3-F",	WINBOND_W83627DHG,	0x2d,	XXX,		XXX,		XXX		},
---
  { "Supermicro",	"X7SLM",	WINBOND_W83627DHG,	0x2d,	XXX,		XXX,		XXX		},
  { "Supermicro",	"X7SLM+",	WINBOND_W83627DHG,	0x2d,	XXX,		XXX,		XXX		},
  { "Supermicro",	"X7SLM-L",	WINBOND_W83627DHG,	0x2d,	XXX,		XXX,		XXX		},
---
  { "Supermicro",	"X8SI6",	WINBOND_W83627DHG,	0x2d,	XXX,		XXX,		XXX		},
  { "Supermicro",	"X8SIE",	WINBOND_W83627DHG,	0x2d,	XXX,		XXX,		XXX		},
  { "Supermicro",	"X8SIL",	WINBOND_W83627DHG,	0x2d,	XXX,		XXX,		XXX		},
  { "Supermicro",	"X8STI",	XXX,			0x2e,	XXX,		XXX,		XXX		},
*/
};

/*
 * X7SLM, X7SLM+, and X7SLM-L all claim to support the following voltages in the BIOS,
 * according to the user manual:
 *
 * Vcore, +3.3Vcc, +12V, VDIMM, +5V, Chipset 1.5V, 3.3VStandby, +5VStandby, Vbatt.
 */

/*
 * Function prototypes
 */
struct board *board_lookup(const char *, const char *);

/*
 * External functions (main.c)
 */
extern void VERBOSE(const char *, ...);

/*
 * External functions (output.c)
 */
extern const char *get_chip_string(const size_t);


/*
 * board_lookup(const char *maker, const char *product)
 *
 *   maker = ASCII string; smbios.planar.maker kenv(2)
 * product = ASCII string; smbios.planar.product kenv(2)
 *
 * Compares maker and product strings against each maker and product
 * strings in the boardlist[] struct.  Both strings must match for
 * successful detection.
 *
 * If a match is found, returns a pointer to the board structure which
 * will be used (e.g. &boardlist[bidx].
 *
 * Otherwise, return NULL (no match).
 */
struct board *
board_lookup(const char *maker, const char *product)
{
	size_t bidx = 0;
	size_t i;
	struct board *b;

	VERBOSE("board_lookup(maker = %p, product = %p)\n", maker, product);

	VERBOSE("\tmaker   = %s\n", maker);
	VERBOSE("\tproduct = %s\n", product);

	for (bidx = 0; boardlist[bidx].maker != NULL; ++bidx) {
		if (strncmp(maker, boardlist[bidx].maker, KENV_MVALLEN) == 0) {
			if (strncmp(product, boardlist[bidx].product, KENV_MVALLEN) == 0) {
				b = &boardlist[bidx];

				VERBOSE("\tboards struct: %p\n", b);
				VERBOSE("\t\tchip  = %s\n", get_chip_string(b->chip));
				VERBOSE("\t\tslave = 0x%02x\n", b->slave);

				VERBOSE("\tvoltages struct: %p\n", b->voltages);
				for (i = 0; b->voltages[i].label != NULL; ++i) {
					VERBOSE("\t\tb->voltages[%u] = %u, %s\n", i,
						b->voltages[i].index,
						b->voltages[i].label
					);
				}

				VERBOSE("\ttemps struct: %p\n", b->temps);
				for (i = 0; b->temps[i].label != NULL; ++i) {
					VERBOSE("\t\tb->temps[%u] = %u, %s\n", i,
						b->temps[i].index,
						b->temps[i].label
					);
				}

				VERBOSE("\tfans struct: %p\n", b->fans);
				for (i = 0; b->fans[i].label != NULL; ++i) {
					VERBOSE("\t\tb->fans[%u] = %u, %s\n", i,
						b->fans[i].index,
						b->fans[i].label
					);
				}

				VERBOSE("board_lookup() returning %p\n", b);
				return (b);
			}
		}
	}
	VERBOSE("board_lookup() returning NULL\n");
	return (NULL);
}

