//Copyright 2011-2016 Tyler Gilbert; All Rights Reserved


#include <mcu/types.h>
#if !defined __link
#include <mcu/arch.h>
#include <mcu/mcu.h>
#endif

#include <unistd.h>



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
	sg_region_t shift_region;
	sg_point_t shift;
	sg_size_t count;
	sg_region_t draw_src_region;

	if( animation->path.step < animation->path.step_total ){
		count = sg_animate_calc_count(animation);

		if( count == 0 ){
			return 1;
		}

		//start with all points and dims with animation
		shift_region = animation->region;
		draw_src_region = animation->region;
		draw_dest_point = animation->region.point;

		//adjust shift values
		shift_region.area.width -= count;
		shift.x = count;
		shift.y = 0;
		sg_transform_shift(bmap, shift, &shift_region);

		//adjust bitmap draw operations
		draw_src_region.point.x = animation->region.point.x + animation->region.area.width - animation->path.motion - count;
		draw_src_region.area.width = count;
		sg_draw_sub_bitmap(bmap, draw_dest_point, scratch, &draw_src_region);

		animation->path.motion += count;

		return 1;
	}
	return 0;

}

int sg_animate_push_left(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation){
	sg_point_t draw_dest_point;
	sg_region_t shift_region;
	sg_region_t draw_src_region;
	sg_point_t shift;
	sg_size_t count;

	if( animation->path.step < animation->path.step_total ){
		count = sg_animate_calc_count(animation);

		if( count == 0 ){
			return 1;
		}

		//start with all points and dims with animation
		shift_region = animation->region;
		draw_src_region = animation->region;
		draw_dest_point = animation->region.point;

		//adjust shift values
		shift_region.point.x += count;
		shift_region.area.width -= count;
		shift.x = -1*count;
		shift.y = 0;
		sg_transform_shift(bmap, shift, &shift_region);

		//adjust bitmap draw operations
		draw_src_region.point.x = animation->region.point.x + animation->path.motion;
		draw_dest_point.x = animation->region.point.x + animation->region.area.width - count;
		draw_src_region.area.width = count;
		sg_draw_sub_bitmap(bmap, draw_dest_point, scratch, &draw_src_region);

		animation->path.motion += count;

		return 1;
	}
	return 0;
}

int sg_animate_push_up(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation){
	sg_point_t dest_point;
	sg_size_t count;
	sg_point_t shift;
	sg_region_t shift_region;
	sg_region_t draw_src_region;


	if( animation->path.step < animation->path.step_total ){
		count = sg_animate_calc_count(animation);

		if( count ){
			shift.x = 0;
			shift.y = -1*count;

			shift_region.point.x = animation->region.point.x;
			shift_region.point.y = animation->region.point.y + count;
			shift_region.area.width = animation->region.area.width;
			shift_region.area.height = animation->region.area.height - count;
			sg_transform_shift(bmap, shift, &shift_region);

			draw_src_region.area.width = animation->region.area.width;
			draw_src_region.area.height = count;
			draw_src_region.point.x = animation->region.point.x;
			draw_src_region.point.y = animation->region.point.y + animation->path.motion;
			dest_point.x = animation->region.point.x;
			dest_point.y = animation->region.point.y + animation->region.area.height - count;
			sg_draw_sub_bitmap(bmap, dest_point, scratch, &draw_src_region);
		}

		animation->path.motion += count;

		return 1;
	}
	return 0;
}

