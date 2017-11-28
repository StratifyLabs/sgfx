//Copyright 2011-2016 Tyler Gilbert; All Rights Reserved

#include <unistd.h>
#include <stdio.h>

#include "sg_config.h"
#include "sg.h"


static void update_bounds(sg_point_t min, sg_point_t max, sg_region_t * region);

static void draw_line(const sg_vector_primitive_t * p, sg_bmap_t * bmap, const sg_vector_map_t * map, sg_region_t * region);
static void draw_arc(const sg_vector_primitive_t * p, sg_bmap_t * bmap, const sg_vector_map_t * map, sg_region_t * region);
static void draw_quadtratic_bezier(const sg_vector_primitive_t * p, sg_bmap_t * bmap, const sg_vector_map_t * map, sg_region_t * region);
static void draw_cubic_bezier(const sg_vector_primitive_t * p, sg_bmap_t * bmap, const sg_vector_map_t * map, sg_region_t * region);
static void draw_fill(const sg_vector_primitive_t * p, sg_bmap_t * bmap, const sg_vector_map_t * map, sg_region_t * region);

static void (*draw_func [SG_TYPE_TOTAL])(const sg_vector_primitive_t * p, sg_bmap_t * bm, const sg_vector_map_t * map, sg_region_t * region) = {
		draw_line,
		draw_arc,
		draw_fill,
		draw_quadtratic_bezier,
		draw_cubic_bezier
};


void sg_vector_draw_primitive(sg_bmap_t * bitmap, const sg_vector_primitive_t * prim, const sg_vector_map_t * map, sg_region_t * region){
	int type;
	type = prim->type & SG_TYPE_MASK;
	if( prim->type & SG_ENABLE_FLAG ){
		if( type < SG_TYPE_TOTAL ){
			draw_func[type](prim, bitmap, map, region);
		}
	}
}

void sg_vector_draw_icon(sg_bmap_t * bmap, const sg_vector_icon_t * icon, const sg_vector_map_t * map, sg_region_t * region){
	unsigned int total;
	if( bmap->pen.o_flags & SG_PEN_FLAG_IS_FILL ){
		total = icon->total;
	} else {
		total = icon->total - icon->fill_total;
	}
	sg_vector_draw_primitive_list(bmap, icon->primitives, total, map, region);
}

void sg_vector_draw_primitive_list(sg_bmap_t * bitmap, const sg_vector_primitive_t prim_list[], unsigned int total, const sg_vector_map_t * map, sg_region_t * region){
	unsigned int i;

	if( region ){
		region->dim.width = 0;
		region->dim.height = 0;
		region->point.x = SG_MAX;
		region->point.y = SG_MAX;
	}

	for(i=0; i < total; i++){
		sg_vector_draw_primitive(bitmap, &(prim_list[i]), map, region);
	}
}


void draw_line(const sg_vector_primitive_t * p, sg_bmap_t * bmap, const sg_vector_map_t * map, sg_region_t * region){
	//draw a line from bottom left to top right
	sg_point_t p1;
	sg_point_t p2;

	p1 = p->line.p1;
	p2 = p->line.p2;

	//apply bitmap space rotation
	sg_point_map(&p1, map);
	sg_point_map(&p2, map);

	if( region ){
		sg_point_t min, max;

		min = p1;
		max = p1;
		if( p2.x < min.x ){ min.x = p2.x; }
		if( p2.y < min.y ){ min.y = p2.y; }

		if( p2.x > max.x ){ max.x = p2.x; }
		if( p2.y > max.y ){ max.y = p2.y; }

		update_bounds(min, max, region);
	}

	//add the option to invert the line
	sg_draw_line(bmap, p1, p2);
}

void draw_arc(const sg_vector_primitive_t * p, sg_bmap_t * bmap, const sg_vector_map_t * map, sg_region_t * region){
	sg_point_t corners[2];
	s32 tmp;

	sg_region_t arc_region;
	sg_point_t radius;

	arc_region.point = p->arc.center;

	//scale radii to bitmap dimension
	tmp = ((p->arc.rx) * map->region.dim.width + SG_MAX) / (SG_MAX-SG_MIN);
	radius.x = tmp;

	tmp = ((p->arc.ry) * map->region.dim.height + SG_MAX) / (SG_MAX-SG_MIN);
	radius.y = tmp;

	sg_point_map(&arc_region.point, map);

	//make dim a bounding box
	arc_region.dim.width = radius.x*2 + 1;
	arc_region.dim.height = radius.y*2 + 1;

	//move point to upper left corner
	arc_region.point.x -= radius.x;
	arc_region.point.y -= radius.y;

	sg_draw_arc(bmap, &arc_region, p->arc.start, p->arc.stop, p->arc.rotation, corners);
	if( region ){
		update_bounds(corners[0], corners[1], region);
	}

}

void draw_quadtratic_bezier(const sg_vector_primitive_t * p, sg_bmap_t * bmap, const sg_vector_map_t * map, sg_region_t * region){
	int i;
	sg_point_t points[3];
	sg_point_t corners[2];

	//maps the points to the bmap
	points[0] = p->quadratic_bezier.p1;
	points[1] = p->quadratic_bezier.p2;
	points[2] = p->quadratic_bezier.p3;

	for(i=0; i < 3; i++){
		sg_point_map(points + i, map);
	}

	sg_draw_quadtratic_bezier(bmap, points[0], points[1], points[2], corners);
	if( region ){
		update_bounds(corners[0], corners[1], region);
	}
}

void draw_cubic_bezier(const sg_vector_primitive_t * p, sg_bmap_t * bmap, const sg_vector_map_t * map, sg_region_t * region){
	int i;
	sg_point_t points[4];
	sg_point_t corners[2];

	//maps the points to the bmap
	points[0] = p->cubic_bezier.p1;
	points[1] = p->cubic_bezier.p2;
	points[2] = p->cubic_bezier.p3;
	points[3] = p->cubic_bezier.p4;

	for(i=0; i < 4; i++){
		sg_point_map(points + i, map);
	}

	sg_draw_cubic_bezier(bmap, points[0], points[1], points[2], points[3], corners);

	if( region ){
		update_bounds(corners[0], corners[1], region);
	}
}

void draw_fill(const sg_vector_primitive_t * p, sg_bmap_t * bmap, const sg_vector_map_t * map, sg_region_t * region){
	sg_point_t center;
	center = p->pour.center;
	sg_point_map(&center, map);
	sg_draw_pour(bmap, center, &map->region);
}

void update_bounds(sg_point_t min, sg_point_t max, sg_region_t * region){
	if( min.x < region->point.x ){
		if( region->dim.width ){
			region->dim.width += (region->point.x - min.x);
		}
		region->point.x = min.x;

	}

	if( (max.x >= region->point.x + region->dim.width) || (region->dim.width == 0) ){
		region->dim.width = max.x - region->point.x + 1;
	}

	if( min.y < region->point.y ){
		if( region->dim.height ){
			region->dim.height += (region->point.y - min.y);
		}
		region->point.y = min.y;
	}

	if( (max.y >= region->point.y + region->dim.height) || (region->dim.width == 0) ){
		region->dim.height = max.y - region->point.y + 1;
	}
}




