//Copyright 2011-2016 Tyler Gilbert; All Rights Reserved

#include "sg_config.h"
#include "sg.h"


int calculate_contrast(
      const sg_cursor_t * cursor
      ){
   sg_cursor_t y_cursor;
   sg_cursor_t x_cursor;
   int contrast = 0;

   y_cursor = *cursor;

   sg_cursor_dec_y(&y_cursor);
   sg_cursor_dec_x(&y_cursor);

   for(int y = 0; y < 3; y++){
      x_cursor = y_cursor;
      sg_cursor_inc_y(&y_cursor);
      for(int x = 0; x < 3; x++){
         contrast += (sg_cursor_get_pixel(&x_cursor) != 0);
      }
   }
   return contrast;
}

int sg_antialias_filter_init(
      sg_antialias_filter_t * filter,
      const sg_color_t contrast_data[8]
){
   memcpy(filter->contrast_map,
          contrast_data,
          sizeof(filter->contrast_map)
          );
   return 0;
}

int sg_antialias_filter_apply(
      const sg_bmap_t * bmap,
      sg_antialias_filter_t * filter,
      sg_region_t region
      ){

   //look for edges (word wise)
   sg_bmap_t tmp_bmap;
   sg_cursor_t y_cursor;
   sg_cursor_t x_cursor;
   sg_size_t width;
   sg_int_t edge_distance;

   tmp_bmap = *bmap;

   int contrast_value;

   sg_cursor_set(
            &y_cursor,
            &tmp_bmap,
            sg_point(region.point.x+1, region.point.y+1)
            );

   for(sg_int_t h = 0; h < region.area.height-1; h++){
      x_cursor = y_cursor;
      sg_cursor_inc_y(&y_cursor);

      width = 0;

      do {
         edge_distance =
               sg_cursor_find_positive_edge(
                  &x_cursor,
                  region.area.width - 2 - width
                  );
         width += (edge_distance+1);


         if( width < region.area.width - 2 ){

            sg_cursor_dec_x(&x_cursor);

            if( sg_cursor_get_pixel_no_inc(&x_cursor) == 0 ){


               contrast_value = calculate_contrast(&x_cursor);

               tmp_bmap.pen.color = filter->contrast_map[contrast_value];
               sg_cursor_draw_pixel(&x_cursor);
               width++;
            }
         }

         //apply contrast to pixel based on

         if( width < region.area.width-2 ){
            edge_distance =
                  sg_cursor_find_negative_edge(
                     &x_cursor,
                     region.area.width - 2 - width
                     );

            if( sg_cursor_get_pixel_no_inc(&x_cursor) == 0 ){
               contrast_value = calculate_contrast(&x_cursor);
               tmp_bmap.pen.color = filter->contrast_map[contrast_value];
               sg_cursor_draw_pixel(&x_cursor);
               width++;

            }
         }

         width += edge_distance;

      } while( width < region.area.width-2 );

   }

   //now calculate contrast and apply anti-aliasing a pos edge, neg edge
   //also look at row-1 and row+1 for vertical edges


   return 0;

}
