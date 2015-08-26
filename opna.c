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

/*
 * PC-9801 extension board bus test program
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>	/* getprogname(3) */
#include <time.h>	/* nanosleep(2) */
#include <unistd.h>	/* getopt(3), usleep(3) */
#include <sys/ioctl.h>	/* ioctl(2) */
#include <sys/mman.h>	/* mmap(2) */
#include <machine/pcex.h>

#include "opna.h"

#define DPRINTF(x)      if (debug) printf x

/* global */
int	opna_debug = 0;
int	opna_ntimbres = 0;	/* number of pre-defined timbres */
int	pcexio_fd;
u_int8_t *pcexio_base;
u_int8_t *opna_bi_reg;	/* YM2608 Basic Index register */
u_int8_t *opna_bd_reg;	/* YM2608 Basic Data register */
u_int8_t *opna_ei_reg;	/* YM2608 Extended Index register */
u_int8_t *opna_ed_reg;	/* YM2608 Extended Data register */

extern struct timbre timbre_lib[];

u_int16_t F_number[] = {
	 654,	/* C# */
	 692,	/* D  */
	 734,	/* D# */
	 777,	/* E  */
	 823,	/* F  */
	 872,	/* F# */
	 924,	/* G  */
	 979,	/* G# */
	1038,	/* A  */
	1099,	/* A# */
	1165,	/* B  */
	1234,	/* C  */
};

/* prototypes (internal use) */
u_int8_t	opna_read(u_int8_t);
void		opna_write(u_int8_t, u_int8_t);

void
opna_init(void)
{
	u_int8_t data;

	/* enable YM2608(OPNA) mode (default is YM2203(OPN) mode) */
	data = opna_read(0x29);
	data |= 0x80;
	opna_write(0x29, data);

#if 0
	opna_ntimbres = sizeof(timbre_lib) / sizeof(timbre_lib[0]);
#endif
	opna_ntimbres = 2;
}

/*
 * Open
 */
int
opna_open(void)
{
	pcexio_fd = open("/dev/pcexio", O_RDWR, 0600);
	if (pcexio_fd == -1) {
		perror("open");
		return -1;
	}

	pcexio_base = (u_int8_t *)mmap(NULL, 0x10000, PROT_READ | PROT_WRITE,
		MAP_SHARED, pcexio_fd, 0x0);

	if (pcexio_base == MAP_FAILED) {
		perror("mmap");
		close(pcexio_fd);
		return -1;
	}

	opna_bi_reg = pcexio_base + 0x188;	
	opna_bd_reg = pcexio_base + 0x18a;	
	opna_ei_reg = pcexio_base + 0x18c;	
	opna_ed_reg = pcexio_base + 0x18e;	

	return pcexio_fd;
}

/*
 * Close
 */
void
opna_close(void)
{
	munmap((void *)pcexio_base, 0x10000);
	close(pcexio_fd);
}

/*
 * OPNA needs wait cycles
 *  after address write: 17 cycles @ 8MHz = 2.125 us 
 *  after data write   : 83 cycles @ 8MHz = 10.375 us
 */
struct timespec opna_wait_waddr = {
	.tv_sec = 0,
	.tv_nsec = 2125
};

struct timespec opna_wait_wdata = {
	.tv_sec = 0,
	.tv_nsec = 10375
};

/*
 * Register read/write
 */
u_int8_t
opna_read(u_int8_t index)
{
	u_int8_t ret;

	*opna_bi_reg = index;
	nanosleep(&opna_wait_waddr, NULL);
	ret = *opna_bd_reg;

	return ret;
}

void
opna_write(u_int8_t index, u_int8_t data)
{

	*opna_bi_reg = index;
	nanosleep(&opna_wait_waddr, NULL);
	*opna_bd_reg = data;
	nanosleep(&opna_wait_wdata, NULL);
}

void
opna_set_debug_level(int level)
{
	opna_debug = level;
}

int
opna_set_sound(int ch, int index)
{
	u_int8_t data;
	struct timbre *tp;

	if ((index < 0) || (index >= opna_ntimbres)) {
		printf("index %d: out of range\n", index);
		return -1;
	}

	if ((ch < 0) || (ch > 2)) {
		printf("ch %d: out of range\n", ch);
		return -1;
	}

	tp = &timbre_lib[index];

	/* Detune(3bit) & Multiple(4bit) */
	opna_write(0x30 + ch, (tp->DT11 << 4) | tp->ML1);	/* op 1 */
	opna_write(0x38 + ch, (tp->DT12 << 4) | tp->ML2);	/* op 2 */
	opna_write(0x34 + ch, (tp->DT13 << 4) | tp->ML3);	/* op 3 */
	opna_write(0x3c + ch, (tp->DT14 << 4) | tp->ML4);	/* op 4 */

	/* Total Level(7bit) */
	opna_write(0x40 + ch, tp->TL1);
	opna_write(0x48 + ch, tp->TL2);
	opna_write(0x44 + ch, tp->TL3);
	opna_write(0x4c + ch, tp->TL4);

	/* Key Scale(2bit) & Attack Rate(5bit) */
	opna_write(0x50 + ch, (tp->KS1 << 6) | tp->AR1);
	opna_write(0x58 + ch, (tp->KS2 << 6) | tp->AR2);
	opna_write(0x54 + ch, (tp->KS3 << 6) | tp->AR3);
	opna_write(0x5c + ch, (tp->KS4 << 6) | tp->AR4);

	/* AMON(1bit) & Decay Rate(5bit) */
	opna_write(0x60 + ch, (tp->AMS1 << 7) | tp->DR1);
	opna_write(0x68 + ch, (tp->AMS2 << 7) | tp->DR2);
	opna_write(0x64 + ch, (tp->AMS3 << 7) | tp->DR3);
	opna_write(0x6c + ch, (tp->AMS4 << 7) | tp->DR4);

	/* Sustain Rate(5bit) */
	opna_write(0x70 + ch, tp->SR1);
	opna_write(0x78 + ch, tp->SR2);
	opna_write(0x74 + ch, tp->SR3);
	opna_write(0x7c + ch, tp->SR4);

	/* Sustain Level(4bit) & Release Rate(4bit) */
	opna_write(0x80 + ch, (tp->SL1 << 4) | tp->RR1);
	opna_write(0x88 + ch, (tp->SL2 << 4) | tp->RR2);
	opna_write(0x84 + ch, (tp->SL3 << 4) | tp->RR3);
	opna_write(0x8c + ch, (tp->SL4 << 4) | tp->RR4);

	/* Self-Feedback & Algorythm */
	opna_write(0xB0 + ch, (tp->FB << 3) | tp->AL);

	/* L&R on */
	data = opna_read(0xb4 + ch);
	data |= 0xc0;
	opna_write(0xb4 + ch, data);

	/* f-number, block -> 'A4' */
	opna_set_note(ch, 4, NOTE_A);
	
	return 0;
}

int
opna_set_note(int ch, int oct, int note) {

	if ((ch < 0) || (ch > 2)) {
		printf("ch %d: out of range\n", ch);
		return -1;
	}

	if ((oct < 0) || (oct > 7)) {
		printf("octave %d: out of range\n", oct);
		return -1;
	}

	opna_write(0xa4 + ch, oct << 3 | ((F_number[note] & 0x700) >> 8));
	opna_write(0xa0 + ch, F_number[note] & 0x0ff);
	printf("0x%02x, 0x%02x\n",
		oct << 3 | ((F_number[note] & 0x700) >> 8),
		F_number[note] & 0x0ff);
};
