#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "bench.c"

typedef bool cell;
typedef uint8_t rule1D;

sl_def(update1D, sl__static,
       sl_glparm(cell * restrict, old),
       sl_glparm(cell * restrict, new),
       sl_glparm(rule1D, rule)
)
{
    sl_index(i);

    cell * restrict cold = sl_getp(old) + i;
    cell * restrict cnew = sl_getp(new) + i;

#define P(p, i) (*(p+i))
#define O(i,o) (P(cold, i) << o)

    size_t offset = O(-1,2) | O(0,1) | O(1,0);

    cell newval = (sl_getp(rule) >> offset) & 1;
    P(cnew, 0) = newval;
#undef P
#undef O
}
sl_enddef

sl_def(update1Dx, sl__static,
       sl_glparm(cell * restrict, old),
       sl_glparm(cell * restrict, new),
       sl_glparm(rule1D, rule)
)
{
    sl_index(p);
    unsigned i = p|(blockIdx.i<<4);

    cell * restrict cold = sl_getp(old) + i;
    cell * restrict cnew = sl_getp(new) + i;

#define P(p, i) (*(p+i))
#define O(i,o) (P(cold, i) << o)

    size_t offset = O(-1,2) | O(0,1) | O(1,0);

    cell newval = (sl_getp(rule) >> offset) & 1;
    P(cnew, 0) = newval;
#undef P
#undef O
}
sl_enddef



#ifndef MAXSIZE
#define MAXSIZE 512
#endif

int main(void) {
    cell *source_image = calloc(MAXSIZE,sizeof(cell));
    cell *target_image = malloc(MAXSIZE*sizeof(cell));

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
	sl_create(,,, i,,,sl__forceseq, update1D, sl_glarg(cell*,, source_image), sl_glarg(cell*,, target_image), sl_glarg(rule1D,,101));
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
	    sl_create(,,, i,,blocksize,sl__forcewait, update1D, sl_glarg(cell*,, source_image), sl_glarg(cell*,, target_image), sl_glarg(rule1D,,101));
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
	    sl_create(,,, 16,i/16,blocksize,sl__forcewait, update1Dx,
		      sl_glarg(cell*,, source_image), sl_glarg(cell*,, target_image), sl_glarg(rule1D,,101));
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
