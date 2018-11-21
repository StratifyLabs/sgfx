/*! \file */ //Copyright 2011-2016 Tyler Gilbert; All Rights Reserved

#ifndef SG_TYPES_H_
#define SG_TYPES_H_

#if defined __StratifyOS__ || defined __link
#include <mcu/types.h>
#else
//need to define u32, s32, u16, s16, etc
#include <stdint.h>
typedef uint8_t u8;
typedef int8_t s8;
typedef uint16_t u16;
typedef int16_t s16;
typedef uint32_t u32;
typedef int32_t s32;
typedef uint64_t u64;
typedef int64_t s64;
#endif

#include <sys/types.h>

#define SG_STR_VERSION "2.3"
#define SG_VERSION 0x0203

#define SG_MAX (32767)
#define SG_MIN (-32767)
#define SG_TRIG_POINTS 512

//MAP max is approximately SG_MAX/sqrt(2) -- this allow icons to be rotated and still fit in the virtual space
#define SG_MAP_MAX (23160)

#define SG_CENTER 0
#define SG_LEFT (-SG_MAP_MAX)
#define SG_RIGHT (SG_MAP_MAX)
#define SG_TOP (-SG_MAP_MAX)
#define SG_BOT (SG_MAP_MAX)
#define SG_BOTTOM (SG_MAP_MAX)

#define SG_ANIMATION_STEP_FLAG (1<<15)
#define SG_MAP_FILL_FLAG (1<<7)
#define SG_MAP_THICKNESS_MASK ~(SG_MAP_FILL_FLAG)


enum {
	SG_FORMAT_VARIABLE /*! Bits-per-pixel is determined by the bitmap */ = 0,
	SG_FORMAT_1BPP /*! 1-bit, 2-color palette (or monochrome graphics) */ = 1,
	SG_FORMAT_2BPP /*! 2-bit, 4-color palette */ = 2,
	SG_FORMAT_4BPP /*! 4-bit, 16-color palette */ = 4,
	SG_FORMAT_8BPP /*! 8-bit, 256-color palette */ = 8,
	SG_FORMAT_RGB565 /*! 16-bit R5 G6 B5 */ = 16
};

#define SG_TYPE_MASK (0xFF)
#define SG_ENABLE_FLAG (1<<15)

typedef s16 sg_int_t;
typedef u16 sg_uint_t;
typedef sg_uint_t sg_size_t;
typedef u16 sg_color_t;

//this needs to be based on 32 bit data so it's faster
typedef u32 sg_bmap_data_t;
typedef u32 sg_unified_t;

/*! \brief Graphics Point
 * \details This is the data structure for a
 * point on a bitmap.
 */
typedef union MCU_PACK {
	struct {
		sg_int_t x /*! X location */;
		sg_int_t y /*! Y location */;
	};
	sg_unified_t point;
} sg_point_t;

/*! \brief Graphics Dimension
 * \details This is the data structure for a
 * dimension on a bitmap.
 */
typedef union MCU_PACK {
	struct {
		sg_size_t width /*! Width */;
		sg_size_t height /*! Height */;
	};
	sg_unified_t dim;
} sg_dim_t;

enum {
	SG_PEN_FLAG_IS_SOLID /*! Assigns color when drawing (default) */ = 0,
	SG_PEN_FLAG_IS_ASSIGN /*! Alias for SG_PEN_FLAG_IS_SOLID */ = SG_PEN_FLAG_IS_SOLID,
	SG_PEN_FLAG_IS_BLEND /*! Blends colors using logical OR */ = (1<<0),
	SG_PEN_FLAG_IS_OR /*! Alias for SG_PEN_FLAG_IS_BLEND */ = SG_PEN_FLAG_IS_BLEND,
	SG_PEN_FLAG_IS_INVERT /*! Draws colors using logical XOR */ = (1<<1),
	SG_PEN_FLAG_IS_XOR /*! Alias for SG_PEN_FLAG_IS_INVERT */ = SG_PEN_FLAG_IS_INVERT,
	SG_PEN_FLAG_IS_ERASE /*! Erases the colors that are set in the pen (logical AND of inverse color) */ = (1<<2),
	SG_PEN_FLAG_IS_AND /*! Alias for SG_PEN_FLAG_IS_ERASE */  = SG_PEN_FLAG_IS_ERASE,
	SG_PEN_FLAG_IS_FILL /*! When drawing vector icons, this flag enables fill points specified by the icon */ = (1<<3)
};

