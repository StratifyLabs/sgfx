//Copyright 2011-2016 Tyler Gilbert; All Rights Reserved

#include "sg.h"


void sg_set_data(sg_bmap_t * bmap, sg_bmap_data_t * mem, sg_size_t w, sg_size_t h, u8 bits_per_pixel){
	bmap->dim.w = w;
	bmap->dim.h = h;
	bmap->columns = sg_calc_byte_width(w);
	bmap->data = mem;
	bmap->bits_per_pixel = bits_per_pixel;
}



void sg_show(const sg_bmap_t * mg){

}
