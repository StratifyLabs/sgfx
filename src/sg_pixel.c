//Copyright 2011-2016 Tyler Gilbert; All Rights Reserved


#include <string.h>
#include "sg.h"

static int sg_op_bitmap(sg_bmap_t * mg, sg_point_t p_dest, const sg_bmap_t * bitmap, sg_point_t p_src, sg_dim_t d, sg_bmap_data_t (*op)(sg_bmap_data_t current, sg_bmap_data_t input));
static int sg_op_area(sg_bmap_t * mg, sg_point_t p, sg_dim_t d, sg_bmap_data_t v, sg_bmap_data_t (*op)(sg_bmap_data_t current, sg_bmap_data_t input));


static sg_bmap_data_t calc_row_mask(sg_bmap_data_t v, sg_size_t row){
	switch(v){
	case 0xAA: return (v >> (row & 0x01));
	case 0x55: return (0xAA >> (1 - (row & 0x01)));
	case 0xCC:
		if( (row & 0x03) > 1 ){
			return 0x33;
		}
		break;
	case 0x33:
		if( (row & 0x03) > 1 ){
			return 0xCC;
		}
		break;
	case 0x0F:
		if( (row & 0x0F) > 7 ){
			return 0xF0;
		}
		break;
	case 0xF0:
		if( (row & 0x0F) > 7 ){
			return 0x0F;
		}
		break;
	}
	return v;
}

static u8 calc_mask(sg_size_t x){
	u8 mask;
	u16 x_mod;
	//divide by 8
	x_mod = x & 0x07;
	x_mod = (1<<(8 - x_mod))-1; //can be a 9-bit calculation
	mask = x_mod;
	return mask;
}

static sg_size_t calc_col_span(sg_int_t x, sg_size_t w){
	sg_size_t col_span;
	u8 left_bits = x & 0x07;
	col_span = ((w + left_bits - 1) >> 3) + 1;
	return col_span;
}

void sg_fill(sg_bmap_t * mg, sg_bmap_data_t v, sg_int_t start, sg_size_t h){
	sg_bmap_data_t * p = sg_data(mg, sg_point(0, start));
	size_t size = h * sg_cols(mg);
	memset(p, v, size);
}


void sg_pour(sg_bmap_t * mg, sg_point_t p){
	sg_int_t xmin, xmax;
	sg_point_t above;
	sg_point_t below;
	u8 is_above, is_below;

	if( !sg_x_visible(mg, p.x) ){
		return;
	}

	sg_tst_hedge(mg, p, &xmin, &xmax); //find the bounding points xmin and xmax
	is_above = !sg_tst_hline(mg, xmin, xmax, p.y+1, &(above.x)); //see if anywhere above the bounded region is blank
	is_below = !sg_tst_hline(mg, xmin, xmax, p.y-1, &(below.x)); //see if anywhere below the bounded region is blank
	sg_set_hline(mg, xmin, xmax, p.y, 1);
	if( is_above ){
		above.y = p.y+1;
		sg_pour(mg, above); //if the above line has a blank spot -- fill it
	}
	if( is_below ){
		below.y = p.y-1;
		sg_pour(mg, below); //if the below line has a blank spot -- fill it
	}
}

sg_bmap_data_t sg_op_set(sg_bmap_data_t current, sg_bmap_data_t input){
	return current | input;
}

sg_bmap_data_t sg_op_clr(sg_bmap_data_t current, sg_bmap_data_t input){
	return current & ~input;
}

sg_bmap_data_t sg_op_inv(sg_bmap_data_t current, sg_bmap_data_t input){
	return current ^ input;
}

sg_bmap_data_t sg_op_assign(sg_bmap_data_t current, sg_bmap_data_t input){
	return input;
}


int sg_set_bitmap(sg_bmap_t * mg, const sg_bmap_t * bitmap, sg_point_t p){
	return sg_op_bitmap(mg, p, bitmap, sg_point(0,0), bitmap->dim, sg_op_set);
}

int sg_set_bitmap_area(sg_bmap_t * mg, sg_point_t dest, const sg_bmap_t * bitmap, sg_point_t p_src,  sg_dim_t d){
	return sg_op_bitmap(mg, dest, bitmap, p_src, d, sg_op_set);
}

int sg_clr_bitmap(sg_bmap_t * mg, const sg_bmap_t * bitmap, sg_point_t p){
	return sg_op_bitmap(mg, p, bitmap, sg_point(0,0), bitmap->dim, sg_op_clr);
}

int sg_clr_bitmap_area(sg_bmap_t * mg, sg_point_t dest, const sg_bmap_t * bitmap, sg_point_t p_src,  sg_dim_t d){
	return sg_op_bitmap(mg, dest, bitmap, p_src, d, sg_op_clr);
}

