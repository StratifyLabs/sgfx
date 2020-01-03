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
 *  Introduction
 *  ---------------------------
 *
 *  The stratify graphics library is designed for working with small displays. It stores
 *  bitmaps using 1, 2, 4, or 8 bits per pixel (bpp). The bitmap can then be mapped to a
 *  display using a color palette. This approach allows powerful graphics to run
 *  on devices with very little memory and then to be mapped to color displays.
 *
 *  Let's take an example setup with the following hardware:
 *
 *  - Cortex M3 processor with 64KB of RAM
 *  - 128x128xRGB565 Display
 *
 *  The native display requires 32KB of video memory in order to perform fast graphics operations.
 *  If you are building a multi-color UI than only uses a few colors, you can use the Stratify
 *  Graphics library and represent the UI using 4bpp (16 colors). The UI could be represented in 128*128*4/8 = 8KB
 *  of RAM leaving plenty of RAM for other parts of the application.
 *
 * Features
 * ---------------------------
 *
 * - Compile time configuration of 1, 2, 4, or 8 bits per pixels
 * - Bitmap memory management
 * - Bitmap Transforms
 * - Bitmap Coordinates
 * - Draw vector graphics and bitmaps using a pen
 * - Pixel manipulation
 *
 *
 *
 */

/*! \addtogroup BMAPMGMT Data Management
 * @{
 */

/*! \details Change effective size without free/alloc sequence */
void sg_bmap_set_data(sg_bmap_t * bmap, sg_bmap_data_t * mem, sg_area_t area, u8 bits_per_pixel);
sg_bmap_data_t * sg_bmap_data(const sg_bmap_t * bmap, sg_point_t p);
size_t sg_calc_bmap_size(const sg_bmap_t * bmap, sg_area_t area);

static inline u16 sg_calc_word_width(sg_size_t w){ return (w + 31) >> 5; }


void sg_bmap_show(const sg_bmap_t * bmap);
static inline void sg_bmap_copy(sg_bmap_t * dest, const sg_bmap_t * src){ memcpy(dest, src, sizeof(sg_bmap_t)); }

static inline sg_size_t sg_bmap_margin_left(const sg_bmap_t * bmap){ return bmap->margin_top_left.width; }
static inline sg_size_t sg_bmap_margin_right(const sg_bmap_t * bmap){ return bmap->margin_bottom_right.width; }
static inline sg_size_t sg_bmap_margin_top(const sg_bmap_t * bmap){ return bmap->margin_top_left.height; }
static inline sg_size_t sg_bmap_margin_bottom(const sg_bmap_t * bmap){ return bmap->margin_bottom_right.height; }
static inline sg_int_t sg_bmap_x_max(const sg_bmap_t * bmap){ return bmap->area.width -1; }
static inline sg_int_t sg_bmap_y_max(const sg_bmap_t * bmap){ return bmap->area.height -1; }
static inline sg_size_t sg_bmap_h(const sg_bmap_t * bmap){ return bmap->area.height; }
static inline sg_size_t sg_bmap_w(const sg_bmap_t * bmap){ return bmap->area.width; }
static inline sg_size_t sg_bmap_cols(const sg_bmap_t * bmap){ return bmap->columns; }


/*! @} */

/*! \addtogroup BMAPOP Transforms
 * @{
 */

/*! \details Flip the x axis of the bitmap (horizontal mirror) */
void sg_transform_flip_x(const sg_bmap_t * bmap);
/*! \details Flip the y axis of the bitmap (vertical mirror) */
void sg_transform_flip_y(const sg_bmap_t * bmap);
/*! \details Flip both axes of the bitmap (horizontal and vertical mirror) */
void sg_transform_flip_xy(const sg_bmap_t * bmap);

/*! \details Shifts a bitmap.
 *
 * @param bmap A pointer to the bitmap object
 * @param shift The amount to shift (signed x,y values)
 * @param p The point in \a bmap to start shifting (top left corner)
 * @param d The dimensions of \a bmap to shift
 *
 */
void sg_transform_shift(const sg_bmap_t * bmap, sg_point_t shift, const sg_region_t * region);



/*! @} */

