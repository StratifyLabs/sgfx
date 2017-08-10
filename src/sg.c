//Copyright 2011-2016 Tyler Gilbert; All Rights Reserved

#include <stdio.h>
#include "sg_config.h"
#include "sg.h"

static int calc_offset(const sg_bmap_t * bmap, sg_point_t p) { return (p.x/SG_PIXELS_PER_WORD) + p.y*(bmap->columns); }

static size_t calc_word_width(sg_size_t w){ return (w + 31) >> 5; }

void sg_bmap_set_data(sg_bmap_t * bmap, sg_bmap_data_t * mem, sg_dim_t dim){
	bmap->dim = dim;
	bmap->columns = calc_word_width(dim.width*SG_BITS_PER_PIXEL);
	bmap->data = mem;
}

size_t sg_calc_bmap_size(sg_dim_t dim){
	//return the number of bytes needed to contain the dimensions (in pixels)
	return calc_word_width(dim.width*SG_BITS_PER_PIXEL) * dim.height * SG_BYTES_PER_WORD;
}

sg_bmap_data_t * sg_bmap_data(const sg_bmap_t * bmap, sg_point_t p){
	return bmap->data + calc_offset(bmap, p);
}


void sg_bmap_show(const sg_bmap_t * bmap){
	sg_size_t i,j;

	sg_color_t color;
	sg_cursor_t y_cursor;
	sg_cursor_t x_cursor;

	sg_cursor_set(&y_cursor, bmap, sg_point(0,0));

	for(i=0; i < bmap->dim.height; i++){
		sg_cursor_copy(&x_cursor, &y_cursor);
		for(j=0; j < bmap->dim.width; j++){
			color = sg_cursor_get_pixel(&x_cursor);
#if SG_BITS_PER_PIXEL > 8
			printf("%04X", color);
#else
			printf("%02X", color);
#endif
			sg_cursor_inc_x(&x_cursor);
			if( j < bmap->dim.width - 1){
				printf(" ");
			}
		}
		printf("\n");
		sg_cursor_inc_y(&y_cursor);
	}
}
