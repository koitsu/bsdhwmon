/*
 * SPDX-License-Identifier: BSD-2-Clause-FreeBSD
 */

#include <sys/types.h>

/*
 * A list of hardware monitoring ASIC types.  Every entry in this enum
 * should have a corresponding entry in boardlist[] (see boards.c).
 * Failure to do so could be detrimental.
 */
enum chips_e {
	CUSTOM_X6DVA,
	WINBOND_W83792D,
	WINBOND_W83793G
};

/*
 * Voltage pin labels used on any chip.
 */
enum voltages_e {
	VOLT_VCOREA,	/* Pin: VCOREA */
	VOLT_VCOREB,	/* Pin: VCOREB */
	VOLT_VTT,	/* Pin: VTT */
	VOLT_VSEN1,	/* Pin: VSEN1 */
	VOLT_VSEN2,	/* Pin: VSEN2 */
	VOLT_3VSEN,	/* Pin: 3VSEN */
	VOLT_12VSEN,	/* Pin: 12VSEN */
	VOLT_5VSB,	/* Pin: 5VSB */
	VOLT_5VCC,	/* Pin: 5VCC */
	VOLT_5VDD,	/* Pin: 5VDD */
	VOLT_VBAT,	/* Pin: VBAT */
	VOLT_VIN0,	/* Pin: VIN0 */
	VOLT_VIN1,	/* Pin: VIN1 */
	VOLT_VIN2,	/* Pin: VIN2 */
	VOLT_VIN3,	/* Pin: VIN3 */
	VOLT_MAX	/* must come last */
};


/*
 * Diode or thermistor pin labels used on any chip.
 */
enum temps_e {
	TEMP_TD1,	/* Pin: TD1 */
	TEMP_TD2,	/* Pin: TD2 */
	TEMP_TD3,	/* Pin: TD3 */
	TEMP_TD4,	/* Pin: TD4 */
	TEMP_TR1,	/* Pin: TR1 */
	TEMP_TR2,	/* Pin: TR1 */
	TEMP_MAX	/* must come last */
};


/*
 * Fan pin labels used on any chip.
 */
enum fans_e {
	FAN_FAN1,	/* Pin: FAN1 */
	FAN_FAN2,	/* Pin: FAN2 */
	FAN_FAN3,	/* Pin: FAN3 */
	FAN_FAN4,	/* Pin: FAN4 */
	FAN_FAN5,	/* Pin: FAN5 */
	FAN_FAN6,	/* Pin: FAN6 */
	FAN_FAN7,	/* Pin: FAN7 */
	FAN_FAN8,	/* Pin: FAN8 */
	FAN_FAN9,	/* Pin: FAN9 */
	FAN_FAN10,	/* Pin: FAN10 */
	FAN_FAN11,	/* Pin: FAN11 */
	FAN_FAN12,	/* Pin: FAN12 */
	FAN_MAX		/* must come last */
};


/*
 * The pinmap struct defines two pieces of information: an index value
 * (which refers to one of the above enums), and an ASCII character string
 * (which is what's printed to the user).
 *
 * The pinmap struct is used heavily in boards.c for declaring multiple
 * "types" of boards.  For example, FAN_FAN1 might be populated/assigned
 * inside of collect.c, but it might actually be wired to "FAN2" according
 * to the BIOS.  This provides a string-to-wire/pin mapping structure.
 *
 * It's also used in the boards struct; see further down...
 */
struct pinmap {
	size_t		index;		/* One of the above enums */
	const char	*label;		/* Name of pinmap (ASCII) */
};

struct voltages_data {
	size_t		index;		/* One of the above enums */
	double		value;
};

struct temps_data {
	size_t		index;		/* One of the above enums */
	size_t		value;
};

struct fans_data {
	size_t		index;		/* One of the above enums */
	uint32_t	value;
};

struct sensors {
	struct voltages_data	voltages[VOLT_MAX];
	struct temps_data	temps[TEMP_MAX];
	struct fans_data	fans[FAN_MAX];
};

struct board {
	const char		*maker;
	const char		*product;
	size_t			chip;
	int			slave;
	const struct pinmap	*voltages;
	const struct pinmap	*temps;
	const struct pinmap	*fans;
};