/*! \addtogroup COORD Coordinates
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

static inline sg_point_t sg_point_region_center(const sg_region_t * region){
	sg_point_t p;
	p.x = region->point.x + region->area.width/2;
	p.y = region->point.y + region->area.height/2;
	return p;
}

static inline u8 sg_y_visible(const sg_bmap_t * bmap, sg_int_t y){
	if( y < 0  ) return 0;
	if( y >= bmap->area.height) return 0;
	return 1;
}

static inline u8 sg_x_visible(const sg_bmap_t * bmap, sg_int_t x){
	if( x < 0  ) return 0;
	if( x >= bmap->area.width) return 0;
	return 1;
}

static inline u8 sg_point_visible(const sg_bmap_t * bmap, sg_point_t p){
	if( p.x < 0 ) return 0;
	if( p.y < 0 ) return 0;
	if( p.x >= bmap->area.width ) return 0;
	if( p.y >= bmap->area.height ) return 0;
	return 1;
}


//point operations
static inline sg_point_t sg_point(sg_int_t x, sg_int_t y){
	sg_point_t p;
	p.x = x;
	p.y = y;
	return p;
}

static inline sg_region_t sg_region(sg_point_t point, sg_area_t area){
	sg_region_t region;
	region.point = point;
	region.area = area;
	return region;
}

static inline sg_area_t sg_dim(sg_size_t w, sg_size_t h){
	sg_area_t d;
	d.width = w;
	d.height = h;
	return d;
}

static inline sg_point_t sg_point_origin(){ sg_point_t p; p.point = 0; return p; }


void sg_point_set(sg_point_t * d, sg_point_t p);
void sg_point_map(sg_point_t * d, const sg_vector_map_t * m);
void sg_point_unmap(sg_point_t * d, const sg_vector_map_t * m);
sg_size_t sg_point_map_pixel_size(const sg_vector_map_t * m);
void sg_point_add(sg_point_t * d, const sg_point_t * a);
void sg_point_subtract(sg_point_t * d, const sg_point_t * a);
void sg_point_arc(sg_point_t * d, sg_size_t rx, sg_size_t ry, s16 angle);
void sg_point_rotate(sg_point_t * d, s16 angle);
void sg_point_scale(sg_point_t * d, u16 a);
void sg_point_shift(sg_point_t * d, sg_point_t p);
void sg_point_shift_x(sg_point_t * d, sg_int_t a);
void sg_point_shift_y(sg_point_t * d, sg_int_t a);
/*! \details Ensures point \a p is in the bitmap \a bmap */
void sg_point_bound(const sg_bmap_t * bmap, sg_point_t * p);
void sg_point_bound_x(const sg_bmap_t * bmap, sg_int_t * x);
void sg_point_bound_y(const sg_bmap_t * bmap, sg_int_t * y);

/*! @} */

/*! \addtogroup CURSOR Cursor Drawing
 * @{
 */

/*! \details Copies a cursor from \a src to \a dest.
 *
 * @param dest A pointer to the destination cursor
 * @param src A pointer to the source cursor
 *
 */
static inline void sg_cursor_copy(sg_cursor_t * dest, const sg_cursor_t * src){ memcpy(dest, src, sizeof(sg_cursor_t)); }

/*! \details Sets the value of the cursor to point to \a p in \a bmap.
 *
 * @param cursor A pointer to the cursor
 * @param bmap A pointer to the source bitmap object
 * @param p The point in the \a bmap where the cursor will point
 */
void sg_cursor_set(sg_cursor_t * cursor, const sg_bmap_t * bmap, sg_point_t p);

void sg_cursor_update(sg_cursor_t * cursor, sg_point_t p);

/*! \details Increments the cursor's x value by one.
 *
 * @param cursor A pointer to the cursor
 *
 * This function does not account for the width of bitmap object. If
 * you need to increment through multiple rows try this approach.
 *
 * \code
 *	sg_cursor_t y_cursor;
 *	sg_cursor_t x_cursor;
 *	sg_size_t i,j;
 *	sg_cursor_set(&y_cursor, bmap, p);
 *
 *	for(i=0; i < 10; i++){
 *		sg_cursor_copy(&x_cursor, &y_cursor);
 *		for(j=0; j < 10; j++){
 *
 *			sg_cursor_inc_x(&x_cursor);
 *		}
 *		sg_cursor_inc_y(&y_cursor);
 *	}
 *
 * \endcode
 *
 */
