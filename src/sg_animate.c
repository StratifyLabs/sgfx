//Copyright 2011-2016 Tyler Gilbert; All Rights Reserved


#include <mcu/mcu.h>
#include <unistd.h>

#if defined __armv7em
#define __FPU_PRESENT 1
#define SysTick_IRQn 0
#define __NVIC_PRIO_BITS 4
typedef int IRQn_Type;
#include <mcu/arch/cmsis/core_cm4.h>
#elif defined __armv7m
#define SysTick_IRQn 0
#define __NVIC_PRIO_BITS 4
typedef int IRQn_Type;
#include <mcu/arch/cmsis/core_cm3.h>
#endif

#include "sg_config.h"
#include "sg.h"

static int sg_animate_push_right(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation);
static int sg_animate_push_left(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation);
static int sg_animate_push_up(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation);
static int sg_animate_push_down(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation);
static int sg_animate_slide_right(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation);
static int sg_animate_undo_slide_right(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation);
static int sg_animate_slide_left(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation);
static int sg_animate_undo_slide_left(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation);
static int sg_animate_slide_up(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation);
static int sg_animate_undo_slide_up(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation);
static int sg_animate_slide_down(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation);
static int sg_animate_undo_slide_down(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation);
static int sg_animate_none(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation);
static int sg_animate_bounce_up(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation);
static int sg_animate_bounce_down(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation);
static int sg_animate_bounce_left(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation);
static int sg_animate_bounce_right(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation);


static u32 sum_of_squares(u32 x){
	u32 i;
	u32 sum = 0;
	for(i=1; i <= x; i++){
		sum += i*i;
	}
	return sum;
}

static u32 animation_step_size(sg_animation_t * animation, u32 i){
	u32 step_size;
	u32 m;

	step_size = 4;

	switch(animation->path.type){
	case SG_ANIMATION_PATH_LINEAR:
		step_size = animation->path.motion_total / animation->path.step_total;
		break;
	case SG_ANIMATION_PATH_SQUARED_UNDO:
		m = i+1;
		step_size = (animation->path.motion_total * m*m  + animation->path.sum_of_squares/2) / animation->path.sum_of_squares;
		break;
	case SG_ANIMATION_PATH_SQUARED:
		m = animation->path.step_total - i;
		step_size = (animation->path.motion_total *  m*m + animation->path.sum_of_squares/2) / animation->path.sum_of_squares;
		break;
	}

	return step_size;
}

static u32 sum_of_steps(sg_animation_t * animation){
	u32 i;
	u32 steps = 0;
	for(i=0; i < animation->path.step_total; i++){
		steps += animation_step_size(animation, i);
	}
	return steps;
}

static sg_size_t sg_animate_calc_count(sg_animation_t * animation){
	sg_size_t count;
	u32 sum;
	u16 step = animation->path.step & ~SG_ANIMATION_STEP_FLAG;
	count = animation_step_size(animation, step);
	if( step == 0 ){
		sum = sum_of_steps(animation);
		count = count + (animation->path.motion_total-sum);
	}
	animation->path.step++; //must come sg_animate_calc_count calc_count
	return count;
}

int sg_animate_push_right(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation){
	sg_point_t dest;
	sg_point_t src;
	sg_dim_t shift_dim;
	sg_size_t count;
	sg_point_t start;
	sg_point_t shift;
	sg_dim_t d;

	shift.y = 0;

	if( animation->path.step < animation->path.step_total ){
		count = sg_animate_calc_count(animation);
		//no work to be performed
		if( count == 0 ){
			return 1;
		}

		start = animation->start;
		d = animation->dim;

		animation->path.motion += count;

		src.x = start.x + animation->dim.w - animation->path.motion;
		src.y = start.y;

		shift_dim.w = count;
		shift_dim.h = d.h;

		dest.x = start.x;
		dest.y = start.y;

		shift.x = count;
		sg_transform_shift(bmap, shift, start, d);
		sg_draw_sub_bitmap(bmap, dest, scratch, src, shift_dim);

		return 1;
	}

	return 0;

}

int sg_animate_push_left(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation){
	sg_point_t dest;
	sg_point_t src;
	sg_dim_t shift_dim;
	sg_point_t start;
	sg_dim_t d;
	sg_point_t shift;
	sg_size_t count;

	if( animation->path.step < animation->path.step_total ){
		count = sg_animate_calc_count(animation);

		if( count == 0 ){
			return 1;
		}

		d = animation->dim;
		start = animation->start;

		dest.x = start.x + animation->dim.w - count;
		dest.y = start.y;

		shift_dim.w = count;
		shift_dim.h = d.h;

		src.x = start.x + animation->path.motion;
		src.y = start.y;

		shift.x = -1*count;
		shift.y = 0;

		sg_transform_shift(bmap, shift, start, d);
		sg_draw_sub_bitmap(bmap, dest, scratch, src, shift_dim);

		animation->path.motion += count;

		return 1;
	}
	return 0;
}

