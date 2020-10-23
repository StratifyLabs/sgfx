// COPYING: Copyright 2011-2020 Tyler Gilbert and Stratify Labs, Inc; see
// LICENSE.md for rights.

#include "sg_config.h"

#include "sg.h"

typedef struct {
  u16 color;
  u16 color_next_row;
} color_pair_t;

static sg_color_t create_pattern(const sg_bmap_t *bmap, sg_color_t color);

static sg_size_t calc_pixels_until_first_boundary(
  const sg_cursor_t *cursor,
  sg_size_t w,
  sg_size_t shift);
static sg_size_t calc_aligned_words(
  const sg_cursor_t *cursor,
  sg_size_t w,
  sg_size_t pixels_until_first_boundary);
static sg_size_t calc_pixels_after_last_boundary(
  const sg_cursor_t *cursor,
  sg_size_t w,
  sg_size_t pixels_until_first_boundary,
  sg_size_t aligned_words);
static void copy_pixel(sg_cursor_t *dest, sg_cursor_t *src);
static void draw_pixel(const sg_cursor_t *cursor, sg_color_t color);
static void draw_pixel_group(
  sg_bmap_data_t *word,
  sg_bmap_data_t pattern,
  sg_bmap_data_t mask,
  u16 o_flags);
static inline sg_color_t get_pixel(const sg_cursor_t *cursor);

// cursor with a single pixel
void sg_cursor_set(sg_cursor_t *cursor, const sg_bmap_t *bmap, sg_point_t p) {
  cursor->bmap = bmap;
  cursor->target = sg_bmap_data(bmap, p);
  cursor->shift
    = ((p.x % SG_PIXELS_PER_WORD(bmap)) * SG_BITS_PER_PIXEL_VALUE(bmap)); // up
                                                                          // to
                                                                          // 32
}

void sg_cursor_update(sg_cursor_t *cursor, sg_point_t p) {
  cursor->target = sg_bmap_data(cursor->bmap, p);
  cursor->shift
    = ((p.x % SG_PIXELS_PER_WORD(cursor->bmap)) * SG_BITS_PER_PIXEL_VALUE(cursor->bmap)); // up to 32
}

sg_color_t sg_cursor_get_pixel_no_increment(sg_cursor_t *cursor) {
  return get_pixel(cursor);
}

sg_color_t sg_cursor_get_pixel_increment(sg_cursor_t *cursor, int x, int y) {
  sg_color_t result = get_pixel(cursor);
  if (x > 0) {
    sg_cursor_inc_x(cursor);
  }
  if (x < 0) {
    sg_cursor_dec_x(cursor);
  }
  if (y > 0) {
    sg_cursor_inc_y(cursor);
  }
  if (y < 0) {
    sg_cursor_dec_y(cursor);
  }
  return result;
}

void sg_cursor_draw_pixel_no_increment(sg_cursor_t *cursor) {
  draw_pixel(cursor, cursor->bmap->pen.color);
}

sg_color_t sg_cursor_get_pixel(sg_cursor_t *cursor) {
  sg_color_t color = get_pixel(cursor);
  sg_cursor_inc_x(cursor);
  return color;
}

void sg_cursor_dec_x(sg_cursor_t *cursor) {

  if (cursor->shift == 0) {
    cursor->target--; // go to the previous word
    cursor->shift = SG_BITS_PER_WORD - SG_BITS_PER_PIXEL_VALUE(cursor->bmap);
  } else {
    cursor->shift -= SG_BITS_PER_PIXEL_VALUE(cursor->bmap);
  }
}

void sg_cursor_dec_y(sg_cursor_t *cursor) {
  cursor->target -= cursor->bmap->columns;
}

void sg_cursor_inc_y(sg_cursor_t *cursor) {
  cursor->target += cursor->bmap->columns;
}

void sg_cursor_draw_pixel(sg_cursor_t *cursor) {
  draw_pixel(cursor, cursor->bmap->pen.color);
  sg_cursor_inc_x(cursor);
}

void sg_cursor_draw_hline(sg_cursor_t *cursor, sg_size_t width) {
  sg_bmap_data_t pattern;
  pattern = create_pattern(cursor->bmap, cursor->bmap->pen.color);
  sg_cursor_draw_pattern(cursor, width, pattern);
}