void sg_cursor_inc_x(sg_cursor_t * cursor);

/*! \details Decrements a cursor's x location.
 *
 * @param cursor A pointer to the cursor
 *
 * This function doesn't account for the size of the bmap.
 *
 * \sa sg_cursor_inc_x()
 *
 */
void sg_cursor_dec_x(sg_cursor_t * cursor);

/*! \details Increments a cursor's y location.
 *
 * @param cursor A pointer to the cursor
 *
 * This function doesn't account for the size of the bmap.
 *
 * \sa sg_cursor_inc_x()
 *
 */
void sg_cursor_inc_y(sg_cursor_t * cursor);

/*! \details Decrements a cursor's y location.
 *
 * @param cursor A pointer to the cursor
 *
 * This function doesn't account for the size of the bmap.
 *
 * \sa sg_cursor_inc_x()
 *
 */
void sg_cursor_dec_y(sg_cursor_t * cursor);

/*! \details Returns the value of the pixel at the cursor's location
 * and increments the location of the pixel.
 *
 * @param cursor A pointer to the cursor
 * @return The value of the pixel at the cursor's location (before incrementing)
 */
sg_color_t sg_cursor_get_pixel(sg_cursor_t * cursor);
sg_color_t sg_cursor_get_pixel_increment(sg_cursor_t * cursor, int x, int y);

/*! \details Draws a pixel at the cursor's location
 * and increments the location of the pixel.
 *
 * @param cursor A pointer to the cursor
 */
void sg_cursor_draw_pixel(sg_cursor_t * cursor);

/*! \details Draws a horizontal line at the cursor's location
 * and increments the cursor's position to the pixel after the
 * end of the line.
 *
 * @param cursor A pointer to the cursor
 * @param width The width of the line
 *
 * The cursor will use the containing sg_bmap_t's pen for
 * color and thickness.
 *
 * This function operates on the bmap using 32-bit words
 * so it is faster than simply drawing pixels using a loop.
 *
 */
void sg_cursor_draw_hline(sg_cursor_t * cursor, sg_size_t width);

/*! \details Draws a horizontal line on \a dest_cursor by copying the values of the \a src_cursor
 * and updates the x location of the \a dest_cursor to the pixel past the end of the operation.
 *
 * @param dest_cursor The cursor where pixels will be drawn
 * @param src_cursor The cursor where pixels are copied from
 * @param width The number of pixels to copy
 *
 * This function operates on 32-bit words. It is much faster
 * than a sg_get_pixel()/sg_draw_pixel() loop.
 *
 */
void sg_cursor_draw_cursor(sg_cursor_t * dest_cursor, const sg_cursor_t * src_cursor, sg_size_t width);

/*! \details Draws the specified pattern in a horizontal line at \a cursor.
 *
 * @param cursor The cursor
 * @param width The number of pixels to draw
 * @param pattern The pattern to draw
 *
 * This function operates on 32-bit words and is much faster
 * than a sg_draw_pixel() loop.
 *
 */
void sg_cursor_draw_pattern(sg_cursor_t * cursor, sg_size_t width, sg_bmap_data_t pattern);

/*! \details Shifts the pixels at cursor to the right.
 *
 * @param cursor A pointer to the cursor
 * @param shift_width The number of pixels to shift
 * @param shift_distance The distance to shift \a shift_width pixels
 */
void sg_cursor_shift_right(sg_cursor_t * cursor, sg_size_t shift_width, sg_size_t shift_distance);

/*! \details Shifts the pixels at cursor to the left.
 *
 * @param cursor A pointer to the cursor
 * @param shift_width The number of pixels to shift
 * @param shift_distance The distance to shift \a shift_width pixels
 */
void sg_cursor_shift_left(sg_cursor_t * cursor, sg_size_t shift_width, sg_size_t shift_distance);



sg_int_t sg_cursor_find_positive_edge(sg_cursor_t * cursor, sg_size_t max_distance);
sg_int_t sg_cursor_find_negative_edge(sg_cursor_t * cursor, sg_size_t max_distance);
sg_int_t sg_cursor_find_edge(sg_cursor_t * cursor, sg_color_t current_color, sg_size_t max_distance);