#define SG_PEN_FLAG_NOT_SOLID_MASK (SG_PEN_FLAG_IS_BLEND|SG_PEN_FLAG_IS_INVERT|SG_PEN_FLAG_IS_ERASE)

/*! \brief Graphics Pen
 * \details Data structure for holding data for a pen.
 */
typedef struct MCU_PACK {
	u16 o_flags /*! Flags (SG_PEN_FLAG_...) */;
	u8 thickness /*! Thickness in pixels */;
	u8 resd;
	sg_color_t color /*! Pen color */;
} sg_pen_t;

/*! \brief Graphics Bitmap
 * \details Data structure for holding data for a bitmap.
 */
typedef struct MCU_PACK {
	sg_bmap_data_t * data /*! A pointer to the bitmap data */;
	sg_pen_t pen /*! The bitmap pen used for drawing on the bitmap */;
	sg_dim_t dim /*! The bitmap's dimensions */;
	sg_dim_t margin_top_left /*! Bitmap's top/left margins */;
	sg_dim_t margin_bottom_right /*! Bitmap's bottom/right margins */;
	sg_size_t columns /*! The number of columns in the bitmap (used internally) */;
	u8 bits_per_pixel /*! The number of bits in each pixel */;
} sg_bmap_t;

typedef struct MCU_PACK {
	const sg_bmap_t * bmap;
	sg_bmap_data_t * target;
	sg_size_t shift;
} sg_cursor_t;


enum {
	SG_LINE /*! A line */,
	SG_ARC /*! An Arc */,
	SG_POUR /*! Pour in the area */,
	SG_QUADRATIC_BEZIER /*! Quadratic Bezier */,
	SG_CUBIC_BEZIER /*! Cubic Bezier */,
	SG_TYPE_TOTAL
};

typedef struct MCU_PACK {
	sg_size_t width;
	sg_size_t height;
	u32 size;
	sg_size_t bits_per_pixel;
	u16 version;
	//this must be 4 byte aligned
} sg_bmap_header_t;


/*! \brief Graphics Region Structure
 * \details Describes an area using a point and a dimension */
typedef struct MCU_PACK {
	sg_point_t point /*! Top left corner of the region */;
	sg_dim_t dim /*! Dimensions of the region */;
} sg_region_t;


typedef struct MCU_PACK {
	sg_point_t p1;
	sg_point_t p2;
} sg_vector_line_t;

typedef struct MCU_PACK {
	sg_point_t center /*! Arc center */;
	sg_size_t rx /*! X radius */;
	sg_size_t ry /*! Y radius */;
	s16 start /*! Start angle */;
	s16 stop /*!  Stop angle */;
	s16 rotation /*! Arc Rotation */;
} sg_vector_arc_t;

typedef struct MCU_PACK {
	sg_point_t p1;
	sg_point_t p2;
	sg_point_t p3;
} sg_vector_quadtratic_bezier_t;

typedef struct MCU_PACK {
	sg_point_t p1;
	sg_point_t p2;
	sg_point_t p3;
	sg_point_t p4;
} sg_vector_cubic_bezier_t;

typedef struct MCU_PACK {
	sg_point_t center;
} sg_vector_pour_t;

/*! \brief Icon Primitive Structure
 * \details Describes an icon primitive */
typedef struct MCU_PACK {
	u16 type /*! type of primitive object (e.g. SG_LINE) */;
	union {
		sg_vector_arc_t arc /*! Primitive data for SG_ARC */;
		sg_vector_line_t line /*! Primitive data for SG_LINE */;
		sg_vector_quadtratic_bezier_t quadratic_bezier /*! Primitive data for SG_QUADRATIC_BEZIER */;
		sg_vector_cubic_bezier_t cubic_bezier /*! Primitive data for SG_CUBIC_BEZIER */;
		sg_vector_pour_t pour /*! Primitive data for pour */;
	};
} sg_vector_primitive_t;

enum {
	SG_VECTOR_PATH_FLAG_CLOSE_PATH = (1<<0),
	SG_VECTOR_PATH_FLAG_IS_FILL_ODD_EVEN = (1<<1),
};

enum {
	SG_VECTOR_PATH_NONE = 0,
	SG_VECTOR_PATH_MOVE /*! A line */,
	SG_VECTOR_PATH_LINE /*! An Arc */,
	SG_VECTOR_PATH_QUADRATIC_BEZIER /*! Quadratic Bezier */,
	SG_VECTOR_PATH_CUBIC_BEZIER /*! Cubic Bezier */,
	SG_VECTOR_PATH_CLOSE /*! Close the path using a line */,
	SG_VECTOR_PATH_POUR /*! Pour at the point specified */,
	SG_VECTOR_PATH_TOTAL
};