sg_int_t sg_cursor_find_edge(
  sg_cursor_t *cursor,
  sg_color_t current_color,
  sg_size_t width) {
  sg_size_t pixels_until_first_boundary;
  sg_size_t pixels_after_last_boundary;
  sg_size_t aligned_words;
  sg_size_t i;
  sg_bmap_data_t color;
  sg_size_t result = 0;

  pixels_until_first_boundary
    = calc_pixels_until_first_boundary(cursor, width, cursor->shift);

  for (i = 0; i < pixels_until_first_boundary; i++) {
    color = sg_cursor_get_pixel_no_increment(cursor);
    if (color != current_color) {
      return result;
    }
    sg_cursor_inc_x(cursor);
    result++;
  }

  aligned_words
    = calc_aligned_words(cursor, width, pixels_until_first_boundary);

  sg_color_t color_pattern = create_pattern(cursor->bmap, current_color);

  for (i = 0; i < aligned_words; i++) {
    if (*cursor->target != color_pattern) {
      for (i = 0; i < SG_PIXELS_PER_WORD(cursor->bmap); i++) {
        color = sg_cursor_get_pixel_no_increment(cursor);
        if (color != current_color) {
          return result;
        }
        sg_cursor_inc_x(cursor);
        result++;
      }
    } else {
      cursor->target++;
      result += SG_PIXELS_PER_WORD(cursor->bmap);
    }
  }

  pixels_after_last_boundary = calc_pixels_after_last_boundary(
    cursor,
    width,
    pixels_until_first_boundary,
    aligned_words);

  for (i = 0; i < pixels_after_last_boundary; i++) {
    color = sg_cursor_get_pixel_no_increment(cursor);
    if (color != current_color) {
      return result;
    }
    sg_cursor_inc_x(cursor);
    result++;
  }

  return width;
}

sg_int_t sg_cursor_find_positive_edge(sg_cursor_t *cursor, sg_size_t width) {
  sg_size_t pixels_until_first_boundary;
  sg_size_t pixels_after_last_boundary;
  sg_size_t aligned_words;
  sg_size_t i;
  sg_bmap_data_t color;
  sg_size_t result = 0;

  pixels_until_first_boundary
    = calc_pixels_until_first_boundary(cursor, width, cursor->shift);
  aligned_words
    = calc_aligned_words(cursor, width, pixels_until_first_boundary);
  pixels_after_last_boundary = calc_pixels_after_last_boundary(
    cursor,
    width,
    pixels_until_first_boundary,
    aligned_words);

  for (i = 0; i < pixels_until_first_boundary; i++) {
    color = sg_cursor_get_pixel_no_increment(cursor);
    if (color != 0) {
      return result;
    }
    sg_cursor_inc_x(cursor);
    result++;
  }

  for (i = 0; i < aligned_words; i++) {
    if (*cursor->target != 0) {
      for (i = 0; i < SG_PIXELS_PER_WORD(cursor->bmap); i++) {
        color = sg_cursor_get_pixel_no_increment(cursor);
        if (color != 0) {
          return result;
        }
        sg_cursor_inc_x(cursor);
        result++;
      }
    } else {
      cursor->target++;
      result += SG_PIXELS_PER_WORD(cursor->bmap);
    }
  }

  for (i = 0; i < pixels_after_last_boundary; i++) {
    color = sg_cursor_get_pixel_no_increment(cursor);
    if (color != 0) {
      return result;
    }
    sg_cursor_inc_x(cursor);
    result++;
  }

  return width;
}

sg_int_t sg_cursor_find_negative_edge(sg_cursor_t *cursor, sg_size_t width) {
  sg_size_t pixels_until_first_boundary;
  sg_size_t pixels_after_last_boundary;
  sg_size_t aligned_words;
  sg_size_t i;
  sg_bmap_data_t color;
  sg_size_t result = 0;

  pixels_until_first_boundary
    = calc_pixels_until_first_boundary(cursor, width, cursor->shift);
  aligned_words
    = calc_aligned_words(cursor, width, pixels_until_first_boundary);
  pixels_after_last_boundary = calc_pixels_after_last_boundary(
    cursor,
    width,
    pixels_until_first_boundary,
    aligned_words);

  for (i = 0; i < pixels_until_first_boundary; i++) {
    color = sg_cursor_get_pixel_no_increment(cursor);
    if (color == 0) {
      return result;
    }
    sg_cursor_inc_x(cursor);
    result++;
  }

  for (i = 0; i < aligned_words; i++) {
    if (*cursor->target != 0xffffffff) {
      for (i = 0; i < SG_PIXELS_PER_WORD(cursor->bmap); i++) {
        color = sg_cursor_get_pixel_no_increment(cursor);
        if (color == 0) {
          return result;
        }
        sg_cursor_inc_x(cursor);
        result++;
      }
    } else {
      cursor->target++;
      result += SG_PIXELS_PER_WORD(cursor->bmap);
    }
  }

  for (i = 0; i < pixels_after_last_boundary; i++) {
    color = sg_cursor_get_pixel_no_increment(cursor);
    if (color == 0) {
      return result;
    }
    sg_cursor_inc_x(cursor);
    result++;
  }

  return width;
}

