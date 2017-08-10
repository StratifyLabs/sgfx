/*
 * sg_config.height
 *
 *  Created on: Jun 8, 2017
 *      Author: tgil
 */

#ifndef SG_CONFIG_H_
#define SG_CONFIG_H_

#include <stdio.h>

#if !defined SG_BITS_PER_PIXEL
#define SG_BITS_PER_PIXEL 1
#endif

#define SG_BITS_PER_WORD 32
#define SG_BYTES_PER_WORD (SG_BITS_PER_WORD/8)
#define SG_PIXELS_PER_WORD (SG_BITS_PER_WORD / SG_BITS_PER_PIXEL)
#define SG_PIXEL_MASK ((1<<SG_BITS_PER_PIXEL) - 1)



#endif /* SG_CONFIG_H_ */
