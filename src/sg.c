//Copyright 2011-2016 Tyler Gilbert; All Rights Reserved

#include <stdio.h>
#include "sg_config.h"
#include "sg.h"

static int calc_offset(const sg_bmap_t * bmap, sg_point_t p){
	return (p.x/SG_PIXELS_PER_WORD(bmap)) + p.y*(bmap->columns);
}


void sg_bmap_set_data(sg_bmap_t * bmap, sg_bmap_data_t * mem, sg_area_t area, u8 bits_per_pixel){
#if SG_BITS_PER_PIXEL == 0
	bmap->bits_per_pixel = bits_per_pixel;
#else
	bmap->bits_per_pixel = SG_BITS_PER_PIXEL;
#endif
	bmap->area = area;
	bmap->columns = sg_calc_word_width(area.width*SG_BITS_PER_PIXEL_VALUE(bmap));
	bmap->data = mem;
	bmap->margin_bottom_right.width = 0;
	bmap->margin_bottom_right.height = 0;
	bmap->margin_top_left.width = 0;
	bmap->margin_top_left.height = 0;
}

size_t sg_calc_bmap_size(const sg_bmap_t * bmap, sg_area_t area){
	//return the number of bytes needed to contain the dimensions (in pixels)
	return sg_calc_word_width(area.width*SG_BITS_PER_PIXEL_VALUE(bmap)) * area.height * SG_BYTES_PER_WORD;
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

	for(i=0; i < bmap->area.height; i++){
		sg_cursor_copy(&x_cursor, &y_cursor);
		for(j=0; j < bmap->area.width; j++){
			color = sg_cursor_get_pixel(&x_cursor);
#if SG_BITS_PER_PIXEL > 8
			printf("%04X", color);
#else
			printf("%02X", color);
#endif
			sg_cursor_inc_x(&x_cursor);
			if( j < bmap->area.width - 1){
				printf(" ");
			}
		}
		printf("\n");
		sg_cursor_inc_y(&y_cursor);
	}
}
