//Copyright 2011-2018 Tyler Gilbert; All Rights Reserved

#ifndef SG_CONFIG_H_
#define SG_CONFIG_H_

#include <stdio.h>

#include "sg_types.h"

#if !defined SG_BITS_PER_PIXEL
#define SG_BITS_PER_PIXEL 1
#endif

#if SG_BITS_PER_PIXEL == 0
#define SG_BITS_PER_PIXEL_VALUE(bmap) (bmap->bits_per_pixel)
#else
#define SG_BITS_PER_PIXEL_VALUE(bmap) SG_BITS_PER_PIXEL
#endif


#define SG_BITS_PER_WORD 32
#define SG_BYTES_PER_WORD (SG_BITS_PER_WORD/8)
#define SG_PIXELS_PER_WORD(bmap) (SG_BITS_PER_WORD / SG_BITS_PER_PIXEL_VALUE(bmap))
#define SG_PIXEL_MASK(bmap) ((1<<SG_BITS_PER_PIXEL_VALUE(bmap)) - 1)

sg_color_t sg_cursor_get_pixel_no_inc(sg_cursor_t * cursor);
void sg_cursor_draw_pixel_no_inc(sg_cursor_t * cursor);


#endif /* SG_CONFIG_H_ */
