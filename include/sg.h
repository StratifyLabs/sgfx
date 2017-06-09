/*! \file */ //Copyright 2011-2016 Tyler Gilbert; All Rights Reserved

#ifndef SG_H_
#define SG_H_

#include <string.h>
#include <mcu/types.h>
#include "sg_types.h"

#if defined __cplusplus
extern "C" {
#endif



/*! \mainpage
 *
 *  #Welcome to the Stratify Graphics Page
 *
 *  The stratify graphics library is designed for working with small displays.
 *
 *  - Bitmap memory management
 * 	- Draw vector graphics and bitmaps using a pen.
 * 	- Pixel manipulation
 *
 *
 *
 *
 */

/*! \addtogroup BMAP Bitmap Management
 * @{
 */

/*! \details Change effective size without free/alloc sequence */
void sg_bmap_set_data(sg_bmap_t * bmap, sg_bmap_data_t * mem, sg_dim_t dim);
sg_bmap_data_t * sg_bmap_data(const sg_bmap_t * bmap, sg_point_t p);
size_t sg_calc_bmap_size(sg_dim_t dim);

void sg_bmap_show(const sg_bmap_t * bmap);
static inline void sg_bmap_copy(sg_bmap_t * dest, const sg_bmap_t * src){ memcpy(dest, src, sizeof(sg_bmap_t)); }

static inline sg_size_t sg_bmap_margin_left(const sg_bmap_t * bmap){ return bmap->margin_top_left.w; }
static inline sg_size_t sg_bmap_margin_right(const sg_bmap_t * bmap){ return bmap->margin_bottom_right.w; }
static inline sg_size_t sg_bmap_margin_top(const sg_bmap_t * bmap){ return bmap->margin_top_left.h; }
static inline sg_size_t sg_bmap_margin_bottom(const sg_bmap_t * bmap){ return bmap->margin_bottom_right.h; }
static inline sg_int_t sg_bmap_x_max(const sg_bmap_t * bmap){ return bmap->dim.w -1; }
static inline sg_int_t sg_bmap_y_max(const sg_bmap_t * bmap){ return bmap->dim.h -1; }
static inline sg_size_t sg_bmap_h(const sg_bmap_t * bmap){ return bmap->dim.h; }
static inline sg_size_t sg_bmap_w(const sg_bmap_t * bmap){ return bmap->dim.w; }
static inline sg_size_t sg_bmap_cols(const sg_bmap_t * bmap){ return bmap->columns; }


/*! @} */

/*! \addtogroup BMAPOP Bitmap Operations
 * @{
 */

/*! \details Flip the x axis of the bitmap (horizontal mirror) */
void sg_transform_flip_x(const sg_bmap_t * bmap);
/*! \details Flip the y axis of the bitmap (vertical mirror) */
void sg_transform_flip_y(const sg_bmap_t * bmap);
/*! \details Flip both axes of the bitmap (horizontal and vertical mirror) */
void sg_transform_flip_xy(const sg_bmap_t * bmap);


void sg_transform_shift(const sg_bmap_t * bmap, sg_point_t shift, sg_point_t p, sg_dim_t d);



/*! @} */

/*! \addtogroup POINTDIM Point and Dimensions
 * @{
 */

/*! \brief Rotation values */
enum Rotation {
	SG_ROT0 /*! Zero degress */,
	SG_ROT90 /*! 90 degress */,
	SG_ROT180 /*! 180 degress */,
	SG_ROT270 /*! 270 degress */,
	SG_ROT360 /*! 360 degress */,
	SG_ROT_SCALE = 65536
};

static inline sg_dim_t sg_point_bounds_dim(const sg_bounds_t * bounds){
	sg_dim_t d;
	d.w = bounds->bottom_right.x - bounds->top_left.x;
	d.h = bounds->bottom_right.y - bounds->top_left.y;
	return d;
}

static inline sg_point_t sg_point_bounds_center(const sg_bounds_t * bounds){
	sg_point_t p;
	p.x = (bounds->bottom_right.x + bounds->top_left.x)/2;
	p.y = (bounds->bottom_right.y + bounds->top_left.y)/2;
	return p;
}

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

static inline sg_point_t sg_point_origin(){ sg_point_t p; p.point = 0; return p; }


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
/*! \details Ensures point \a p is in the bitmap \a bmap */
void sg_point_bound(const sg_bmap_t * bmap, sg_point_t * p);
void sg_point_bound_x(const sg_bmap_t * bmap, sg_int_t * x);
void sg_point_bound_y(const sg_bmap_t * bmap, sg_int_t * y);

/*! @} */

/*! \addtogroup CURSOR Cursor Operations
 * @{
 */

static inline void sg_cursor_copy(sg_cursor_t * dest, const sg_cursor_t * src){ memcpy(dest, src, sizeof(sg_cursor_t)); }

void sg_cursor_set(sg_cursor_t * cursor, const sg_bmap_t * bmap, sg_point_t p);
void sg_cursor_inc_x(sg_cursor_t * cursor);
void sg_cursor_dec_x(sg_cursor_t * cursor);
void sg_cursor_inc_y(sg_cursor_t * cursor);
void sg_cursor_dec_y(sg_cursor_t * cursor);
sg_color_t sg_cursor_get_pixel(sg_cursor_t * cursor);
void sg_cursor_draw_pixel(sg_cursor_t * cursor);
void sg_cursor_draw_hline(sg_cursor_t * cursor, sg_size_t width);
void sg_cursor_invert_hline(sg_cursor_t * cursor, sg_size_t width);
void sg_cursor_clear_hline(sg_cursor_t * cursor, sg_size_t width);
void sg_cursor_draw_cursor(sg_cursor_t * dest_cursor, const sg_cursor_t * src_cursor, sg_size_t width);
void sg_cursor_draw_pattern(sg_cursor_t * cursor, sg_size_t width, sg_bmap_data_t pattern);
void sg_cursor_shift_right(sg_cursor_t * cursor, sg_size_t shift_width, sg_size_t shift_distance);
void sg_cursor_shift_left(sg_cursor_t * cursor, sg_size_t shift_width, sg_size_t shift_distance);

/*! @} */


/*! \addtogroup BMAPPRIMOP Bitmap Drawing Operations
 * @{
 */


//the above functions need to be replaced with -- the functions need to support 1-bit, 2-bit, 4-bit, 8-bit, 16-bit, 24-bit, and 32-bit colors
sg_color_t sg_get_pixel(const sg_bmap_t * bmap, sg_point_t p);
void sg_draw_pixel(const sg_bmap_t * bmap, sg_point_t p);
void sg_draw_line(const sg_bmap_t * bmap, sg_point_t p1, sg_point_t p2);
void sg_draw_quadtratic_bezier(const sg_bmap_t * bmap, sg_point_t p1, sg_point_t p2, sg_point_t p3);
void sg_draw_cubic_bezier(const sg_bmap_t * bmap, sg_point_t p1, sg_point_t p2, sg_point_t p3, sg_point_t p4);
void sg_draw_rectangle(const sg_bmap_t * bmap, sg_point_t p, sg_dim_t d);
void sg_invert_rectangle(const sg_bmap_t * bmap, sg_point_t p, sg_dim_t d);
void sg_clear_rectangle(const sg_bmap_t * bmap, sg_point_t p, sg_dim_t d);
void sg_draw_pour(const sg_bmap_t * bmap, sg_point_t p);
void sg_draw_pattern(const sg_bmap_t * bmap, sg_point_t p, sg_dim_t d, sg_bmap_data_t odd_pattern, sg_bmap_data_t even_pattern, sg_size_t pattern_height);


/*! \details This function sets the pixels in a bitmap
 * based on the pixels of the source bitmap
 *
 * @param bmap_dest The destination bitmap
 * @param p_dest The point in the destination bitmap of the top left corner of \a bitmap
 * @param bmap_src The source bitmap
 * @return Zero on success
 */
void sg_draw_bitmap(const sg_bmap_t * bmap_dest, sg_point_t p_dest, const sg_bmap_t * bmap_src);

/*! \details This function draws a subset of
 * the source bitmap on the destination bitmap.
 *
 * @param bmap_dest The destination bitmap
 * @param p_dest The point in the destination bitmap to start setting pixels
 * @param bmap_src The source bitmap
 * @param p_src The top left corner of the source bitmap to copy
 * @param d_src The dimensions of the area to copy
 * @return Zero on success
 */
void sg_draw_sub_bitmap(const sg_bmap_t * bmap_dest, sg_point_t p_dest, const sg_bmap_t * bmap_src, sg_point_t p_src, sg_dim_t d_src);

/*! @} */


/*! \addtogroup BMAPDRAW Draw Scalable Bitmap Graphics
 * @{
 */

//drawing
void sg_vector_draw_primitive(sg_bmap_t * bitmap, const sg_icon_primitive_t * prim, const sg_map_t * map, sg_bounds_t * bounds);
void sg_vector_draw_primitive_list(sg_bmap_t * bitmap, const sg_icon_primitive_t prim_list[], unsigned int total, const sg_map_t * map, sg_bounds_t * bounds);
void sg_vector_draw_icon(sg_bmap_t * bitmap, const sg_icon_t * icon, const sg_map_t * map, sg_bounds_t * bounds);


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


typedef struct MCU_PACK {
	u32 version;
	sg_size_t bits_per_pixel;
	void (*bmap_set_data)(sg_bmap_t * bmap, sg_bmap_data_t * mem, sg_dim_t dim);
	sg_bmap_data_t * (*bmap_data)(const sg_bmap_t * bmap, sg_point_t p);
	size_t (*calc_bmap_size)(sg_dim_t dim);

	void (*point_set)(sg_point_t * d, sg_point_t p);
	void (*point_map)(sg_point_t * d, const sg_map_t * m);
	sg_size_t (*point_map_pixel_size)(const sg_map_t * m);
	void (*point_add)(sg_point_t * d, const sg_point_t * a);
	void (*point_subtract)(sg_point_t * d, const sg_point_t * a);
	void (*point_arc)(sg_point_t * d, sg_size_t rx, sg_size_t ry, i16 angle);
	void (*point_rotate)(sg_point_t * d, i16 angle);
	void (*point_scale)(sg_point_t * d, u16 a);
	void (*point_shift)(sg_point_t * d, sg_point_t p);
	void (*point_shift_x)(sg_point_t * d, sg_int_t a);
	void (*point_shift_y)(sg_point_t * d, sg_int_t a);
	void (*point_bound)(const sg_bmap_t * bmap, sg_point_t * p);
	void (*point_bound_x)(const sg_bmap_t * bmap, sg_int_t * x);
	void (*point_bound_y)(const sg_bmap_t * bmap, sg_int_t * y);

	void (*transform_flip_x)(const sg_bmap_t * bmap);
	void (*transform_flip_y)(const sg_bmap_t * bmap);
	void (*transform_flip_xy)(const sg_bmap_t * bmap);
	void (*transform_shift)(const sg_bmap_t * bmap, sg_point_t shift, sg_point_t p, sg_dim_t d);

	void (*cursor_set)(sg_cursor_t * cursor, const sg_bmap_t * bmap, sg_point_t p);
	void (*cursor_inc_x)(sg_cursor_t * cursor);
	void (*cursor_dec_x)(sg_cursor_t * cursor);
	void (*cursor_inc_y)(sg_cursor_t * cursor);
	void (*cursor_dec_y)(sg_cursor_t * cursor);
	sg_color_t (*cursor_get_pixel)(sg_cursor_t * cursor);
	void (*cursor_draw_pixel)(sg_cursor_t * cursor);
	void (*cursor_draw_hline)(sg_cursor_t * cursor, sg_size_t width);
	void (*cursor_invert_hline)(sg_cursor_t * cursor, sg_size_t width);
	void (*cursor_clear_hline)(sg_cursor_t * cursor, sg_size_t width);
	void (*cursor_draw_cursor)(sg_cursor_t * dest_cursor, const sg_cursor_t * src_cursor, sg_size_t width);
	void (*cursor_draw_pattern)(sg_cursor_t * cursor, sg_size_t width, sg_bmap_data_t pattern);
	void (*cursor_shift_right)(sg_cursor_t * cursor, sg_size_t shift_width, sg_size_t shift_distance);
	void (*cursor_shift_left)(sg_cursor_t * cursor, sg_size_t shift_width, sg_size_t shift_distance);

	sg_color_t (*get_pixel)(const sg_bmap_t * bmap, sg_point_t p);
	void (*draw_pixel)(const sg_bmap_t * bmap, sg_point_t p);
	void (*draw_line)(const sg_bmap_t * bmap, sg_point_t p1, sg_point_t p2);
	void (*draw_quadtratic_bezier)(const sg_bmap_t * bmap, sg_point_t p1, sg_point_t p2, sg_point_t p3);
	void (*draw_cubic_bezier)(const sg_bmap_t * bmap, sg_point_t p1, sg_point_t p2, sg_point_t p3, sg_point_t p4);
	void (*draw_rectangle)(const sg_bmap_t * bmap, sg_point_t p, sg_dim_t d);
	void (*invert_rectangle)(const sg_bmap_t * bmap, sg_point_t p, sg_dim_t d);
	void (*clear_rectangle)(const sg_bmap_t * bmap, sg_point_t p, sg_dim_t d);
	void (*draw_pour)(const sg_bmap_t * bmap, sg_point_t p);
	void (*draw_pattern)(const sg_bmap_t * bmap, sg_point_t p, sg_dim_t d, sg_bmap_data_t odd_pattern, sg_bmap_data_t even_pattern, sg_size_t pattern_height);
	void (*draw_bitmap)(const sg_bmap_t * bmap_dest, sg_point_t p_dest, const sg_bmap_t * bmap_src);
	void (*draw_sub_bitmap)(const sg_bmap_t * bmap_dest, sg_point_t p_dest, const sg_bmap_t * bmap_src, sg_point_t p_src, sg_dim_t d_src);

	void (*vector_draw_primitive)(sg_bmap_t * bitmap, const sg_icon_primitive_t * prim, const sg_map_t * map, sg_bounds_t * bounds);
	void (*vector_draw_primitive_list)(sg_bmap_t * bitmap, const sg_icon_primitive_t prim_list[], unsigned int total, const sg_map_t * map, sg_bounds_t * bounds);
	void (*vector_draw_icon)(sg_bmap_t * bitmap, const sg_icon_t * icon, const sg_map_t * map, sg_bounds_t * bounds);

	int (*animate)(sg_bmap_t * bmap, sg_bmap_t * bitmap, sg_animation_t * animation);
	int (*animate_init)(sg_animation_t * animation, u8 type, u8 path, u8 step_total, sg_size_t motion_total, sg_point_t start, sg_dim_t dim);

} sg_api_t;

const sg_api_t * sg_api();




#if defined __cplusplus
}
#endif

#endif /* SG_H_ */
