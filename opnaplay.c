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
#include <unistd.h>	/* getopt(3) */

#include "mml.h"
#include "opna.h"

void	usage(void);
void mml_callback(MML_INFO *, void *);

int
main(int argc, char **argv)
{

	/* MML related */
	MML mml;
	MML_OPTION mml_opt;
	MML_RESULT mr;
	void *mml_extobj;

	/*
	 * parse options
	 */
	int ch;
	extern char *optarg;
	extern int optind, opterr;

	while ((ch = getopt(argc, argv, "dt:")) != -1) {
		switch (ch) {
		case 'd':	/* debug flag */
			opna_set_debug_level(1);
			break;
		case 't':	/* tone */
			opna_timbre = atoi(optarg);
			break;
		default:
			usage();
			break;
		}
	}
	argc -= optind;
	argv += optind;

	if (argc != 1) {
		usage();
		return 1;
	}

	if ((opna_timbre < 0) || (opna_timbre >= opna_ntimbres)) {
		usage();
		return 1;
	}

	if (opna_open() == -1)
		return 1;

	opna_init();
	opna_set_sound(0, opna_timbre);

	mml_extobj = NULL;
	mr = mml_init(&mml, mml_callback, mml_extobj);
	if (mr != MML_RESULT_OK) {
		printf("[ERROR] : init failed.\n");
		return 1;
	}

	MML_OPTION_INITIALIZER_DEFAULT(&mml_opt);
	mr = mml_setup(&mml, &mml_opt, argv[0]);
	if (mr != MML_RESULT_OK) {
		printf("[ERROR] : setup failed (input text='%s').\n", argv[0]);
	}

	while ((mr = mml_fetch(&mml)) == MML_RESULT_OK) {
	}

	if (mr != MML_RESULT_EOT) {
		printf("[ERROR] : The error code is %d.\n", (int)mr);
	}

	opna_close();
	return (mr == MML_RESULT_EOT) ? 0 : (int)mr;
}

/*
 * Usage
 */
void
usage(void)
{
	printf("Usage: %s [options] 'MML string'\n", getprogname());
	printf("\t-d        : debug flag\n");
	printf("\t-t number : specify timbre number (0 to %d)\n",
	    opna_ntimbres - 1);
	exit(1);
}