void sg_cursor_draw_pattern(
  sg_cursor_t *cursor,
  sg_size_t width,
  sg_bmap_data_t pattern) {
  sg_size_t pixels_until_first_boundary;
  sg_size_t pixels_after_last_boundary;
  sg_size_t aligned_words;
  sg_size_t i;
  sg_bmap_data_t color;
  u16 o_flags = cursor->bmap->pen.o_flags;

  // calculate the pixels around the boundaries
  pixels_until_first_boundary
    = calc_pixels_until_first_boundary(cursor, width, cursor->shift);
  aligned_words
    = calc_aligned_words(cursor, width, pixels_until_first_boundary);
  pixels_after_last_boundary = calc_pixels_after_last_boundary(
    cursor,
    width,
    pixels_until_first_boundary,
    aligned_words);

  // this loop could be done in one operation
  for (i = 0; i < pixels_until_first_boundary; i++) {
    color = (pattern >> cursor->shift);
    draw_pixel(cursor, color);
    sg_cursor_inc_x(cursor);
  }

  for (i = 0; i < aligned_words; i++) {
    draw_pixel_group(cursor->target++, pattern, 0, o_flags);
  }

  // this loop could also be done in one operation
  for (i = 0; i < pixels_after_last_boundary; i++) {
    color = (pattern >> cursor->shift);
    draw_pixel(cursor, color);
    sg_cursor_inc_x(cursor);
  }
}

void sg_cursor_draw_cursor(
  sg_cursor_t *dest_cursor,
  const sg_cursor_t *src_cursor,
  sg_size_t width) {
  sg_size_t pixels_until_first_boundary;
  sg_size_t pixels_after_last_boundary;
  sg_size_t aligned_words;

  sg_size_t i;
  sg_bmap_data_t intermediate_value;
  sg_bmap_data_t mask;
  sg_cursor_t shift_cursor;

  u16 o_flags = dest_cursor->bmap->pen.o_flags;

  sg_cursor_copy(&shift_cursor, src_cursor);

  if (
    (dest_cursor->bmap->bits_per_pixel == src_cursor->bmap->bits_per_pixel)
    && ((o_flags & SG_PEN_FLAG_IS_ZERO_TRANSPARENT) == 0)) {

    // calculate the pixels around boundaries
    pixels_until_first_boundary
      = calc_pixels_until_first_boundary(src_cursor, width, shift_cursor.shift);
    aligned_words
      = calc_aligned_words(src_cursor, width, pixels_until_first_boundary);
    pixels_after_last_boundary = calc_pixels_after_last_boundary(
      src_cursor,
      width,
      pixels_until_first_boundary,
      aligned_words);

    for (i = 0; i < pixels_until_first_boundary; i++) {
      copy_pixel(dest_cursor, &shift_cursor);
    }

    // now handle all the words that are aligned
    for (i = 0; i < aligned_words; i++) {

      intermediate_value = *(shift_cursor.target);

      // shift into the dest area
      mask = (1 << dest_cursor->shift) - 1;
      draw_pixel_group(
        dest_cursor->target,
        ((intermediate_value) << dest_cursor->shift),
        mask,
        o_flags);

      if (mask != 0) { // if mask is zero, then this copy is aligned -- no need
                       // for a second operation
        draw_pixel_group(
          dest_cursor->target + 1,
          (intermediate_value >> (SG_BITS_PER_WORD - dest_cursor->shift)),
          ~mask,
          o_flags);
      }

      dest_cursor->target++;
      shift_cursor.target++;
    }

    // copy the pixels past the boundary
    for (i = 0; i < pixels_after_last_boundary; i++) {
      copy_pixel(dest_cursor, &shift_cursor);
    }

  } else {
    for (i = 0; i < width; i++) {
      copy_pixel(dest_cursor, &shift_cursor);
    }
  }
}

