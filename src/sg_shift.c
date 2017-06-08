/*
 * sg_shift.c
 *
 *  Created on: Jun 7, 2017
 *      Author: tgil
 */

#include "sg.h"


static void shift_right(sg_bmap_t * bmap, int count, sg_point_t start, sg_dim_t d);
static void shift_left(sg_bmap_t * bmap, int count, sg_point_t start, sg_dim_t d);
static void shift_up(sg_bmap_t * bmap, int count, sg_point_t start, sg_dim_t d);
static void shift_down(sg_bmap_t * bmap, int count, sg_point_t start, sg_dim_t d);

void sg_shift(sg_bmap_t * bmap, sg_point_t shift, sg_point_t p, sg_dim_t d){

	if( shift.x < 0 ){
		shift_left(bmap, shift.x*-1, p, d);
	} else {
		shift_right(bmap, shift.x, p, d);
	}

	if( shift.y < 0 ){
		shift_up(bmap, shift.y*-1, p, d);
	} else {
		shift_down(bmap, shift.y, p, d);
	}

}

void shift_right(sg_bmap_t * bmap, int count, sg_point_t start, sg_dim_t d){
	sg_int_t i;

	int page_size;
	int bytes_shifted = 0;

	sg_cursor_t y_cursor;
	sg_cursor_t x_cursor;

	sg_cursor_set(&y_cursor, bmap, start);

	while( bytes_shifted < count ){

		page_size = count - bytes_shifted;
		if( page_size > SG_PIXELS_PER_WORD ){
			page_size = SG_PIXELS_PER_WORD;
		}

		for(i=start.y; i < start.y+d.h; i++){
			sg_cursor_copy(&x_cursor, &y_cursor);
			if( sg_y_visible(bmap, i) ){
				sg_cursor_shift_right(&x_cursor, d.w, count);
			}
			sg_cursor_inc_y(&y_cursor);
		}

		bytes_shifted += page_size;
	}
}

void shift_left(sg_bmap_t * bmap, int count, sg_point_t start, sg_dim_t d){
	sg_int_t i;
	int page_size;
	int bytes_shifted = 0;

	sg_cursor_t y_cursor;
	sg_cursor_t x_cursor;

	sg_cursor_set(&y_cursor, bmap, start);

	while( bytes_shifted < count ){

		page_size = count - bytes_shifted;
		if( page_size > SG_PIXELS_PER_WORD ){
			page_size = SG_PIXELS_PER_WORD;
		}

		for(i=start.y; i < start.y+d.h; i++){
			sg_cursor_copy(&x_cursor, &y_cursor);
			if( sg_y_visible(bmap, i) ){
				sg_cursor_shift_left(&x_cursor, d.w, count);
			}
			sg_cursor_inc_y(&y_cursor);
		}

		bytes_shifted += page_size;
	}
}

void shift_up(sg_bmap_t * bmap, int count, sg_point_t start, sg_dim_t d){
	sg_point_t dest;
	sg_point_t src;
	sg_size_t rows_shifted;
	sg_dim_t shift_dim;

	if( count > bmap->dim.h ){
		count = bmap->dim.h;
	}

	if( count == 0 ){
		return;
	}

	if( d.h + start.y > bmap->dim.h ){
		d.h = bmap->dim.h - start.y;
	}

	//clear the top area
	dest.x = start.x;
	dest.y = start.y;
	shift_dim.w = d.w;
	shift_dim.h = count;

	src.x = start.x;
	rows_shifted = 0;

	while( rows_shifted < (d.h - count) ){
		dest.y = start.y + rows_shifted;
		shift_dim.h = count;
		src.y = start.y + rows_shifted + count;
		if( rows_shifted + shift_dim.h > bmap->dim.h ){
			shift_dim.h = d.h - rows_shifted;
		}

		sg_draw_sub_bitmap(bmap, dest, bmap, src, shift_dim);

		rows_shifted += shift_dim.h;

		if( rows_shifted < (d.h - count) ){
			//sg_clr_area(bmap, src, shift_dim, 0xFF);
		}

	}
}

void shift_down(sg_bmap_t * bmap, int count, sg_point_t start, sg_dim_t d){
	sg_point_t dest;
	sg_point_t src;
	sg_dim_t shift_dim;
	sg_size_t rows_shifted;

	if( count > bmap->dim.h ){
		count = bmap->dim.h;
	}

	if( count == 0 ){
		return;
	}

	if( d.h + start.y > bmap->dim.h ){
		d.h = bmap->dim.h - start.y;
	}

	//clear the top area
	dest.x = start.x;
	dest.y = start.y + d.h - count;
	shift_dim.w = d.w;
	shift_dim.h = count;

	src.x = start.x;
	rows_shifted = 0;

	while( rows_shifted < (d.h - count) ){

		if( bmap->dim.h > (2*count + rows_shifted) ){
			shift_dim.h = count;
			dest.y = start.y + d.h - rows_shifted - count;
			src.y = dest.y - count;
		} else {
			shift_dim.h = d.h - rows_shifted - count;
			src.y = 0;
			dest.y = rows_shifted - shift_dim.h;
		}

		sg_draw_sub_bitmap(bmap, dest, bmap, src, shift_dim);

		rows_shifted += shift_dim.h;

		if( rows_shifted < (d.h - count) ){
			//sg_clr_area(bmap, src, shift_dim, 0xFF);
		}

	}
}
