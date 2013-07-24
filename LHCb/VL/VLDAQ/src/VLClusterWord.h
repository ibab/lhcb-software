#ifndef VL_CLUSTER_WORD_H
#define VL_CLUSTER_WORD_H 1

/** @class VLClusterWord
 *
 *  Decoding/encoding of VeloLite cluster words
 *
 */

class VLClusterWord {

public:
  /// Constructors
  VLClusterWord(unsigned int channel, double isp, bool highAdc) {
    unsigned int position = (unsigned int)(isp * (1 << interStripPrecision));
    unsigned int threshold = (unsigned int)(highAdc);
    m_value = (position << positionBits) + (channel << channelBits) + 
              (threshold << thresholdBits);
  }
  explicit VLClusterWord(unsigned int value) : m_value(value) {}
  /// Destructor
  ~VLClusterWord() {}

  unsigned int value() const {return m_value;}
  double interStripPosition() const {
    unsigned int position = (m_value & positionMask) >> positionBits;
    return position / double(1 << interStripPrecision);
  }
  unsigned int channel() const {
    return (m_value & channelMask) >> channelBits;
  }
  bool highAdc() const {
    return ((m_value & thresholdMask) >> thresholdBits != 0) ? true : false;
  }
 
private:
 
  unsigned short m_value;

  enum bits {
    positionBits        =  0,
    channelBits         =  3,
    thresholdBits       = 15,
    interStripPrecision =  3 
  };
  
  enum mask {
    positionMask  = 0x0007,
    channelMask   = 0x7ff8,
    thresholdMask = 0x8000
  };

};
#endif

