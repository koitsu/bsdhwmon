/*
 * SPDX-License-Identifier: BSD-2-Clause-FreeBSD
 */

#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <inttypes.h>
#include <sysexits.h>
#include "global.h"

/*
 * Function prototypes
 */
const char *	get_chip_string(const size_t);
void		list_models(const struct board *);
void		sensors_output(const struct board *, const struct sensors *);
void		sensors_output_delim(const struct board *, const struct sensors *);
void		sensors_output_json(const struct board *, const struct sensors *);


/*
 * get_chip_string(size_t idx)
 *
 * idx = index value; see "chips_e" enum in global.h
 *
 * Return pointer to an ASCII string representation of the H/W
 * monitoring chip model.
 */
const char *
get_chip_string(const size_t idx)
{
	switch (idx) {
		case CUSTOM_X6DVA:	return ("X6DVA/X6DVL/X6DAL");
		case WINBOND_W83792D:	return ("Winbond W83792D");
		case WINBOND_W83793G:	return ("Winbond W83793G");
	}
	return ("Unknown");
}


/*
 * list_models(struct board *b)
 *
 * b = Pointers to board struct; see boards.c for a definition
 *
 * Outputs a list of everything in the boardlist[] struct.  This is
 * used when calling the main program with the "-l" argument.
 */
void
list_models(const struct board *b)
{
	size_t i = 0;

	VERBOSE("list_models(b = %p)\n", b);

	printf("maker          product                chip type           slave addr\n");
	printf("-------------  ---------------------  ------------------  -----------\n");

	while (b[i].maker != NULL) {
		/*
		 * For boards with multiple SMBus slave addresses or custom
		 * methods, show "Custom" rather than the SMBus slave
		 * address.
		 */
		if (b[i].slave == -1) {
			printf("%-13s  %-21s  %-18s  Custom\n",
				b[i].maker,
				b[i].product,
				get_chip_string(b[i].chip)
			);
		} else {
			printf("%-13s  %-21s  %-18s  0x%02x\n",
				b[i].maker,
				b[i].product,
				get_chip_string(b[i].chip),
				b[i].slave
			);
		}
		++i;
	}
	printf("-------------  ---------------------  ------------------  -----------\n");

	VERBOSE("list_models() returning\n");
	exit(EX_OK);
}


void
sensors_output(const struct board *b, const struct sensors *s)
{
	size_t i;
	size_t enum_index;

	VERBOSE("sensors_output(b = %p, s = %p)\n", b, s);

	for (i = 0; b->temps[i].label != NULL; ++i) {
		enum_index = b->temps[i].index;

		printf("%-20s %8zu C\n",
			b->temps[i].label,
			s->temps[enum_index].value
		);
	}

	for (i = 0; b->fans[i].label != NULL; ++i) {
		enum_index = b->fans[i].index;

		printf("%-20s %8" PRIu32 " RPM\n",
			b->fans[i].label,
			s->fans[enum_index].value
		);
	}

	for (i = 0; b->voltages[i].label != NULL; ++i) {
		enum_index = b->voltages[i].index;

		printf("%-20s %8.3f V\n",
			b->voltages[i].label,
			s->voltages[enum_index].value
		);
	}

	VERBOSE("sensors_output() returning\n");
}


void
sensors_output_delim(const struct board *b, const struct sensors *s)
{
	size_t i;
	size_t enum_index;

	VERBOSE("sensors_output_delim(b = %p, s = %p)\n", b, s);

	for (i = 0; b->temps[i].label != NULL; ++i) {
		enum_index = b->temps[i].index;

		printf("%s,%zu,C\n",
			b->temps[i].label,
			s->temps[enum_index].value
		);
	}

	for (i = 0; b->fans[i].label != NULL; ++i) {
		enum_index = b->fans[i].index;

		printf("%s,%" PRIu32 ",RPM\n",
			b->fans[i].label,
			s->fans[enum_index].value
		);
	}

	for (i = 0; b->voltages[i].label != NULL; ++i) {
		enum_index = b->voltages[i].index;

		printf("%s,%.3f,V\n",
			b->voltages[i].label,
			s->voltages[enum_index].value
		);
	}

	VERBOSE("sensors_output_delim() returning\n");
}


void
sensors_output_json(const struct board *b, const struct sensors *s)
{
	size_t i;
	size_t enum_index;

	VERBOSE("sensors_output_json(b = %p, s = %p)\n", b, s);

	printf("{\n");

	printf("\t\"temps\": {\n");
	for (i = 0; b->temps[i].label != NULL; ++i) {
		enum_index = b->temps[i].index;

		printf("\t\t\"%s\": \"%zu C\"%s\n",
			b->temps[i].label,
			s->temps[enum_index].value,
			(b->temps[i+1].label == NULL ? "" : ",")
		);
	}
	printf("\t},\n");

	printf("\t\"fans\": {\n");
	for (i = 0; b->fans[i].label != NULL; ++i) {
		enum_index = b->fans[i].index;

		printf("\t\t\"%s\": \"%" PRIu32 " RPM\"%s\n",
			b->fans[i].label,
			s->fans[enum_index].value,
			(b->fans[i+1].label == NULL ? "" : ",")
		);
	}
	printf("\t},\n");

	printf("\t\"voltages\": {\n");
	for (i = 0; b->voltages[i].label != NULL; ++i) {
		enum_index = b->voltages[i].index;

		printf("\t\t\"%s\": \"%.3f V\"%s\n",
			b->voltages[i].label,
			s->voltages[enum_index].value,
			(b->voltages[i+1].label == NULL ? "" : ",")
		);
	}
	printf("\t}\n");

	printf("}\n");
	VERBOSE("sensors_output_json() returning\n");
}

