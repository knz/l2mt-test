#include <stdint.h>
#include <stdlib.h>
#include <svp/perf.h>
#include <stdio.h>


extern FILE *stduart;
extern FILE *dbgstdout;

void report_perf(int sz, int b, const char *pre, counter_t t, counter_t i) {
    fprintf(stduart, "%s %d %u %u %u\n", pre, b, sz, t, i);
}

#ifndef TABLE_SIZE
#define TABLE_SIZE 10
#endif
#include "fft.h"

#include "fft_impl3.c"

__attribute__((always_inline))
unsigned delta(counter_t end, counter_t start) {
    return (start <= end) ? (end - start) : ((0xffffffffu-end) + start + 1);
}

#ifndef BLOCKSIZES
#define BLOCKSIZES 1,2,4,8,16,30
#endif
static const int blocksizes[] = { BLOCKSIZES };
#define NBLSZ (sizeof(blocksizes)/sizeof(blocksizes[0]))


int main(void) {
#ifndef LSZ
#define LSZ 1
#endif
    counter_t c1, c2;
    counter_t i1, i2;
    cpx_t *y_fft = (cpx_t*)calloc((1<<TABLE_SIZE), sizeof(cpx_t));
    dbgstdout = stduart;

    unsigned i;
    fprintf(stduart, "Columns:\nLN2(SZ)\tINSNS");
    for (i = 0; i < NBLSZ; i++)
	fprintf(stduart, "\tCC_BLK%d", blocksizes[i]);
    fputc('\n', stduart);

    unsigned long M;
    for (M = 1; M <= TABLE_SIZE; M++) {
	unsigned long N = 1 << M;
	fprintf(stduart, "%u", N);
	for (i = 0; i < NBLSZ; i++) {
	    unsigned blocksize = blocksizes[i];
	    
	    c1 = mtperf_sample1(MTPERF_CLOCKS); 
	    i1 = mtperf_sample1(MTPERF_EXECUTED_INSNS); 
	    FFT_1(M, y_fft, N/2, sc_table_ptr, blocksize);
	    c2 = mtperf_sample1(MTPERF_CLOCKS); 
	    i2 = mtperf_sample1(MTPERF_EXECUTED_INSNS);
	    
	    if (i == 0) {
		fprintf(stduart, "\t%u", delta(i2, i1));
	    }
	    fprintf(stduart, "\t%u", delta(c2, c1));
	}
	fputc('\n', stduart);
    }
    fprintf(stduart, "Done!\n");
    return 0;
}
