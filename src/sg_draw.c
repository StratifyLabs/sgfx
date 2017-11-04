//Copyright 2011-2016 Tyler Gilbert; All Rights Reserved

#include <string.h>
#include "sg_config.h"
#include "sg.h"

extern sg_color_t sg_cursor_get_pixel_no_inc(sg_cursor_t * cursor);

static inline int abs_value(int x){  if( x < 0 ){ return x*-1; } return x; }

static int is_point_visible(const sg_bmap_t * bmap, sg_point_t p);
static int truncate_visible(const sg_bmap_t * bmap, sg_point_t * p, sg_dim_t * d);

static void draw_hline(const sg_bmap_t * bmap, sg_int_t xmin, sg_int_t xmax, sg_int_t y);
static void get_hedge(const sg_bmap_t * bmap, sg_point_t p, sg_int_t * xmin, sg_int_t * xmax, u8 active, sg_bounds_t bounds);
static void draw_pour_recursive(const sg_bmap_t * bmap, sg_point_t p, sg_bounds_t bounds, u8 active);
static u8 get_hline(const sg_bmap_t * mg, sg_int_t xmin, sg_int_t xmax, sg_int_t y, sg_int_t * pos, u8 active);
static u16 calc_largest_delta(sg_point_t p0, sg_point_t p1);

sg_color_t sg_get_pixel(const sg_bmap_t * bmap, sg_point_t p){
	sg_cursor_t cursor;
	if( is_point_visible(bmap,p) ){
		sg_cursor_set(&cursor, bmap, p);
		return sg_cursor_get_pixel(&cursor);
	}
	return (sg_color_t)-1;
}

void sg_draw_pixel(const sg_bmap_t * bmap, sg_point_t p){
	//draw a pixel at point p
	sg_cursor_t cursor;
	if( is_point_visible(bmap,p) ){
		sg_cursor_set(&cursor, bmap, p);
		sg_cursor_draw_pixel(&cursor);
	}
}


void sg_draw_line(const sg_bmap_t * bmap, sg_point_t p1, sg_point_t p2){
	int dx, dy;
	int adx, ady;
	int rise, run;
	int i;
	sg_point_t p;
	sg_dim_t d;
	sg_point_t tmp;
	sg_size_t thickness = bmap->pen.thickness;
	sg_size_t half_thick;

	if( thickness == 0 ){
		thickness = 1;
	}

	if( p2.y == p1.y ){
		if( p1.x < p2.x ){
			p.x = p1.x;
			d.width = p2.x - p1.x;
		} else {
			p.x = p2.x;
			d.width = p1.x - p2.x;
		}

		d.height = bmap->pen.thickness;
		p.y = p2.y - d.height / 2;
		sg_draw_rectangle(bmap, p, d);
		return;
	}

	if( p2.x == p1.x ){

		if( p1.y < p2.y ){
			p.y = p1.y;
			d.height = p2.y - p1.y;
		} else {
			p.y = p2.y;
			d.height = p1.y - p2.y;
		}

		d.width = bmap->pen.thickness;
		p.x = p2.x - d.width / 2;
		sg_draw_rectangle(bmap, p, d);
		return;
	}

	half_thick = thickness/2;

	if( p2.y > p1.y ){
		dy = 1;
	} else {
		dy = -1;
	}

	if( p2.x > p1.x ){
		dx = 1;
	} else {
		dx = -1;
	}

	adx = abs_value(p2.x - p1.x);
	ady = abs_value(p2.y - p1.y);
	rise = (p2.y - p1.y);
	run = (p2.x - p1.x);

	p.x = p1.x;
	p.y = p1.y;
	if( adx > ady ){

		while(p.x != p2.x ){
			for(i=0; i < thickness; i++){
				tmp.point = p.point;
				tmp.y = p.y - half_thick + i;
				sg_draw_pixel(bmap, tmp);
			}
			p.x += dx;
			p.y = ((p.x - p1.x) * rise + dy*run/2) / run + p1.y;
		}
	} else {
		while(p.y != p2.y ){
			for(i=0; i < thickness; i++){
				tmp.point = p.point;
				tmp.x = p.x - half_thick + i;
				sg_draw_pixel(bmap, tmp);
			}
			p.y += dy;
			p.x = ((p.y - p1.y) * run + dx*rise/2) / rise + p1.x;
		}
	}

	if( adx <= ady ){
		for(i=0; i < thickness; i++){
			tmp.point = p2.point;
			tmp.x = p2.x - half_thick + i;
			sg_draw_pixel(bmap, tmp);
		}
	} else {
		for(i=0; i < thickness; i++){
			tmp.point = p2.point;
			tmp.y = p2.y - half_thick + i;
			sg_draw_pixel(bmap, tmp);
		}
	}
}

