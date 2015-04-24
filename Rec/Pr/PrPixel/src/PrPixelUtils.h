#ifndef PRPIXELUTILS_H
#define PRPIXELUTILS_H

/** @file PrPixelUtils.h
 *
 *  Useful named constants and utilities for algorithms and
 *  tools in PrPixel.
 *
 *  @author Kurt Rinnert
 *  @date   2014-03-10
 */

namespace PrPixel {

/// named constants for module layout
enum {
  CHIP_COLUMNS = 256,
  CHIP_0_END = CHIP_COLUMNS - 1,
  CHIP_1_START = CHIP_COLUMNS,
  CHIP_1_END = 2 * CHIP_COLUMNS - 1,
  CHIP_2_START = 2 * CHIP_COLUMNS,
};

}

#endif  // PRPIXELUTILS_H
