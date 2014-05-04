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
  MODULE_SENSORS = 4,
  SENSOR_CHIPS = 3,
  CHIP_ROWS = 256,
  CHIP_COLUMNS = 256,
  CHIP_0_END = CHIP_COLUMNS - 1,
  CHIP_1_START = CHIP_COLUMNS,
  CHIP_1_END = 2 * CHIP_COLUMNS - 1,
  CHIP_2_START = 2 * CHIP_COLUMNS,
  SENSOR_ROWS = CHIP_ROWS,
  SENSOR_COLUMNS = CHIP_COLUMNS * SENSOR_CHIPS,
  SENSOR_PIXELS = SENSOR_ROWS * SENSOR_COLUMNS,
  TOT_MODULES = 52,
  TOT_SENSORS = TOT_MODULES * MODULE_SENSORS
};

/// named constants for lite cluster decoding
enum {
  HEADERNCLUSHIFT = 0,
  PIXELSHIFT = 0,
  TOTVALUESHIFT = 22,
  XFRACTSHIFT = 25,
  YFRACTSHIFT = 28,
  ISLONGSHIFT = 31,
  HEADERNCLUMASK = 0x0000FFFF,
  PIXELMASK = 0x003FFFFF,
  TOTVALUEMASK = 0x01C00000,
  XFRACTMASK = 0x0E000000,
  YFRACTMASK = 0x70000000,
  ISLONGMASK = 0x80000000
};
}

#endif  // PRPIXELUTILS_H
