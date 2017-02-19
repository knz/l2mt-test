/*
	Copyright (C) 2012,2015 European Space Agency
	Copyright (C) 2016,2017 Raphael 'kena' Poss

        euclide5-srptb-1d is free software: you can redistribute it and/or modify
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

//	This function calculates the mean of the odd and even pixel of the first 4 and
//	last 4 rows and removes it from the odd and even pixel of the image.
sl_def(subtractReferencePixelTopBottom,,
       sl_glparm(frame16ptr_t , frame)
    ) {
    sl_index(stripe);
    frame16ptr_t frame = sl_getp(frame);
    
    
#define nPixels (8 * (PIXEL_PER_STRIPE / 2))
    uint32_t oddSum = 0, evenSum = 0;
    for (unsigned y = 0; y < 4; y++) {
	for (unsigned x = stripe * PIXEL_PER_STRIPE; x < (stripe + 1) * PIXEL_PER_STRIPE; x += 2) {
	    evenSum += (*frame)[y][x];
	    oddSum += (*frame)[y][x+1];
	}
    }

    for (unsigned y = FRAME_DIM_Y - 4; y < FRAME_DIM_Y; y++) {
	for (unsigned x = stripe * PIXEL_PER_STRIPE; x < (stripe + 1) * PIXEL_PER_STRIPE; x = x + 2) {
	    evenSum += (*frame)[y][x];
	    oddSum += (*frame)[y][x+1];
	}
    }

    int32_t meanEvenPixel = ((int32_t)evenSum + (nPixels / 2)) / nPixels - OFFSET_DUE_TO_UINT;
    int32_t meanOddPixel = ((int32_t)oddSum + (nPixels / 2)) / nPixels - OFFSET_DUE_TO_UINT;
    
    /* remove mean from even and odd pixels */
    for (unsigned y = 0; y < FRAME_DIM_Y; y++) {
	for (unsigned x = stripe * PIXEL_PER_STRIPE; x < (stripe + 1) * PIXEL_PER_STRIPE; x = x + 2) {
	    (*frame)[y][x] -= meanEvenPixel;
	    (*frame)[y][x+1] -= meanOddPixel;
	}
    }
} sl_enddef

int main(void) {
    frame16ptr_t frame = calloc(1,sizeof(frame16_t));
    
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
    sl_create(,,,STRIPES_PER_FRAME,,BLOCKSIZE,, subtractReferencePixelTopBottom,
	      sl_glarg(frame16ptr_t,,frame));
    sl_sync();
    c2 = mtperf_sample1(MTPERF_CLOCKS); 
    i2 = mtperf_sample1(MTPERF_EXECUTED_INSNS); 
    report_perf(FRAME_RATIO, b, "subtractReferencePixelTopBottom", delta(c2,c1), delta(i2,i1));
    return 0;
}
