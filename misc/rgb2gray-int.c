#include <stdint.h>
#include <stdlib.h>
#include <svp/perf.h>
#include <stdio.h>
#include <stdbool.h>

#include "bench.c"

sl_def(convert,sl__static,
       sl_glparm(uint32_t*, src), sl_glparm(uint8_t*, dst)) {
    sl_index(i);

    uint32_t pix = sl_getp(src)[i];
    uint8_t r = pix >> 16, g = pix >> 8, b = pix;

    // The following performs an approximation,
    // using R*0,25 + G*0,5 + B*0,25 instead of
    // the usual formula.
    sl_getp(dst)[i] = (r >> 2) + (g >> 1) + (b >> 2);
} sl_enddef

sl_def(convertx,sl__static,
       sl_glparm(uint32_t*, src), sl_glparm(uint8_t*, dst)) {
    sl_index(p);
    unsigned i = p|(blockIdx.i<<4);

    uint32_t pix = sl_getp(src)[i];
    uint8_t r = pix >> 16, g = pix >> 8, b = pix;

    // The following performs an approximation,
    // using R*0,25 + G*0,5 + B*0,25 instead of
    // the usual formula.
    sl_getp(dst)[i] = (r >> 2) + (g >> 1) + (b >> 2);
} sl_enddef

#ifndef MAXSIZE
#define MAXSIZE 512
#endif

int main(void) {
    uint32_t *source_image = calloc(MAXSIZE,sizeof(uint32_t));
    uint8_t *target_image = malloc(MAXSIZE*sizeof(uint8_t));

    dbgstdout = stduart;
    
    counter_t c1, c2;
    counter_t i1, i2;
    int i,base;

    printf("Output from: %s\n", __FILE__);
    printf("Columns:\nSZ\tSEQINSN\tCC_SEQ");

    printf("\tP_INSN");
    for (i = 0; i < NBLSIZES; i++)
	printf("\tCC_P%d", blocksizes[i]);

    printf("\tB_INSN");
    for (i = 0; i < NBLSIZES; i++)
	printf("\tCC_B%d", blocksizes[i]);
    putchar('\n');
    
    for (i = 0; i < MAXSIZE; i += 8) {
	output_uint(i, 1);
	c1 = mtperf_sample1(MTPERF_CLOCKS); 
	i1 = mtperf_sample1(MTPERF_EXECUTED_INSNS); 
	sl_create(,,, i,,,sl__forceseq, convert, sl_glarg(uint32_t*,, source_image), sl_glarg(uint8_t*,, target_image));
	sl_sync();
	c2 = mtperf_sample1(MTPERF_CLOCKS); 
	i2 = mtperf_sample1(MTPERF_EXECUTED_INSNS);
	output_char('\t', 1);
	output_uint(delta(i2,i1), 1);
	output_char('\t', 1);
	output_uint(delta(c2,c1), 1);

	for (int ibsz = 0; ibsz < NBLSIZES; ibsz++) {
	    int blocksize = blocksizes[ibsz];
	    c1 = mtperf_sample1(MTPERF_CLOCKS); 
	    i1 = mtperf_sample1(MTPERF_EXECUTED_INSNS); 
	    sl_create(,,, i,,blocksize,sl__forcewait, convert, sl_glarg(uint32_t*,, source_image), sl_glarg(uint8_t*,, target_image));
	    sl_sync();
	    c2 = mtperf_sample1(MTPERF_CLOCKS); 
	    i2 = mtperf_sample1(MTPERF_EXECUTED_INSNS); 
	    if (ibsz == 0) {
		output_char('\t', 1);
		output_uint(delta(i2,i1), 1);
	    }
	    output_char('\t', 1);
	    output_uint(delta(c2,c1), 1);
	}
	for (int ibsz = 0; ibsz < NBLSIZES; ibsz++) {
	    int blocksize = blocksizes[ibsz];
	    c1 = mtperf_sample1(MTPERF_CLOCKS); 
	    i1 = mtperf_sample1(MTPERF_EXECUTED_INSNS); 
	    sl_create(,,, 16,i/16,blocksize,sl__forcewait, convertx, sl_glarg(uint32_t*,, source_image), sl_glarg(uint8_t*,, target_image));
	    sl_sync();
	    c2 = mtperf_sample1(MTPERF_CLOCKS); 
	    i2 = mtperf_sample1(MTPERF_EXECUTED_INSNS); 
	    if (ibsz == 0) {
		output_char('\t', 1);
		output_uint(delta(i2,i1), 1);
	    }
	    output_char('\t', 1);
	    output_uint(delta(c2,c1), 1);
	}
	output_char('\n', 1);
    }
    printf("Done!\n");
    return 0;
}
