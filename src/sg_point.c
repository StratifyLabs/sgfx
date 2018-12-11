//Copyright 2011-2016 Tyler Gilbert; All Rights Reserved

#include "sg_config.h"
#include "sg.h"

typedef struct {
	s16 cosine;
	s16 sine;
} trig_t;

static const trig_t trig_table[SG_TRIG_POINTS] = {
		{ 32767, 0 },
		{ 32764, 402 },
		{ 32757, 804 },
		{ 32744, 1206 },
		{ 32727, 1607 },
		{ 32705, 2009 },
		{ 32678, 2410 },
		{ 32646, 2811 },
		{ 32609, 3211 },
		{ 32567, 3611 },
		{ 32520, 4011 },
		{ 32468, 4409 },
		{ 32412, 4807 },
		{ 32350, 5205 },
		{ 32284, 5601 },
		{ 32213, 5997 },
		{ 32137, 6392 },
		{ 32056, 6786 },
		{ 31970, 7179 },
		{ 31880, 7571 },
		{ 31785, 7961 },
		{ 31684, 8351 },
		{ 31580, 8739 },
		{ 31470, 9126 },
		{ 31356, 9511 },
		{ 31236, 9895 },
		{ 31113, 10278 },
		{ 30984, 10659 },
		{ 30851, 11038 },
		{ 30713, 11416 },
		{ 30571, 11792 },
		{ 30424, 12166 },
		{ 30272, 12539 },
		{ 30116, 12909 },
		{ 29955, 13278 },
		{ 29790, 13645 },
		{ 29621, 14009 },
		{ 29446, 14372 },
		{ 29268, 14732 },
		{ 29085, 15090 },
		{ 28897, 15446 },
		{ 28706, 15799 },
		{ 28510, 16150 },
		{ 28309, 16499 },
		{ 28105, 16845 },
		{ 27896, 17189 },
		{ 27683, 17530 },
		{ 27466, 17868 },
		{ 27244, 18204 },
		{ 27019, 18537 },
		{ 26789, 18867 },
		{ 26556, 19194 },
		{ 26318, 19519 },
		{ 26077, 19840 },
		{ 25831, 20159 },
		{ 25582, 20474 },
		{ 25329, 20787 },
		{ 25072, 21096 },
		{ 24811, 21402 },
		{ 24546, 21705 },
		{ 24278, 22004 },
		{ 24006, 22301 },
		{ 23731, 22594 },
		{ 23452, 22883 },
		{ 23169, 23169 },
		{ 22883, 23452 },
		{ 22594, 23731 },
		{ 22301, 24006 },
		{ 22004, 24278 },
		{ 21705, 24546 },
		{ 21402, 24811 },
		{ 21096, 25072 },
		{ 20787, 25329 },
		{ 20474, 25582 },
		{ 20159, 25831 },
		{ 19840, 26077 },
		{ 19519, 26318 },
		{ 19194, 26556 },
		{ 18867, 26789 },
		{ 18537, 27019 },
		{ 18204, 27244 },
		{ 17868, 27466 },
		{ 17530, 27683 },
		{ 17189, 27896 },
		{ 16845, 28105 },
		{ 16499, 28309 },
		{ 16150, 28510 },
		{ 15799, 28706 },
		{ 15446, 28897 },
		{ 15090, 29085 },
		{ 14732, 29268 },
		{ 14372, 29446 },
		{ 14009, 29621 },
		{ 13645, 29790 },
		{ 13278, 29955 },
		{ 12909, 30116 },
		{ 12539, 30272 },
		{ 12166, 30424 },
		{ 11792, 30571 },
		{ 11416, 30713 },
		{ 11038, 30851 },
		{ 10659, 30984 },
		{ 10278, 31113 },
		{ 9895, 31236 },
		{ 9511, 31356 },
		{ 9126, 31470 },
		{ 8739, 31580 },
		{ 8351, 31684 },
		{ 7961, 31785 },
		{ 7571, 31880 },
		{ 7179, 31970 },
		{ 6786, 32056 },
		{ 6392, 32137 },
		{ 5997, 32213 },
		{ 5601, 32284 },
		{ 5205, 32350 },
		{ 4807, 32412 },
		{ 4409, 32468 },
		{ 4011, 32520 },
		{ 3611, 32567 },
		{ 3211, 32609 },
		{ 2811, 32646 },
		{ 2410, 32678 },
		{ 2009, 32705 },
		{ 1607, 32727 },
		{ 1206, 32744 },
		{ 804, 32757 },
		{ 402, 32764 },
		{ 0, 32767 },
		{ -402, 32764 },
		{ -804, 32757 },
		{ -1206, 32744 },
		{ -1607, 32727 },
		{ -2009, 32705 },
		{ -2410, 32678 },
		{ -2811, 32646 },
		{ -3211, 32609 },
		{ -3611, 32567 },
		{ -4011, 32520 },
		{ -4409, 32468 },
		{ -4807, 32412 },
		{ -5205, 32350 },
		{ -5601, 32284 },
		{ -5997, 32213 },
		{ -6392, 32137 },
		{ -6786, 32056 },
		{ -7179, 31970 },
		{ -7571, 31880 },
		{ -7961, 31785 },
		{ -8351, 31684 },
		{ -8739, 31580 },
		{ -9126, 31470 },
		{ -9511, 31356 },
		{ -9895, 31236 },
		{ -10278, 31113 },
		{ -10659, 30984 },
		{ -11038, 30851 },
		{ -11416, 30713 },
		{ -11792, 30571 },
		{ -12166, 30424 },
		{ -12539, 30272 },
		{ -12909, 30116 },
		{ -13278, 29955 },
		{ -13645, 29790 },
		{ -14009, 29621 },
		{ -14372, 29446 },
		{ -14732, 29268 },
		{ -15090, 29085 },
		{ -15446, 28897 },
		{ -15799, 28706 },
		{ -16150, 28510 },
		{ -16499, 28309 },
		{ -16845, 28105 },
		{ -17189, 27896 },
		{ -17530, 27683 },
		{ -17868, 27466 },
		{ -18204, 27244 },
		{ -18537, 27019 },
		{ -18867, 26789 },
		{ -19194, 26556 },
		{ -19519, 26318 },
		{ -19840, 26077 },
		{ -20159, 25831 },
		{ -20474, 25582 },
		{ -20787, 25329 },
		{ -21096, 25072 },
		{ -21402, 24811 },
		{ -21705, 24546 },
		{ -22004, 24278 },
		{ -22301, 24006 },
		{ -22594, 23731 },
		{ -22883, 23452 },
		{ -23169, 23169 },
		{ -23452, 22883 },
		{ -23731, 22594 },
		{ -24006, 22301 },
		{ -24278, 22004 },
		{ -24546, 21705 },
		{ -24811, 21402 },
		{ -25072, 21096 },
		{ -25329, 20787 },
		{ -25582, 20474 },
		{ -25831, 20159 },
		{ -26077, 19840 },
		{ -26318, 19519 },
		{ -26556, 19194 },
		{ -26789, 18867 },
		{ -27019, 18537 },
		{ -27244, 18204 },
		{ -27466, 17868 },
		{ -27683, 17530 },
		{ -27896, 17189 },
		{ -28105, 16845 },
		{ -28309, 16499 },
		{ -28510, 16150 },
		{ -28706, 15799 },
		{ -28897, 15446 },
		{ -29085, 15090 },
		{ -29268, 14732 },
		{ -29446, 14372 },
		{ -29621, 14009 },
		{ -29790, 13645 },
		{ -29955, 13278 },
		{ -30116, 12909 },
		{ -30272, 12539 },
		{ -30424, 12166 },
		{ -30571, 11792 },
		{ -30713, 11416 },
		{ -30851, 11038 },
		{ -30984, 10659 },
		{ -31113, 10278 },
		{ -31236, 9895 },
		{ -31356, 9511 },
		{ -31470, 9126 },
		{ -31580, 8739 },
		{ -31684, 8351 },
		{ -31785, 7961 },
		{ -31880, 7571 },
		{ -31970, 7179 },
		{ -32056, 6786 },
		{ -32137, 6392 },
		{ -32213, 5997 },
		{ -32284, 5601 },
		{ -32350, 5205 },
		{ -32412, 4807 },
		{ -32468, 4409 },
		{ -32520, 4011 },
		{ -32567, 3611 },
		{ -32609, 3211 },
		{ -32646, 2811 },
		{ -32678, 2410 },
		{ -32705, 2009 },
		{ -32727, 1607 },
		{ -32744, 1206 },
		{ -32757, 804 },
		{ -32764, 402 },
		{ -32767, 0 },
		{ -32764, -402 },
		{ -32757, -804 },
		{ -32744, -1206 },
		{ -32727, -1607 },
		{ -32705, -2009 },
		{ -32678, -2410 },
		{ -32646, -2811 },
		{ -32609, -3211 },
		{ -32567, -3611 },
		{ -32520, -4011 },
		{ -32468, -4409 },
		{ -32412, -4807 },
		{ -32350, -5205 },
		{ -32284, -5601 },
		{ -32213, -5997 },
		{ -32137, -6392 },
		{ -32056, -6786 },
		{ -31970, -7179 },
		{ -31880, -7571 },
		{ -31785, -7961 },
		{ -31684, -8351 },
		{ -31580, -8739 },
		{ -31470, -9126 },
		{ -31356, -9511 },
		{ -31236, -9895 },
		{ -31113, -10278 },
		{ -30984, -10659 },
		{ -30851, -11038 },
		{ -30713, -11416 },
		{ -30571, -11792 },
		{ -30424, -12166 },
		{ -30272, -12539 },
		{ -30116, -12909 },
		{ -29955, -13278 },
		{ -29790, -13645 },
		{ -29621, -14009 },
		{ -29446, -14372 },
		{ -29268, -14732 },
		{ -29085, -15090 },
		{ -28897, -15446 },
		{ -28706, -15799 },
		{ -28510, -16150 },
		{ -28309, -16499 },
		{ -28105, -16845 },
		{ -27896, -17189 },
		{ -27683, -17530 },
		{ -27466, -17868 },
		{ -27244, -18204 },
		{ -27019, -18537 },
		{ -26789, -18867 },
		{ -26556, -19194 },
		{ -26318, -19519 },
		{ -26077, -19840 },
		{ -25831, -20159 },
		{ -25582, -20474 },
		{ -25329, -20787 },
		{ -25072, -21096 },
		{ -24811, -21402 },
		{ -24546, -21705 },
		{ -24278, -22004 },
		{ -24006, -22301 },
		{ -23731, -22594 },
		{ -23452, -22883 },
		{ -23169, -23169 },
		{ -22883, -23452 },
		{ -22594, -23731 },
		{ -22301, -24006 },
		{ -22004, -24278 },
		{ -21705, -24546 },
		{ -21402, -24811 },
		{ -21096, -25072 },
		{ -20787, -25329 },
		{ -20474, -25582 },
		{ -20159, -25831 },
		{ -19840, -26077 },
		{ -19519, -26318 },
		{ -19194, -26556 },
		{ -18867, -26789 },
		{ -18537, -27019 },
		{ -18204, -27244 },
		{ -17868, -27466 },
		{ -17530, -27683 },
		{ -17189, -27896 },
		{ -16845, -28105 },
		{ -16499, -28309 },
		{ -16150, -28510 },
		{ -15799, -28706 },
		{ -15446, -28897 },
		{ -15090, -29085 },
		{ -14732, -29268 },
		{ -14372, -29446 },
		{ -14009, -29621 },
		{ -13645, -29790 },
		{ -13278, -29955 },
		{ -12909, -30116 },
		{ -12539, -30272 },
		{ -12166, -30424 },
		{ -11792, -30571 },
		{ -11416, -30713 },
		{ -11038, -30851 },
		{ -10659, -30984 },
		{ -10278, -31113 },
		{ -9895, -31236 },
		{ -9511, -31356 },
		{ -9126, -31470 },
		{ -8739, -31580 },
		{ -8351, -31684 },
		{ -7961, -31785 },
		{ -7571, -31880 },
		{ -7179, -31970 },
		{ -6786, -32056 },
		{ -6392, -32137 },
		{ -5997, -32213 },
		{ -5601, -32284 },
		{ -5205, -32350 },
		{ -4807, -32412 },
		{ -4409, -32468 },
		{ -4011, -32520 },
		{ -3611, -32567 },
		{ -3211, -32609 },
		{ -2811, -32646 },
		{ -2410, -32678 },
		{ -2009, -32705 },
		{ -1607, -32727 },
		{ -1206, -32744 },
		{ -804, -32757 },
		{ -402, -32764 },
		{ 0, -32767 },
		{ 402, -32764 },
		{ 804, -32757 },
		{ 1206, -32744 },
		{ 1607, -32727 },
		{ 2009, -32705 },
		{ 2410, -32678 },
		{ 2811, -32646 },
		{ 3211, -32609 },
		{ 3611, -32567 },
		{ 4011, -32520 },
		{ 4409, -32468 },
		{ 4807, -32412 },
		{ 5205, -32350 },
		{ 5601, -32284 },
		{ 5997, -32213 },
		{ 6392, -32137 },
		{ 6786, -32056 },
		{ 7179, -31970 },
		{ 7571, -31880 },
		{ 7961, -31785 },
		{ 8351, -31684 },
		{ 8739, -31580 },
		{ 9126, -31470 },
		{ 9511, -31356 },
		{ 9895, -31236 },
		{ 10278, -31113 },
		{ 10659, -30984 },
		{ 11038, -30851 },
		{ 11416, -30713 },
		{ 11792, -30571 },
		{ 12166, -30424 },
		{ 12539, -30272 },
		{ 12909, -30116 },
		{ 13278, -29955 },
		{ 13645, -29790 },
		{ 14009, -29621 },
		{ 14372, -29446 },
		{ 14732, -29268 },
		{ 15090, -29085 },
		{ 15446, -28897 },
		{ 15799, -28706 },
		{ 16150, -28510 },
		{ 16499, -28309 },
		{ 16845, -28105 },
		{ 17189, -27896 },
		{ 17530, -27683 },
		{ 17868, -27466 },
		{ 18204, -27244 },
		{ 18537, -27019 },
		{ 18867, -26789 },
		{ 19194, -26556 },
		{ 19519, -26318 },
		{ 19840, -26077 },
		{ 20159, -25831 },
		{ 20474, -25582 },
		{ 20787, -25329 },
		{ 21096, -25072 },
		{ 21402, -24811 },
		{ 21705, -24546 },
		{ 22004, -24278 },
		{ 22301, -24006 },
		{ 22594, -23731 },
		{ 22883, -23452 },
		{ 23169, -23169 },
		{ 23452, -22883 },
		{ 23731, -22594 },
		{ 24006, -22301 },
		{ 24278, -22004 },
		{ 24546, -21705 },
		{ 24811, -21402 },
		{ 25072, -21096 },
		{ 25329, -20787 },
		{ 25582, -20474 },
		{ 25831, -20159 },
		{ 26077, -19840 },
		{ 26318, -19519 },
		{ 26556, -19194 },
		{ 26789, -18867 },
		{ 27019, -18537 },
		{ 27244, -18204 },
		{ 27466, -17868 },
		{ 27683, -17530 },
		{ 27896, -17189 },
		{ 28105, -16845 },
		{ 28309, -16499 },
		{ 28510, -16150 },
		{ 28706, -15799 },
		{ 28897, -15446 },
		{ 29085, -15090 },
		{ 29268, -14732 },
		{ 29446, -14372 },
		{ 29621, -14009 },
		{ 29790, -13645 },
		{ 29955, -13278 },
		{ 30116, -12909 },
		{ 30272, -12539 },
		{ 30424, -12166 },
		{ 30571, -11792 },
		{ 30713, -11416 },
		{ 30851, -11038 },
		{ 30984, -10659 },
		{ 31113, -10278 },
		{ 31236, -9895 },
		{ 31356, -9511 },
		{ 31470, -9126 },
		{ 31580, -8739 },
		{ 31684, -8351 },
		{ 31785, -7961 },
		{ 31880, -7571 },
		{ 31970, -7179 },
		{ 32056, -6786 },
		{ 32137, -6392 },
		{ 32213, -5997 },
		{ 32284, -5601 },
		{ 32350, -5205 },
		{ 32412, -4807 },
		{ 32468, -4409 },
		{ 32520, -4011 },
		{ 32567, -3611 },
		{ 32609, -3211 },
		{ 32646, -2811 },
		{ 32678, -2410 },
		{ 32705, -2009 },
		{ 32727, -1607 },
		{ 32744, -1206 },
		{ 32757, -804 },
		{ 32764, -402 }
};

