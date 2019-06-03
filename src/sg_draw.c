//Copyright 2011-2016 Tyler Gilbert; All Rights Reserved

#include <string.h>
#include "sg_config.h"
#include "sg.h"


static inline int abs_value(int x){  if( x < 0 ){ return x*-1; } return x; }

static int is_point_visible(const sg_bmap_t * bmap, sg_point_t p);
static int truncate_visible(const sg_bmap_t * bmap, sg_point_t * p, sg_area_t * d);

static int draw_pour_recursive(const sg_bmap_t * bmap, sg_point_t p, const sg_region_t * region, sg_color_t active_color);
static u16 calc_largest_delta(sg_point_t p0, sg_point_t p1);

sg_color_t sg_get_pixel(const sg_bmap_t * bmap, sg_point_t p){
	sg_cursor_t cursor;
	if( is_point_visible(bmap,p) ){
		sg_cursor_set(&cursor, bmap, p);
		return sg_cursor_get_pixel(&cursor);
	}
	return (sg_color_t)-1;
}

//add antialiasing here, needs both a pixel and another point for adjustment
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
	sg_region_t region;
	sg_point_t tmp;
	sg_size_t thickness = bmap->pen.thickness;
	sg_size_t half_thick;

	if( thickness == 0 ){
		thickness = 1;
	}

	if( p2.y == p1.y ){
		if( p1.x < p2.x ){
			region.point.x = p1.x;
			region.area.width = p2.x - p1.x + 1;
		} else {
			region.point.x = p2.x;
			region.area.width = p1.x - p2.x + 1;
		}

		region.area.height = bmap->pen.thickness;
		region.point.y = p2.y - region.area.height / 2;
		sg_draw_rectangle(bmap, &region);
		return;
	}

	if( p2.x == p1.x ){

		if( p1.y < p2.y ){
			region.point.y = p1.y;
			region.area.height = p2.y - p1.y + 1;
		} else {
			region.point.y = p2.y;
			region.area.height = p1.y - p2.y + 1;
		}

		region.area.width = bmap->pen.thickness;
		region.point.x = p2.x - region.area.width / 2;
		sg_draw_rectangle(bmap, &region);
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

	region.point.x = p1.x;
	region.point.y = p1.y;
	if( adx > ady ){

		while(region.point.x != p2.x ){
			for(i=0; i < thickness; i++){
				tmp.point = region.point.point;
				tmp.y = region.point.y - half_thick + i;
				sg_draw_pixel(bmap, tmp);
			}
			region.point.x += dx;
			region.point.y = ((region.point.x - p1.x) * rise + dy*run/2) / run + p1.y;
		}
	} else {
		while(region.point.y != p2.y ){
			for(i=0; i < thickness; i++){
				tmp.point = region.point.point;
				tmp.x = region.point.x - half_thick + i;
				sg_draw_pixel(bmap, tmp);
			}
			region.point.y += dy;
			region.point.x = ((region.point.y - p1.y) * run + dx*rise/2) / rise + p1.x;
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

void sg_draw_arc(const sg_bmap_t * bmap, const sg_region_t * region, s16 start, s16 end, s16 rotation, sg_point_t * corners){
	sg_size_t half_thick;
	sg_size_t thickness;
	sg_point_t pen;
	sg_size_t t;
	sg_int_t thick;
	int i;
	sg_point_t center;
	sg_point_t last_point;
	sg_point_t p;
	sg_area_t d;
	sg_point_t min, max;
	sg_size_t rx;
	sg_size_t ry;

	p = region->point;
	d = region->area;

	thickness = bmap->pen.thickness;
	if( thickness == 0 ){
		thickness = 1;
	}

	rx = d.width/2 - thickness/2;
	ry = d.height/2 - thickness/2;
	center.x = p.x + d.width/2;
	center.y = p.y + d.height/2;

	if( corners ){
		min.x = SG_MAX;
		min.y = SG_MAX;
		max.x = SG_MIN;
		max.y = SG_MIN;
	}

	last_point.point = 0;
	half_thick = thickness/2;
	for(t=0; t < thickness; t++){
		thick =  t - half_thick;
		for(i=start; i < end; i++){
			sg_point_arc(&pen, rx + thick, ry + thick, i);
			if( i == 0 || (pen.point != last_point.point) ){
				last_point.point = pen.point;
				sg_point_rotate(&pen, rotation);
				sg_point_shift(&pen, center);
				sg_draw_pixel(bmap, pen);
				if( corners ){
					if( pen.x < min.x ){ min.x = pen.x; }
					if( pen.y < min.y ){ min.y = pen.y; }
					if( pen.x > max.x ){ max.x = pen.x; }
					if( pen.y > max.y ){ max.y = pen.y; }
				}
			}
		}
	}

	if( corners ){
		//update corners with min/max values
		corners[0] = min;
		corners[1] = max;
	}

}

void sg_draw_quadratic_bezier(const sg_bmap_t * bmap, sg_point_t p0, sg_point_t p1, sg_point_t p2, sg_point_t * corners){
	u32 i;
	s32 x;
	s32 y;
	u32 steps;
	u32 steps2;
	sg_point_t current;
	sg_point_t last;
	sg_point_t min, max;

	steps = calc_largest_delta(p0, p1);
	steps += calc_largest_delta(p1, p2);
	steps2 = steps*steps;

	if( steps2 == 0 ){
		return;
	}

	if( corners ){
		min.x = SG_MAX;
		min.y = SG_MAX;
		max.x = SG_MIN;
		max.y = SG_MIN;
	}

	//t goes from zero to one
	for(i=0; i < steps; i++){
		//(1-t)^2*P0 + 2*(1-t)*t*P2 + t^2*P2

		if( i < steps ){
			x = (steps - i)*(steps - i)*p0.x + 2*(steps - i)*i*p1.x + i*i*p2.x;
			y = (steps - i)*(steps - i)*p0.y + 2*(steps - i)*i*p1.y + i*i*p2.y;

			current.x = x / (steps2);
			current.y = y / (steps2);
		}

		if( corners ){
			if( current.x < min.x ){ min.x = current.x; }
			if( current.y < min.y ){ min.y = current.y; }
			if( current.x > max.x ){ max.x = current.x; }
			if( current.y > max.y ){ max.y = current.y; }
		}

		if( i != 0 ){
			sg_draw_line(bmap, current, last);
		}
		last.point = current.point;
	}

	sg_draw_line(bmap, last, p2);

	if( corners ){
		//update corners with min/max values
		corners[0] = min;
		corners[1] = max;
	}


}

void sg_draw_cubic_bezier(const sg_bmap_t * bmap, sg_point_t p0, sg_point_t p1, sg_point_t p2, sg_point_t p3, sg_point_t * corners){
	u32 i;
	s32 x;
	s32 y;
	u32 steps;
	u32 steps3;
	sg_point_t current;
	sg_point_t last;
	sg_point_t min, max;

	//calc distance to determine number of steps
	steps = calc_largest_delta(p0, p1);
	steps += calc_largest_delta(p1, p2);
	steps += calc_largest_delta(p2, p3);

	steps3 = steps*steps*steps;
	if( steps3 == 0 ){
		return;
	}

	if( corners ){
		min.x = SG_MAX;
		min.y = SG_MAX;
		max.x = SG_MIN;
		max.y = SG_MIN;
	}

	//t goes from zero to one
	for(i=0; i < steps; i++){

		if( i < steps ){
			x = (steps-i)*(steps-i)*(steps-i)*p0.x +
					3*(steps-i)*(steps-i)*i*p1.x +
					3*(steps-i)*i*i*p2.x +
					i*i*i*p3.x;

			y = (steps-i)*(steps-i)*(steps-i)*p0.y +
					3*(steps-i)*(steps-i)*i*p1.y +
					3*(steps-i)*i*i*p2.y +
					i*i*i*p3.y;

			current.x = x / (steps3);
			current.y = y / (steps3);
		}

		if( corners ){
			if( current.x < min.x ){ min.x = current.x; }
			if( current.y < min.y ){ min.y = current.y; }
			if( current.x > max.x ){ max.x = current.x; }
			if( current.y > max.y ){ max.y = current.y; }
		}

		if( i != 0 ){
			sg_draw_line(bmap, current, last);
		}

		last.point = current.point;
	}

	sg_draw_line(bmap, last, p3);

	if( corners ){
		//update corners with min/max values
		corners[0] = min;
		corners[1] = max;
	}

}

void sg_draw_rectangle(const sg_bmap_t * bmap, const sg_region_t * region){
	sg_cursor_t y_cursor;
	sg_cursor_t x_cursor;
	sg_point_t p;
	sg_area_t d;
	sg_size_t i;

	p = region->point;
	d = region->area;

	if( truncate_visible(bmap, &p, &d) ){
		sg_cursor_set(&y_cursor, bmap, p);
		for(i=0; i < d.height; i++){
			sg_cursor_copy(&x_cursor, &y_cursor);
			sg_cursor_draw_hline(&x_cursor, d.width);
			sg_cursor_inc_y(&y_cursor);
		}
	}
}

void sg_draw_pattern(const sg_bmap_t * bmap, const sg_region_t * region, sg_bmap_data_t odd_pattern, sg_bmap_data_t even_pattern, sg_size_t pattern_height){
	//fill the specified region with the specified patterns
	sg_size_t i;
	sg_cursor_t y_cursor;
	sg_cursor_t x_cursor;
	sg_bmap_data_t pattern;
	sg_bmap_data_t odd_pattern_color;
	sg_bmap_data_t even_pattern_color;
	sg_point_t p;
	sg_area_t d;

	p = region->point;
	d = region->area;

	if( truncate_visible(bmap, &p, &d) ){

		sg_cursor_set(&y_cursor, bmap, p);

		sg_color_t color = bmap->pen.color & ((1<<SG_BITS_PER_PIXEL_VALUE(bmap))-1);

		even_pattern_color = 0;
		odd_pattern_color = 0;
		for(i=0; i < SG_PIXELS_PER_WORD(bmap); i++){
			if( even_pattern & (1<<i) ){
				even_pattern_color |= (color << (i*SG_BITS_PER_PIXEL_VALUE(bmap)));
			}

			if( odd_pattern & (1<<i) ){
				odd_pattern_color |= (color << (i*SG_BITS_PER_PIXEL_VALUE(bmap)));
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
	sg_region_t region;
	region.point.point = 0;
	region.area = bmap_src->area;
	sg_draw_sub_bitmap(bmap_dest, p_dest, bmap_src, &region);
}

void sg_draw_sub_bitmap(const sg_bmap_t * bmap_dest, sg_point_t p_dest, const sg_bmap_t * bmap_src, const sg_region_t * region_src){
	sg_int_t i;
	sg_point_t p_src;
	sg_area_t d_src;
	sg_cursor_t y_dest_cursor;
	sg_cursor_t x_dest_cursor;
	sg_cursor_t y_src_cursor;
	sg_cursor_t x_src_cursor;
	sg_int_t h;
	sg_int_t w;

	p_src = region_src->point;
	d_src = region_src->area;

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

		if( p_dest.y + h > bmap_dest->area.height ){
			h = bmap_dest->area.height - p_dest.y;
		}

		if( p_dest.x + w > bmap_dest->area.width ){
			w = bmap_dest->area.width - p_dest.x;
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


void sg_draw_pour(const sg_bmap_t * bmap, sg_point_t p, const sg_region_t * region){

	sg_color_t active;
	sg_region_t tmp_region;

	if( (bmap->pen.color == 0) || (bmap->pen.o_flags & SG_PEN_FLAG_IS_ERASE) ){
		active = 0;
	} else {
		active = 1;
	}

	//limit bounds to inside the bmap
	if( (region->point.x + region->area.width < 0) || (region->point.y + region->area.height < 0) ){
		return;
	}

	if( (region->point.x >= bmap->area.width) || (region->point.y >= bmap->area.height) ){
		return;
	}

	tmp_region = *region;

	if( tmp_region.point.x < 0 ){ tmp_region.point.x = 0; }
	if( tmp_region.point.y < 0 ){ tmp_region.point.y = 0; }
	if( tmp_region.point.x + tmp_region.area.width >= bmap->area.width ){ tmp_region.area.width = bmap->area.width - tmp_region.point.x; }
	if( tmp_region.point.y + tmp_region.area.height >= bmap->area.height ){ tmp_region.area.height = bmap->area.height - tmp_region.point.y; }

	draw_pour_recursive(bmap, p, &tmp_region, active);
}

static int is_active_color(sg_color_t color, sg_color_t active_color){
	return (color == 0) == active_color;
}

int draw_pour_recursive(const sg_bmap_t * bmap, sg_point_t p, const sg_region_t * region, sg_color_t active){
	sg_cursor_t cursor;
	sg_cursor_set(&cursor, bmap, p);
	if( is_active_color(sg_cursor_get_pixel_no_inc(&cursor), active) ){
		sg_point_t draw_point;
		sg_int_t xmin, xmax;
		sg_int_t x;
		sg_int_t x_max_bound = region->point.x + region->area.width;
		sg_int_t y_max_bound = region->point.y + region->area.height;
		draw_point = p;
		xmin = p.x;
		xmax = p.y;
		while( is_active_color(sg_cursor_get_pixel_no_inc(&cursor), active) && (draw_point.x < x_max_bound) ){
			sg_cursor_draw_pixel(&cursor);
			draw_point.x++;
		}
		xmax = draw_point.x;

		sg_cursor_set(&cursor, bmap, p);
		draw_point = p;
		sg_cursor_dec_x(&cursor);
		draw_point.x--;
		while( is_active_color(sg_cursor_get_pixel_no_inc(&cursor), active) && (draw_point.x > region->point.x) ){
			sg_cursor_draw_pixel_no_inc(&cursor);
			sg_cursor_dec_x(&cursor);
			draw_point.x--;
		}
		xmin = draw_point.x;

		for(x = xmin+1; x < xmax; x++){
			if( p.y+1 < y_max_bound ){
				draw_point.x = x;
				draw_point.y = p.y+1;
				x = draw_pour_recursive(bmap, draw_point, region, active);
			}
		}

		for(x = xmin+1; x < xmax; x++){
			if( p.y-1 >= region->point.y ){
				draw_point.x = x;
				draw_point.y = p.y-1;
				x = draw_pour_recursive(bmap, draw_point, region, active);
			}
		}

		return xmax;
	}
	return p.x;
}



int is_point_visible(const sg_bmap_t * bmap, sg_point_t p){
	if( (p.x < 0) || (p.x >= bmap->area.width) ){
		return 0;
	}

	if( (p.y < 0) || (p.y >= bmap->area.height) ){
		return 0;
	}

	return 1;
}

//TODO this needs to account for margins
int truncate_visible(const sg_bmap_t * bmap, sg_point_t * p, sg_area_t * d){
	if( p->x < 0 ){
		if( p->x + d->width >= 0 ){
			d->width += p->x;
			p->x = 0;
		} else {
			return 0;
		}
	}

	if( p->y < 0 ){
		if( p->y + bmap->area.height >= 0 ){
			d->height += p->y;
			p->y = 0;
		} else {
			return 0;
		}
	}

	if( p->x >= bmap->area.width ){
		return 0;
	} else if( p->x + d->width >= bmap->area.width ){
		d->width = bmap->area.width - p->x;
	}

	if( p->y >= bmap->area.height ){
		return 0;
	} else if( p->y + d->height >= bmap->area.height ){
		d->height = bmap->area.height - p->y;
	}

	return 1;
}
