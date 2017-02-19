/*
	Copyright (C) 2012,2015 European Space Agency
	Copyright (C) 2016,2017 Raphael 'kena' Poss

        euclide1-pllsf-3d is free software: you can redistribute it and/or modify
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

#define FRAME_DIM_Y 256
#define FRAME_DIM_X 256
#define PIXEL_PER_STRIPE 8
#define STRIPES_PER_FRAME (FRAME_DIM_X/PIXEL_PER_STRIPE)

typedef unsigned frame_t[FRAME_DIM_Y][FRAME_DIM_X];
typedef frame_t *frameptr_t;

extern FILE *stduart;
extern FILE *dbgstdout;


// This kernel enhances two bias frames that are used in a later
// stage, based on all non-saturated pixels in the input frame.  The
// saturation mask is assumed to have been computed in a previous
// stage.
sl_def(progressiveLinearLeastSquaresFit,,
       sl_glparm(frameptr_t,saturationFrame),
       sl_glparm(frameptr_t,frame),
       sl_glparm(frameptr_t,offsetCosmicFrame),
       sl_glparm(frameptr_t,sumXYFrame),
       sl_glparm(frameptr_t,sumYFrame),
       sl_glparm(unsigned,N)) {
    sl_index(blah);
    unsigned y = blockIdx.y;
    unsigned stripe = blockIdx.x;
    unsigned x = stripe * PIXEL_PER_STRIPE + threadIdx.i;
    frameptr_t saturationFrame = sl_getp(saturationFrame);
    frameptr_t frame = sl_getp(frame);
    frameptr_t offsetCosmicFrame = sl_getp(offsetCosmicFrame);
    frameptr_t sumXYFrame = sl_getp(sumXYFrame);
    frameptr_t sumYFrame = sl_getp(sumYFrame);

    if (!(*saturationFrame)[y][x]) {
	unsigned frameMinusOffset = (*frame)[y][x] - (*offsetCosmicFrame)[y][x];
	(*sumXYFrame)[y][x] = sl_getp(N)*frameMinusOffset + (*sumXYFrame)[y][x];
	(*sumYFrame)[y][x] = frameMinusOffset + (*sumYFrame)[y][x];
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

void report_perf(int sz, int b, const char *pre, counter_t t, counter_t i) {
    fprintf(stduart, "%s %d %u %u %u\n", pre, b, sz, t, i);
}

int main(void) {
    frameptr_t saturationFrame = malloc(sizeof(frame_t));
    frameptr_t frame = calloc(1, sizeof(frame_t));
    frameptr_t sumXYFrame = calloc(1,sizeof(frame_t));
    frameptr_t sumYFrame = calloc(1,sizeof(frame_t));
    frameptr_t offsetCosmicFrame = calloc(1,sizeof(frame_t));
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
    sl_create(,,,PIXEL_PER_STRIPE, (FRAME_DIM_Y << 9)|STRIPES_PER_FRAME,BLOCKSIZE,, progressiveLinearLeastSquaresFit,
	      sl_glarg(frameptr_t,,saturationFrame),
	      sl_glarg(frameptr_t,,frame),
	      sl_glarg(frameptr_t,,offsetCosmicFrame),
	      sl_glarg(frameptr_t,,sumXYFrame),
	      sl_glarg(frameptr_t,,sumYFrame),
	      sl_glarg(unsigned,,3));
    sl_sync();
    c2 = mtperf_sample1(MTPERF_CLOCKS); 
    i2 = mtperf_sample1(MTPERF_EXECUTED_INSNS);
    report_perf(FRAME_RATIO, b, "progressiveLinearLeastSquaresFit-3d", c2-c1, i2-i1);
    return 0;
}