static int sign_value(int value){
	return (value > 0) - (value < 0);
}


void sg_point_set(sg_point_t * d, sg_point_t p){
	d->x = p.x;
	d->y = p.y;
}

void sg_point_map(sg_point_t * d, const sg_vector_map_t * m){
	sg_point_rotate(d, m->rotation);
	//map to the space
	s32 tmp;
	tmp = ((d->x + SG_MAX) * m->region.area.width + SG_MAX) / (SG_MAX-SG_MIN);
	d->x = m->region.point.x + tmp;
	tmp = ((d->y + SG_MAX) * m->region.area.height + SG_MAX) / (SG_MAX-SG_MIN);
	d->y = m->region.point.y + tmp;
}

void sg_point_unmap(sg_point_t * d, const sg_vector_map_t * m){
	sg_point_rotate(d, (SG_TRIG_POINTS - m->rotation) % SG_TRIG_POINTS);
	//api::SgfxObject::api()->point_rotate(d, (SG_TRIG_POINTS - m->rotation) % SG_TRIG_POINTS);
	//map to the space
	s32 tmp_x;
	s32 tmp_y;
	//x' = m->region.point.x + ((x + SG_MAX) * m->region.area.width) / (SG_MAX-SG_MIN);
	//y' = m->region.point.y + ((y + SG_MAX) * m->region.area.height) / (SG_MAX-SG_MIN);

	//(x' - m->region.point.x)*(SG_MAX-SG_MIN)/ m->region.area.width - SG_MAX = x
	//printer().message("%d - %d * %d / %d", d->x, m->region.point.x, (SG_MAX-SG_MIN), m->region.area.width);
	tmp_x = ((d->x - m->region.point.x)*(SG_MAX-SG_MIN) + m->region.area.width/2) / m->region.area.width;
	tmp_y = ((d->y - m->region.point.y)*(SG_MAX-SG_MIN) + m->region.area.height/2) / m->region.area.height;

	d->x = tmp_x - SG_MAX,
	d->y = tmp_y - SG_MAX;
}

