/*
 * mbfont.height
 *
 *  Created on: Nov 22, 2013
 *      Author: tgil
 */

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
	u16 h;
	union {
		void * font_ptr;
		const char * font_path;
	};
	u32 font_file_offset;
} sg_font_ref_t;

/*! \brief Defines the header for a monochrome bitmap font */
/*! \details A monochrome bitmap font looks like this in memory (or a file):
 *
 * sg_font_header_t hdr;
 * sg_font_char_t char0;
 * sg_font_char_t char1;
 * ...
 * sg_font_char_t char94;
 * const char char0_data[char0.height][MFont::byte_width(char0.width)];
 * const char char1_data[char1.height][MFont::byte_width(char1.width)];
 * ...
 * const char char94_data[char0.height][MFont::byte_width(char94.width)];
 * sg_font_kerning_pair_t kerning0;
 * sg_font_kerning_pair_t kerning1;
 * ...
 *
 */
typedef struct MCU_PACK {
	u8 num_chars /*! Number of characters in a font */;
	u8 max_byte_width /*! Max number of bytes per line of a font */;
	u8 max_height /*! Maximum height of a character */;
	u8 bits_per_pixel /*! Number of bits in each pixel of the font bitmap */;
	u32 size /*! Number of bytes the font occupies */;
	u16 kerning_pairs /*! Number of kerning pairs stored in the font */;
	u16 version /*! Version of the font format */;
} sg_font_header_t;


typedef struct MCU_PACK {
	u16 id;
	u16 offset;
	u8 width;
	u8 height;
	u8 xadvance;
	u8 resd;
	s16 xoffset;
	s16 yoffset;
} sg_font_char_t;

typedef struct MCU_PACK {
	u8 first;
	u8 second;
	s16 kerning;
} sg_font_kerning_pair_t;


#endif /* SGFX_FONT_H_ */