int sg_animate_push_up(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation){
	sg_point_t dest;
	sg_point_t src;
	sg_dim_t d;
	sg_size_t count;
	sg_point_t shift;
	sg_point_t start;

	shift.x = 0;

	if( animation->path.step < animation->path.step_total ){
		start = animation->start;
		count = sg_animate_calc_count(animation);

		src.x = start.x;
		src.y = start.y + animation->path.motion;

		d.w = animation->dim.w;
		d.h = count;

		dest.x = start.x;
		dest.y = start.y + animation->dim.h - d.h;

		shift.y = -1*count;

		sg_transform_shift(bmap, shift, start, animation->dim);
		sg_draw_sub_bitmap(bmap, dest, scratch, src, d);

		animation->path.motion += count;

		return 1;
	}
	return 0;
}

int sg_animate_push_down(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation){
	sg_point_t src;
	sg_dim_t d;
	sg_size_t count;
	sg_point_t shift;
	sg_point_t start;

	shift.x = 0;

	if( animation->path.step < animation->path.step_total ){
		start = animation->start;
		count = sg_animate_calc_count(animation);
		animation->path.motion += count;

		src.x = start.x;
		src.y = start.y + animation->dim.h - animation->path.motion;

		d.w = animation->dim.w;
		d.h = count;

		shift.y = count;
		sg_transform_shift(bmap, shift, start, animation->dim);
		sg_draw_sub_bitmap(bmap, start, scratch, src, d);


		return 1;
	}
	return 0;
}

int sg_animate_slide_right(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation){
	//this could be used for drawers
	return 0;
}

int sg_animate_undo_slide_right(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation){
	//this could be used for drawers
	return 0;
}

int sg_animate_slide_left(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation){
	//this could be used for drawers
	return 0;
}

int sg_animate_undo_slide_left(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation){
	//this could be used for drawers
	return 0;
}

int sg_animate_slide_up(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation){
	sg_point_t dest;
	sg_point_t src;
	sg_dim_t d;

	sg_point_t start;

	if( animation->path.step < animation->path.step_total ){
		start = animation->start;
		animation->path.motion += sg_animate_calc_count(animation);

		src.x = start.x;
		src.y = start.y;

		dest.x = start.x;
		dest.y = start.y + animation->dim.h - animation->path.motion;

		d.w = animation->dim.w;
		d.h = animation->path.motion;

		sg_draw_sub_bitmap(bmap, dest, scratch, src, d);


		return 1;
	}
	return 0;
}

int sg_animate_undo_slide_up(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation){
	sg_point_t dest;
	sg_point_t src;
	sg_dim_t d;
	sg_size_t count;
	sg_point_t shift;
	sg_point_t start;

	shift.x = 0;

	if( animation->path.step < animation->path.step_total ){
		start = animation->start;
		count = sg_animate_calc_count(animation);
		animation->path.motion += count;

		src.x = start.x;
		src.y = start.y;

		dest.x = start.x;
		dest.y = start.y;

		d.w = animation->dim.w;
		d.h = animation->path.motion;

		shift.y = count;

		sg_transform_shift(bmap, shift, start, animation->dim);

		sg_draw_sub_bitmap(bmap, dest, scratch, src, d);

		return 1;
	}
	return 0;
}

int sg_animate_slide_down(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation){
	sg_point_t src;
	sg_point_t dest;
	sg_dim_t d;

	sg_point_t start;

	if( animation->path.step < animation->path.step_total ){
		start = animation->start;
		animation->path.motion += sg_animate_calc_count(animation);
		dest.x = start.x;
		dest.y = start.y;

		src.x = start.x;
		src.y = start.y + animation->dim.h - animation->path.motion;

		d.w = animation->dim.w;
		d.h = animation->path.motion;

		sg_draw_sub_bitmap(bmap, dest, scratch, src, d);
		return 1;
	}
	return 0;
}