u16 calc_largest_delta(sg_point_t p0, sg_point_t p1){
	s16 dx; s16 dy;
	dx = p0.x - p1.x;
	dy = p0.y - p1.y;

	dx = dx < 0 ? dx*-1 : dx;
	dy = dy < 0 ? dy*-1 : dy;

	return (dx > dy) ? dx : dy;

}

void sg_draw_arc(const sg_bmap_t * bmap, sg_point_t p, sg_dim_t d, s16 start, s16 end){

}

void sg_draw_quadtratic_bezier(const sg_bmap_t * bmap, sg_point_t p0, sg_point_t p1, sg_point_t p2){
	u32 i;
	s32 x;
	s32 y;
	u32 steps;
	u32 steps2;
	sg_point_t current;
	sg_point_t last;

	steps = calc_largest_delta(p0, p1);
	steps += calc_largest_delta(p1, p2);
	steps2 = steps*steps;


	//t goes from zero to one
	for(i=0; i < steps; i++){
		//(1-t)^2*P0 + 2*(1-t)*t*P2 + t^2*P2

		x = (steps - i)*(steps - i)*p0.x + 2*(steps - i)*i*p1.x + i*i*p2.x;
		y = (steps - i)*(steps - i)*p0.y + 2*(steps - i)*i*p1.y + i*i*p2.y;

		current.x = x / (steps2);
		current.y = y / (steps2);

		if( i == 0 || (current.point != last.point)){
			last.point = current.point;
			sg_draw_pixel(bmap, current);
		}
	}
}

void sg_draw_cubic_bezier(const sg_bmap_t * bmap, sg_point_t p0, sg_point_t p1, sg_point_t p2, sg_point_t p3){
	u32 i;
	s32 x;
	s32 y;
	u32 steps;
	u32 steps3;
	sg_point_t current;
	sg_point_t last;

	//calc distance to determine number of steps
	steps = calc_largest_delta(p0, p1);
	steps += calc_largest_delta(p1, p2);
	steps += calc_largest_delta(p2, p3);

	steps3 = steps*steps*steps;

	//t goes from zero to one
	for(i=0; i < steps; i++){
		//(1-t)^2*P0 + 2*(1-t)*t*P2 + t^2*P2

		x = (steps-i)*(steps-i)*p0.x +
				3*(steps-i)*(steps-i)*i*p1.x +
				3*(steps-i)*i*i*p1.x +
				i*i*i*p2.x;
		y = (steps-i)*(steps-i)*p0.y +
				3*(steps-i)*(steps-i)*i*p1.y +
				3*(steps-i)*i*i*p1.y +
				i*i*i*p2.y;

		current.x = x / (steps3);
		current.y = y / (steps3);

		if( i == 0 || (current.point != last.point)){
			last.point = current.point;
			sg_draw_pixel(bmap, current);
		}
	}
}

void sg_draw_rectangle(const sg_bmap_t * bmap, sg_point_t p, sg_dim_t d){
	sg_cursor_t y_cursor;
	sg_cursor_t x_cursor;
	sg_size_t i;

	if( truncate_visible(bmap, &p, &d) ){
		sg_cursor_set(&y_cursor, bmap, p);
		for(i=0; i < d.height; i++){
			sg_cursor_copy(&x_cursor, &y_cursor);
			sg_cursor_draw_hline(&x_cursor, d.width);
			sg_cursor_inc_y(&y_cursor);
		}
	}
}

