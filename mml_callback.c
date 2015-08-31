/*
 * Copyright (c) 2014-2015 Shinichiro Nakamura
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * MML to OPNA driver:
 *  mostly based on Shinichiro Nakamura's 'A tiny MML parser' sample program.
 */

#include <stdio.h>

#include "mml.h"
#include "opna.h"

inline int
opna_tick2ms(int ticks)
{
	return 60 * 1000 * ticks / opna_tempo / MML_OPTION_DEFAULT_BTICKS;
}

void
mml_callback(MML_INFO *p, void *extobj) {
	static int cnt = 0;

	printf("%3d : ", ++cnt);
	switch (p->type) {
	case MML_TYPE_NOTE:
		{
			MML_ARGS_NOTE *args = &(p->args.note);
			printf("[NOTE  : Number=%3d, Ticks=%4d]\n",
			    args->number, args->ticks);
			opna_set_note(0, args->number / 12, args->number % 12);
			opna_write(0x28, 0xf0 | 0);	/* note on ch.0 */
			opna_wait_ms(opna_tick2ms(args->ticks));
			opna_write(0x28, 0x00 | 0);	/* note off ch.0 */
		}
		break;
	case MML_TYPE_REST:
		{
			MML_ARGS_REST *args = &(p->args.rest);
			printf("[REST  :             Ticks=%4d]\n",
			    args->ticks);
			opna_wait_ms(opna_tick2ms(args->ticks));
		}
		break;
	case MML_TYPE_TEMPO:
		{
			MML_ARGS_TEMPO *args = &(p->args.tempo);
			printf("[TEMPO : Value=%3d]\n", args->value);
			opna_tempo = args->value;
		}
		break;
	case MML_TYPE_LENGTH:
		{
			MML_ARGS_LENGTH *args = &(p->args.length);
			printf("[LENGTH: Value=%3d]\n", args->value);
		}
		break;
	case MML_TYPE_VOLUME:
		{
			MML_ARGS_VOLUME *args = &(p->args.volume);
			printf("[VOLUME: Value=%3d]\n", args->value);
		}
		break;
	case MML_TYPE_OCTAVE:
		{
			MML_ARGS_OCTAVE *args = &(p->args.octave);
			printf("[OCTAVE: Value=%3d]\n", args->value);
		}
		break;
	case MML_TYPE_OCTAVE_UP:
		{
			MML_ARGS_OCTAVE *args = &(p->args.octave_up);
			printf("[OCTAVE: Value=%3d(Up)]\n", args->value);
		}
		break;
	case MML_TYPE_OCTAVE_DOWN:
		{
			MML_ARGS_OCTAVE *args = &(p->args.octave_down);
			printf("[OCTAVE: Value=%3d(Down)]\n", args->value);
		}
		break;
	case MML_TYPE_USER_EVENT:
		{
			MML_ARGS_USER_EVENT *args = &(p->args.user_event);
			printf("[USER_EVENT: Name='%s']\n", args->name);
		}
		break;
	}
}