void sg_cursor_shift_right(
  sg_cursor_t *cursor,
  sg_size_t shift_width,
  sg_size_t shift_distance) {
  sg_size_t pixels_until_first_boundary;
  sg_size_t pixels_after_last_boundary;
  sg_size_t aligned_words;

  sg_size_t i;
  sg_cursor_t shift_cursor;
  sg_cursor_t dest_cursor;
  sg_bmap_data_t value;
  sg_bmap_data_t mask;
  sg_size_t operating_width;

  // calculate the pixels around boundaries
  pixels_until_first_boundary
    = calc_pixels_until_first_boundary(cursor, shift_width, cursor->shift);
  aligned_words
    = calc_aligned_words(cursor, shift_width, pixels_until_first_boundary);
  pixels_after_last_boundary = calc_pixels_after_last_boundary(
    cursor,
    shift_width,
    pixels_until_first_boundary,
    aligned_words);

  sg_cursor_copy(&shift_cursor, cursor);

  for (i = 0; i < shift_distance; i++) {
    sg_cursor_inc_x(cursor);
  }

  sg_cursor_copy(&dest_cursor, cursor);

  for (i = 0; i < shift_width; i++) {
    sg_cursor_inc_x(&shift_cursor);
    sg_cursor_inc_x(&dest_cursor);
  }

  if (pixels_after_last_boundary > 0) {

    shift_cursor.shift = 0;
    for (i = 0; i < pixels_after_last_boundary; i++) {
      sg_cursor_dec_x(&dest_cursor);
    }

    operating_width = pixels_after_last_boundary;

    if (operating_width > shift_width) {
      operating_width = shift_width;
    }

    mask = ((1 << operating_width * SG_BITS_PER_PIXEL_VALUE(cursor->bmap)) - 1);
    value = (*shift_cursor.target) & mask;

    *shift_cursor.target &= ~(mask);

    // now put mask and value in dest
    (*dest_cursor.target) &= ~(mask << dest_cursor.shift);
    (*dest_cursor.target) |= (value << dest_cursor.shift);

    if (dest_cursor.shift > 0) {
      *(dest_cursor.target + 1)
        &= ~(mask >> (SG_BITS_PER_WORD - dest_cursor.shift));
      *(dest_cursor.target + 1)
        |= (value >> (SG_BITS_PER_WORD - dest_cursor.shift));
    }
  }

  shift_cursor.target--;
  dest_cursor.target--;

  for (i = 0; i < aligned_words; i++) {

    mask = (u32)-1;
    value = *shift_cursor.target;

    *shift_cursor.target = 0;

    // now put mask and value in dest
    (*dest_cursor.target) &= ~(mask << dest_cursor.shift);
    (*dest_cursor.target) |= (value << dest_cursor.shift);

    if (dest_cursor.shift > 0) {
      *(dest_cursor.target + 1)
        &= ~(mask >> (SG_BITS_PER_WORD - dest_cursor.shift));
      *(dest_cursor.target + 1)
        |= (value >> (SG_BITS_PER_WORD - dest_cursor.shift));
    }

    dest_cursor.target--;
    shift_cursor.target--;
  }

  if (pixels_until_first_boundary) {
    operating_width = pixels_until_first_boundary;

    dest_cursor.target++;
    shift_cursor.target++;

    for (i = 0; i < pixels_until_first_boundary; i++) {
      sg_cursor_dec_x(&shift_cursor);
      sg_cursor_dec_x(&dest_cursor);
    }

    mask = ((1 << operating_width * SG_BITS_PER_PIXEL_VALUE(cursor->bmap)) - 1);
    value = (*shift_cursor.target >> shift_cursor.shift) & mask;

    *shift_cursor.target &= ~(mask << shift_cursor.shift);

    // now put mask and value in dest
    (*dest_cursor.target) &= ~(mask << dest_cursor.shift);
    (*dest_cursor.target) |= (value << dest_cursor.shift);

    if (dest_cursor.shift > 0) {
      *(dest_cursor.target + 1)
        &= ~(mask >> (SG_BITS_PER_WORD - dest_cursor.shift));
      *(dest_cursor.target + 1)
        |= (value >> (SG_BITS_PER_WORD - dest_cursor.shift));
    }
  }
}