void sg_draw_pattern(const sg_bmap_t * bmap, sg_point_t p, sg_dim_t d, sg_bmap_data_t odd_pattern, sg_bmap_data_t even_pattern, sg_size_t pattern_height){
	//fill the specified region with the specified patterns
	sg_size_t i;
	sg_cursor_t y_cursor;
	sg_cursor_t x_cursor;
	sg_bmap_data_t pattern;
	sg_bmap_data_t odd_pattern_color;
	sg_bmap_data_t even_pattern_color;


	if( truncate_visible(bmap, &p, &d) ){

		sg_cursor_set(&y_cursor, bmap, p);

		sg_color_t color = bmap->pen.color & ((1<<SG_BITS_PER_PIXEL)-1);

		even_pattern_color = 0;
		odd_pattern_color = 0;
		for(i=0; i < SG_PIXELS_PER_WORD; i++){
			if( even_pattern & (1<<i) ){
				even_pattern_color |= (color << (i*SG_BITS_PER_PIXEL));
			}

			if( odd_pattern & (1<<i) ){
				odd_pattern_color |= (color << (i*SG_BITS_PER_PIXEL));
			}
		}

		for(i=0; i < d.height; i++){
			sg_cursor_copy(&x_cursor, &y_cursor);
			if( (i/pattern_height) % 2 ){
				pattern = odd_pattern_color;
			} else {
				pattern = even_pattern_color;
			}
			sg_cursor_draw_pattern(&x_cursor, d.width, pattern);
			sg_cursor_inc_y(&y_cursor);
		}
	}

}

void sg_draw_bitmap(const sg_bmap_t * bmap_dest, sg_point_t p_dest, const sg_bmap_t * bmap_src){
	sg_draw_sub_bitmap(bmap_dest, p_dest, bmap_src, sg_point(0,0), bmap_src->dim);
}

void sg_draw_sub_bitmap(const sg_bmap_t * bmap_dest, sg_point_t p_dest, const sg_bmap_t * bmap_src, sg_point_t p_src, sg_dim_t d_src){
	sg_int_t i;
	sg_cursor_t y_dest_cursor;
	sg_cursor_t x_dest_cursor;
	sg_cursor_t y_src_cursor;
	sg_cursor_t x_src_cursor;
	sg_int_t h;
	sg_int_t w;

	if( truncate_visible(bmap_src, &p_src, &d_src) && truncate_visible(bmap_dest, &p_dest, &d_src) ){

		//check to see if p_dest values are negative and truncate source accordingly
		w = d_src.width;
		h = d_src.height;
		if( p_dest.x < 0 ){
			p_src.x += p_dest.x;
			w += p_dest.x; //reduce width
			p_dest.x = 0;
		}
		if( p_dest.y < 0 ){
			p_src.y += p_dest.y;
			h += p_dest.y; //reduce height
			p_dest.y = 0;
		}

		sg_cursor_set(&y_dest_cursor, bmap_dest, p_dest);
		sg_cursor_set(&y_src_cursor, bmap_src, p_src);

		if( p_dest.y + h > bmap_dest->dim.height ){
			h = bmap_dest->dim.height - p_dest.y;
		}

		if( p_dest.x + w > bmap_dest->dim.width ){
			w = bmap_dest->dim.width - p_dest.x;
		}

		if( w < 0 ){
			return;
		}

		//take bitmap and draw it on bmap
		for(i=0; i < h; i++){

			sg_cursor_copy(&x_dest_cursor, &y_dest_cursor);
			sg_cursor_copy(&x_src_cursor, &y_src_cursor);

			//copy the src cursor to the dest cursor over the source width
			sg_cursor_draw_cursor(&x_dest_cursor, &x_src_cursor, w);

			sg_cursor_inc_y(&y_dest_cursor);
			sg_cursor_inc_y(&y_src_cursor);
		}
	}

}

void draw_hline(const sg_bmap_t * bmap, sg_int_t xmin, sg_int_t xmax, sg_int_t y){
	sg_cursor_t x_cursor;
	sg_cursor_set(&x_cursor, bmap, sg_point(xmin,y));
	sg_cursor_draw_hline(&x_cursor, xmax - xmin);

}

void sg_draw_pour(const sg_bmap_t * bmap, sg_point_t p, sg_bounds_t bounds){
	u8 active;

	if( bmap->pen.color ){
		//set the values to
		active = 1;
	} else {
		active = 0;
	}

	//limit bounds to inside the bmap
	if( (bounds.bottom_right.x < 0) || (bounds.bottom_right.y < 0) ){
		return;
	}

	if( (bounds.top_left.x >= bmap->dim.width) || (bounds.top_left.y >= bmap->dim.height) ){
		return;
	}

	if( bounds.top_left.x < 0 ){ bounds.top_left.x = 0; }
	if( bounds.top_left.y < 0 ){ bounds.top_left.y = 0; }
	if( bounds.bottom_right.x >= bmap->dim.width ){ bounds.bottom_right.x = bmap->dim.width-1; }
	if( bounds.bottom_right.y >= bmap->dim.height ){ bounds.bottom_right.y = bmap->dim.height-1; }

	draw_pour_recursive(bmap, p, bounds, active);
}

