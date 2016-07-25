/*! \file */ //Copyright 2011-2016 Tyler Gilbert; All Rights Reserved

#ifndef SG_H_
#define SG_H_

#include <mcu/types.h>
#include "sg_types.h"

#if defined __cplusplus
extern "C" {
#endif


/*! \addtogroup BMAP Bitmap Management
 * @{
 */

/*! \details Change effective size without free/alloc sequence */
void sg_set_data(sg_bmap_t * bmap, sg_bmap_data_t * mem, sg_size_t w, sg_size_t h);

/*! \details This function returns a pointer to the sg_bmap_data_t at point \a p */
sg_bmap_data_t * sg_data(const sg_bmap_t * bmap, sg_point_t p);

int sg_set_size(sg_bmap_t * bmap, sg_size_t w, sg_size_t h, sg_size_t offset);
size_t sg_calc_size(sg_int_t w, sg_int_t h);

/*! \details Returns the size of the bitmap data in bytes */
size_t sg_size(const sg_bmap_t * bmap);

/*! \details Returns the byte width of the bitmap */
size_t sg_byte_width(const sg_bmap_t * bmap);

/*! \details Returns the 32-bit word width of the bitmap */
size_t sg_word_width(const sg_bmap_t * bmap);
size_t sg_calc_byte_width(sg_size_t w);
size_t sg_calc_word_width(sg_size_t w);
sg_int_t sg_x_max(const sg_bmap_t * bmap);
sg_int_t sg_y_max(const sg_bmap_t * bmap);
void sg_show(const sg_bmap_t * bmap);

static inline sg_size_t sg_margin_left(const sg_bmap_t * bmap){ return bmap->margin_top_left.w; }
static inline sg_size_t sg_margin_right(const sg_bmap_t * bmap){ return bmap->margin_bottom_right.w; }
static inline sg_size_t sg_margin_top(const sg_bmap_t * bmap){ return bmap->margin_top_left.h; }
static inline sg_size_t sg_margin_bottom(const sg_bmap_t * bmap){ return bmap->margin_bottom_right.h; }

/*! \details Returns the bitmap height in pixels */
static inline sg_size_t sg_h(sg_bmap_t * bmap){ return bmap->dim.h; }
/*! \details Returns the bitmap width in pixels */
static inline sg_size_t sg_w(sg_bmap_t * bmap){ return bmap->dim.w; }
/*! \details Returns the number of byte columns of the bitmap */
static inline sg_size_t sg_cols(sg_bmap_t * bmap){ return bmap->columns; }

/*! \details Ensures point \a p is in the bitmap \a bmap */
void sg_bound(const sg_bmap_t * bmap, sg_point_t * p);
void sg_bound_x(const sg_bmap_t * bmap, sg_int_t * x);
void sg_bound_y(const sg_bmap_t * bmap, sg_int_t * y);

/*! @} */

/*! \addtogroup BMAPOP Bitmap Operations
 * @{
 */

/*! \details Flip the x axis of the bitmap (horizontal mirror) */
void sg_flip_x(sg_bmap_t * bmap);
/*! \details Flip the y axis of the bitmap (vertical mirror) */
void sg_flip_y(sg_bmap_t * bmap);
/*! \details Flip both axes of the bitmap (horizontal and vertical mirror) */
void sg_flip_xy(sg_bmap_t * bmap);

void sg_shift_right(sg_bmap_t * bmap, int count, sg_point_t start, sg_dim_t d);
void sg_shift_left(sg_bmap_t * bmap, int count, sg_point_t start, sg_dim_t d);
void sg_shift_up(sg_bmap_t * bmap, int count, sg_point_t start, sg_dim_t d);
void sg_shift_down(sg_bmap_t * bmap, int count, sg_point_t start, sg_dim_t d);

/*! \details This function sets the pixels in a bitmap
 * based on the pixels of the source bitmap
 *
 * @param bmap The destination bitmap
 * @param bitmap The source bitmap
 * @param p The point in the destination bitmap of the top left corner of \a bitmap
 * @return Zero on success
 */
int sg_set_bitmap(sg_bmap_t * bmap, const sg_bmap_t * bitmap, sg_point_t p);

/*! \details This function sets the pixels in a bitmap
 * based on an area of the source bitmap
 *
 * @param bmap The destination bitmap
 * @param dest The point in the destination bitmap to start setting pixels
 * @param bitmap The source bitmap
 * @param p_src The top left corner of the source bitmap to copy
 * @param d The dimensions of the area to copy
 * @return Zero on success
 */
int sg_set_bitmap_area(sg_bmap_t * bmap, sg_point_t dest, const sg_bmap_t * bitmap, sg_point_t p_src, sg_dim_t d);

/*! \details This funciont clears a bitmap.  See sg_set_bitmap() for details.
 */
int sg_clr_bitmap(sg_bmap_t * bmap, const sg_bmap_t * bitmap, sg_point_t p);

/*! \details This function clears an area see sg_set_bitmap_area() for details
 */
int sg_clr_bitmap_area(sg_bmap_t * bmap, sg_point_t dest, const sg_bmap_t * bitmap, sg_point_t p_src, sg_dim_t d);

/*! \details This function inverts a bitmap.  See sg_set_bitmap() for details.
 */
int sg_inv_bitmap(sg_bmap_t * bmap, const sg_bmap_t * bitmap, sg_point_t p);

/*! \details This function inverts an area see sg_set_bitmap_area() for details
 */
int sg_inv_bitmap_area(sg_bmap_t * bmap, sg_point_t dest, const sg_bmap_t * bitmap, sg_point_t p_src, sg_dim_t d);

/*! \details This function assigns a bitmap.  See sg_set_bitmap() for details.
 */
int sg_assign_bitmap(sg_bmap_t * bmap, const sg_bmap_t * bitmap, sg_point_t p);

/*! \details This function assigns an area see sg_set_bitmap_area() for details
 */
int sg_assign_bitmap_area(sg_bmap_t * bmap, sg_point_t dest, const sg_bmap_t * bitmap, sg_point_t p_src, sg_dim_t d);


/*! @} */


/*! \addtogroup ANIMATION Graphics Animations
 * @{
 */

int sg_animate(sg_bmap_t * bmap, sg_bmap_t * bitmap, sg_animation_t * animation);
int sg_animate_init(sg_animation_t * animation,
		u8 type,
		u8 path,
		u8 step_total,
		sg_size_t motion_total,
		sg_point_t start,
		sg_dim_t dim);

/*! @} */


/*! \brief Rotation values */
enum Rotation {
	SG_ROT0 /*! Zero degress */,
	SG_ROT90 /*! 90 degress */,
	SG_ROT180 /*! 180 degress */,
	SG_ROT270 /*! 270 degress */,
	SG_ROT360 /*! 360 degress */,
	SG_ROT_SCALE = 65536
};

/*! \addtogroup BMAPPRIMOP Bitmap Primitive Operations
 * @{
 */

sg_bmap_data_t sg_op_set(sg_bmap_data_t current, sg_bmap_data_t input);
sg_bmap_data_t sg_op_clr(sg_bmap_data_t current, sg_bmap_data_t input);
sg_bmap_data_t sg_op_inv(sg_bmap_data_t current, sg_bmap_data_t input);
sg_bmap_data_t sg_op_assign(sg_bmap_data_t current, sg_bmap_data_t input);


u8 sg_tst_hline(const sg_bmap_t * bmap, sg_int_t xmin, sg_int_t xmax, sg_int_t y, sg_int_t * pos, u8 active);
void sg_tst_hedge(const sg_bmap_t * bmap, sg_point_t p, sg_int_t * xmin, sg_int_t * xmax, u8 active);

void sg_set_vline(sg_bmap_t * bmap, sg_int_t x, sg_int_t ymin, sg_int_t ymax, sg_size_t thickness);
void sg_set_hline(sg_bmap_t * bmap, sg_int_t xmin, sg_int_t xmax, sg_int_t y, sg_size_t thickness);
void sg_clr_vline(sg_bmap_t * bmap, sg_int_t x, sg_int_t ymin, sg_int_t ymax, sg_size_t thickness);
void sg_clr_hline(sg_bmap_t * bmap, sg_int_t xmin, sg_int_t xmax, sg_int_t y, sg_size_t thickness);
void sg_inv_vline(sg_bmap_t * bmap, sg_int_t x, sg_int_t ymin, sg_int_t ymax, sg_size_t thickness);
void sg_inv_hline(sg_bmap_t * bmap, sg_int_t xmin, sg_int_t xmax, sg_int_t y, sg_size_t thickness);
void sg_set_line(sg_bmap_t * bmap, sg_point_t p1, sg_point_t p2, sg_size_t thickness);
void sg_clr_line(sg_bmap_t * bmap, sg_point_t p1, sg_point_t p2, sg_size_t thickness);
void sg_inv_line(sg_bmap_t * bmap, sg_point_t p1, sg_point_t p2, sg_size_t thickness);

u8 sg_tst_pixel(const sg_bmap_t * bmap, sg_point_t p);
u8 sg_tst_bounded_pixel(const sg_bmap_t * bmap, sg_point_t p);

void sg_set_pixel(sg_bmap_t * bmap, sg_point_t p);
void sg_set_bounded_pixel(sg_bmap_t * bmap, sg_point_t p);
void sg_inv_pixel(sg_bmap_t * bmap, sg_point_t p);
void sg_inv_bounded_pixel(sg_bmap_t * bmap, sg_point_t p);
void sg_clr_pixel(sg_bmap_t * bmap, sg_point_t p);
void sg_clr_bounded_pixel(sg_bmap_t * bmap, sg_point_t p);

void sg_fill(sg_bmap_t * bmap, sg_bmap_data_t v, sg_int_t start, sg_size_t h);
void sg_set_area(sg_bmap_t * bmap, sg_point_t p, sg_dim_t d, sg_bmap_data_t v);
void sg_clr_area(sg_bmap_t * bmap, sg_point_t p, sg_dim_t d, sg_bmap_data_t v);
void sg_inv_area(sg_bmap_t * bmap, sg_point_t p, sg_dim_t d, sg_bmap_data_t v);

void sg_pour(sg_bmap_t * bmap, sg_point_t p, const sg_pen_t * pen);

/*! @} */


/*! \addtogroup BMAPDRAW Draw Scalable Bitmap Graphics
 * @{
 */

//drawing
void sg_draw(sg_bmap_t * bitmap, const sg_icon_primitive_t * prim, const sg_map_t * map, sg_bounds_t * bounds);
void sg_draw_list(sg_bmap_t * bitmap, const sg_icon_primitive_t prim_list[], unsigned int total, const sg_map_t * map, sg_bounds_t * bounds);
void sg_draw_icon(sg_bmap_t * bitmap, const sg_icon_t * icon, const sg_map_t * map, sg_bounds_t * bounds);

static inline sg_dim_t sg_draw_attr_dim(const sg_bounds_t * bounds){
	sg_dim_t d;
	d.w = bounds->bottom_right.x - bounds->top_left.x;
	d.h = bounds->bottom_right.y - bounds->top_left.y;
	return d;
}

static inline sg_point_t sg_draw_attr_center(const sg_bounds_t * bounds){
	sg_point_t p;
	p.x = (bounds->bottom_right.x + bounds->top_left.x)/2;
	p.y = (bounds->bottom_right.y + bounds->top_left.y)/2;
	return p;
}

/*! @} */


/*! \addtogroup POINTDIM Point and Dimensions
 * @{
 */

static inline u8 sg_y_visible(const sg_bmap_t * bmap, sg_int_t y){
	if( y < 0  ) return 0;
	if( y >= bmap->dim.h) return 0;
	return 1;
}

static inline u8 sg_x_visible(const sg_bmap_t * bmap, sg_int_t x){
	if( x < 0  ) return 0;
	if( x >= bmap->dim.w) return 0;
	return 1;
}

static inline u8 sg_point_visible(const sg_bmap_t * bmap, sg_point_t p){
	if( p.x < 0 ) return 0;
	if( p.y < 0 ) return 0;
	if( p.x >= bmap->dim.w ) return 0;
	if( p.y >= bmap->dim.h ) return 0;
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


/*! @} */


#if defined __cplusplus
}
#endif

#endif /* SG_H_ */