sg_size_t sg_point_map_pixel_size(const sg_vector_map_t * m){
	sg_size_t p;
	sg_size_t max = m->region.area.width > m->region.area.height ? m->region.area.width : m->region.area.height;
	s32 tmp;
	//how many map space units maps to one pixel
	tmp = ((SG_MAX-SG_MIN) + (max>>1)) / max;
	p = tmp;
	return p;
}


void sg_point_add(sg_point_t * d, const sg_point_t * a){
	sg_point_shift(d, *a);
}

void sg_point_subtract(sg_point_t * d, const sg_point_t * a){
	d->x -= a->x;
	d->y -= a->y;
}

void sg_point_arc(sg_point_t * d, sg_size_t rx, sg_size_t ry, s16 angle){
	int x, y;
	int rc, rs;
	int tmp;
	angle = angle % SG_TRIG_POINTS;
	rc = trig_table[angle].cosine;
	rs = trig_table[angle].sine;
	tmp = rx*rc;
	x = (tmp + sign_value(tmp)*SG_MAX/2)/SG_MAX;
	tmp = ry*rs;
	y = (tmp + sign_value(tmp)*SG_MAX/2)/SG_MAX;
	d->x = x;
	d->y = y;
}

void sg_point_rotate(sg_point_t * d, s16 angle){
	int x, y;
	int rc, rs;
	int tmp;
	if( angle < 0 ){
		angle += SG_TRIG_POINTS;
	}
	angle = angle % SG_TRIG_POINTS;
	if( angle == 0 ){
		return; //nothing to rotate
	}
	rc = trig_table[angle].cosine;
	rs = trig_table[angle].sine;
	tmp = (int)d->x * rc - (int)d->y * rs;
	x = (tmp + sign_value(tmp)*SG_MAX/2) / SG_MAX;
	tmp = (int)d->x * rs + (int)d->y * rc;
	y = (tmp + sign_value(tmp)*SG_MAX/2) / SG_MAX;
	d->x = x;
	d->y = y;
}

