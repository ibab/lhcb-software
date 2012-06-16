#ifndef VL_CLUSTER_WORD_H
#define VL_CLUSTER_WORD_H 1

/** @class VLClusterWord
 *
 *  Decoding/encoding of VeloLite cluster words
 *  (based on SiClusterWord)
 *
 */

class VLClusterWord {

public:
  /// Constructors
  VLClusterWord(unsigned int channel, double isp,
                unsigned int clusterSize, bool highThreshold) {

    unsigned int position = (unsigned int)(isp * (1 << interStripPrecision));
    unsigned int sBit = clusterSize > 2 ? 1 : 0;
    unsigned int tBit = (unsigned int)(highThreshold);
    m_value = (position << positionBits) + (channel  << channelBits) +
              (sBit << sizeBits)+ (tBit << thresBits);
  }
  explicit VLClusterWord(unsigned int value) : m_value(value) {}
  /// Destructor
  ~VLClusterWord() {}

  unsigned int value() const {return m_value;}
  unsigned int fracStripBits() const {
    return (m_value & positionMask) >> positionBits;
  }
  unsigned int pseudoSizeBits() const {
    return (m_value & sizeMask) >> sizeBits;
  }
  double interStripPosition() const {
    return fracStripBits() / double(1 << interStripPrecision);
  }
  unsigned int channel() const {
    return (m_value & channelMask) >> channelBits;
  }
  unsigned int pseudoSize() const {
    unsigned int cSize = 1 + ((m_value & sizeMask) >> sizeBits);
    if (cSize == 1) {
      if (fracStripBits() > 0) cSize = 2;
    } else {
      cSize = 3;
    }
    return cSize;
  }
  bool hasHighThreshold() const {
    return ((m_value & thresMask) >> thresBits != 0) ? true : false;
  }
 
private:
 
  unsigned short m_value;

  enum bits {
    positionBits        =  0,
    channelBits         =  2,
    sizeBits            = 14,
    thresBits           = 15,
    interStripPrecision =  2
  };
  
  enum mask {
    positionMask = 0x0007,
    channelMask  = 0x3ffc,
    sizeMask     = 0x4000,
    thresMask    = 0x8000
  };

};
#endif