void sg_cursor_shift_left(
  sg_cursor_t *cursor,
  sg_size_t shift_width,
  sg_size_t shift_distance) {
  sg_size_t pixels_until_first_boundary;
  sg_size_t pixels_after_last_boundary;
  sg_size_t aligned_words;

  sg_size_t i;
  sg_cursor_t shift_cursor;
  sg_cursor_t dest_cursor;
  sg_bmap_data_t value;
  sg_bmap_data_t mask;
  sg_size_t operating_width;

  // calculate the pixels around boundaries
  pixels_until_first_boundary
    = calc_pixels_until_first_boundary(cursor, shift_width, cursor->shift);
  aligned_words
    = calc_aligned_words(cursor, shift_width, pixels_until_first_boundary);
  pixels_after_last_boundary = calc_pixels_after_last_boundary(
    cursor,
    shift_width,
    pixels_until_first_boundary,
    aligned_words);

  sg_cursor_copy(&shift_cursor, cursor);

  for (i = 0; i < shift_distance; i++) {
    sg_cursor_dec_x(cursor);
  }

  sg_cursor_copy(&dest_cursor, cursor);

  if (pixels_until_first_boundary > 0) {

    operating_width = pixels_until_first_boundary;

    if (operating_width > shift_width) {
      operating_width = shift_width;
    }

    mask = ((1 << operating_width * SG_BITS_PER_PIXEL_VALUE(cursor->bmap)) - 1);
    value = (*shift_cursor.target >> shift_cursor.shift) & mask;

    (*shift_cursor.target) &= ~(mask << shift_cursor.shift);

    // now put mask and value in dest
    (*dest_cursor.target) &= ~(mask << dest_cursor.shift);
    (*dest_cursor.target) |= (value << dest_cursor.shift);

    if (dest_cursor.shift > 0) {
      *(dest_cursor.target + 1)
        &= ~(mask >> (SG_BITS_PER_WORD - dest_cursor.shift));
      *(dest_cursor.target + 1)
        |= (value >> (SG_BITS_PER_WORD - dest_cursor.shift));
    }

    shift_cursor.target++;
    shift_cursor.shift = 0;

    for (i = 0; i < pixels_until_first_boundary; i++) {
      sg_cursor_inc_x(&dest_cursor);
    }
  }

  for (i = 0; i < aligned_words; i++) {

    mask = (u32)-1;
    value = *shift_cursor.target;

    *shift_cursor.target = 0;

    // now put mask and value in dest
    (*dest_cursor.target) &= ~(mask << dest_cursor.shift);
    (*dest_cursor.target) |= (value << dest_cursor.shift);

    if (dest_cursor.shift > 0) {
      *(dest_cursor.target + 1)
        &= ~(mask >> (SG_BITS_PER_WORD - dest_cursor.shift));
      *(dest_cursor.target + 1)
        |= (value >> (SG_BITS_PER_WORD - dest_cursor.shift));
    }

    dest_cursor.target++;
    shift_cursor.target++;
  }

  if (pixels_after_last_boundary > 0) {
    operating_width = pixels_after_last_boundary;

    mask = ((1 << operating_width * SG_BITS_PER_PIXEL_VALUE(cursor->bmap)) - 1);
    value = (*shift_cursor.target) & mask;

    *shift_cursor.target &= ~(mask);

    // now put mask and value in dest
    (*dest_cursor.target) &= ~(mask << dest_cursor.shift);
    (*dest_cursor.target) |= (value << dest_cursor.shift);

    if (dest_cursor.shift > 0) {
      *(dest_cursor.target + 1)
        &= ~(mask >> (SG_BITS_PER_WORD - dest_cursor.shift));
      *(dest_cursor.target + 1)
        |= (value >> (SG_BITS_PER_WORD - dest_cursor.shift));
    }
  }
}

sg_color_t get_pixel(const sg_cursor_t *cursor) {
  sg_color_t color = (u32)-1;
  sg_bmap_data_t value;
  value = *(cursor->target) >> cursor->shift;
  color = value & SG_PIXEL_MASK(cursor->bmap);
  return color;
}

