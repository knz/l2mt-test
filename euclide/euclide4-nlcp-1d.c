/*
	Copyright (C) 2012,2015 European Space Agency
	Copyright (C) 2016,2017 Raphael 'kena' Poss

        euclide4-nlcp-1d is free software: you can redistribute it and/or modify
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

//		linearity correction with the associated polynomial
sl_def(nonLinearityCorrectionPolynomial,,
       sl_glparm(frame16ptr_t , frame),
       sl_glparm(coeffptr_t , coeff)
    ) {
    sl_index(stripe);
    frame16ptr_t frame = sl_getp(frame);
    coeffptr_t coeff = sl_getp(coeff);
    int32_t   x1, x2, x3, x4;
    int32_t   coeff1, coeff2, coeff3, coeff4;

    coeff1 = (*coeff)[stripe][0];
    coeff2 = (*coeff)[stripe][1];
    coeff3 = (*coeff)[stripe][2];
    coeff4 = (*coeff)[stripe][3];

    for (unsigned y = 0; y < FRAME_DIM_Y; ++y) {
	for (unsigned x = stripe * PIXEL_PER_STRIPE; x < (stripe + 1) * PIXEL_PER_STRIPE; ++x) {
	    x1 = ((int32_t) (*frame)[y][x]) - OFFSET_DUE_TO_UINT;
	    x2 = (x1 >> 1) * x1;
	    x3 = (x2 >> 16) * x1;
	    x4 = (x3 >> 16) * x1;
	    (*frame)[y][x] = (uint16_t) (((x1 * coeff1) >> 13) + (( (x2 >> 16) * coeff2) >> 17) + (( (x3 >> 16) * coeff3) >> 21) + (( (x4 >> 16) * coeff4) >> 19)) + OFFSET_DUE_TO_UINT;
	}
    }
} sl_enddef

int main(void) {
    frame16ptr_t frame = calloc(1,sizeof(frame16_t));
    coeffptr_t coeff = calloc(1,sizeof(coeff_t));
    
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
    sl_create(,,,STRIPES_PER_FRAME,,BLOCKSIZE,, nonLinearityCorrectionPolynomial,
	      sl_glarg(frame16ptr_t,,frame),
	      sl_glarg(coeffptr_t,,coeff));
    sl_sync();
    c2 = mtperf_sample1(MTPERF_CLOCKS); 
    i2 = mtperf_sample1(MTPERF_EXECUTED_INSNS); 
    report_perf(FRAME_RATIO, b, "nonLinearityCorrectionPolynomial", delta(c2,c1), delta(i2,i1));
    return 0;
}