/*! @} */


/*! \addtogroup BMAPPRIMOP Drawing
 * @{
 */


/*! \details Returns the color of the pixel.
 *
 * @param bmap A pointer to the bitmap object
 * @param p The point in \a bmap to get the color
 * @returns The color of the pixel at \a p
 */
sg_color_t sg_get_pixel(const sg_bmap_t * bmap, sg_point_t p);

/*! \details Draws a pixel.
 *
 * @param bmap A pointer to the bitmap
 * @param p Point where to draw the pixel
 *
 * The color of the pixel is determined by the bmap->pen
 * object.
 *
 *
 */
void sg_draw_pixel(const sg_bmap_t * bmap, sg_point_t p);

/*! \details Draws a line on the bitmap.
 *
 * @param bmap A pointer to the bmap
 * @param p1 First point of the line
 * @param p2 Second point of the line
 *
 * The color and thickness of the line are determined
 * by the bmap->pen object.
 *
 */
void sg_draw_line(const sg_bmap_t * bmap, sg_point_t p1, sg_point_t p2);

/*! \details Draws a quadratic bezier curve on the bitmap.
 *
 * @param bmap A pointer to the bitmap object
 * @param p1 First point of the bezier function
 * @param p2 Second point of the bezier function
 * @param p3 Third point of the bezier function
 * @param corners If non-zero, corners[0] will be the top left corner and corners[1] will be the bottom right corner enclosing the curve
 *
 * The color and thickness of the curve are determined by
 * the bmap->pen object.
 *
 */
void sg_draw_quadratic_bezier(const sg_bmap_t * bmap, sg_point_t p1, sg_point_t p2, sg_point_t p3, sg_point_t * corners);

/*! \details Draws a cubic bezier curve on the bitmap.
 *
 * @param bmap A pointer to the bitmap object
 * @param p1 First point of the bezier function
 * @param p2 Second point of the bezier function
 * @param p3 Third point of the bezier function
 * @param p4 Third point of the bezier function
 * @param corners If non-zero, corners[0] will be the top left corner and corners[1] will be the bottom right corner enclosing the curve
 *
 * The color and thickness of the curve are determined by
 * the bmap->pen object.
 *
 */
void sg_draw_cubic_bezier(const sg_bmap_t * bmap, sg_point_t p1, sg_point_t p2, sg_point_t p3, sg_point_t p4, sg_point_t * corners);

/*! \details Draws a rectangle.
 *
 * @param bmap A pointer to the bitmap object
 * @param p The top left corner of the rectangle
 * @param d The dimensions of the rectangle
 *
 * The color of the rectangle is determined by the bmap->pen
 * object.
 *
 */
void sg_draw_rectangle(const sg_bmap_t * bmap, const sg_region_t * region);


/*! \details Draws an arc on the bitmap.
 *
 * @param bmap A pointer to the bitmap
 * @param region Specifies the region to draw the ellipse in assuming the rotation is zero
 * @param start The starting angle
 * @param end The ending angle
 * @param rotation A rotation angle that is applied to every point in the arc
 * @param corners If non-zero, corners[0] will be the top left corner and corners[1] will be the bottom right corner enclosing the curve
 *
 */
void sg_draw_arc(const sg_bmap_t * bmap, const sg_region_t * region, s16 start, s16 end, s16 rotation, sg_point_t * corners);


/*! \details Pours a color on the bitmap.
 *
 * @param bmap A pointer to the bitmap object
 * @param p The point where the pour should start.
 * @param bounds The bounds for the pour.
 *
 * The pour will fill in the surrounding area until it its
 * a line or a bound. If the bmap's pen color is zero, the pour
 * will act as an eraser pour rather than an ink pour.
 *
 */
void sg_draw_pour(const sg_bmap_t * bmap, sg_point_t p, const sg_region_t * region);


