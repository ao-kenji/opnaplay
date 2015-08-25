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
#include <unistd.h>	/* getopt(3), usleep(3) */
#include <sys/ioctl.h>	/* ioctl(2) */
#include <sys/mman.h>	/* mmap(2) */
#include <machine/pcex.h>

#include "opna.h"

void	usage(void);

int
main(int argc, char **argv)
{
	u_int8_t val;

	/*
	 * parse options
	 */
	int ch;
	extern char *optarg;
	extern int optind, opterr;

	while ((ch = getopt(argc, argv, "dr:")) != -1) {
		switch (ch) {
		case 'd':	/* debug flag */
			opna_set_debug_level(1);
			break;
		default:
			usage();
			break;
		}
	}
	argc -= optind;
	argv += optind;

	if (argc != 0) {
		usage();
		return 1;
	}

	if (opna_open() == -1)
		return 1;

	opna_init();
	opna_set_sound(0, 0);
	opna_set_sound(1, 1);

	opna_set_note(0, 3, NOTE_C);
	opna_set_note(1, 4, NOTE_C);
	opna_write(0x28, 0xf0 | 0);	/* note on ch.0 */
	opna_write(0x28, 0xf0 | 1);	/* note on ch.0 */
	sleep(1);
	opna_write(0x28, 0x00 | 0);	/* note off ch.0 */
	opna_write(0x28, 0x00 | 1);	/* note off ch.0 */

	opna_set_note(0, 4, NOTE_D);
	opna_set_note(1, 5, NOTE_D);
	opna_write(0x28, 0xf0 | 0);	/* note on ch.0 */
	opna_write(0x28, 0xf0 | 1);	/* note on ch.0 */
	sleep(1);
	opna_write(0x28, 0x00 | 0);	/* note off ch.0 */
	opna_write(0x28, 0x00 | 1);	/* note off ch.0 */

	opna_set_note(0, 4, NOTE_E);
	opna_set_note(1, 5, NOTE_E);
	opna_write(0x28, 0xf0 | 0);	/* note on ch.0 */
	opna_write(0x28, 0xf0 | 1);	/* note on ch.0 */
	sleep(1);
	opna_write(0x28, 0x00 | 0);	/* note off ch.0 */
	opna_write(0x28, 0x00 | 1);	/* note off ch.0 */

	opna_write(0x28, 0xf0 | 1);
	sleep(1);
	opna_write(0x28, 0x00 | 1);

	/* something to do */

	val = opna_read(0xff);
	printf("reg(0xff) = 0x%02x\n", val);

	opna_close();
	return 0;
}

/*
 * Usage
 */
void
usage(void)
{
	printf("Usage: %s [options]\n", getprogname());
	printf("\t-d	: debug flag\n");
	exit(1);
}
