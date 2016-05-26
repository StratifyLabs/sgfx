/*! \file */ //Copyright 2011-2016 Tyler Gilbert; All Rights Reserved

#ifndef SG_TYPES_H_
#define SG_TYPES_H_

#include <mcu/types.h>
#include <stdint.h>
#include <sys/types.h>

#define SG_VERSION "0.0.1"

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
#define SG_MAP_FILL_FLAG (1<<8)

enum {
	SG_LINE,
	SG_ARC,
	SG_FILL,
	SG_TYPE_TOTAL
};

enum {
	SG_OP_SET,
	SG_OP_CLR,
	SG_OP_INV
};

#define SG_TYPE_MASK (0xFF)
#define SG_ENABLE_FLAG (1<<15)
#define SG_FILL_FLAG (1<<14)

typedef i16 sg_int_t;
typedef u16 sg_uint_t;
typedef sg_uint_t sg_size_t;

//this needs to be based on 32 bit data so it's faster
typedef u8 sg_bmap_data_t;
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


typedef struct MCU_PACK {
	sg_bmap_data_t * data;
	sg_dim_t dim;
	sg_dim_t margin_top_left;
	sg_dim_t margin_bottom_right;
	sg_size_t columns;
} sg_bmap_t;


typedef struct MCU_PACK {
	sg_point_t p[2];
} sg_triangle_t;

typedef struct MCU_PACK {
	sg_size_t rx; //x radius
	sg_size_t ry; //y radius
	i16 start, stop; //start/stop angles
} sg_arc_t;

typedef struct MCU_PACK {
	//list of points
	sg_size_t r; //inner/outer radii
} sg_circle_t;

typedef struct MCU_PACK {
	sg_point_t p;
} sg_line_t;

typedef struct MCU_PACK {
	sg_dim_t dim;
} sg_rect_t;

typedef struct MCU_PACK {
	sg_size_t w;
	sg_size_t h;
	size_t size;
	//this must be 4 byte aligned
} sg_bitmap_hdr_t;

typedef struct MCU_PACK {
	u16 type; //type of primitive object
	i16 rotation; //vector space rotation
	sg_point_t shift;
	union {
		sg_arc_t arc;
		sg_triangle_t triangle;
		sg_line_t line;
		sg_rect_t rect;
		sg_circle_t circle;
	};
} sg_icon_primitive_t;

typedef struct MCU_PACK {
	sg_point_t top_left;
	sg_point_t bottom_right;
} sg_bounds_t;

typedef struct HPWL_PACK {
	sg_point_t point;
	sg_dim_t dim;
} sg_region_t;

typedef struct MCU_PACK {
	u16 total /*! Total number of elements */;
	u16 fill_total /*! Total number of fill elements */;
	const sg_icon_primitive_t * elements;
} sg_icon_t;

/*! \brief Describes how an sg_icon_t is mapped to a sg_bitmap_t */
typedef struct MCU_PACK {
	sg_point_t shift; //shift within screen (absolute)
	i16 rotation; //rotate within screen (absolute)
	sg_dim_t size; //scaling
	u8 thickness_fill; //execute fill items -- fill if SG_MAP_FILL_FLAG
	u8 op;
} sg_map_t;


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