/*! \details Draws a pattern in the specified area of the bitmap.
 *
 * @param bmap A pointer to the bitmap
 * @param p The top left corner of the bitmap where the pattern will start
 * @param d The dimensions of the pattern within the bitmap
 * @param odd_pattern The pattern for odd lines
 * @param even_pattern The pattern for even lines
 * @param pattern_height The height of the pattern lines
 *
 * The \a odd_pattern and \a even_pattern values are bitmask values.
 * A value of 1 will draw the pattern using the bmap->pen color. A
 * zero value will be drawn as color zero.
 *
 */
void sg_draw_pattern(const sg_bmap_t * bmap, const sg_region_t * region, sg_bmap_data_t odd_pattern, sg_bmap_data_t even_pattern, sg_size_t pattern_height);

/*! \details Draws a bitmap on the bitmap.
 * based on the pixels of the source bitmap
 *
 * @param bmap_dest The destination bitmap
 * @param p_dest The point in the destination bitmap of the top left corner of \a bitmap
 * @param bmap_src The source bitmap
 * @return Zero on success
 */
void sg_draw_bitmap(const sg_bmap_t * bmap_dest, sg_point_t p_dest, const sg_bmap_t * bmap_src);

/*! \details Draws a subset of
 * the source bitmap on the destination bitmap.
 *
 * @param bmap_dest The destination bitmap
 * @param p_dest The point in the destination bitmap to start setting pixels
 * @param bmap_src The source bitmap
 * @param p_src The top left corner of the source bitmap to copy
 * @param d_src The dimensions of the area to copy
 * @return Zero on success
 */
void sg_draw_sub_bitmap(const sg_bmap_t * bmap_dest, sg_point_t p_dest, const sg_bmap_t * bmap_src, const sg_region_t * region_src);

/*! @} */


/*! \addtogroup BMAPVECTOR Vector Graphics
 * @{
 */

//drawing
/*! \details Draws a vector path icon on the bitmap.
 *
 * @param bmap The bitmap to draw on
 * @param path The path to draw
 * @param map The map that describes how the path will be drawn on the bitmap
 *
 */
void sg_vector_draw_path(sg_bmap_t * bmap, sg_vector_path_t * path, const sg_vector_map_t * map);


/*! @} */


/*! \addtogroup ANIMATION Animations
 * @{
 */

/*! \details Executes an animation.
 *
 */
int sg_animate(sg_bmap_t * bmap, sg_bmap_t * bitmap, sg_animation_t * animation);

/*! \details Initializes an animation.
 *
 */
int sg_animate_init(sg_animation_t * animation,
		u8 type,
		u8 path,
		u8 step_total,
		sg_size_t motion_total,
		sg_point_t start,
		sg_area_t area);

int sg_antialias_filter_init(
      sg_antialias_filter_t * filter,
      const u8 contrast_data[8]
);

int sg_antialias_filter_apply(
      const sg_bmap_t * bmap,
      sg_antialias_filter_t * filter,
      sg_region_t region
      );

/*! @} */

#define SG_API_T 1

