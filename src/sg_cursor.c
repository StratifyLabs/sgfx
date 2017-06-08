/*! \file */ //Copyright 2011-2016 Tyler Gilbert; All Rights Reserved

#include "sg.h"

static sg_size_t calc_pixels_until_first_boundary(sg_size_t w, sg_int_t x);
static sg_size_t calc_aligned_words(sg_size_t w, sg_size_t pixels_until_first_boundary);
static sg_size_t calc_pixels_after_last_boundary(sg_size_t w, sg_size_t pixels_until_first_boundary, sg_size_t aligned_words);
static void copy_pixel(sg_cursor_t * dest, sg_cursor_t * src);

//cursor with a single pixel
void sg_cursor_set(sg_cursor_t * cursor, const sg_bmap_t * bmap, sg_point_t p){
	cursor->bmap = bmap;
	cursor->p = p;
	cursor->target = sg_data(bmap, p);
	cursor->shift = ((p.x % SG_PIXELS_PER_WORD)*SG_BITS_PER_PIXEL);
}

sg_color_t sg_cursor_get_pixel(const sg_cursor_t * cursor){
	sg_color_t color;
	sg_bmap_data_t value;
	value = *(cursor->target) >> cursor->shift;
	color = value & SG_PIXEL_MASK;
	return color;
}

void sg_cursor_dec_x(sg_cursor_t * cursor){
	cursor->p.x--;
	if( (cursor->p.x % SG_PIXELS_PER_WORD) == (SG_PIXELS_PER_WORD - 1) ){
		cursor->target--; //go to the previous word
		cursor->shift = 32 - SG_BITS_PER_PIXEL;
	} else {
		cursor->shift -= SG_BITS_PER_PIXEL;
	}
}

void sg_cursor_inc_x(sg_cursor_t * cursor){
	cursor->p.x++;
	if( (cursor->p.x % SG_PIXELS_PER_WORD) == 0){
		cursor->target++; //go to the next word
		cursor->shift = 0;
	} else {
		cursor->shift += SG_BITS_PER_PIXEL;
	}
}

void sg_cursor_dec_y(sg_cursor_t * cursor){
	cursor->p.y--;
	cursor->target -= cursor->bmap->columns;
}


void sg_cursor_inc_y(sg_cursor_t * cursor){
	cursor->p.y++;
	cursor->target += cursor->bmap->columns;
}

void sg_cursor_draw_pixel(const sg_cursor_t * cursor){
	*(cursor->target) &= ~(SG_PIXEL_MASK << cursor->shift);
	*(cursor->target) |= (cursor->bmap->pen.color << cursor->shift);
}

void sg_cursor_draw_hline(sg_cursor_t * cursor, sg_size_t width){
	sg_size_t pixels_until_first_boundary;
	sg_size_t pixels_after_last_boundary;
	sg_size_t aligned_words;
	sg_size_t i;
	sg_bmap_data_t word_color;

	//calculate the pixels around the boundaries
	pixels_until_first_boundary = calc_pixels_until_first_boundary(width, cursor->p.x);
	aligned_words = calc_aligned_words(width, pixels_until_first_boundary);
	pixels_after_last_boundary = calc_pixels_after_last_boundary(width, pixels_until_first_boundary, aligned_words);


	for(i=0; i < pixels_until_first_boundary; i++){
		sg_cursor_draw_pixel(cursor);
		sg_cursor_inc_x(cursor);
	}

	word_color = 0;
	for(i=0; i < 32; i+=SG_BITS_PER_PIXEL){
		word_color |= cursor->bmap->pen.color << i;
	}

	for(i=0; i < aligned_words; i++){
		*(cursor->target) = word_color; //assignment of new value
		cursor->p.x += SG_PIXELS_PER_WORD;
	}

	for(i=0; i < pixels_after_last_boundary; i++){
		sg_cursor_draw_pixel(cursor);
		sg_cursor_inc_x(cursor);
	}
}

void sg_cursor_draw_cursor(sg_cursor_t * dest_cursor, sg_cursor_t * src_cursor, sg_size_t width){
	sg_size_t pixels_until_first_boundary;
	sg_size_t pixels_after_last_boundary;
	sg_size_t aligned_words;

	sg_size_t i;
	sg_bmap_data_t intermediate_value;
	sg_bmap_data_t mask;

	//calculate the pixels around boundaries
	pixels_until_first_boundary = calc_pixels_until_first_boundary(width, src_cursor->p.x);
	aligned_words = calc_aligned_words(width, pixels_until_first_boundary);
	pixels_after_last_boundary = calc_pixels_after_last_boundary(width, pixels_until_first_boundary, aligned_words);

	for(i=0; i < pixels_until_first_boundary; i++){
		copy_pixel(dest_cursor, src_cursor);
	}

	//now handle all the words that are aligned
	for(i=0; i < aligned_words; i++){

		intermediate_value = *(src_cursor->target);

		//shift into the dest area
		mask = (1<<dest_cursor->shift)-1;
		*(dest_cursor->target) &= mask; //mask the bits where dest_value will OR in
		*(dest_cursor->target) |= (intermediate_value << dest_cursor->shift);

		if( mask != 0){ //if mask is zero, then this copy is aligned
			*(dest_cursor->target+1) &= ~mask; //mask the bits where dest_value will OR in
			*(dest_cursor->target+1) |= (intermediate_value << dest_cursor->shift);
		}

		dest_cursor->target++;
		src_cursor->target++;
		dest_cursor->p.x += SG_PIXELS_PER_WORD;
		src_cursor->p.x += SG_PIXELS_PER_WORD;
	}

	//copy the pixels past the boundary
	for(i=0; i < pixels_after_last_boundary; i++){
		copy_pixel(dest_cursor, src_cursor);
	}

}