void draw_pour_recursive(const sg_bmap_t * bmap, sg_point_t p, sg_bounds_t bounds, u8 active){
	sg_int_t xmin, xmax;
	sg_point_t above;
	sg_point_t below;
	u8 is_above, is_below;

	//check the pen
	xmin = p.x;
	xmax = p.x;

	get_hedge(bmap, p, &xmin, &xmax, active, bounds); //find the bounding points xmin and xmax
	if( p.y+1 <= bounds.bottom_right.y ){
		is_below = !get_hline(bmap, xmin, xmax, p.y+1, &(below.x), active); //see if anywhere above the bounded region is blank
	} else {
		is_below = 0;
	}

	if( p.y-1 >= bounds.top_left.y ){
		is_above = !get_hline(bmap, xmin, xmax, p.y-1, &(above.x), active); //see if anywhere below the bounded region is blank
	} else {
		is_above = 0;
	}

	draw_hline(bmap, xmin, xmax, p.y);

	if( is_below ){
		below.y = p.y+1;
		draw_pour_recursive(bmap, below, bounds, active); //if the below line has a blank spot -- fill it
	}

	if( is_above ){
		above.y = p.y-1;
		draw_pour_recursive(bmap, above, bounds, active); //if the above line has a blank spot -- fill it
	}
}

void get_hedge(const sg_bmap_t * bmap, sg_point_t p, sg_int_t * xmin, sg_int_t * xmax, u8 active, sg_bounds_t bounds){
	sg_cursor_t min_cursor;
	sg_cursor_t max_cursor;
	sg_point_bound(bmap, &p);
	sg_int_t min = *xmin;
	sg_int_t max = *xmax;

	sg_cursor_set(&min_cursor, bmap, p);
	sg_cursor_set(&max_cursor, bmap, p);

	while( ((sg_cursor_get_pixel_no_inc(&min_cursor) != 0) != active)
			&& (min > bounds.top_left.x) ){
		sg_cursor_dec_x(&min_cursor);
		min--;
	}

	//get pixel auto increments the cursor
	while( ((sg_cursor_get_pixel(&max_cursor) != 0) != active)
			&& (max < bounds.bottom_right.x) ){
		max++;
	}

	*xmin = min+1;
	*xmax = max;
	return;
}

//This checks to see if anywhere along the line is an inactive hole
u8 get_hline(const sg_bmap_t * bmap, sg_int_t xmin, sg_int_t xmax, sg_int_t y, sg_int_t * pos, u8 active){
	sg_point_t p;
	sg_cursor_t cursor;

	p.x = xmin;
	p.y = y;

	sg_cursor_set(&cursor, bmap, p);

	for(p.x = xmin; p.x < xmax; p.x++){
		if( (sg_cursor_get_pixel(&cursor) != 0) != active ){
			*pos = p.x;
			return 0;
		}
	}
	return 1;
}

int is_point_visible(const sg_bmap_t * bmap, sg_point_t p){
	if( (p.x < 0) || (p.x >= bmap->dim.width) ){
		return 0;
	}

	if( (p.y < 0) || (p.y >= bmap->dim.height) ){
		return 0;
	}

	return 1;
}

int truncate_visible(const sg_bmap_t * bmap, sg_point_t * p, sg_dim_t * d){
	if( p->x < 0 ){
		if( p->x + d->width >= 0 ){
			d->width += p->x;
			p->x = 0;
		} else {
			return 0;
		}
	}

	if( p->y < 0 ){
		if( p->y + bmap->dim.height >= 0 ){
			d->height += p->y;
			p->y = 0;
		} else {
			return 0;
		}
	}

	if( p->x >= bmap->dim.width ){
		return 0;
	} else if( p->x + d->width >= bmap->dim.width ){
		d->width = bmap->dim.width - p->x;
	}

	if( p->y >= bmap->dim.height ){
		return 0;
	} else if( p->y + d->height >= bmap->dim.height ){
		d->height = bmap->dim.height - p->y;
	}

	return 1;
}
