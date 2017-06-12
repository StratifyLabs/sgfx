/*! \file */ //Copyright 2011-2016 Tyler Gilbert; All Rights Reserved

#ifndef SG_TYPES_H_
#define SG_TYPES_H_

#if defined __StratifyOS__
#include <mcu/types.h>
#else
//need to define u32, s32, u16, s16, etc
#include <stdint.h>
typedef uint8_t u8;
typedef int8_t s8;
typedef uint16_t u16;
typedef int16_t s16;
typedef u32 u32;
typedef int32_t s32;
typedef uint64_t u64;
typedef int64_t s64;
#endif

#include <sys/types.h>

#define SG_STR_VERSION "2.0.0"
#define SG_VERSION 0x0200

#define SG_MAX (32767)
#define SG_MIN (-32767)
#define SG_TRIG_POINTS 512

#define SG_MAP_MAX (SG_MAX/2)

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

typedef union MCU_PACK {
	struct {
		sg_int_t x;
		sg_int_t y;
	};
	sg_unified_t point;
} sg_point_t;

typedef union MCU_PACK {
	struct {
		sg_size_t w;
		sg_size_t h;
	};
	sg_unified_t dim;
} sg_dim_t;

enum {
	SG_PEN_FLAG_TRANSPARENT_BACKGROUND /*! When set, the pen will not operate when the source color is zero */ = (1<<0),
	SG_PEN_FLAG_INVERT = (1<<1),
	SG_PEN_FLAG_BLEND = (1<<2),
	SG_PEN_FLAG_FILL /*! When set, the icon will be drawn filled */ = (1<<3)
};

typedef struct MCU_PACK {
	u16 o_flags;
	u8 thickness;
	u8 resd;
	//the following union should just be sg_color_t color;
	sg_color_t color;
} sg_pen_t;


typedef struct MCU_PACK {
	sg_bmap_data_t * data;
	sg_pen_t pen;
	sg_dim_t dim;
	sg_dim_t margin_top_left;
	sg_dim_t margin_bottom_right;
	sg_size_t columns;
} sg_bmap_t;

typedef struct MCU_PACK {
	const sg_bmap_t * bmap;
	//sg_point_t p;
	sg_bmap_data_t * target;
	sg_size_t shift;
} sg_cursor_t;


enum {
	SG_LINE /*! A line */,
	SG_ARC /*! An Arc */,
	SG_FILL,
	SG_POUR /*! Pour in the area */ = SG_FILL,
	SG_QUADRATIC_BEZIER,
	SG_CUBIC_BEZIER,
	SG_TYPE_TOTAL
};

typedef struct MCU_PACK {
	sg_size_t w;
	sg_size_t h;
	size_t size;
	sg_size_t bits_per_pixel;
	u16 version;
	//this must be 4 byte aligned
} sg_bitmap_hdr_t;



/*! \brief Graphics Bounds Structure
 * \details Describes an area using two points */
typedef struct MCU_PACK {
	sg_point_t top_left /*! Top left corner of the bounded area */;
	sg_point_t bottom_right /*! Bottom right corner of the bounded area */;
} sg_bounds_t;


/*! \brief Graphics Region Structure
 * \details Describes an area using a point and a dimension */
typedef struct MCU_PACK {
	sg_point_t point /*! Top left corner of the region */;
	sg_dim_t dim /*! Dimesions of the region */;
} sg_region_t;


typedef struct MCU_PACK {
	sg_point_t p;
} sg_vector_line_t;

typedef struct MCU_PACK {
	sg_size_t rx /*! X radius */;
	sg_size_t ry /*! Y radius */;
	s16 start /*! Start angle */;
	s16 stop /*!  Stop angle */;
} sg_vector_arc_t;

typedef struct MCU_PACK {
	sg_point_t p1;
	sg_point_t p2;
} sg_vector_quadtratic_bezier_t;

typedef struct MCU_PACK {
	sg_point_t p1;
	sg_point_t p2;
	sg_point_t p3;
} sg_vector_cubic_bezier_t;

/*! \brief Icon Primitive Structure
 * \details Describes an icon primitive */
typedef struct MCU_PACK {
	u16 type /*! type of primitive object (e.g. SG_LINE) */;
	s16 rotation /*! Primitive rotation within the map */;
	sg_point_t shift /*! Primitive offset within the map */;
	union {
		sg_vector_arc_t arc /*! Primitive data for SG_ARC */;
		sg_vector_line_t line /*! Primitive data for SG_LINE */;
		sg_vector_quadtratic_bezier_t quadratic_bezier /*! Primitive data for SG_QUADRATIC_BEZIER */;
		sg_vector_cubic_bezier_t cubic_bezier /*! Primitive data for SG_CUBIC_BEZIER */;
	};
} sg_vector_primitive_t;

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
	sg_point_t shift; //shift within screen (absolute)
	s16 rotation; //rotate within screen (absolute)
	sg_dim_t size; //scaling
} sg_vector_map_t;


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
	sg_point_t start /*! \brief Animation start point */;
	sg_dim_t dim /*! \brief Animate within these dimensions with start in the top left corner */;
	sg_animation_path_t path;
} sg_animation_t;


#endif /* SG_TYPES_H_ */
