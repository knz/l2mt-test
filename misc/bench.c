#include <svp/perf.h>
#include <stdio.h>

extern FILE *stduart;
extern FILE *dbgstdout;
void report_perf(int sz, int b, const char *pre, counter_t t, counter_t i) {
    fprintf(stduart, "%s %d %u %u %u\n", pre, b, sz, t, i);
}

__attribute__((always_inline))
unsigned delta(counter_t end, counter_t start) {
    return (start <= end) ? (end - start) : ((0xffffffffu-end) + start + 1);
}

#ifndef BLOCKSIZES
#define BLOCKSIZES 1,2,4,8,16,30
#endif

static const int blocksizes[] = { BLOCKSIZES };
#define NBLSIZES (sizeof(blocksizes)/sizeof(blocksizes[0]))

#define NSIZES 14
static const int sizes[] = {1, 10, 20, 32, 35, 50, 64, 75, 100, 128, 200, 256, 350, 500 };
