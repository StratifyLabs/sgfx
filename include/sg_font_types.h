/*! \file */ //Copyright 2011-2016 Tyler Gilbert; All Rights Reserved


#ifndef SGFX_FONT_H_
#define SGFX_FONT_H_


#include <mcu/types.h>


#define SG_FONT_TYPE_NONE 0
#define SG_FONT_MEMORY 1
#define SG_FONT_FILE 2

#define SG_FONT_FLAG_BOLD (1<<0)

typedef struct MCU_PACK {
	u8 type;
	u8 flags;
	u16 height;
	union {
		void * font_ptr;
		const char * font_path;
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
 * const u8 char0_data[sg_calc_bmap_size(sg_dim(char0.width, char0.height)];
 * const u32 char1_data[sg_calc_bmap_size(sg_dim(char1.width, char1.height)];
 * ...
 * const u32 char94_data[sg_calc_bmap_size(sg_dim(char94.width, char94.height)];
 *
 */
typedef struct MCU_PACK {
	u8 num_chars /*! Number of characters in a font */;
	u8 max_word_width /*! Max number of bytes per line of a font */;
	u8 max_height /*! Maximum height of a character */;
	u8 bits_per_pixel /*! Number of bits in each pixel of the font bitmap */;
	u32 size /*! Number of bytes the font occupies */;
	u16 kerning_pairs /*! Number of kerning pairs stored in the font */;
	u16 version /*! Version of the font format */;
} sg_font_header_t;

/*! \brief Font Character Data
 * \details Holds the data for a font character.
 */
typedef struct MCU_PACK {
	u16 id;
	u16 offset /*! Characters data location within the font file or memory;
	u8 width /*! Width of the character */;
	u8 height /*! Height of the character */;
	u8 xadvance;
	u8 resd;
	s16 xoffset /*! Character's x offset */;
	s16 yoffset /*! Character's y offset */;
} sg_font_char_t;

typedef struct MCU_PACK {
	u8 first;
	u8 second;
	s16 kerning;
} sg_font_kerning_pair_t;


#endif /* SGFX_FONT_H_ */
