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
int	opna_nsound = 0;	/* number of pre-defined sounds */
int	pcexio_fd;
u_int8_t *pcexio_base;
u_int8_t *opna_bi_reg;	/* YM2608 Basic Index register */
u_int8_t *opna_bd_reg;	/* YM2608 Basic Data register */
u_int8_t *opna_ei_reg;	/* YM2608 Extended Index register */
u_int8_t *opna_ed_reg;	/* YM2608 Extended Data register */

extern struct fmsound fmsound_lib[];

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

void
opna_init(void)
{
	u_int8_t data;

	/* enable YM2608(OPNA) mode (default is YM2203(OPN) mode) */
	data = opna_read(0x29);
	data |= 0x80;
	opna_write(0x29, data);

#if 0
	opna_nsound = sizeof(fmsound_lib) / sizeof(fmsound_lib[0]);
#endif
	opna_nsound = 2;
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
 * Register read/write
 */
u_int8_t
opna_read(u_int8_t index)
{
	u_int8_t ret;

	*opna_bi_reg = index;
	ret = *opna_bd_reg;

	return ret;
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
	struct fmsound *sp;

	if ((index < 0) || (index >= opna_nsound)) {
		printf("index %d: out of range\n");
		return -1;
	}

	if ((ch < 0) || (ch > 2)) {
		printf("ch %d: out of range\n");
		return -1;
	}

	sp = &fmsound_lib[index];

	/* Detune(3bit) & Multiple(4bit) */
	opna_write(0x30 + ch, (sp->DT11 << 4) | sp->ML1);	/* op 1 */
	opna_write(0x38 + ch, (sp->DT12 << 4) | sp->ML2);	/* op 2 */
	opna_write(0x34 + ch, (sp->DT13 << 4) | sp->ML3);	/* op 3 */
	opna_write(0x3c + ch, (sp->DT14 << 4) | sp->ML4);	/* op 4 */

	/* Total Level(7bit) */
	opna_write(0x40 + ch, sp->TL1);
	opna_write(0x48 + ch, sp->TL2);
	opna_write(0x44 + ch, sp->TL3);
	opna_write(0x4c + ch, sp->TL4);

	/* Key Scale(2bit) & Attack Rate(5bit) */
	opna_write(0x50 + ch, (sp->KS1 << 6) | sp->AR1);
	opna_write(0x58 + ch, (sp->KS2 << 6) | sp->AR2);
	opna_write(0x54 + ch, (sp->KS3 << 6) | sp->AR3);
	opna_write(0x5c + ch, (sp->KS4 << 6) | sp->AR4);

	/* AMON(1bit) & Decay Rate(5bit) */
	opna_write(0x60 + ch, (sp->AMS1 << 7) | sp->DR1);
	opna_write(0x68 + ch, (sp->AMS2 << 7) | sp->DR2);
	opna_write(0x64 + ch, (sp->AMS3 << 7) | sp->DR3);
	opna_write(0x6c + ch, (sp->AMS4 << 7) | sp->DR4);

	/* Sustain Rate(5bit) */
	opna_write(0x70 + ch, sp->SR1);
	opna_write(0x78 + ch, sp->SR2);
	opna_write(0x74 + ch, sp->SR3);
	opna_write(0x7c + ch, sp->SR4);

	/* Sustain Level(4bit) & Release Rate(4bit) */
	opna_write(0x80 + ch, (sp->SL1 << 4) | sp->RR1);
	opna_write(0x88 + ch, (sp->SL2 << 4) | sp->RR2);
	opna_write(0x84 + ch, (sp->SL3 << 4) | sp->RR3);
	opna_write(0x8c + ch, (sp->SL4 << 4) | sp->RR4);

	/* Self-Feedback & Algorythm */
	opna_write(0xB0 + ch, (sp->FB << 3) | sp->AL);

	/* L&R on */
	data = opna_read(0xb4 + ch);
	data |= 0xc0;
	opna_write(0xb4 + ch, data);

	/* f-number, block -> 'A4' */
	opna_write(0xa4 + ch, 0x24);
	opna_write(0xa0 + ch, 0x0e);
#if 0
	/* f-number, block -> 'A4' */
	opna_write(0xa4 + ch, 0x24);
	opna_write(0xa0 + ch, 0xe0);
#endif
	
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
