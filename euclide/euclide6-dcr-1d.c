/*
	Copyright (C) 2012,2015 European Space Agency
	Copyright (C) 2016,2017 Raphael 'kena' Poss

        euclide6-dcr-1d is free software: you can redistribute it and/or modify
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
#include "slext.h"

/*
see Integer Square Roots by Jack W. Crenshaw (http://www.embedded.com/98/9802fe2.htm) - specifically, Figure 2 of that article.
*/

__attribute__((always_inline))
static uint32_t isqrt_3 (uint32_t n)
{
    register uint32_t // OR register uint16 OR register uint8 - respectively
        root, remainder, place;

    root = 0;
    remainder = n;
    place = 0x40000000; // OR place = 0x4000; OR place = 0x40; - respectively

    while (place > remainder)
        place = place >> 2;
    while (place)
    {
        if (remainder >= root + place)
        {
            remainder = remainder - root - place;
            root = root + (place << 1);
        }
        root = root >> 1;
        place = place >> 2;
    }
    return root;
}

__attribute__((always_inline))
static int32_t abs_int(int32_t v) {
    return v < 0 ? -v : v;
}

// This kernel enhances two bias frames that are used in a later
// stage, based on all non-saturated pixels in the input frame.  The
// satura`tion mask is assumed to have been computed in a previous
// stage.
sl_def(detectCosmicRays,,
       sl_glparm(frame16ptr_t , frame),
       sl_glparm(frame32ptr_t , sumXYFrame),
       sl_glparm(frame32ptr_t , sumYFrame),
       sl_glparm(frame16ptr_t , offsetCosmicFrame),
       sl_glparm(frame8ptr_t , numberOfFramesAfterCosmicRay),
       sl_glparm(uint32_t, N),
       sl_glparm(uint32_t, NTimesNm1),
       sl_redvar(redresults)
    ) {
    sl_index(stripe);
    frame16ptr_t frame = sl_getp(frame);
    frame16ptr_t offsetCosmicFrame = sl_getp(offsetCosmicFrame);
    frame32ptr_t sumXYFrame = sl_getp(sumXYFrame);
    frame32ptr_t sumYFrame = sl_getp(sumYFrame);
    frame8ptr_t numberOfFramesAfterCosmicRay = sl_getp(numberOfFramesAfterCosmicRay);
    int16_t Np1 = sl_getp(N)+1, NTimesNm1 = sl_getp(NTimesNm1);

    unsigned nbCosmicRay = 0;
    for (unsigned y = 0; y < FRAME_DIM_Y; y++) {
	for (unsigned x = stripe * PIXEL_PER_STRIPE; x < (stripe + 1) * PIXEL_PER_STRIPE; x++) {

	    /* compute the threshold (above which a cosmic ray hit is detected) based on the estimates of the previous frame
	       threshold = alpha[N-1] + beta[N-1]*N + noise[N] */

	    /* more optimized (precalculated some variables which don't change over the image, e.g. N*(N-1)) */
	    int32_t betaTimesNplus1 = 6*(2*(int32_t)(*sumXYFrame)[y][x]-(Np1*(int32_t)(*sumYFrame)[y][x])) / NTimesNm1;
	    //alpha = (INT32BIT)sumYFrame[y][x]/N - betaTimesNplus1/2;
	    int32_t noiseEstimateSqr = abs_int(betaTimesNplus1) + readOutNoiseSquare;
	    int32_t noiseEstimate = isqrt_3(noiseEstimateSqr);
	    uint16_t thresholdValue = (int32_t)(*sumYFrame)[y][x]/sl_getp(N) + (betaTimesNplus1/2) + THRESHOLDFACTOR_FOR_COSMICRAY * noiseEstimate;
	    uint16_t frameMinusOffset = (*frame)[y][x] - (*offsetCosmicFrame)[y][x];

	    /* comparison of frame-offsetCosmicFrame (reality) to the tresholdValue (interpolation) to check if cosmic ray hit the pixel */
	    if (frameMinusOffset > thresholdValue) {
		(*offsetCosmicFrame)[y][x] = (*frame)[y][x] - thresholdValue;
		nbCosmicRay++;
	    }

	    /* in case we had a cosmic ray before, we can improve the offsetCosmicFrame value in every step that follows */
	    if ((*offsetCosmicFrame)[y][x] != 0){
		(*numberOfFramesAfterCosmicRay)[y][x]++;
		(*offsetCosmicFrame)[y][x] += (abs_int((*frame)[y][x] - thresholdValue) - (*offsetCosmicFrame)[y][x])/(*numberOfFramesAfterCosmicRay)[y][x];
	    }
	}
    }

    sl_setresult(redresults, sl_getresult(redresults) + nbCosmicRay);
    
} sl_enddef


int main(void) {
    frame16ptr_t frame = calloc(1,sizeof(frame16_t));
    frame32ptr_t sumXYFrame = calloc(1,sizeof(frame32_t));
    frame32ptr_t sumYFrame = calloc(1,sizeof(frame32_t));
    frame16ptr_t offsetCosmicFrame = calloc(1,sizeof(frame16_t));
    frame8ptr_t numberOfFramesAfterCosmicRay = calloc(1,sizeof(frame8_t));
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
	    //(*offsetCosmicFrame)[i][j] = (i + j/2) % 200 ;
	    (*frame)[i][j] =  ((i + j) % 200);
	    //(*sumYFrame)[i][j] =  ((i/3 + j) % 200);
	    //(*sumXYFrame)[i][j] =  ((i + j/3) % 200);
	}
    readOutNoiseSquare = 0;
    int N = 5;
    
    c1 = mtperf_sample1(MTPERF_CLOCKS); 
    i1 = mtperf_sample1(MTPERF_EXECUTED_INSNS); 
    sl_create(,,,PIXEL_PER_STRIPE, ,BLOCKSIZE,, detectCosmicRays,
	      sl_glarg(frame16ptr_t,,frame),
	      sl_glarg(frame32ptr_t,,sumXYFrame),
	      sl_glarg(frame32ptr_t,,sumYFrame),
	      sl_glarg(frame16ptr_t,,offsetCosmicFrame),
	      sl_glarg(frame8ptr_t,,numberOfFramesAfterCosmicRay),
	      sl_glarg(uint32_t,,N),
	      sl_glarg(uint32_t,,N*(N-1)),
	      sl_redvararg(srb)
	);
    sl_sync();
    unsigned nbCosmicRay = sl_foldvar(srb, uint32_t, +, 0);
    c2 = mtperf_sample1(MTPERF_CLOCKS); 
    i2 = mtperf_sample1(MTPERF_EXECUTED_INSNS);
    sl_free_redvar(srb);
    report_perf(FRAME_RATIO, b, "detectCosmicRays", delta(c2,c1), delta(i2,i1));
    fprintf(stduart, "nb cosmic rays: %u\n", nbCosmicRay);

    return 0;
}
