#include <stdint.h>

#ifndef FRAME_RATIO
#define FRAME_RATIO 16
#endif
#define FRAME_DIM_X (2048/FRAME_RATIO)
#define FRAME_DIM_Y (2048/FRAME_RATIO)
#define STRIPES_PER_FRAME 32
#define PIXEL_PER_STRIPE (FRAME_DIM_X/STRIPES_PER_FRAME)
#define PIXEL_PER_FRAME (FRAME_DIM_Y*FRAME_DIM_X)

#define OFFSET_DUE_TO_UINT 500
#define THRESHOLDFACTOR_FOR_COSMICRAY 6

/*
#define FRAME_DIM_Y 256
#define FRAME_DIM_X 256
#define PIXEL_PER_STRIPE 8
#define STRIPES_PER_FRAME (FRAME_DIM_X/PIXEL_PER_STRIPE)
*/

typedef uint16_t frame16_t[FRAME_DIM_Y][FRAME_DIM_X];
typedef frame16_t *frame16ptr_t;
typedef uint32_t frame32_t[FRAME_DIM_Y][FRAME_DIM_X];
typedef frame32_t *frame32ptr_t;
typedef uint8_t frame8_t[FRAME_DIM_Y][FRAME_DIM_X];
typedef frame8_t *frame8ptr_t;
typedef int16_t coeff_t[STRIPES_PER_FRAME][4];
typedef coeff_t *coeffptr_t;

typedef uint16_t stripe_vec16_t[STRIPES_PER_FRAME];
typedef stripe_vec16_t *stripe_vec16ptr_t;

uint16_t readOutNoiseSquare = 0;