typedef struct MCU_PACK {
	u32 version;
	sg_size_t bits_per_pixel;
    const char * git_hash;
	void (*bmap_set_data)(sg_bmap_t * bmap, sg_bmap_data_t * mem, sg_area_t area, u8 bits_per_pixel);
	sg_bmap_data_t * (*bmap_data)(const sg_bmap_t * bmap, sg_point_t p);
	size_t (*calc_bmap_size)(const sg_bmap_t * bmap, sg_area_t area);

	void (*point_set)(sg_point_t * d, sg_point_t p);
	void (*point_map)(sg_point_t * d, const sg_vector_map_t * m);
	void (*point_unmap)(sg_point_t * d, const sg_vector_map_t * m);
	sg_size_t (*point_map_pixel_size)(const sg_vector_map_t * m);
	void (*point_add)(sg_point_t * d, const sg_point_t * a);
	void (*point_subtract)(sg_point_t * d, const sg_point_t * a);
	void (*point_arc)(sg_point_t * d, sg_size_t rx, sg_size_t ry, s16 angle);
	void (*point_rotate)(sg_point_t * d, s16 angle);
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
	void (*transform_shift)(const sg_bmap_t * bmap, sg_point_t shift, const sg_region_t * region);

	void (*cursor_set)(sg_cursor_t * cursor, const sg_bmap_t * bmap, sg_point_t p);
	void (*cursor_update)(sg_cursor_t * cursor, sg_point_t p);
	sg_color_t (*cursor_get_pixel_increment)(sg_cursor_t * cursor, int x, int y);

	void (*cursor_increment)(sg_cursor_t * cursor, sg_point_t p);
	void (*cursor_inc_x)(sg_cursor_t * cursor);
	void (*cursor_dec_x)(sg_cursor_t * cursor);
	void (*cursor_inc_y)(sg_cursor_t * cursor);
	void (*cursor_dec_y)(sg_cursor_t * cursor);
	sg_color_t (*cursor_get_pixel)(sg_cursor_t * cursor);
	void (*cursor_draw_pixel)(sg_cursor_t * cursor);
	void (*cursor_draw_hline)(sg_cursor_t * cursor, sg_size_t width);
	void (*cursor_draw_cursor)(sg_cursor_t * dest_cursor, const sg_cursor_t * src_cursor, sg_size_t width);
	void (*cursor_draw_pattern)(sg_cursor_t * cursor, sg_size_t width, sg_bmap_data_t pattern);
	void (*cursor_shift_right)(sg_cursor_t * cursor, sg_size_t shift_width, sg_size_t shift_distance);
	void (*cursor_shift_left)(sg_cursor_t * cursor, sg_size_t shift_width, sg_size_t shift_distance);

	sg_int_t (*cursor_find_positive_edge)(sg_cursor_t * cursor, sg_size_t width);
	sg_int_t (*cursor_find_negative_edge)(sg_cursor_t * cursor, sg_size_t width);
	sg_int_t (*cursor_find_edge)(sg_cursor_t * cursor, sg_color_t current_color, sg_size_t width);


	sg_color_t (*get_pixel)(const sg_bmap_t * bmap, sg_point_t p);
	void (*draw_pixel)(const sg_bmap_t * bmap, sg_point_t p);
	void (*draw_line)(const sg_bmap_t * bmap, sg_point_t p1, sg_point_t p2);
	void (*draw_quadratic_bezier)(const sg_bmap_t * bmap, sg_point_t p1, sg_point_t p2, sg_point_t p3, sg_point_t * corners);
	void (*draw_cubic_bezier)(const sg_bmap_t * bmap, sg_point_t p1, sg_point_t p2, sg_point_t p3, sg_point_t p4, sg_point_t * corners);
	void (*draw_rectangle)(const sg_bmap_t * bmap, const sg_region_t * region);
	void (*draw_arc)(const sg_bmap_t * bmap, const sg_region_t * region, s16 start, s16 end, s16 rotation, sg_point_t * corners);
	void (*draw_pour)(const sg_bmap_t * bmap, sg_point_t p, const sg_region_t * region);
	void (*draw_pattern)(const sg_bmap_t * bmap, const sg_region_t * region, sg_bmap_data_t odd_pattern, sg_bmap_data_t even_pattern, sg_size_t pattern_height);
	void (*draw_bitmap)(const sg_bmap_t * bmap_dest, sg_point_t p_dest, const sg_bmap_t * bmap_src);
	void (*draw_sub_bitmap)(const sg_bmap_t * bmap_dest, sg_point_t p_dest, const sg_bmap_t * bmap_src, const sg_region_t * src_region);

	void (*vector_draw_path)(sg_bmap_t * bmap, sg_vector_path_t * path, const sg_vector_map_t * map);

	int (*animate)(sg_bmap_t * bmap, sg_bmap_t * bitmap, sg_animation_t * animation);
	int (*animate_init)(sg_animation_t * animation, u8 type, u8 path, u8 step_total, sg_size_t motion_total, sg_point_t start, sg_area_t area);

   int (*antialias_filter_init)(
         sg_antialias_filter_t * filter,
         const u8 contrast_data[8]
   );

   int (*antialias_filter_apply)(
         const sg_bmap_t * bmap,
         sg_antialias_filter_t * filter,
         sg_region_t region
         );

} sg_api_t;

extern const sg_api_t sg_api;

#if defined __link
#define SGFX_API_REQUEST &sg_api
#else
#define SGFX_API_REQUEST MCU_API_REQUEST_CODE('s','g','f','x')
#endif



#if defined __cplusplus
}
#endif

#endif /* SG_H_ */
