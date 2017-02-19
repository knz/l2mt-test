//
// fft_impl2.c: this file is part of the SL program suite.
//
// Copyright (C) 2009 The SL project.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or (at your option) any later version.
//
// The complete GNU General Public Licence Notice can be found as the
// `COPYING' file in the root directory.
//

enum { MAX_M = TABLE_SIZE, MAX_N = 1 << MAX_M };

static const cpx_t sc_table[ MAX_N ] = {
#define HEADERNAME MAKENAME(2, TABLE_SIZE)
#define HEADER STRINGIFY(HEADERNAME)

#include HEADER
};
const void* sc_table_ptr = sc_table;

sl_def(FFT_2, sl__static,
       sl_glparm(unsigned long, LE2),
       sl_glparm(const cpx_t* restrict, cos_sin),
       sl_glparm(cpx_t* restrict, X),
       sl_glparm(unsigned long, Z))
{
    sl_index(p);
    unsigned i = threadIdx.x | (threadIdx.y << 8);
    
    const unsigned long w  = i & (sl_getp(LE2) - 1);
    const unsigned long j  = (i - w) * 2 + w;
    const unsigned long ip = j + sl_getp(LE2);
    cpx_t* restrict x = sl_getp(X);

     const cpx_t U = sl_getp(cos_sin)[w * sl_getp(Z)];

    const cpx_t T = {
        U.re * x[ip].re - U.im * x[ip].im,
        U.im * x[ip].re + U.re * x[ip].im
    };

    const cpx_t xj = { x[j].re, x[j].im };
    x[ip].re = xj.re - T.re;
    x[ip].im = xj.im - T.im;
    x[j].re  = xj.re + T.re;
    x[j].im  = xj.im + T.im;
}
sl_enddef

sl_def(FFT_1_mt, sl__static,
       sl_glparm(cpx_t*restrict, X),
       sl_glparm(unsigned long, N2),
       sl_glparm(unsigned, blocksize),
       sl_glparm(const void*, t))
{
    sl_index(p);
    unsigned k = p+1;
    
    
    const cpx_t*restrict cos_sin = (const cpx_t*restrict)(const void*)sl_getp(t);
    unsigned long Z  = (MAX_N >> k);
    unsigned long LE = (1 << k);
    sl_create(,, , sl_getp(N2),,sl_getp(blocksize),, FFT_2,
	      sl_glarg(unsigned long, gLE2, LE / 2),
	      sl_glarg(const cpx_t*restrict, gCS, cos_sin),
	      sl_glarg(cpx_t*restrict, gX, sl_getp(X)),
	      sl_glarg(unsigned long, gZ, Z));
    sl_sync();
}
sl_enddef

static
void FFT_1(unsigned long M, cpx_t*restrict X, unsigned long N2, const void* t, unsigned blocksize)
{
    if (N2 > 256) {
	N2 = ((N2 / 256) << 9) | 256;
    }
  sl_create(,,,M,,,sl__forceseq, FFT_1_mt,
	    sl_glarg(cpx_t*restrict, , X),
	    sl_glarg(unsigned long, , N2),
	    sl_glarg(unsigned , , blocksize),
	    sl_glarg(const void*, , t),
      );
  sl_sync();
}