int sg_animate_push_down(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation){
	sg_point_t dest_point;
	sg_size_t count;
	sg_point_t shift;
	sg_region_t shift_region;
	sg_region_t draw_src_region;


	if( animation->path.step < animation->path.step_total ){
		count = sg_animate_calc_count(animation);

		if( count ){
			shift.x = 0;
			shift.y = count;

			shift_region = animation->region;
			shift_region.area.height = animation->region.area.height - count;
			sg_transform_shift(bmap, shift, &shift_region);


			animation->path.motion += count;

			draw_src_region.area.width = animation->region.area.width;
			draw_src_region.area.height = count;
			draw_src_region.point.x = animation->region.point.x;
			draw_src_region.point.y = animation->region.point.y + animation->region.area.height - animation->path.motion;
			dest_point.x = animation->region.point.x;
			dest_point.y = animation->region.point.y;
			sg_draw_sub_bitmap(bmap, dest_point, scratch, &draw_src_region);
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
	sg_region_t region;

	if( animation->path.step < animation->path.step_total ){
		animation->path.motion += sg_animate_calc_count(animation);

		region.point = animation->region.point;
		region.area.width = animation->region.area.width;
		region.area.height = animation->path.motion;
		dest.x = animation->region.point.x;
		dest.y = animation->region.point.y + animation->region.area.height - animation->path.motion;
		sg_draw_sub_bitmap(bmap, dest, scratch, &region);

		return 1;
	}
	return 0;
}

int sg_animate_undo_slide_up(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation){
	sg_size_t count;
	sg_point_t shift;
	sg_region_t shift_region;
	sg_region_t draw_src_region;

	if( animation->path.step < animation->path.step_total ){
		count = sg_animate_calc_count(animation);

		if( count ){

			shift_region.point.x = animation->region.point.x;
			shift_region.point.y = animation->region.point.y + animation->path.motion;
			shift.x = 0;
			shift.y = count;

			shift_region.area.width = animation->region.area.width;
			shift_region.area.height = animation->region.area.height - (animation->path.motion + count);
			sg_transform_shift(bmap, shift, &shift_region);

			draw_src_region.point.x = animation->region.point.x;
			draw_src_region.point.y = animation->region.point.y + animation->path.motion;

			draw_src_region.area.width = animation->region.area.width;
			draw_src_region.area.height = count;
			sg_draw_sub_bitmap(bmap, draw_src_region.point, scratch, &draw_src_region);

			animation->path.motion += count;

		}

		return 1;
	}
	return 0;
}

int sg_animate_slide_down(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation){
	sg_point_t dest;
	sg_region_t draw_src_region;

	sg_point_t start;

	if( animation->path.step < animation->path.step_total ){
		start = animation->region.point;
		animation->path.motion += sg_animate_calc_count(animation);
		dest.x = start.x;
		dest.y = start.y;

		draw_src_region.point.x = start.x;
		draw_src_region.point.y = start.y + animation->region.area.height - animation->path.motion;

		draw_src_region.area.width = animation->region.area.width;
		draw_src_region.area.height = animation->path.motion;

		sg_draw_sub_bitmap(bmap, dest, scratch, &draw_src_region);
		return 1;
	}
	return 0;
}

int sg_animate_undo_slide_down(sg_bmap_t * bmap, sg_bmap_t * scratch, sg_animation_t * animation){
	sg_size_t count;
	sg_point_t shift;
	sg_region_t shift_region;
	sg_region_t draw_region;

	if( animation->path.step < animation->path.step_total ){
		count = sg_animate_calc_count(animation);

		if( count ){

			animation->path.motion += count;

			shift_region.point.x = animation->region.point.x;
			shift_region.point.y = animation->region.point.y + count;
			shift.x = 0;
			shift.y = -1*count;

			shift_region.area.width = animation->region.area.width;
			shift_region.area.height = animation->region.area.height - (animation->path.motion);
			sg_transform_shift(bmap, shift, &shift_region);

			draw_region.point.x = animation->region.point.x;
			draw_region.point.y = animation->region.point.y + animation->region.area.height - animation->path.motion;

			draw_region.area.width = animation->region.area.width;
			draw_region.area.height = count;
			sg_draw_sub_bitmap(bmap, draw_region.point, scratch, &draw_region);


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
	sg_size_t count;
	u16 step;
	sg_point_t shift;
	sg_region_t region;


	step = animation->path.step & ~(SG_ANIMATION_STEP_FLAG);

	if( step < animation->path.step_total ){
		region.point = animation->region.point;
		count = sg_animate_calc_count(animation);

		if( animation->path.step & (SG_ANIMATION_STEP_FLAG) ){
			//bounce back phase

			dest.x = animation->region.point.x;
			dest.y = animation->region.point.y + animation->path.motion_total - animation->path.motion;

			region.area.width = animation->region.area.width;
			region.area.height = animation->region.area.height - animation->path.motion_total + animation->path.motion;

			sg_draw_sub_bitmap(bmap, dest, scratch, &region);

		} else {
			//bounce phase
			if( step == 0 ){
				sg_draw_sub_bitmap(bmap, animation->region.point, scratch, &animation->region);
			}

			region.point.x = animation->region.point.x;
			region.point.y = animation->region.point.y + animation->path.motion;

			region.area.width = animation->region.area.width;
			region.area.height = animation->region.area.height - animation->path.motion - count;

			dest = animation->region.point;

			shift.x = 0;
			shift.y = count;
			sg_transform_shift(bmap, shift, &region);

			region.area.height = animation->path.motion + count;

			//Checkerboard Area
			region.point = dest;
			sg_draw_pattern(bmap, &region, 0xAAAAAAAA, 0x55555555, 1);


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
	sg_size_t count;
	u16 step;
	sg_point_t shift;
	sg_point_t start;
	shift.x = 0;
	sg_region_t region;

	step = animation->path.step & ~(SG_ANIMATION_STEP_FLAG);

	if( step < animation->path.step_total ){
		start = animation->region.point;
		count = sg_animate_calc_count(animation);

		if( animation->path.step & (SG_ANIMATION_STEP_FLAG) ){
			//bounce back phase

			region.point.x = start.x;
			region.point.y = start.y + animation->path.motion_total - animation->path.motion;

			dest.x = animation->region.point.x;
			dest.y = animation->region.point.y;

			region.area.width = animation->region.area.width;
			region.area.height = animation->region.area.height - animation->path.motion_total + animation->path.motion;

			sg_draw_sub_bitmap(bmap, dest, scratch, &region);

		} else {
			//bounce phase

			if( step == 0 ){
				//copy the drawing to the scratch area
				sg_draw_sub_bitmap(bmap, animation->region.point, scratch, &animation->region);
			}

			region.point = start;

			region.area.width = animation->region.area.width;
			region.area.height = animation->region.area.height - animation->path.motion - count;

			shift.y = -1*count;
			sg_transform_shift(bmap, shift, &region);

			region.area.height = animation->path.motion + count;
			region.point.x = start.x;
			region.point.y = start.y + animation->region.area.height - region.area.height;

			//Checkboard background
			sg_draw_pattern(bmap, &region, 0xAAAAAAAA, 0x55555555, 1);

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
	sg_size_t count;
	u16 step;

	sg_point_t shift;
	sg_region_t region;


	step = animation->path.step & ~(SG_ANIMATION_STEP_FLAG);

	if( step < animation->path.step_total ){
		count = sg_animate_calc_count(animation);

		if( animation->path.step & (SG_ANIMATION_STEP_FLAG) ){

			//bounce back phase
			region.point = animation->region.point;
			region.area.width = animation->region.area.width - animation->path.motion_total + animation->path.motion;
			region.area.height = animation->region.area.height;
			dest.x = animation->region.point.x + animation->path.motion_total - animation->path.motion;
			dest.y = animation->region.point.y;
			sg_draw_sub_bitmap(bmap, dest, scratch, &region);

		} else {
			//bounce phase
			if( step == 0 ){
				sg_draw_sub_bitmap(bmap, animation->region.point, scratch, &animation->region);
			}

			region.point.x = animation->region.point.x + animation->path.motion;
			region.point.y = animation->region.point.y;
			region.area.width = animation->region.area.width - animation->path.motion;
			region.area.height = animation->region.area.height;
			shift.x = count;
			shift.y = 0;
			sg_transform_shift(bmap, shift, &region);

			region.point = animation->region.point;
			region.area.width = animation->path.motion + count;
			sg_draw_pattern(bmap, &region, 0xAAAAAAAA, 0x55555555, 1); //checkerboard area

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
	sg_size_t count;
	u16 step;
	sg_point_t shift;
	sg_region_t region;


	step = animation->path.step & ~(SG_ANIMATION_STEP_FLAG);

	if( step < animation->path.step_total ){
		count = sg_animate_calc_count(animation);

		if( animation->path.step & (SG_ANIMATION_STEP_FLAG) ){
			//bounce back phase

			region.point.x = animation->region.point.x + animation->path.motion_total - animation->path.motion;
			region.point.y = animation->region.point.y;

			dest = animation->region.point;

			region.area.width = animation->region.area.width - animation->path.motion_total + animation->path.motion;
			region.area.height = animation->region.area.height;

			sg_draw_sub_bitmap(bmap, dest, scratch, &region);

		} else {
			//bounce phase

			if( step == 0 ){
				//copy the drawing to the scratch area
				sg_draw_sub_bitmap(bmap, animation->region.point, scratch, &animation->region);
			}

			region.point = animation->region.point;
			region.area.width = animation->region.area.width - animation->path.motion;
			region.area.height = animation->region.area.height;
			shift.x = -1*count;
			shift.y = 0;
			sg_transform_shift(bmap, shift, &region);

			region.area.width = animation->path.motion + count;
			region.point.x = animation->region.point.x + animation->region.area.width - region.area.width;
			region.point.y = animation->region.point.y;

			//Checkboard area
			sg_draw_pattern(bmap, &region, 0xAAAAAAAA, 0x55555555, 1);

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
		sg_area_t area){
	animation->type = type;
	animation->path.type = path;
	animation->path.step = 0;
	animation->region.point = start;
	animation->region.area = area;
	animation->path.step_total = step_total;
	animation->path.motion = 0;
	animation->path.motion_total = motion_total;
	animation->path.sum_of_squares = sum_of_squares(step_total);
	return 0;

}