int sg_inv_bitmap(sg_bmap_t * mg, const sg_bmap_t * bitmap, sg_point_t p){
	return sg_op_bitmap(mg, p, bitmap, sg_point(0,0), bitmap->dim, sg_op_inv);
}

int sg_inv_bitmap_area(sg_bmap_t * mg, sg_point_t dest, const sg_bmap_t * bitmap, sg_point_t p_src,  sg_dim_t d){
	return sg_op_bitmap(mg, dest, bitmap, p_src, d, sg_op_inv);
}

int sg_assign_bitmap(sg_bmap_t * mg, const sg_bmap_t * bitmap, sg_point_t p){
	return sg_op_bitmap(mg, p, bitmap, sg_point(0,0), bitmap->dim, sg_op_assign);

}

int sg_assign_bitmap_area(sg_bmap_t * mg, sg_point_t dest, const sg_bmap_t * bitmap, sg_point_t p_src, sg_dim_t d){
	sg_clr_area(mg, dest, d, 0xFF);
	return sg_set_bitmap_area(mg, dest, bitmap, p_src, d);
}

u8 sg_tst_hline(const sg_bmap_t * mg, sg_int_t xmin, sg_int_t xmax, sg_int_t y, sg_int_t * pos){
	sg_point_t p;
	if( !sg_y_visible(mg, y) ){
		return 1;
	}
	sg_bound_x(mg, &xmin);
	sg_bound_x(mg, &xmax);
	p.y = y;
	for(p.x = xmin; p.x < xmax; p.x++){
		if( sg_tst_bounded_pixel(mg,p) == 0 ){
			*pos = p.x;
			return false;
		}
	}
	return 1;
}

void sg_tst_hedge(const sg_bmap_t * mg, sg_point_t p, sg_int_t * xmin, sg_int_t * xmax){
	sg_point_t min;
	sg_point_t max;
	sg_bound(mg, &p);
	min.point = p.point;
	max.point = p.point;
	while( sg_tst_bounded_pixel(mg, min) == false ){ min.x--; }
	while( sg_tst_bounded_pixel(mg, max) == false ){ max.x++; }
	*xmin = min.x+1;
	*xmax = max.x;
	return;
}

int sg_op_area(sg_bmap_t * mg, sg_point_t p, sg_dim_t d, sg_bmap_data_t v, sg_bmap_data_t (*op)(sg_bmap_data_t current, sg_bmap_data_t input)){
	sg_int_t i, j;
	u8 byte;
	u8 left_mask;
	u8 right_mask;
	sg_size_t col_span;
	u8 row_mask;

	sg_size_t sg_cols = sg_byte_width(mg);
	sg_bmap_data_t * dest;

	left_mask = calc_mask(p.x);
	right_mask = ~calc_mask(p.x + d.w);
	if( right_mask == 0 ){ right_mask = ~right_mask; }
	col_span = calc_col_span(p.x, d.w);

	for(i=0; i < d.h; i++){
		if( (sg_y_visible(mg, p.y+i)) ){ //if p.y is not visible don't do this row
			sg_point_t tp;
			tp.x = p.x;
			tp.y = p.y+i;
			dest = mg->data + sg_offset(tp, sg_cols);
			row_mask = calc_row_mask(v, i);

			for(j=0; j < col_span; j++){

				if( (((p.x>>3) + j) >= sg_cols) || ((((p.x>>3) + j) < 0)) ){ //stop if the rest is off the screen
					break;
				}

				byte = row_mask;

				if( (j == (col_span - 1)) ){
					byte &= right_mask;
				}

				if( j == 0 ){
					byte &= left_mask;
				}

				dest[0] = op(dest[0], byte);
				dest++;
			}
		} else {
			//if writing is past the bottom then writing is done
			if( p.y+i >= mg->dim.h ){
				return 0;
			}
		}
	}

	return 0;
}

