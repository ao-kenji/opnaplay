/*
 * Copyright (c) 2015 Kenji Aoyama.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/types.h>

/*
 * Tweaking YM2608 (OPNA) on PC-9801-86
 */

/* prototypes */
int	opna_open(void);
void	opna_close(void);
void	opna_set_debug_level(int);
int	opna_set_note(int, int, int);
int	opna_set_sound(int, int);

/* structure */
struct timbre {
	u_int8_t AL;
	u_int8_t FB;
	u_int8_t AR1, DR1, SR1, RR1, SL1, TL1, KS1, ML1, DT11, AMS1;
	u_int8_t AR2, DR2, SR2, RR2, SL2, TL2, KS2, ML2, DT12, AMS2;
	u_int8_t AR3, DR3, SR3, RR3, SL3, TL3, KS3, ML3, DT13, AMS3;
	u_int8_t AR4, DR4, SR4, RR4, SL4, TL4, KS4, ML4, DT14, AMS4;
};

/* notes */
#define NOTE_C_S	 0	/* C# */
#define NOTE_D		 1	/* D  */
#define NOTE_D_S	 2	/* D# */
#define NOTE_E		 3	/* E  */
#define NOTE_F		 4	/* F  */
#define NOTE_F_S	 5	/* F# */
#define NOTE_G		 6	/* G  */
#define NOTE_G_S	 7	/* G# */
#define NOTE_A		 8	/* A  */
#define NOTE_A_S	 9	/* A# */
#define NOTE_B		10	/* B  */
#define NOTE_C		11	/* C  */
