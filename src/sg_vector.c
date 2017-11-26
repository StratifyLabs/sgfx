//Copyright 2011-2016 Tyler Gilbert; All Rights Reserved

#include <unistd.h>
#include <stdio.h>

#include "sg_config.h"
#include "sg.h"


static void draw_line(const sg_vector_primitive_t * p, sg_bmap_t * bmap, const sg_vector_map_t * map, sg_bounds_t * bounds);
static void draw_arc(const sg_vector_primitive_t * p, sg_bmap_t * bmap, const sg_vector_map_t * map, sg_bounds_t * bounds);
static void draw_quadtratic_bezier(const sg_vector_primitive_t * p, sg_bmap_t * bmap, const sg_vector_map_t * map, sg_bounds_t * bounds);
static void draw_cubic_bezier(const sg_vector_primitive_t * p, sg_bmap_t * bmap, const sg_vector_map_t * map, sg_bounds_t * attr);
static void draw_fill(const sg_vector_primitive_t * p, sg_bmap_t * bmap, const sg_vector_map_t * map, sg_bounds_t * bounds);

static void (*draw_func [SG_TYPE_TOTAL])(const sg_vector_primitive_t * p, sg_bmap_t * bm, const sg_vector_map_t * map, sg_bounds_t * attr) = {
		draw_line,
		draw_arc,
		draw_fill,
		draw_quadtratic_bezier,
		draw_cubic_bezier
};


void sg_vector_draw_primitive(sg_bmap_t * bitmap, const sg_vector_primitive_t * prim, const sg_vector_map_t * map, sg_bounds_t * attr){
	int type;
	type = prim->type & SG_TYPE_MASK;
	if( prim->type & SG_ENABLE_FLAG ){
		if( type < SG_TYPE_TOTAL ){
			draw_func[type](prim, bitmap, map, attr);
		}
	}
}

void sg_vector_draw_icon(sg_bmap_t * bmap, const sg_vector_icon_t * icon, const sg_vector_map_t * map, sg_bounds_t * bounds){
	unsigned int total;
	if( bmap->pen.o_flags & SG_PEN_FLAG_IS_FILL ){
		total = icon->total;
	} else {
		total = icon->total - icon->fill_total;
	}
	sg_vector_draw_primitive_list(bmap, icon->primitives, total, map, bounds);
}

void sg_vector_draw_primitive_list(sg_bmap_t * bitmap, const sg_vector_primitive_t prim_list[], unsigned int total, const sg_vector_map_t * map, sg_bounds_t * bounds){
	unsigned int i;

	if( bounds ){
		bounds->bottom_right.x = -SG_MAX;
		bounds->bottom_right.y = -SG_MAX;
		bounds->top_left.x = SG_MAX;
		bounds->top_left.y = SG_MAX;
	}

	for(i=0; i < total; i++){
		sg_vector_draw_primitive(bitmap, &(prim_list[i]), map, bounds);
	}
}


void draw_line(const sg_vector_primitive_t * p, sg_bmap_t * bmap, const sg_vector_map_t * map, sg_bounds_t * bounds){
	//draw a line from bottom left to top right
	sg_point_t p1;
	sg_point_t p2;

	p1 = p->line.p1;
	p2 = p->line.p2;

	//apply bitmap space rotation
	sg_point_map(&p1, map);
	sg_point_map(&p2, map);

	if( bounds ){
		if( p1.x < bounds->top_left.x ){ bounds->top_left.x = p1.x; }
		if( p1.y < bounds->top_left.y ){ bounds->top_left.y = p1.y; }
		if( p2.x < bounds->top_left.x ){ bounds->top_left.x = p2.x; }
		if( p2.y < bounds->top_left.y ){ bounds->top_left.y = p2.y; }

		if( p1.x > bounds->bottom_right.x ){ bounds->bottom_right.x = p1.x; }
		if( p1.y > bounds->bottom_right.y ){ bounds->bottom_right.y = p1.y; }
		if( p2.x > bounds->bottom_right.x ){ bounds->bottom_right.x = p2.x; }
		if( p2.y > bounds->bottom_right.y ){ bounds->bottom_right.y = p2.y; }
	}

	//add the option to invert the line
	sg_draw_line(bmap, p1, p2);
}

