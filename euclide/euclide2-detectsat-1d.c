/*
	Copyright (C) 2012,2015 European Space Agency
	Copyright (C) 2016,2017 Raphael 'kena' Poss

        euclide2-detectsat-1d is free software: you can redistribute it and/or modify
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
#include "slext.h"

// This function detects if a pixel is above the saturation limit
// and flags it in the saturationFrame
sl_def(detectSaturation,,
       sl_glparm(frame16ptr_t, frame),
       sl_glparm(stripe_vec16ptr_t, saturationLimit),
       sl_glparm(frame8ptr_t, saturationFrame),
       sl_redvar(redresults))
{
    sl_index(stripe);

    frame16ptr_t frame = sl_getp(frame);
    stripe_vec16ptr_t saturationLimit = sl_getp(saturationLimit);
    frame8ptr_t saturationFrame = sl_getp(saturationFrame);

    uint16_t saturationLimitValue = (*saturationLimit)[stripe];
    uint32_t nbPixelSature = 0;
    for (unsigned y = 0; y < FRAME_DIM_Y; ++y) {
	for (unsigned x = stripe * PIXEL_PER_STRIPE; x < (stripe + 1) * PIXEL_PER_STRIPE; ++x) {
	    if ((*frame)[y][x] > saturationLimitValue) {
		(*saturationFrame)[y][x] = 1;
		nbPixelSature++;
	    }
	}
    }
    sl_setresult(redresults, sl_getresult(redresults) + nbPixelSature);
}
sl_enddef

int main(void) {
    
    frame8ptr_t saturationFrame = calloc(1,sizeof(frame8_t));
    frame16ptr_t frame = calloc(1,sizeof(frame16_t));
    stripe_vec16ptr_t saturationLimit = calloc(1,sizeof(stripe_vec16_t));
#ifndef BLOCKSIZE
#define BLOCKSIZE 0
#endif
    int b = BLOCKSIZE;

    dbgstdout = stduart;
    
    counter_t c1, c2;
    counter_t i1, i2;

    unsigned srb;
    sl_alloc_redvar(srb);
    if (srb == 0) {
	fprintf(stduart, "can't alloc sRB\n");
	return 1;
    }
    int i, j;
    for (i = 0; i < FRAME_DIM_Y; i++)
	for (j = 0; j < FRAME_DIM_X; j++) {
	    (*frame)[i][j] = (i + j) % 3;
	}
    c1 = mtperf_sample1(MTPERF_CLOCKS); 
    i1 = mtperf_sample1(MTPERF_EXECUTED_INSNS); 
    sl_create(,,,STRIPES_PER_FRAME,,BLOCKSIZE,, detectSaturation,
	      sl_glarg(frame16ptr_t,,frame),
	      sl_glarg(stripe_vec16ptr_t,, saturationLimit),
	      sl_glarg(frame8ptr_t,,saturationFrame),
	      sl_redvararg(srb));
    sl_sync();
    unsigned nbPixelSature = sl_foldvar(srb, uint32_t, +, 0);
    c2 = mtperf_sample1(MTPERF_CLOCKS); 
    i2 = mtperf_sample1(MTPERF_EXECUTED_INSNS);

    sl_free_redvar(srb);
    
    report_perf(FRAME_RATIO, b, "detectSaturation", delta(c2,c1), delta(i2,i1));
    fprintf(stduart, "nb pixels: %u\n", nbPixelSature);
    
    return 0;
}

