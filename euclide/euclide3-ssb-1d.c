/*
	Copyright (C) 2012,2015 European Space Agency
	Copyright (C) 2016,2017 Raphael 'kena' Poss

        euclide3-ssb-1d is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	this program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>

#include "bench.c"
#include "euclide.h"

//	This function removes the "super-bias" from the "frame" given as input #1.
//	The function will iterate through all pixels, one by one, and
//	update the original "frame" data.
sl_def(substractSuperBias,,
       sl_glparm(frame16ptr_t , frame),
       sl_glparm(frame16ptr_t , bias)
    ) {
    sl_index(stripe);
    frame16ptr_t frame = sl_getp(frame);
    frame16ptr_t bias = sl_getp(bias);
    
    for (unsigned y = 0; y < FRAME_DIM_Y; ++y) {
	for (unsigned x = stripe * PIXEL_PER_STRIPE; x < (stripe + 1) * PIXEL_PER_STRIPE; ++x) {
		(*frame)[y][x] = (*frame)[y][x] - (*bias)[y][x];
	}
    }
} sl_enddef

int main(void) {
    frame16ptr_t frame = calloc(1,sizeof(frame16_t));
    frame16ptr_t bias = calloc(1,sizeof(frame16_t));
    
#ifndef BLOCKSIZE
#define BLOCKSIZE 0
#endif
    int b = BLOCKSIZE;

    dbgstdout = stduart;
    
    counter_t c1, c2;
    counter_t i1, i2;

    int i, j;
    c1 = mtperf_sample1(MTPERF_CLOCKS); 
    i1 = mtperf_sample1(MTPERF_EXECUTED_INSNS); 
    sl_create(,,,STRIPES_PER_FRAME,,BLOCKSIZE,, substractSuperBias,
	      sl_glarg(frame16ptr_t,,frame),
	      sl_glarg(frame16ptr_t,,bias));
    sl_sync();
    c2 = mtperf_sample1(MTPERF_CLOCKS); 
    i2 = mtperf_sample1(MTPERF_EXECUTED_INSNS); 
    report_perf(FRAME_RATIO, b, "substractSuperBias", delta(c2,c1), delta(i2,i1));
    return 0;
}