int sg_op_bitmap(sg_bmap_t * mg, sg_point_t p_dest, const sg_bmap_t * bitmap, sg_point_t p_src, sg_dim_t d, sg_bmap_data_t (*op)(sg_bmap_data_t current, sg_bmap_data_t input)){
	sg_int_t i, j;
	u8 shift = p_dest.x & 0x07;
	u8 not_shift =  8 - shift;
	u8 byte;
	u8 next_byte;
	u8 width_mask;
	u8 src_shift;
	u8 src_not_shift;
	sg_size_t col_span;
	sg_point_t tp;
	sg_size_t sg_cols = sg_byte_width(mg);
	sg_size_t bitmap_cols = sg_byte_width(bitmap);

	sg_bmap_data_t * dest;
	const sg_bmap_data_t * src;


	src_shift = p_src.x & 0x07;
	src_not_shift = 8 - src_shift;
	col_span = calc_col_span(p_src.x, d.w);

	width_mask = ~calc_mask(d.w);
	if( width_mask == 0 ){ width_mask = ~width_mask; }

	for(i=0; i < d.h; i++){

		if( (sg_y_visible(mg, p_dest.y+i)) && (sg_y_visible(bitmap, p_src.y+i)) ){ //if p.y is not visible don't do this row

			tp.x = p_dest.x;
			tp.y = p_dest.y+i;
			dest = mg->data + sg_offset(tp, sg_cols);
			tp.x = p_src.x;
			tp.y = p_src.y + i;
			src = bitmap->data + sg_offset(tp, bitmap_cols);
			if( src == 0 ){
				return -1;
			}

			for(j=0; j < col_span; j++){

				if( (((p_dest.x>>3) + j) >= sg_cols) || ((p_dest.x>>3) < 0) ){ //stop if the rest is off the screen
					break;
				}

				if( ((p_src.x>>3) + j) >= bitmap_cols ){ //stop if the rest is off the screen
					break;
				}

				byte = *src << src_shift;

				if( src_shift != 0 ){
					next_byte = *(src+1) >> src_not_shift;
					byte |= next_byte;
				}

				if( j == col_span -1 ){
					byte &= width_mask;
				}

				dest[0] = op(dest[0], byte>>shift);
				if( shift != 0 ){
					dest[1] = op(dest[1], byte << not_shift);
				}

				dest++;
				src++;

			}
		} else {
			//if writing is past the bottom then writing is done
			if( p_src.y+i >= mg->dim.h ){
				return 0;
			}
		}
	}

	return 0;
}


u8 sg_tst_bounded_pixel(const sg_bmap_t * mg, sg_point_t p){
	sg_bmap_data_t * target = mg->data + sg_offset(p,mg->columns);
	if( *target & sg_mask(p.x) ){
		return 1;
	}
	return 0;
}


u8 sg_tst_pixel(const sg_bmap_t * mg, sg_point_t p){
	if( sg_point_visible(mg, p) ){
		return sg_tst_bounded_pixel(mg,p);
	}
	return 1;
}

void sg_set_pixel(sg_bmap_t * mg, sg_point_t p){
	if( sg_point_visible(mg, p) ){
		sg_set_bounded_pixel(mg, p);
	}
}

void sg_set_bounded_pixel(sg_bmap_t * mg, sg_point_t p){
	sg_bmap_data_t * target = mg->data + sg_offset(p,mg->columns);
	*target |= sg_mask(p.x);
}


void sg_inv_pixel(sg_bmap_t * mg, sg_point_t p){
	if( sg_point_visible(mg, p) ){
		sg_inv_bounded_pixel(mg, p);
	}
}

void sg_inv_bounded_pixel(sg_bmap_t * mg, sg_point_t p){
	sg_bmap_data_t * target = mg->data + sg_offset(p,mg->columns);
	*target ^= sg_mask(p.x);
}

void sg_clr_pixel(sg_bmap_t * mg, sg_point_t p){
	if( sg_point_visible(mg, p) ){
		sg_clr_bounded_pixel(mg, p);
	}
}

void sg_clr_bounded_pixel(sg_bmap_t * mg, sg_point_t p){
	sg_bmap_data_t * target = mg->data + sg_offset(p,mg->columns);
	*target &= ~sg_mask(p.x);
}

void sg_set_area(sg_bmap_t * mg, sg_point_t p, sg_dim_t d, sg_bmap_data_t v){
	sg_op_area(mg, p, d, v, sg_op_set);
}

void sg_clr_area(sg_bmap_t * mg, sg_point_t p, sg_dim_t d, sg_bmap_data_t v){
	sg_op_area(mg, p, d, 0xFF, sg_op_clr);
}

void sg_inv_area(sg_bmap_t * mg, sg_point_t p, sg_dim_t d, sg_bmap_data_t v){
	sg_op_area(mg, p, d, 0xFF, sg_op_inv);
}

void sg_inv_rows(sg_bmap_t * mg, sg_int_t start, sg_int_t stop){
	int i;
	sg_int_t sr;
	sg_int_t er;
	if( start < stop ){
		sr = start;
		er = stop;
	} else {
		sr = stop;
		er = start;
	}
	sg_bmap_data_t * data = sg_data(mg, sg_point(0, sr));
	size_t size = sg_byte_width(mg)*(er - sr);
	for(i=0; i < size; i++){
		data[i] = ~data[i];
	}
}

static inline int abs_value(int x){
	if( x < 0 ){ return x*-1; }
	return x;
}

static void sg_op_vline(sg_bmap_t * mg,
		sg_int_t x,
		sg_int_t ymin,
		sg_int_t ymax,
		sg_size_t thickness,
		void (*op)(sg_bmap_t *, sg_point_t)){
	sg_point_t p;
	sg_size_t half_thick = thickness/2;
	sg_size_t it;
	//for(i=ymin-half_thick; i <= ymax+half_thick; i++){
	for(p.y=ymin; p.y <= ymax; p.y++){
		for(it=0; it < thickness; it++){
			//! \todo Use byte level operations for thickness
			p.x = x - half_thick + it;
			op(mg, p);
		}
	}
}

