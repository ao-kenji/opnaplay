/*
 * FM sound parameters are originally from:
 *
 *  VALSOUND FM-Library / (C)Takeshi Abo.
 *  http://www.valsound.com/
 */

#include "opna.h"

const struct timbre timbre_lib[] = {
	/* Aco Piano2 (Attack) */
	{   4,   5,
	   31,   5,   0,   0,   0,  23,   1,   1,   3,   0,
	   20,  10,   3,   7,   8,   0,   1,   1,   3,   0,
	   31,   3,   0,   0,   0,  25,   1,   1,   7,   0,
	   31,  12,   3,   7,  10,   2,   1,   1,   7,   0 },
	/* Old fulute */
	{   2,   7,
	   20,   5,   0, 14,   1,  50,   0,   4,   0,   0,
	   15,  15,   0, 14,   2,  45,   0,   8,   0,   0,
	   18,  15,   0, 14,   2,  50,   0,   8,   0,   0,
	   14,   2,   0, 14,   0,   0,   0,   4,   0,   0 },
	/* Koto */
	{   3,   0,
	   31,   0,   0, 10,   0,  38,   0,   6,   0,   0,
	   24,  13,   0, 10,   5,  40,   0,   2,   3,   0,
	   28,  15,   0, 10,   3,  40,   0,   4,   0,   0,
	   24,  12,   8, 10,   2,   0,   1,   2,   0,   0 },
};

int opna_ntimbres = sizeof(timbre_lib) / sizeof(timbre_lib[0]);
