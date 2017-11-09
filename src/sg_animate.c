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
	sg_point_t draw_dest_point;
	sg_point_t draw_src_point;
	sg_point_t shift_point;
	sg_dim_t shift_dim;
	sg_dim_t draw_dim;
	sg_point_t shift;
	sg_size_t count;

	if( animation->path.step < animation->path.step_total ){
		count = sg_animate_calc_count(animation);

		if( count == 0 ){
			return 1;
		}

		//start with all points and dims with animation
		shift_point = animation->start;
		shift_dim = animation->dim;
		draw_src_point = animation->start;
		draw_dest_point = animation->start;
		draw_dim = animation->dim;

		//adjust shift values
		shift_dim.width -= count;
		shift.x = count;
		shift.y = 0;
		sg_transform_shift(bmap, shift, shift_point, shift_dim);

		//adjust bitmap draw operations
		draw_src_point.x = animation->start.x + animation->dim.width - animation->path.motion - count;
		draw_dim.width = count;
		sg_draw_sub_bitmap(bmap, draw_dest_point, scratch, draw_src_point, draw_dim);

		animation->path.motion += count;

		return 1;
	}
	return 0;

}

int sg_animate_push_left(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation){
	sg_point_t draw_dest_point;
	sg_point_t draw_src_point;
	sg_point_t shift_point;
	sg_dim_t shift_dim;
	sg_dim_t draw_dim;
	sg_point_t shift;
	sg_size_t count;

	if( animation->path.step < animation->path.step_total ){
		count = sg_animate_calc_count(animation);

		if( count == 0 ){
			return 1;
		}

		//start with all points and dims with animation
		shift_point = animation->start;
		shift_dim = animation->dim;
		draw_src_point = animation->start;
		draw_dest_point = animation->start;
		draw_dim = animation->dim;

		//adjust shift values
		shift_point.x += count;
		shift_dim.width -= count;
		shift.x = -1*count;
		shift.y = 0;
		sg_transform_shift(bmap, shift, shift_point, shift_dim);

		//adjust bitmap draw operations
		draw_src_point.x = animation->start.x + animation->path.motion;
		draw_dest_point.x = animation->start.x + animation->dim.width - count;
		draw_dim.width = count;
		sg_draw_sub_bitmap(bmap, draw_dest_point, scratch, draw_src_point, draw_dim);

		animation->path.motion += count;

		return 1;
	}
	return 0;
}

int sg_animate_push_up(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation){
	sg_point_t dest_point;
	sg_point_t src_point;
	sg_dim_t draw_dim;
	sg_size_t count;
	sg_point_t shift;
	sg_point_t shift_point;
	sg_dim_t shift_dim;


	if( animation->path.step < animation->path.step_total ){
		count = sg_animate_calc_count(animation);

		if( count ){
			shift.x = 0;
			shift.y = -1*count;

			shift_point.x = animation->start.x;
			shift_point.y = animation->start.y + count;
			shift_dim.width = animation->dim.width;
			shift_dim.height = animation->dim.height - count;
			sg_transform_shift(bmap, shift, shift_point, shift_dim);

			draw_dim.width = animation->dim.width;
			draw_dim.height = count;
			src_point.x = animation->start.x;
			src_point.y = animation->start.y + animation->path.motion;
			dest_point.x = animation->start.x;
			dest_point.y = animation->start.y + animation->dim.height - count;
			sg_draw_sub_bitmap(bmap, dest_point, scratch, src_point, draw_dim);
		}

		animation->path.motion += count;

		return 1;
	}
	return 0;
}

int sg_animate_push_down(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation){
	sg_point_t dest_point;
	sg_point_t src_point;
	sg_dim_t draw_dim;
	sg_size_t count;
	sg_point_t shift;
	sg_point_t shift_point;
	sg_dim_t shift_dim;


	if( animation->path.step < animation->path.step_total ){
		count = sg_animate_calc_count(animation);

		if( count ){
			shift.x = 0;
			shift.y = count;

			shift_point.x = animation->start.x;
			shift_point.y = animation->start.y;
			shift_dim.width = animation->dim.width;
			shift_dim.height = animation->dim.height - count;
			sg_transform_shift(bmap, shift, shift_point, shift_dim);


			animation->path.motion += count;

			draw_dim.width = animation->dim.width;
			draw_dim.height = count;
			src_point.x = animation->start.x;
			src_point.y = animation->start.y + animation->dim.height - animation->path.motion;
			dest_point.x = animation->start.x;
			dest_point.y = animation->start.y;
			sg_draw_sub_bitmap(bmap, dest_point, scratch, src_point, draw_dim);
		}


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
		dest.y = start.y + animation->dim.height - animation->path.motion;

		d.width = animation->dim.width;
		d.height = animation->path.motion;

		sg_draw_sub_bitmap(bmap, dest, scratch, src, d);


		return 1;
	}
	return 0;
}

int sg_animate_undo_slide_up(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation){
	sg_point_t draw_point;
	sg_dim_t draw_dim;
	sg_size_t count;
	sg_point_t shift_point;
	sg_point_t shift_start;
	sg_dim_t shift_dim;

	if( animation->path.step < animation->path.step_total ){
		count = sg_animate_calc_count(animation);

		if( count ){

			shift_start.x = animation->start.x;
			shift_start.y = animation->start.y + animation->path.motion;
			shift_point.x = 0;
			shift_point.y = count;

			shift_dim.width = animation->dim.width;
			shift_dim.height = animation->dim.height - (animation->path.motion + count);
			sg_transform_shift(bmap, shift_point, shift_start, shift_dim);

			draw_point.x = animation->start.x;
			draw_point.y = animation->start.y + animation->path.motion;

			draw_dim.width = animation->dim.width;
			draw_dim.height = count;
			sg_draw_sub_bitmap(bmap, draw_point, scratch, draw_point, draw_dim);

			animation->path.motion += count;

		}

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
		src.y = start.y + animation->dim.height - animation->path.motion;

		d.width = animation->dim.width;
		d.height = animation->path.motion;

		sg_draw_sub_bitmap(bmap, dest, scratch, src, d);
		return 1;
	}
	return 0;
}

