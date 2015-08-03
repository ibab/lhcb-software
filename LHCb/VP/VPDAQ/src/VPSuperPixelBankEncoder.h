#pragma once
#include <vector>
#include <algorithm>
#include <string>

#include "GaudiAlg/GaudiAlgorithm.h"

#include "Event/RawEvent.h"
#include "Event/VPDigit.h"
#include "Kernel/VPConstants.h"

/** @class VPSuperPixelBankEncoder VPSuperPixelBankEncoder.h
 * Algorithm to create Super Pixel Raw Bank from VP digits.
 *
 * There is one raw bank per sensor, that is the sensor number (0-207)
 * is the source ID of the bank. Note that this means there is no
 * need to encode the sensor in the Super Pixel addresses.
 *
 * Each bank has a four byte word header, followed by a four byte
 * Super Pixel word for each Super Pixel on the sensor.
 *
 * The header word is currently simply the number of Super Pixels
 * on the sensor. There are plenty of bits available to encode
 * error conditions and such in the future.
 *
 * The Super Pixel word encoding is the following:
 *
 * bit 0-7    Super Pixel Pattern
 * bit 8-13   Super Pixel Row (0-63)
 * bit 14-22  Super Pixel Column (0-383)
 * bit 23-30  UNUSED
 * bit 31     Super Pixel isolation flag
 *
 * Super Pixel Pattern bits encode columns and rows as follows:
 *
 * row,y
 *
 *  ^  37
 *  |  26
 *  |  15
 *  |  04
 *  +---> col,x
 * 
 * @author Karol Hennessy
 * @author Kurt Rinnert
 * @date   2014-03-06
 */

class VPSuperPixelBankEncoder : public GaudiAlgorithm {

 public:

  /// Standard constructor
  VPSuperPixelBankEncoder(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~VPSuperPixelBankEncoder();  ///< Destructor
  virtual StatusCode initialize();     ///< Algorithm initialization
  virtual StatusCode execute();        ///< Algorithm execution

 private:

  /// to sort super pixels by column (major) and row (minor)
  struct SPLowerThan {
    inline bool operator()(unsigned int lhs, unsigned int rhs) const {
      return (lhs & 0x7FFF00) < (rhs & 0x7FFF00);
    }
  };

  /// where to get the digits from
  std::string m_digitLocation;
  /// where to write the clusters to
  std::string m_rawEventLocation;
  /// bank version. (change this every time semantics change!)
  const unsigned int m_bankVersion;
  /// quick check for debug state
  bool m_isDebug;
  /// quick check for verbose state
  bool m_isVerbose;
  /// event counter
  unsigned int m_evt;
  /// per sensor buffers of super pixel words
  std::vector<std::vector<unsigned int> > m_spBySensor;
  /// buffer for checking super pixel neighbours
  unsigned char m_buffer[VP::NPixelsPerSensor];
  /// buffer for all non-zero super pixel indices on a sensor
  std::vector<unsigned int> m_idx;
};
