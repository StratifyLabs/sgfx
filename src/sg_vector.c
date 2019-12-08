//Copyright 2011-2016 Tyler Gilbert; All Rights Reserved

#include <unistd.h>
#include <stdio.h>

#include "sg_config.h"
#include "sg.h"


static void draw_line_with_map(sg_point_t p1, sg_point_t p2, sg_bmap_t * bmap, const sg_vector_map_t * map, sg_region_t * region);
static void draw_quadtratic_bezier_with_map(sg_point_t p1, sg_point_t p2, sg_point_t p3, sg_bmap_t * bmap, const sg_vector_map_t * map, sg_region_t * region);
static void draw_cubic_bezier_with_map(sg_point_t p1, sg_point_t p2, sg_point_t p3, sg_point_t p4, sg_bmap_t * bmap, const sg_vector_map_t * map, sg_region_t * region);

static void update_bounds(sg_point_t min, sg_point_t max, sg_region_t * region);

static void draw_path_none(sg_bmap_t * bmap, sg_vector_path_t * path, const sg_vector_map_t * map, const sg_vector_path_description_t * description);
static void draw_path_move(sg_bmap_t * bmap, sg_vector_path_t * path, const sg_vector_map_t * map, const sg_vector_path_description_t * description);
static void draw_path_line(sg_bmap_t * bmap, sg_vector_path_t * path, const sg_vector_map_t * map, const sg_vector_path_description_t * description);
static void draw_path_quadtratic_bezier(sg_bmap_t * bmap, sg_vector_path_t * path, const sg_vector_map_t * map, const sg_vector_path_description_t * description);
static void draw_path_cubic_bezier(sg_bmap_t * bmap, sg_vector_path_t * path, const sg_vector_map_t * map, const sg_vector_path_description_t * description);
static void draw_path_close(sg_bmap_t * bmap, sg_vector_path_t * path, const sg_vector_map_t * map, const sg_vector_path_description_t * description);
static void draw_path_pour(sg_bmap_t * bmap, sg_vector_path_t * path, const sg_vector_map_t * map, const sg_vector_path_description_t * description);



static void (*draw_path_func [SG_VECTOR_PATH_TOTAL])(sg_bmap_t * bmap, sg_vector_path_t * path, const sg_vector_map_t * map, const sg_vector_path_description_t * description) = {
		draw_path_none,
		draw_path_move,
		draw_path_line,
		draw_path_quadtratic_bezier,
		draw_path_cubic_bezier,
		draw_path_close,
		draw_path_pour
};


void sg_vector_draw_path(
		sg_bmap_t * bmap,
		sg_vector_path_t * path,
		const sg_vector_map_t * map
		){
	u32 i;
	u32 type;
	for(i=0; i < path->icon.count; i++){
		type = path->icon.list[i].type;
		if( type < SG_VECTOR_PATH_TOTAL ){
			draw_path_func[type](bmap, path, map, path->icon.list + i);
		}
	}
}

void update_bounds(sg_point_t min, sg_point_t max, sg_region_t * region){
	if( min.x < region->point.x ){
		if( region->area.width ){
			region->area.width += (region->point.x - min.x);
		}
		region->point.x = min.x;

	}

	if( (max.x >= region->point.x + region->area.width) || (region->area.width == 0) ){
		region->area.width = max.x - region->point.x + 1;
	}

	if( min.y < region->point.y ){
		if( region->area.height ){
			region->area.height += (region->point.y - min.y);
		}
		region->point.y = min.y;
	}

	if( (max.y >= region->point.y + region->area.height) || (region->area.width == 0) ){
		region->area.height = max.y - region->point.y + 1;
	}
}

void draw_path_none(sg_bmap_t * bmap, sg_vector_path_t * path, const sg_vector_map_t * map, const sg_vector_path_description_t * description){

}

void draw_path_move(sg_bmap_t * bmap, sg_vector_path_t * path, const sg_vector_map_t * map, const sg_vector_path_description_t * description){
	path->start = description->move.point;
	path->current = description->move.point;
}

void draw_path_line(sg_bmap_t * bmap, sg_vector_path_t * path, const sg_vector_map_t * map, const sg_vector_path_description_t * description){
	draw_line_with_map(
				path->current,
				description->line.point,
				bmap,
				map,
				&path->region
				);

	path->current = description->line.point;
}

void draw_path_quadtratic_bezier(sg_bmap_t * bmap, sg_vector_path_t * path, const sg_vector_map_t * map, const sg_vector_path_description_t * description){
	draw_quadtratic_bezier_with_map(path->current,
			description->quadratic_bezier.control,
			description->quadratic_bezier.point,
			bmap, map, &path->region);
	path->current = description->quadratic_bezier.point;
}

void draw_path_cubic_bezier(sg_bmap_t * bmap, sg_vector_path_t * path, const sg_vector_map_t * map, const sg_vector_path_description_t * description){
	draw_cubic_bezier_with_map(path->current,
			description->cubic_bezier.control[0],
			description->cubic_bezier.control[1],
			description->cubic_bezier.point,
			bmap, map, &path->region);
	path->current = description->quadratic_bezier.point;
}

void draw_path_close(sg_bmap_t * bmap, sg_vector_path_t * path, const sg_vector_map_t * map, const sg_vector_path_description_t * description){
	draw_line_with_map(path->current,
			path->start,
			bmap, map, &path->region);
	path->current = path->start;
}

void draw_path_pour(sg_bmap_t * bmap, sg_vector_path_t * path, const sg_vector_map_t * map, const sg_vector_path_description_t * description){
	sg_point_t point = description->pour.point;
	sg_point_map(&point, map);
	sg_draw_pour(bmap, point, &(path->region));
}


void draw_line_with_map(sg_point_t p1,
		sg_point_t p2,
		sg_bmap_t * bmap,
		const sg_vector_map_t * map,
		sg_region_t * region){
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
void draw_quadtratic_bezier_with_map(sg_point_t p1, sg_point_t p2, sg_point_t p3, sg_bmap_t * bmap, const sg_vector_map_t * map, sg_region_t * region){
	int i;
	sg_point_t points[3];
	sg_point_t corners[2];

	//maps the points to the bmap
	points[0] = p1;
	points[1] = p2;
	points[2] = p3;

	for(i=0; i < 3; i++){
		sg_point_map(points + i, map);
	}

	sg_draw_quadratic_bezier(bmap, points[0], points[1], points[2], corners);
	if( region ){
		update_bounds(corners[0], corners[1], region);
	}
}

void draw_cubic_bezier_with_map(sg_point_t p1, sg_point_t p2, sg_point_t p3, sg_point_t p4, sg_bmap_t * bmap, const sg_vector_map_t * map, sg_region_t * region){
	int i;
	sg_point_t points[4];
	sg_point_t corners[2];

	//maps the points to the bmap
	points[0] = p1;
	points[1] = p2;
	points[2] = p3;
	points[3] = p4;

	for(i=0; i < 4; i++){
		sg_point_map(points + i, map);
	}

	sg_draw_cubic_bezier(bmap, points[0], points[1], points[2], points[3], corners);

	if( region ){
		update_bounds(corners[0], corners[1], region);
	}
}






