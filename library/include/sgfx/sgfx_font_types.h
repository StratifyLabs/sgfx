// COPYING: Copyright 2011-2020 Tyler Gilbert and Stratify Labs, Inc; see
// LICENSE.md for rights.

#ifndef SGFX_FONT_H_
#define SGFX_FONT_H_

#include <mcu/types.h>

#define SG_FONT_VERSION 0x0301
#define SG_FONT_ICON_MAX_NAME_LENGTH 47

enum {
  SG_FONT_TYPE_NONE,
  SG_FONT_MEMORY,
  SG_FONT_FILE,
  SG_FONT_VECTOR_MEMORY,
  SG_FONT_VECTOR_FILE,
  SG_FONT_TOTAL
};

enum { SG_FONT_FLAG_IS_BOLD = (1 << 0) };
#define SG_FONT_FLAG_BOLD SG_FONT_FLAG_IS_BOLD

typedef struct MCU_PACK {
  u8 type;
  u8 flags;
  u16 height;
  union {
    void *font_ptr;
    const char *font_path;
  };
  u32 font_file_offset;
} sg_font_ref_t;

/*! \brief Defines the header for a bitmap font */
/*! \details A bitmap font looks like this in memory (or a file):
 *
 * sg_font_header_t hdr;
 * sg_font_kerning_pair_t kerning0;
 * sg_font_kerning_pair_t kerning1;
 * ...
 * sg_font_char_t char0;
 * sg_font_char_t char1;
 * ...
 * sg_font_char_t char94;
 * const u32 char0_data[sg_calc_bmap_size(sg_dim(char0.width, char0.height)];
 * const u32 char1_data[sg_calc_bmap_size(sg_dim(char1.width, char1.height)];
 * ...
 * const u32 char94_data[sg_calc_bmap_size(sg_dim(char94.width, char94.height)];
 *
 */
typedef struct MCU_PACK {
  u8 character_count /*! Number of characters in a font */;
  u8 max_word_width /*! Max number of bytes per line of a font */;
  u8 max_height /*! Maximum height of a character */;
  u8 bits_per_pixel /*! Number of bits in each pixel of the font bitmap */;
  u32 size /*! Number of bytes the header occupies (header, character desc, and
              kerning info) */
    ;
  u16 kerning_pair_count /*! Number of kerning pairs stored in the font */;
  u16 version /*! Version of the font format */;
  u16 canvas_width /*! Width of the canvas in pixels */;
  u16 canvas_height /*! Height of the canvas in pixels */;
} sg_font_header_t;

/*! \brief Font Character Data
 * \details Holds the data for a font character.
 */
typedef struct MCU_PACK {
  u16 id;
  s16 canvas_x /*! X location of the character on the canvas */;
  s16 canvas_y /*! Y location of the character on the canvas */;
  u8 canvas_idx /*! The offset of the canvas */;
  u8 width /*! Width of the character */;
  u8 height /*! Height of the character */;
  u8 advance_x /*! How far to advance the cursor for this letter*/;
  s8 offset_x /*! Character's x offset */;
  s8 offset_y /*! Character's y offset */;
} sg_font_char_t;

typedef struct MCU_PACK {
  u16 unicode_first;
  u16 unicode_second;
  s16 horizontal_kerning;
} sg_font_kerning_pair_t;

typedef struct MCU_PACK {
  u16 version /*! Version of the font format */;
  u16 icon_count /*! Number of characters in a font */;
  u32 size /*! Number of bytes the header occupies (header + names) */;
  u8 max_width /*! Max Icon width */;
  u8 max_height /*! Max Icon height */;
  u8 bits_per_pixel /*! Number of bits in each pixel of the font bitmap */;
  u8 resd;
  u16 canvas_width /*! Width of the canvas in pixels */;
  u16 canvas_height /*! Height of the canvas in pixels */;
} sg_font_icon_header_t;

typedef struct MCU_PACK {
  u16 id;
  s16 canvas_x /*! X location of the character on the canvas */;
  s16 canvas_y /*! Y location of the character on the canvas */;
  u16 canvas_idx /*! The offset of the canvas */;
  u16 width /*! Width of the icon */;
  u16 height /*! Height of the icon */;
  char name[SG_FONT_ICON_MAX_NAME_LENGTH + 1];
} sg_font_icon_t;

#endif /* SGFX_FONT_H_ */