int sg_animate_undo_slide_down(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation){
#if 0
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
		src.y = start.y + animation->dim.height - animation->path.motion;

		dest = src;

		d.width = animation->dim.width;
		d.height = animation->dim.height - animation->path.motion + count;

		shift.y = -1*count;
		sg_transform_shift(bmap, shift, start, d);

		d.height = count;

		sg_draw_sub_bitmap(bmap, dest, scratch, src, d);

		return 1;
	}
	return 0;
#endif

	sg_point_t draw_point;
	sg_dim_t draw_dim;
	sg_size_t count;
	sg_point_t shift_point;
	sg_point_t shift_start;
	sg_dim_t shift_dim;

	if( animation->path.step < animation->path.step_total ){
		count = sg_animate_calc_count(animation);

		if( count ){

			animation->path.motion += count;

			shift_start.x = animation->start.x;
			shift_start.y = animation->start.y + count;
			shift_point.x = 0;
			shift_point.y = -1*count;

			shift_dim.width = animation->dim.width;
			shift_dim.height = animation->dim.height - (animation->path.motion);
			sg_transform_shift(bmap, shift_point, shift_start, shift_dim);

			draw_point.x = animation->start.x;
			draw_point.y = animation->start.y + animation->dim.height - animation->path.motion;

			draw_dim.width = animation->dim.width;
			draw_dim.height = count;
			sg_draw_sub_bitmap(bmap, draw_point, scratch, draw_point, draw_dim);


		}

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

			d.width = animation->dim.width;
			d.height = animation->dim.height - animation->path.motion_total + animation->path.motion;

			sg_draw_sub_bitmap(bmap, dest, scratch, start, d);

		} else {
			//bounce phase
			if( step == 0 ){
				sg_draw_sub_bitmap(bmap, animation->start, scratch, animation->start, animation->dim);
			}

			src.x = start.x;
			src.y = start.y + animation->path.motion;

			d.width = animation->dim.width;
			d.height = animation->dim.height - animation->path.motion - count;

			dest.x = start.x;
			dest.y = start.y;

			shift.y = count;
			sg_transform_shift(bmap, shift, src, d);

			d.height = animation->path.motion + count;

			//Checkerboard Area
			sg_draw_pattern(bmap, dest, d, 0xAAAAAAAA, 0x55555555, 1);


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

			d.width = animation->dim.width;
			d.height = animation->dim.height - animation->path.motion_total + animation->path.motion;

			sg_draw_sub_bitmap(bmap, dest, scratch, src, d);

		} else {
			//bounce phase

			if( step == 0 ){
				//copy the drawing to the scratch area
				sg_draw_sub_bitmap(bmap, animation->start, scratch, animation->start, animation->dim);
			}

			src.x = start.x;
			src.y = start.y;

			d.width = animation->dim.width;
			d.height = animation->dim.height - animation->path.motion - count;

			shift.y = -1*count;
			sg_transform_shift(bmap, shift, src, d);

			d.height = animation->path.motion + count;
			dest.x = start.x;
			dest.y = start.y + animation->dim.height - d.height;

			//Checkboard background
			sg_draw_pattern(bmap, dest, d, 0xAAAAAAAA, 0x55555555, 1);

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

			d.width = animation->dim.width - animation->path.motion_total + animation->path.motion;
			d.height = animation->dim.height;

			sg_draw_sub_bitmap(bmap, dest, scratch, start, d);

		} else {
			//bounce phase
			if( step == 0 ){
				sg_draw_sub_bitmap(bmap, animation->start, scratch, animation->start, animation->dim);
			}

			src.x = start.x + animation->path.motion;
			src.y = start.y;

			d.width = animation->dim.width - animation->path.motion;
			d.height = animation->dim.height;

			dest.x = start.x;
			dest.y = start.y;

			shift.x = count;
			sg_transform_shift(bmap, shift, src, d);

			d.width = animation->path.motion + count;

			//checkerboard area
			sg_draw_pattern(bmap, dest, d, 0xAAAAAAAA, 0x55555555, 1);

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

			d.width = animation->dim.width - animation->path.motion_total + animation->path.motion;
			d.height = animation->dim.height;

			sg_draw_sub_bitmap(bmap, dest, scratch, src, d);

		} else {
			//bounce phase

			if( step == 0 ){
				//copy the drawing to the scratch area
				sg_draw_sub_bitmap(bmap, animation->start, scratch, animation->start, animation->dim);
			}

			src.x = start.x;
			src.y = start.y;

			d.width = animation->dim.width - animation->path.motion;
			d.height = animation->dim.height;

			shift.x = -1*count;
			sg_transform_shift(bmap, shift, src, d);

			d.width = animation->path.motion + count;
			dest.x = start.x + animation->dim.width - d.width;
			dest.y = start.y;

			//Checkboard area
			sg_draw_pattern(bmap, dest, d, 0xAAAAAAAA, 0x55555555, 1);

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