void sg_cursor_shift_right(sg_cursor_t * cursor, sg_size_t shift_width, sg_size_t shift_distance){
	sg_size_t pixels_until_first_boundary;
	sg_size_t pixels_after_last_boundary;
	sg_size_t aligned_words;

	sg_size_t i;
	sg_cursor_t shift_cursor;
	sg_bmap_data_t value;
	sg_bmap_data_t mask;

	//calculate the pixels around boundaries
	pixels_until_first_boundary = calc_pixels_until_first_boundary(shift_width, cursor->p.x);
	aligned_words = calc_aligned_words(shift_width, pixels_until_first_boundary);
	pixels_after_last_boundary = calc_pixels_after_last_boundary(shift_width, pixels_until_first_boundary, aligned_words);

	sg_cursor_set(&shift_cursor, cursor->bmap, sg_point(cursor->p.x + shift_width - pixels_after_last_boundary, cursor->p.y));

	*(shift_cursor.target) <<= (shift_distance*SG_BITS_PER_PIXEL);

	for(i=0; i < aligned_words; i++){
		*(shift_cursor.target) |= (*(shift_cursor.target-1) >> (32 - shift_distance*SG_BITS_PER_PIXEL));
		shift_cursor.target--;
		*(shift_cursor.target) <<= (shift_distance*SG_BITS_PER_PIXEL);
	}

	*(shift_cursor.target) |= (*(shift_cursor.target-1) >> (32 - shift_distance*SG_BITS_PER_PIXEL));
	shift_cursor.target--;

	mask = ((1<<(32 - pixels_after_last_boundary*SG_BITS_PER_PIXEL))-1);
	value = (*(shift_cursor.target) << (shift_distance*SG_BITS_PER_PIXEL)) & mask;
	*(shift_cursor.target) &= mask;
	*(shift_cursor.target) |= value;

	for(i=0; i < shift_distance; i++){
		sg_cursor_inc_x(cursor);
	}

}

void sg_cursor_shift_left(sg_cursor_t * cursor, sg_size_t shift_width, sg_size_t shift_distance){
	sg_size_t pixels_until_first_boundary;
	sg_size_t pixels_after_last_boundary;
	sg_size_t aligned_words;

	sg_size_t i;
	sg_cursor_t shift_cursor;
	sg_bmap_data_t value;
	sg_bmap_data_t mask;

	//calculate the pixels around boundaries
	pixels_until_first_boundary = calc_pixels_until_first_boundary(shift_width, cursor->p.x);
	aligned_words = calc_aligned_words(shift_width, pixels_until_first_boundary);
	pixels_after_last_boundary = calc_pixels_after_last_boundary(shift_width, pixels_until_first_boundary, aligned_words);

	sg_cursor_copy(&shift_cursor, cursor);

	*(shift_cursor.target) >>= (shift_distance*SG_BITS_PER_PIXEL);

	for(i=0; i < aligned_words; i++){
		*(shift_cursor.target) |= (*(shift_cursor.target+1) << (32 - shift_distance*SG_BITS_PER_PIXEL));
		shift_cursor.target++;
		*(shift_cursor.target) >>= (shift_distance*SG_BITS_PER_PIXEL);
	}

	*(shift_cursor.target) |= (*(shift_cursor.target+1) >> (32 - shift_distance*SG_BITS_PER_PIXEL));
	shift_cursor.target++;

	mask = ((1<<(32 - pixels_after_last_boundary*SG_BITS_PER_PIXEL))-1);
	value = (*(shift_cursor.target) >> (shift_distance*SG_BITS_PER_PIXEL)) & mask;
	*(shift_cursor.target) &= mask;
	*(shift_cursor.target) |= value;

	for(i=0; i < shift_distance; i++){
		sg_cursor_dec_x(cursor);
	}
}

void copy_pixel(sg_cursor_t * dest, sg_cursor_t * src){
	sg_color_t color = sg_cursor_get_pixel(src);
	*(dest->target) &= ~(SG_PIXEL_MASK << dest->shift);
	*(dest->target) |= (color << dest->shift);
	sg_cursor_inc_x(dest);
	sg_cursor_inc_x(src);
}

sg_size_t calc_pixels_until_first_boundary(sg_size_t w, sg_int_t x){
	sg_size_t pixels_until_first_boundary;

	pixels_until_first_boundary = (SG_PIXELS_PER_WORD - x % SG_PIXELS_PER_WORD);

	if( pixels_until_first_boundary == SG_PIXELS_PER_WORD ){
		pixels_until_first_boundary = 0;
	}

	if( pixels_until_first_boundary > w ){
		pixels_until_first_boundary = w;
	}

	return pixels_until_first_boundary;
}

sg_size_t calc_aligned_words(sg_size_t w, sg_size_t pixels_until_first_boundary){
	return (w - pixels_until_first_boundary) / SG_PIXELS_PER_WORD;
}

sg_size_t calc_pixels_after_last_boundary(sg_size_t w, sg_size_t pixels_until_first_boundary, sg_size_t aligned_words){
	return w - pixels_until_first_boundary - aligned_words * SG_PIXELS_PER_WORD;
}