int sg_animate_undo_slide_down(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation){
	sg_point_t dest;
	sg_point_t src;
	sg_dim_t d;
	sg_size_t count;
	sg_point_t shift;
	sg_point_t start;

	shift.x = 0;

	if( animation->path.step < animation->path.step_total ){
		start = animation->start;
		count = sg_animate_calc_count(animation);

		animation->path.motion += count;

		src.x = start.x;
		src.y = start.y + animation->dim.h - animation->path.motion;

		dest = src;

		d.w = animation->dim.w;
		d.h = animation->dim.h - animation->path.motion + count;

		shift.y = -1*count;
		sg_transform_shift(bmap, shift, start, d);

		d.h = count;

		sg_draw_sub_bitmap(bmap, dest, scratch, src, d);

		return 1;
	}
	return 0;

}

int sg_animate_none(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation){
	if( animation->path.step < animation->path.step_total ){
		animation->path.motion = sg_animate_calc_count(animation);
		return 1;
	}
	return 0;
}

int sg_animate_bounce_up(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation){
	sg_point_t dest;
	sg_point_t src;
	sg_dim_t d;
	sg_size_t count;
	u16 step;
	sg_point_t shift;
	sg_point_t start;

	shift.x = 0;

	step = animation->path.step & ~(SG_ANIMATION_STEP_FLAG);

	if( step < animation->path.step_total ){
		start = animation->start;
		count = sg_animate_calc_count(animation);

		if( animation->path.step & (SG_ANIMATION_STEP_FLAG) ){
			//bounce back phase

			dest.x = animation->start.x;
			dest.y = animation->start.y + animation->path.motion_total - animation->path.motion;

			d.w = animation->dim.w;
			d.h = animation->dim.h - animation->path.motion_total + animation->path.motion;

			sg_draw_sub_bitmap(bmap, dest, scratch, start, d);

		} else {
			//bounce phase
			if( step == 0 ){
				sg_draw_sub_bitmap(bmap, animation->start, scratch, animation->start, animation->dim);
			}

			src.x = start.x;
			src.y = start.y + animation->path.motion;

			d.w = animation->dim.w;
			d.h = animation->dim.h - animation->path.motion;

			dest.x = start.x;
			dest.y = start.y;

			shift.y = count;
			sg_transform_shift(bmap, shift, src, d);

			d.h = animation->path.motion + count;

			//! \todo CHECKERBOARD AREA
			//sg_set_area(bmap, dest, d, 0xAA);


			if( animation->path.step == animation->path.step_total ){
				animation->path.step = SG_ANIMATION_STEP_FLAG;
				animation->path.motion = 0;
			}
		}

		animation->path.motion += count;

		return 1;
	}
	return 0;
}

static int sg_animate_bounce_down(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation){
	sg_point_t dest;
	sg_point_t src;
	sg_dim_t d;
	sg_size_t count;
	u16 step;
	sg_point_t shift;
	sg_point_t start;
	shift.x = 0;

	step = animation->path.step & ~(SG_ANIMATION_STEP_FLAG);

	if( step < animation->path.step_total ){
		start = animation->start;
		count = sg_animate_calc_count(animation);

		if( animation->path.step & (SG_ANIMATION_STEP_FLAG) ){
			//bounce back phase

			src.x = start.x;
			src.y = start.y + animation->path.motion_total - animation->path.motion;

			dest.x = animation->start.x;
			dest.y = animation->start.y;

			d.w = animation->dim.w;
			d.h = animation->dim.h - animation->path.motion_total + animation->path.motion;

			sg_draw_sub_bitmap(bmap, dest, scratch, src, d);

		} else {
			//bounce phase

			if( step == 0 ){
				//copy the drawing to the scratch area
				sg_draw_sub_bitmap(bmap, animation->start, scratch, animation->start, animation->dim);
			}

			src.x = start.x;
			src.y = start.y;

			d.w = animation->dim.w;
			d.h = animation->dim.h - animation->path.motion;

			shift.y = count;
			sg_transform_shift(bmap, shift, src, d);

			d.h = animation->path.motion + count;
			dest.x = start.x;
			dest.y = start.y + animation->dim.h - d.h;

			//! \todo Checkboard background
			//sg_set_area(bmap, dest, d, 0xAA);

			if( animation->path.step == animation->path.step_total ){
				animation->path.step = SG_ANIMATION_STEP_FLAG;
				animation->path.motion = 0;
			}
		}

		animation->path.motion += count;

		return 1;
	}
	return 0;
}

