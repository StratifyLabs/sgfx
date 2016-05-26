/*! \file */ //Copyright 2011-2016 Tyler Gilbert; All Rights Reserved

#ifndef SG_H_
#define SG_H_

#include <mcu/types.h>
#include "sg_types.h"

#if defined __cplusplus
extern "C" {
#endif


/*! \brief Change effective size without free/alloc sequence */
void sg_set_data(sg_bmap_t * mg, sg_bmap_data_t * mem, sg_size_t w, sg_size_t h);
sg_bmap_data_t * sg_data(const sg_bmap_t * mg, sg_point_t p);
int sg_set_size(sg_bmap_t * mg, sg_size_t w, sg_size_t h, sg_size_t offset);
size_t sg_calc_size(sg_int_t w, sg_int_t h);
size_t sg_size(const sg_bmap_t * mg);
size_t sg_byte_width(const sg_bmap_t * mg);
size_t sg_word_width(const sg_bmap_t * mg);
size_t sg_calc_byte_width(sg_size_t w);
size_t sg_calc_word_width(sg_size_t w);
sg_int_t sg_x_max(const sg_bmap_t * mg);
sg_int_t sg_y_max(const sg_bmap_t * mg);
void sg_show(const sg_bmap_t * mg);

void sg_bound(const sg_bmap_t * mg, sg_point_t * p);
void sg_bound_x(const sg_bmap_t * mg, sg_int_t * x);
void sg_bound_y(const sg_bmap_t * mg, sg_int_t * y);

static inline sg_size_t sg_margin_left(const sg_bmap_t * mg){ return mg->margin_top_left.w; }
static inline sg_size_t sg_margin_right(const sg_bmap_t * mg){ return mg->margin_bottom_right.w; }
static inline sg_size_t sg_margin_top(const sg_bmap_t * mg){ return mg->margin_top_left.h; }
static inline sg_size_t sg_margin_bottom(const sg_bmap_t * mg){ return mg->margin_bottom_right.h; }

static inline sg_size_t sg_h(sg_bmap_t * mg){ return mg->dim.h; }
static inline sg_size_t sg_w(sg_bmap_t * mg){ return mg->dim.w; }
static inline sg_size_t sg_cols(sg_bmap_t * mg){ return mg->columns; }

void sg_flip_x(sg_bmap_t * mg);
void sg_flip_y(sg_bmap_t * mg);
void sg_flip_xy(sg_bmap_t * mg);

void sg_shift_right(sg_bmap_t * mg, int count, sg_point_t start, sg_dim_t d);
void sg_shift_left(sg_bmap_t * mg, int count, sg_point_t start, sg_dim_t d);
void sg_shift_up(sg_bmap_t * mg, int count, sg_point_t start, sg_dim_t d);
void sg_shift_down(sg_bmap_t * mg, int count, sg_point_t start, sg_dim_t d);

int sg_animate(sg_bmap_t * mg, sg_bmap_t * bitmap, sg_animation_t * animation);
int sg_animate_init(sg_animation_t * animation,
		u8 type,
		u8 path,
		u8 step_total,
		sg_size_t motion_total,
		sg_point_t start,
		sg_dim_t dim);


/*! \brief Rotation values */
enum Rotation {
	SG_ROT0 /*! Zero degress */,
	SG_ROT90 /*! 90 degress */,
	SG_ROT180 /*! 180 degress */,
	SG_ROT270 /*! 270 degress */,
	SG_ROT360 /*! 360 degress */,
	SG_ROT_SCALE = 65536
};


sg_bmap_data_t sg_op_set(sg_bmap_data_t current, sg_bmap_data_t input);
sg_bmap_data_t sg_op_clr(sg_bmap_data_t current, sg_bmap_data_t input);
sg_bmap_data_t sg_op_inv(sg_bmap_data_t current, sg_bmap_data_t input);
sg_bmap_data_t sg_op_assign(sg_bmap_data_t current, sg_bmap_data_t input);

/*! \brief Set the pixels of a bitmap.
 * \details This method sets a bitmap on to the
 * current bitmap.  It only has bit level positioning but is
 * slower than the copy() method
 *
 * @param bitmap The bitmap to set
 * @param x The x location to start copying
 * @param y The y location to start copying
 * @param pos true to set pixels and false to clear them
 * @return Zero on success
 */
int sg_set_bitmap(sg_bmap_t * mg, const sg_bmap_t * bitmap, sg_point_t p);
int sg_set_bitmap_area(sg_bmap_t * mg, sg_point_t dest, const sg_bmap_t * bitmap, sg_point_t p_src, sg_dim_t d);
int sg_clr_bitmap(sg_bmap_t * mg, const sg_bmap_t * bitmap, sg_point_t p);
int sg_clr_bitmap_area(sg_bmap_t * mg, sg_point_t dest, const sg_bmap_t * bitmap, sg_point_t p_src, sg_dim_t d);
int sg_inv_bitmap(sg_bmap_t * mg, const sg_bmap_t * bitmap, sg_point_t p);
int sg_inv_bitmap_area(sg_bmap_t * mg, sg_point_t dest, const sg_bmap_t * bitmap, sg_point_t p_src, sg_dim_t d);
int sg_assign_bitmap(sg_bmap_t * mg, const sg_bmap_t * bitmap, sg_point_t p);
int sg_assign_bitmap_area(sg_bmap_t * mg, sg_point_t dest, const sg_bmap_t * bitmap, sg_point_t p_src, sg_dim_t d);


u8 sg_tst_hline(const sg_bmap_t * mg, sg_int_t xmin, sg_int_t xmax, sg_int_t y, sg_int_t * pos);
void sg_tst_hedge(const sg_bmap_t * mg, sg_point_t p, sg_int_t * xmin, sg_int_t * xmax);

void sg_set_vline(sg_bmap_t * mg, sg_int_t x, sg_int_t ymin, sg_int_t ymax, sg_size_t thickness);
void sg_set_hline(sg_bmap_t * mg, sg_int_t xmin, sg_int_t xmax, sg_int_t y, sg_size_t thickness);
void sg_clr_vline(sg_bmap_t * mg, sg_int_t x, sg_int_t ymin, sg_int_t ymax, sg_size_t thickness);
void sg_clr_hline(sg_bmap_t * mg, sg_int_t xmin, sg_int_t xmax, sg_int_t y, sg_size_t thickness);
void sg_inv_vline(sg_bmap_t * mg, sg_int_t x, sg_int_t ymin, sg_int_t ymax, sg_size_t thickness);
void sg_inv_hline(sg_bmap_t * mg, sg_int_t xmin, sg_int_t xmax, sg_int_t y, sg_size_t thickness);
void sg_set_line(sg_bmap_t * mg, sg_point_t p1, sg_point_t p2, sg_size_t thickness);
void sg_clr_line(sg_bmap_t * mg, sg_point_t p1, sg_point_t p2, sg_size_t thickness);
void sg_inv_line(sg_bmap_t * mg, sg_point_t p1, sg_point_t p2, sg_size_t thickness);

u8 sg_tst_pixel(const sg_bmap_t * mg, sg_point_t p);
u8 sg_tst_bounded_pixel(const sg_bmap_t * mg, sg_point_t p);

void sg_set_pixel(sg_bmap_t * mg, sg_point_t p);
void sg_set_bounded_pixel(sg_bmap_t * mg, sg_point_t p);
void sg_inv_pixel(sg_bmap_t * mg, sg_point_t p);
void sg_inv_bounded_pixel(sg_bmap_t * mg, sg_point_t p);
void sg_clr_pixel(sg_bmap_t * mg, sg_point_t p);
void sg_clr_bounded_pixel(sg_bmap_t * mg, sg_point_t p);

void sg_fill(sg_bmap_t * mg, sg_bmap_data_t v, sg_int_t start, sg_size_t h);
void sg_set_area(sg_bmap_t * mg, sg_point_t p, sg_dim_t d, sg_bmap_data_t v);
void sg_clr_area(sg_bmap_t * mg, sg_point_t p, sg_dim_t d, sg_bmap_data_t v);
void sg_inv_area(sg_bmap_t * mg, sg_point_t p, sg_dim_t d, sg_bmap_data_t v);

void sg_pour(sg_bmap_t * mg, sg_point_t p);

//drawing
void sg_draw(sg_bmap_t * bitmap, const sg_icon_primitive_t * prim, const sg_map_t * map, sg_bounds_t * attr);
void sg_draw_list(sg_bmap_t * bitmap, const sg_icon_primitive_t prim_list[], unsigned int total, const sg_map_t * map, sg_bounds_t * attr);
void sg_draw_icon(sg_bmap_t * bitmap, const sg_icon_t * icon, const sg_map_t * map, sg_bounds_t * attr);

static inline sg_dim_t sg_draw_attr_dim(const sg_bounds_t * attr){
	sg_dim_t d;
	d.w = attr->bottom_right.x - attr->top_left.x;
	d.h = attr->bottom_right.y - attr->top_left.y;
	return d;
}

static inline sg_point_t sg_draw_attr_center(const sg_bounds_t * attr){
	sg_point_t p;
	p.x = (attr->bottom_right.x + attr->top_left.x)/2;
	p.y = (attr->bottom_right.y + attr->top_left.y)/2;
	return p;
}

static inline u8 sg_y_visible(const sg_bmap_t * mg, sg_int_t y){
	if( y < 0  ) return 0;
	if( y >= mg->dim.h) return 0;
	return 1;
}

static inline u8 sg_x_visible(const sg_bmap_t * mg, sg_int_t x){
	if( x < 0  ) return 0;
	if( x >= mg->dim.w) return 0;
	return 1;
}

static inline u8 sg_point_visible(const sg_bmap_t * mg, sg_point_t p){
	if( p.x < 0 ) return 0;
	if( p.y < 0 ) return 0;
	if( p.x >= mg->dim.w ) return 0;
	if( p.y >= mg->dim.h ) return 0;
	return 1;
}

//this needs to be based on 32 bit data so it's faster
static inline int sg_offset(sg_point_t p, sg_size_t columns) {
	return (p.x/8) + p.y*(columns);
}

//this needs to be based on 32 bit data so it's faster
static inline  sg_bmap_data_t sg_mask(sg_int_t x){
	return ( 0x80 >> (x&0x07) );
}


//point operations
static inline sg_point_t sg_point(sg_int_t x, sg_int_t y){
	sg_point_t p;
	p.x = x;
	p.y = y;
	return p;
}

static inline sg_dim_t sg_dim(sg_size_t w, sg_size_t h){
	sg_dim_t d;
	d.w = w;
	d.h = h;
	return d;
}

void sg_point_set(sg_point_t * d, sg_point_t p);
void sg_point_map(sg_point_t * d, const sg_map_t * m);
sg_size_t sg_point_map_pixel_size(const sg_map_t * m);
void sg_point_add(sg_point_t * d, const sg_point_t * a);
void sg_point_subtract(sg_point_t * d, const sg_point_t * a);
void sg_point_arc(sg_point_t * d, sg_size_t rx, sg_size_t ry, i16 angle);
void sg_point_rotate(sg_point_t * d, i16 angle);
void sg_point_scale(sg_point_t * d, u16 a);
void sg_point_shift(sg_point_t * d, sg_point_t p);
void sg_point_shift_x(sg_point_t * d, sg_int_t a);
void sg_point_shift_y(sg_point_t * d, sg_int_t a);

static inline sg_point_t sg_point_origin(){ sg_point_t p; p.point = 0; return p; }

typedef struct {

} sg_lib_t;

#if defined __cplusplus
}
#endif

#endif /* SG_H_ */