void sg_cursor_inc_x(sg_cursor_t *cursor) {
  cursor->shift += SG_BITS_PER_PIXEL_VALUE(cursor->bmap);
  if (cursor->shift == SG_BITS_PER_WORD) {
    cursor->target++; // go to the next word
    cursor->shift = 0;
  }
}

void copy_pixel(sg_cursor_t *dest, sg_cursor_t *src) {

  sg_color_t color = sg_cursor_get_pixel(src);
  if (src->bmap->bits_per_pixel > dest->bmap->bits_per_pixel) {
    // take only the most significant bits
    color = (color >> (src->bmap->bits_per_pixel - dest->bmap->bits_per_pixel));
  } else if (src->bmap->bits_per_pixel < dest->bmap->bits_per_pixel) {
    // lookup the color from a palette
    // convert from 1,2,4,8 to 2,4,8,16,32 using a palette if a palette is
    // available
    if (color) {
      color = color + dest->bmap->pen.color - 1;
    }
  }

  draw_pixel(dest, color);
  sg_cursor_inc_x(dest);
}

sg_size_t calc_pixels_until_first_boundary(
  const sg_cursor_t *cursor,
  sg_size_t w,
  sg_size_t shift) {
  sg_size_t pixels_until_first_boundary;

  pixels_until_first_boundary
    = (SG_BITS_PER_WORD - shift) / SG_BITS_PER_PIXEL_VALUE(cursor->bmap);

  if (pixels_until_first_boundary == SG_PIXELS_PER_WORD(cursor->bmap)) {
    pixels_until_first_boundary = 0;
  }

  if (pixels_until_first_boundary > w) {
    pixels_until_first_boundary = w;
  }

  return pixels_until_first_boundary;
}

sg_size_t calc_aligned_words(
  const sg_cursor_t *cursor,
  sg_size_t w,
  sg_size_t pixels_until_first_boundary) {
  return (w - pixels_until_first_boundary) / SG_PIXELS_PER_WORD(cursor->bmap);
}

sg_size_t calc_pixels_after_last_boundary(
  const sg_cursor_t *cursor,
  sg_size_t w,
  sg_size_t pixels_until_first_boundary,
  sg_size_t aligned_words) {
  return w - pixels_until_first_boundary
         - aligned_words * SG_PIXELS_PER_WORD(cursor->bmap);
}

void draw_pixel(const sg_cursor_t *cursor, sg_color_t color) {
  u16 o_flags = cursor->bmap->pen.o_flags;
  sg_bmap_data_t data = (color & SG_PIXEL_MASK(cursor->bmap)) << cursor->shift;
  if (o_flags & SG_PEN_FLAG_IS_ERASE) {
    // clear color bits
    *(cursor->target) &= ~data;
  } else if (o_flags & SG_PEN_FLAG_IS_INVERT) {
    // XOR color bits
    *(cursor->target) ^= data;
  } else if (o_flags & SG_PEN_FLAG_IS_BLEND) {
    // OR color bits
    *(cursor->target) |= data;
  } else if (o_flags & SG_PEN_FLAG_IS_ZERO_TRANSPARENT) {
    // ignore data if it is zero
    if (data) {
      *(cursor->target) &= ~(SG_PIXEL_MASK(cursor->bmap) << cursor->shift);
      *(cursor->target) |= data;
    }
  } else {
    // Assign color bits
    *(cursor->target) &= ~(SG_PIXEL_MASK(cursor->bmap) << cursor->shift);
    *(cursor->target) |= data;
  }
}

void draw_pixel_group(
  sg_bmap_data_t *word,
  sg_bmap_data_t pattern,
  sg_bmap_data_t mask,
  u16 o_flags) {
  if (o_flags & SG_PEN_FLAG_IS_ERASE) {
    *word &= ~pattern;
  } else if (o_flags & SG_PEN_FLAG_IS_INVERT) {
    *word ^= pattern;
  } else if (o_flags & SG_PEN_FLAG_IS_BLEND) {
    *word |= pattern;
  } else {
    *word &= mask;
    *word |= pattern;
  }
}

sg_bmap_data_t create_pattern(const sg_bmap_t *bmap, sg_color_t color) {
  sg_bmap_data_t pattern;
  sg_size_t i;

  pattern = 0;
  for (i = 0; i < SG_BITS_PER_WORD; i += SG_BITS_PER_PIXEL_VALUE(bmap)) {
    pattern |= ((color & SG_PIXEL_MASK(bmap)) << i);
  }
  return pattern;
}
