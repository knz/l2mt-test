/*
	Copyright (C) 2012,2015 European Space Agency
	Copyright (C) 2016,2017 Raphael 'kena' Poss

        euclide1-pllsf-2d is free software: you can redistribute it and/or modify
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
#include <svp/perf.h>
#include <stdio.h>
#include <stdlib.h>

#include "bench.c"
#include "euclide.h"

// This kernel enhances two bias frames that are used in a later
// stage, based on all non-saturated pixels in the input frame.  The
// saturation mask is assumed to have been computed in a previous
// stage.
sl_def(progressiveLinearLeastSquaresFit,,
       sl_glparm(frame8ptr_t restrict ,saturationFrame),
       sl_glparm(frame16ptr_t restrict ,frame),
       sl_glparm(frame16ptr_t restrict ,offsetCosmicFrame),
       sl_glparm(frame32ptr_t restrict ,sumXYFrame),
       sl_glparm(frame32ptr_t restrict ,sumYFrame),
       sl_glparm(unsigned,N)) {
    sl_index(blah);
    unsigned y = blockIdx.i;
    frame8ptr_t saturationFrame = sl_getp(saturationFrame);
    frame16ptr_t frame = sl_getp(frame);
    frame16ptr_t offsetCosmicFrame = sl_getp(offsetCosmicFrame);
    frame32ptr_t sumXYFrame = sl_getp(sumXYFrame);
    frame32ptr_t sumYFrame = sl_getp(sumYFrame);

    for (unsigned i = 0; i < STRIPES_PER_FRAME; i++) {
	unsigned x = threadIdx.x + (PIXEL_PER_STRIPE * i);

    if (!(*saturationFrame)[y][x]) {
	unsigned frameMinusOffset = (*frame)[y][x] - (*offsetCosmicFrame)[y][x];
	(*sumXYFrame)[y][x] = sl_getp(N)*frameMinusOffset + (*sumXYFrame)[y][x];
	(*sumYFrame)[y][x] = frameMinusOffset + (*sumYFrame)[y][x];
    }
    }
} sl_enddef

sl_def(clearframe,,sl_glparm(unsigned*,dst)) {
    sl_index(i);
    sl_getp(dst)[i] = 0;
} sl_enddef

sl_def(setsat,,sl_glparm(unsigned*,dst)) {
    sl_index(i);
    sl_getp(dst)[i] = (i & 3);
} sl_enddef


int main(void) {
    frame8ptr_t saturationFrame = malloc(sizeof(frame8_t));
    frame16ptr_t frame = calloc(1, sizeof(frame16_t));
    frame32ptr_t sumXYFrame = calloc(1,sizeof(frame32_t));
    frame32ptr_t sumYFrame = calloc(1,sizeof(frame32_t));
    frame16ptr_t offsetCosmicFrame = calloc(1,sizeof(frame16_t));
#ifndef BLOCKSIZE
#define BLOCKSIZE 30
#endif
    int b = BLOCKSIZE; 
    dbgstdout = stduart;
    
    counter_t c1, c2;
    counter_t i1, i2;
   
    int i, j;
    for (i = 0; i < FRAME_DIM_Y; i++)
	for (j = 0; j < FRAME_DIM_X; j++) {
	    (*saturationFrame)[i][j] = (i + j) % 3;
	}
    
    
    c1 = mtperf_sample1(MTPERF_CLOCKS); 
    i1 = mtperf_sample1(MTPERF_EXECUTED_INSNS); 
    sl_create(,,,PIXEL_PER_STRIPE, FRAME_DIM_Y ,BLOCKSIZE,, progressiveLinearLeastSquaresFit,
	      sl_glarg(frame8ptr_t,,saturationFrame),
	      sl_glarg(frame16ptr_t,,frame),
	      sl_glarg(frame16ptr_t,,offsetCosmicFrame),
	      sl_glarg(frame32ptr_t,,sumXYFrame),
	      sl_glarg(frame32ptr_t,,sumYFrame),
	      sl_glarg(unsigned,,3));
    sl_sync();
    c2 = mtperf_sample1(MTPERF_CLOCKS); 
    i2 = mtperf_sample1(MTPERF_EXECUTED_INSNS);
    report_perf(FRAME_RATIO, b, "progressiveLinearLeastSquaresFit-2d", delta(c2,c1), delta(i2,i1));
    return 0;
}