void draw_arc(const sg_vector_primitive_t * p, sg_bmap_t * bmap, const sg_vector_map_t * map, sg_bounds_t * bounds){
	int i;
	int points;
	int step;
	int radians = p->arc.stop - p->arc.start;
	sg_int_t t;
	sg_int_t r;
	sg_size_t unit;
	sg_int_t thick;

	sg_size_t half_thick;
	sg_size_t thickness;


	thickness = bmap->pen.thickness;
	if( thickness == 0 ){
		thickness = 1;
	}

	half_thick = thickness/2;

	sg_point_t pen;
	sg_point_t circum;
	circum.point = 0;

	unit = sg_point_map_pixel_size(map);

	if( p->arc.rx < p->arc.ry ){
		r = p->arc.ry;
	} else {
		r = p->arc.rx;
	}

	r += (thickness-1)*unit;

	sg_point_shift_x(&circum, r);
	sg_point_map(&circum, map);

	points = ((circum.x - map->point.x) * 2 * 320) / 100 * (radians) / SG_TRIG_POINTS;
	if( points > SG_TRIG_POINTS ){
		points = SG_TRIG_POINTS;
	}

	if( points == 0 ){
		points = 1;
	}

	if( radians == 0 ){
		step = 1;
	} else {
		step = points / (radians);
	}

	if( step == 0 ){
		step = 1;
	}

	for(t=0; t < thickness; t++){
		thick = unit * (t - half_thick);
		for(i=p->arc.start; i < p->arc.stop; i+=step){
			pen.point = 0;
			sg_point_arc(&pen, p->arc.rx + thick, p->arc.ry + thick, i);
			sg_point_rotate(&pen, p->arc.rotation);
			sg_point_shift(&pen, p->arc.center);
			sg_point_map(&pen, map);

			if( bounds ){
				if( pen.x < bounds->top_left.x ){ bounds->top_left.x = pen.x; }
				if( pen.y < bounds->top_left.y ){ bounds->top_left.y = pen.y; }
				if( pen.x > bounds->bottom_right.x ){ bounds->bottom_right.x = pen.x; }
				if( pen.y > bounds->bottom_right.y ){ bounds->bottom_right.y = pen.y; }
			}

			sg_draw_pixel(bmap, pen);
		}
	}
}

void draw_quadtratic_bezier(const sg_vector_primitive_t * p, sg_bmap_t * bmap, const sg_vector_map_t * map, sg_bounds_t * bounds){
	int i;
	sg_point_t points[3];

	//maps the points to the bmap
	points[0] = p->quadratic_bezier.p1;
	points[1] = p->quadratic_bezier.p2;
	points[2] = p->quadratic_bezier.p3;

	for(i=0; i < 3; i++){
		sg_point_map(points + i, map);

		if( bounds ){
			if( points[i].x < bounds->top_left.x ){ bounds->top_left.x = points[i].x; }
			if( points[i].y < bounds->top_left.y ){ bounds->top_left.y = points[i].y; }
			if( points[i].x > bounds->bottom_right.x ){ bounds->bottom_right.x = points[i].x; }
			if( points[i].y > bounds->bottom_right.y ){ bounds->bottom_right.y = points[i].y; }
		}
	}

	sg_draw_quadtratic_bezier(bmap, points[0], points[1], points[2]);
}

void draw_cubic_bezier(const sg_vector_primitive_t * p, sg_bmap_t * bmap, const sg_vector_map_t * map, sg_bounds_t * bounds){
	int i;
	sg_point_t points[4];

	//maps the points to the bmap
	points[0] = p->cubic_bezier.p1;
	points[1] = p->cubic_bezier.p2;
	points[2] = p->cubic_bezier.p3;
	points[3] = p->cubic_bezier.p4;

	for(i=0; i < 4; i++){
		sg_point_map(points + i, map);

		if( bounds ){
			if( points[i].x < bounds->top_left.x ){ bounds->top_left.x = points[i].x; }
			if( points[i].y < bounds->top_left.y ){ bounds->top_left.y = points[i].y; }
			if( points[i].x > bounds->bottom_right.x ){ bounds->bottom_right.x = points[i].x; }
			if( points[i].y > bounds->bottom_right.y ){ bounds->bottom_right.y = points[i].y; }
		}
	}

	sg_draw_cubic_bezier(bmap, points[0], points[1], points[2], points[3]);
}

void draw_fill(const sg_vector_primitive_t * p, sg_bmap_t * bmap, const sg_vector_map_t * map, sg_bounds_t * bounds){
	sg_point_t center;
	sg_bounds_t bounds_data;
	if( bounds ){
		bounds_data = *bounds;
	} else {
		bounds_data.top_left.point = 0;
		bounds_data.bottom_right.x = bmap->dim.width - 1;
		bounds_data.bottom_right.y = bmap->dim.height - 1;
	}
	center = p->fill.center;
	sg_point_map(&center, map);
	sg_draw_pour(bmap, center, bounds_data);
}




