//Copyright 2011-2016 Tyler Gilbert; All Rights Reserved

#include "sg_config.h"
#include "sg.h"


static void shift_right(const sg_bmap_t * bmap, int count, sg_point_t start, sg_area_t d);
static void shift_left(const sg_bmap_t * bmap, int count, sg_point_t start, sg_area_t d);
static void shift_up(const sg_bmap_t * bmap, int count, sg_point_t start, sg_area_t d);
static void shift_down(const sg_bmap_t * bmap, int count, sg_point_t start, sg_area_t d);


void sg_transform_flip_xy(const sg_bmap_t * bmap){}

void sg_transform_flip_x(const sg_bmap_t * bmap){}

void sg_transform_flip_y(const sg_bmap_t * bmap){}

void sg_transform_shift(const sg_bmap_t * bmap, sg_point_t shift, const sg_region_t * region){
	sg_point_t p = region->point;
	sg_area_t d = region->area;
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

void shift_right(const sg_bmap_t * bmap, int count, sg_point_t start, sg_area_t d){
	sg_int_t i;

	sg_cursor_t y_cursor;
	sg_cursor_t x_cursor;

	sg_cursor_set(&y_cursor, bmap, start);

	for(i=start.y; i < start.y+d.height; i++){
		sg_cursor_copy(&x_cursor, &y_cursor);
		if( sg_y_visible(bmap, i) ){
			sg_cursor_shift_right(&x_cursor, d.width, count);
		}
		sg_cursor_inc_y(&y_cursor);
	}

}

void shift_left(const sg_bmap_t * bmap, int count, sg_point_t start, sg_area_t d){
	sg_int_t i;

	sg_cursor_t y_cursor;
	sg_cursor_t x_cursor;

	sg_cursor_set(&y_cursor, bmap, start);

	for(i=start.y; i < start.y+d.height; i++){
		sg_cursor_copy(&x_cursor, &y_cursor);
		if( sg_y_visible(bmap, i) ){
			sg_cursor_shift_left(&x_cursor, d.width, count);
		}
		sg_cursor_inc_y(&y_cursor);
	}

}

void shift_up(const sg_bmap_t * bmap, int count, sg_point_t start, sg_area_t d){
	sg_point_t dest;
	sg_size_t rows_shifted;
	sg_region_t region;

	if( count > bmap->area.height ){
		count = bmap->area.height;
	}

	if( count == 0 ){
		return;
	}

	if( d.height + start.y > bmap->area.height ){
		d.height = bmap->area.height - start.y;
	}

	//clear the top area
	dest.x = start.x;
	region.point.x = start.x;
	region.area.width = d.width;

	rows_shifted = 0;

	while( rows_shifted < d.height ){

		dest.y = start.y + rows_shifted - count;

		if( d.height - rows_shifted > count ){
			region.area.height = count;
		} else {
			region.area.height = d.height - rows_shifted;
		}

		region.point.y = dest.y + count;

		sg_draw_sub_bitmap(bmap, dest, bmap, &region);

		rows_shifted += region.area.height;
	}
}

void shift_down(const sg_bmap_t * bmap, int count, sg_point_t start, sg_area_t d){
	sg_point_t dest;
	sg_size_t rows_shifted;
	sg_region_t region;

	if( count > bmap->area.height ){
		count = bmap->area.height;
	}

	if( count == 0 ){
		return;
	}

	if( d.height + start.y > bmap->area.height ){
		d.height = bmap->area.height - start.y;
	}

	dest.x = start.x;
	region.area.width = d.width;
	region.point.x = start.x;
	rows_shifted = 0;

	while( rows_shifted < d.height ){

		dest.y = start.y + d.height - rows_shifted;

		if( d.height - rows_shifted > count ){
			region.area.height = count;
		} else {
			region.area.height = d.height - rows_shifted;
			dest.y += (count - region.area.height);
		}

		region.point.y = dest.y - count;

		sg_draw_sub_bitmap(bmap, dest, bmap, &region);

		rows_shifted += region.area.height;

	}
}