static int sg_animate_bounce_left(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation){
	sg_point_t dest;
	sg_point_t src;
	sg_dim_t d;
	sg_size_t count;
	u16 step;

	sg_point_t start;
	sg_point_t shift;

	shift.y = 0;

	step = animation->path.step & ~(SG_ANIMATION_STEP_FLAG);

	if( step < animation->path.step_total ){
		start = animation->start;
		count = sg_animate_calc_count(animation);

		if( animation->path.step & (SG_ANIMATION_STEP_FLAG) ){
			//bounce back phase

			dest.x = animation->start.x + animation->path.motion_total - animation->path.motion;
			dest.y = animation->start.y;

			d.w = animation->dim.w - animation->path.motion_total + animation->path.motion;
			d.h = animation->dim.h;

			sg_draw_sub_bitmap(bmap, dest, scratch, start, d);

		} else {
			//bounce phase
			if( step == 0 ){
				sg_draw_sub_bitmap(bmap, animation->start, scratch, animation->start, animation->dim);
			}

			src.x = start.x + animation->path.motion;
			src.y = start.y;

			d.w = animation->dim.w - animation->path.motion;
			d.h = animation->dim.h;

			dest.x = start.x;
			dest.y = start.y;

			shift.x = count;
			sg_transform_shift(bmap, shift, src, d);

			d.w = animation->path.motion + count;

			//! \todo checkerboard area
			//sg_set_area(bmap, dest, d, 0xAA);

			if( animation->path.step == animation->path.step_total ){
				animation->path.step = SG_ANIMATION_STEP_FLAG;
				animation->path.motion = 0;
			}
		}

		animation->path.motion += count;

		return 1;
	}
	return 0;
}

static int sg_animate_bounce_right(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation){
	sg_point_t dest;
	sg_point_t src;
	sg_dim_t d;
	sg_size_t count;
	u16 step;
	sg_point_t shift;
	sg_point_t start;
	shift.y = 0;

	step = animation->path.step & ~(SG_ANIMATION_STEP_FLAG);

	if( step < animation->path.step_total ){
		start = animation->start;
		count = sg_animate_calc_count(animation);

		if( animation->path.step & (SG_ANIMATION_STEP_FLAG) ){
			//bounce back phase

			src.x = start.x + animation->path.motion_total - animation->path.motion;
			src.y = start.y;

			dest.x = animation->start.x;
			dest.y = animation->start.y;

			d.w = animation->dim.w - animation->path.motion_total + animation->path.motion;
			d.h = animation->dim.h;

			sg_draw_sub_bitmap(bmap, dest, scratch, src, d);

		} else {
			//bounce phase

			if( step == 0 ){
				//copy the drawing to the scratch area
				sg_draw_sub_bitmap(bmap, animation->start, scratch, animation->start, animation->dim);
			}

			src.x = start.x;
			src.y = start.y;

			d.w = animation->dim.w - animation->path.motion;
			d.h = animation->dim.h;

			shift.y = -1*count;
			sg_transform_shift(bmap, shift, src, d);

			d.w = animation->path.motion + count;
			dest.x = start.x + animation->dim.w - d.w;
			dest.y = start.y;

			//! \todo CHECKERBOAR AREA
			//sg_set_area(bmap, dest, d, 0xAA);

			if( animation->path.step == animation->path.step_total ){
				animation->path.step = SG_ANIMATION_STEP_FLAG;
				animation->path.motion = 0;
			}
		}

		animation->path.motion += count;

		return 1;
	}
	return 0;}

int (* const animations[SG_ANIMATION_TYPE_TOTAL])(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation) = {
		sg_animate_push_left,
		sg_animate_push_right,
		sg_animate_push_up,
		sg_animate_push_down,
		sg_animate_slide_left,
		sg_animate_undo_slide_left,
		sg_animate_slide_right,
		sg_animate_undo_slide_right,
		sg_animate_slide_up,
		sg_animate_undo_slide_up,
		sg_animate_slide_down,
		sg_animate_undo_slide_down,
		sg_animate_none,
		sg_animate_bounce_up,
		sg_animate_bounce_down,
		sg_animate_bounce_left,
		sg_animate_bounce_right
};

int sg_animate(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation){
	if( animation->type < SG_ANIMATION_TYPE_TOTAL ){
		return animations[animation->type](bmap, scratch, animation);
	}
	return -1;
}

int sg_animate_init(sg_animation_t * animation,
		u8 type,
		u8 path,
		u8 step_total,
		sg_size_t motion_total,
		sg_point_t start,
		sg_dim_t dim){
	animation->type = type;
	animation->path.type = path;
	animation->path.step = 0;
	animation->start = start;
	animation->dim = dim;
	animation->path.step_total = step_total;
	animation->path.motion = 0;
	animation->path.motion_total = motion_total;
	animation->path.sum_of_squares = sum_of_squares(step_total);
	return 0;

}
