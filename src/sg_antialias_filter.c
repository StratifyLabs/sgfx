//Copyright 2011-2016 Tyler Gilbert; All Rights Reserved

#include "sg_config.h"
#include "sg.h"

/*
 * Anti-alias 2 colors to a 4 color spectrum
 *
 * Caller provides a lookup table that blends 2
 * colors in the bitmap
 *
 * for 4bpp source bitmap, 4 primary colors can be anti-aliased to 16-colors
 *
 * The table should look like this
 *
 *    Blend with ->|  0  1  2  3 |
 * primary color 0 | 00 01 02 03 |
 * primary color 1 | 04 05 06 07 |
 * primary color 2 | 08 09 10 11 |
 * primary color 3 | 12 13 14 15 |
 *
 * For example, when anti-aliasing an edge
 * between primary color 0 and 1 colors
 * 00, 01, 04, and 05 are used.
 *
 * 00 is pure color 0, 01 mixes 0 with 1,
 * 04 mixes 1 with 0, and 05 is pure color 1
 *
 *
 */

static int lookup_color(
      const sg_bmap_t * bmap,
      const sg_antialias_filter_t * filter,
      const sg_cursor_t * cursor,
      sg_color_t first_color,
      sg_color_t second_color
      ){
   u8 bpp = bmap->bits_per_pixel;
   sg_color_t first_primary_color = first_color - first_color/bpp;
   sg_color_t second_primary_color = second_color - second_color/bpp;
   sg_color_t color_options[4];
   color_options[0] = first_primary_color + first_primary_color/bpp;
   color_options[1] = first_primary_color + second_primary_color/bpp;
   color_options[2] = second_primary_color + first_primary_color/bpp;
   color_options[3] = second_primary_color + second_primary_color/bpp;


   //calculate the contrast
   sg_cursor_t y_cursor;
   sg_cursor_t x_cursor;
   int contrast = 0;

   y_cursor = *cursor;

   sg_color_t color;
   sg_cursor_dec_y(&y_cursor);
   sg_cursor_dec_x(&y_cursor);

   for(int y = 0; y < 3; y++){
      x_cursor = y_cursor;
      sg_cursor_inc_y(&y_cursor);
      for(int x = 0; x < 3; x++){
         color = sg_cursor_get_pixel(&x_cursor);
         contrast += (color == second_color);
      }
   }

   return color_options[ filter->contrast_map[contrast] ];
}

int sg_antialias_filter_init(
      sg_antialias_filter_t * filter,
      const u8 contrast_data[8]
){
   for(u32 i=0; i < 8; i++){
      if( contrast_data[i] > 3 ){
         return -1;
      }
   }

   memcpy(filter->contrast_map,
          contrast_data,
          sizeof(filter->contrast_map)
          );

   return 0;
}

int sg_antialias_filter_apply(
      const sg_bmap_t * bmap,
      const sg_antialias_filter_t * filter,
      sg_region_t region
      ){

   //look for edges (word wise)
   sg_bmap_t tmp_bmap;
   sg_cursor_t y_cursor;
   sg_cursor_t x_cursor;
   sg_size_t width;
   sg_int_t edge_distance;
   sg_color_t current_color, next_color;
   tmp_bmap = *bmap;

   sg_cursor_set(
            &y_cursor,
            &tmp_bmap,
            sg_point(region.point.x, region.point.y+1)
            );

   for(sg_int_t h = 0; h < region.area.height-1; h++){
      x_cursor = y_cursor;
      sg_cursor_inc_y(&y_cursor);
      current_color = sg_cursor_get_pixel_no_increment(&y_cursor);
      sg_cursor_inc_x(&x_cursor);

      u8 odd_or_even = 1;
      width = 0;

      do {
         edge_distance =
               sg_cursor_find_edge(
                  &x_cursor,
                  current_color,
                  region.area.width - 2 - width
                  );
         width += edge_distance;

         //this checks to see if an edge was found or not
         if( width < region.area.width - 2 ){

            //cursor points to the new color
            next_color = sg_cursor_get_pixel_no_increment(&x_cursor);

            //edge was found -- backup on pixel to be on the first color side
            if( odd_or_even & 0x01 ){
               sg_cursor_dec_x(&x_cursor);
               tmp_bmap.pen.color = lookup_color(
                        &tmp_bmap,
                        filter,
                        &x_cursor,
                        current_color,
                        next_color
                        );
            } else {
               tmp_bmap.pen.color = lookup_color(
                        &tmp_bmap,
                        filter,
                        &x_cursor,
                        next_color,
                        current_color
                        );
            }


            sg_cursor_draw_pixel(&x_cursor); //update the pixel with the new value -- increments the cursor
            if( odd_or_even & 0x01 ){
               width++;
            }
            current_color = next_color;
            odd_or_even++;
         }

      } while( width < region.area.width-2 );

   }


   return 0;

}