static void sg_op_hline(sg_bmap_t * mg,
		sg_int_t xmin,
		sg_int_t xmax,
		sg_int_t y,
		sg_size_t thickness,
		void (*op)(sg_bmap_t *, sg_point_t)){
	sg_point_t p;
	sg_size_t half_thick = thickness/2;
	sg_size_t it;
	//for(i=xmin-half_thick; i <= xmax+half_thick; i++){
	for(p.x=xmin; p.x <= xmax; p.x++){
		for(it=0; it < thickness; it++){
			//! \todo Use byte level operations here
			p.y = y - half_thick + it;
			op(mg, p);
		}
	}
}


static void sg_op_line(sg_bmap_t * mg,
		sg_point_t p1,
		sg_point_t p2,
		sg_size_t thickness,
		void (*op)(sg_bmap_t *, sg_point_t)){
	int dx, dy;
	int adx, ady;
	int rise, run;
	int i;
	sg_point_t p;
	sg_point_t tmp;
	sg_size_t half_thick;

	if( thickness == 0 ){
		thickness = 1;
	}

	if( p2.y == p1.y ){
		sg_op_hline(mg, p1.x < p2.x ? p1.x : p2.x, p1.x > p2.x ? p1.x : p2.x, p1.y, thickness, op);
		return;
	}

	if( p2.x == p1.x ){
		sg_op_vline(mg, p1.x, p1.y < p2.y ? p1.y : p2.y, p1.y > p2.y ? p1.y : p2.y, thickness, op);
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
				op(mg,tmp);
			}
			p.x += dx;
			p.y = ((p.x - p1.x) * rise + dy*run/2) / run + p1.y;
		}
	} else {
		while(p.y != p2.y ){
			for(i=0; i < thickness; i++){
				tmp.point = p.point;
				tmp.x = p.x - half_thick + i;
				op(mg,tmp);
			}
			p.y += dy;
			p.x = ((p.y - p1.y) * run + dx*rise/2) / rise + p1.x;
		}
	}

	if( adx <= ady ){
		for(i=0; i < thickness; i++){
			tmp.point = p2.point;
			tmp.x = p2.x - half_thick + i;
			op(mg, tmp);
		}
	} else {
		for(i=0; i < thickness; i++){
			tmp.point = p2.point;
			tmp.y = p2.y - half_thick + i;
			op(mg, tmp);
		}
	}
	//op(mg, p2);
}

void sg_set_vline(sg_bmap_t * mg, sg_int_t x, sg_int_t ymin, sg_int_t ymax, sg_size_t thickness){
	sg_op_vline(mg, x, ymin, ymax, thickness, sg_set_pixel);
}

void sg_clr_vline(sg_bmap_t * mg, sg_int_t x, sg_int_t ymin, sg_int_t ymax, sg_size_t thickness){
	sg_op_vline(mg, x, ymin, ymax, thickness, sg_clr_pixel);
}

void sg_inv_vline(sg_bmap_t * mg, sg_int_t x, sg_int_t ymin, sg_int_t ymax, sg_size_t thickness){
	sg_op_vline(mg, x, ymin, ymax, thickness, sg_inv_pixel);
}

void sg_set_hline(sg_bmap_t * mg, sg_int_t xmin, sg_int_t xmax, sg_int_t y, sg_size_t thickness){
	sg_op_hline(mg, xmin, xmax, y, thickness, sg_set_pixel);
}

void sg_clr_hline(sg_bmap_t * mg, sg_int_t xmin, sg_int_t xmax, sg_int_t y, sg_size_t thickness){
	sg_op_hline(mg, xmin, xmax, y, thickness, sg_clr_pixel);
}

void sg_inv_hline(sg_bmap_t * mg, sg_int_t xmin, sg_int_t xmax, sg_int_t y, sg_size_t thickness){
	sg_op_hline(mg, xmin, xmax, y, thickness, sg_inv_pixel);
}


void sg_set_line(sg_bmap_t * mg, sg_point_t p1, sg_point_t p2, sg_size_t thickness){
	sg_op_line(mg, p1, p2, thickness, sg_set_pixel);
}

void sg_clr_line(sg_bmap_t * mg, sg_point_t p1, sg_point_t p2, sg_size_t thickness){
	sg_op_line(mg, p1, p2, thickness, sg_clr_pixel);
}

void sg_inv_line(sg_bmap_t * mg, sg_point_t p1, sg_point_t p2, sg_size_t thickness){
	sg_op_line(mg, p1, p2, thickness, sg_inv_pixel);
}