typedef struct MCU_PACK {
	sg_point_t point;
} sg_vector_path_move_t;

typedef struct MCU_PACK {
	sg_point_t point;
} sg_vector_path_line_t;

typedef struct MCU_PACK {
	sg_point_t point;
	sg_point_t control;
} sg_vector_path_quadtratic_bezier_t;

typedef struct MCU_PACK {
	sg_point_t point;
	sg_point_t control[2];
} sg_vector_path_cubic_bezier_t;

typedef struct MCU_PACK {
	sg_point_t point;
} sg_vector_path_pour_t;

/*! \brief Icon Path Structure
 * \details Describes a vector path */
typedef struct MCU_PACK {
	u16 type /*! type of primitive object (e.g. SG_LINE) */;
	union {
		sg_vector_path_move_t move /*! Move to a point */;
		sg_vector_path_line_t line /*! Line to a point */;
		sg_vector_path_quadtratic_bezier_t quadratic_bezier /*! Path for quadratic bezier */;
		sg_vector_path_cubic_bezier_t cubic_bezier /*! Path for cubic bezier*/;
		sg_vector_path_pour_t pour /*! Pour at the specified point (cursor is not affected) */;
	};
} sg_vector_path_description_t;

typedef struct MCU_PACK {
	u32 count; /*! The number of objects in \a path_description */
	const sg_vector_path_description_t * list;
} sg_vector_path_icon_t;

/*! \brief Graphics Icon Structure
 * \details Describes a scalable graphic that can be drawn on a bitmap */
typedef struct MCU_PACK {
	u16 total /*! Total number of primitives */;
	u16 fill_total /*! Total number of fill primitives */;
	const sg_vector_primitive_t * primitives;
} sg_vector_icon_t;

/*! \brief Graphics Map Structure
 * \details Describes how an sg_icon_t is mapped to a sg_bitmap_t */
typedef struct MCU_PACK {
	sg_region_t region;
	s16 rotation; //rotation angle of map on the display
} sg_vector_map_t;


/*! \brief Data for drawing vectors using paths
 * \sa sg_draw_vector_path()
 */
typedef struct MCU_PACK {
	sg_vector_path_icon_t icon /*! The icon to draw */;
	sg_point_t start /*! Internal use */ ;
	sg_point_t current /*! Internal use */;
	sg_region_t region /*! Destination for region specifications */;
} sg_vector_path_t;


enum {
	SG_ANIMATION_TYPE_PUSH_LEFT,
	SG_ANIMATION_TYPE_PUSH_RIGHT,
	SG_ANIMATION_TYPE_PUSH_UP,
	SG_ANIMATION_TYPE_PUSH_DOWN,
	SG_ANIMATION_TYPE_SLIDE_LEFT,
	SG_ANIMATION_TYPE_UNDO_SLIDE_LEFT,
	SG_ANIMATION_TYPE_SLIDE_RIGHT,
	SG_ANIMATION_TYPE_UNDO_SLIDE_RIGHT,
	SG_ANIMATION_TYPE_SLIDE_UP,
	SG_ANIMATION_TYPE_UNDO_SLIDE_UP,
	SG_ANIMATION_TYPE_SLIDE_DOWN,
	SG_ANIMATION_TYPE_UNDO_SLIDE_DOWN,
	SG_ANIMATION_TYPE_NONE,
	SG_ANIMATION_TYPE_BOUNCE_UP,
	SG_ANIMATION_TYPE_BOUNCE_DOWN,
	SG_ANIMATION_TYPE_BOUNCE_LEFT,
	SG_ANIMATION_TYPE_BOUNCE_RIGHT,
	SG_ANIMATION_TYPE_TOTAL
};


enum {
	SG_ANIMATION_PATH_LINEAR,
	SG_ANIMATION_PATH_SQUARED,
	SG_ANIMATION_PATH_SQUARED_UNDO
};

typedef struct MCU_PACK {
	u8 type;
	u16 step;
	u16 step_total;
	u32 sum_of_squares;
	sg_size_t motion;
	sg_size_t motion_total /*! \brief Total amount of animation movement */;
} sg_animation_path_t;

typedef struct MCU_PACK {
	u8 type /*! \brief Animation type */;
	sg_region_t region /*! Animation region (start point and dimensions) */;
	sg_animation_path_t path;
} sg_animation_t;


#endif /* SG_TYPES_H_ */