void sg_point_scale(sg_point_t * d, u16 a){
	d->x *= a;
	d->y *= a;
}

void sg_point_shift(sg_point_t * d, sg_point_t p){
	d->x += p.x;
	d->y += p.y;
}

void sg_point_shift_x(sg_point_t * d, sg_int_t a){
	d->x += a;
}

void sg_point_shift_y(sg_point_t * d, sg_int_t a){
	d->y += a;
}

void sg_point_bound(const sg_bmap_t * bmap, sg_point_t * p){
#if defined __link
	sg_int_t t = p->x;
	if( t < 0 ){
		t = 0;
	}

	if( t >= bmap->area.width ){
		t = bmap->area.width - 1 ;
	}

	p->x = t;

	t = p->y;
	if( t < 0 ){
		t = 0;
	}

	if( t >= bmap->area.height ){
		t = bmap->area.height - 1 ;
	}

	p->y = t;

#else
	sg_point_bound_x(bmap, &(p->x));
	sg_point_bound_y(bmap, &(p->y));
#endif
}

void sg_point_bound_x(const sg_bmap_t * bmap, sg_int_t * x){
	sg_int_t t = *x;
	if( t < 0 ){
		t = 0;
	}

	if( t >= bmap->area.width ){
		t = bmap->area.width - 1 ;
	}

	*x = t;
}

void sg_point_bound_y(const sg_bmap_t * bmap, sg_int_t * y){
	sg_int_t t = *y;
	if( t < 0 ){
		t = 0;
	}

	if( t >= bmap->area.height ){
		t = bmap->area.height - 1 ;
	}

	*y = t;
}


