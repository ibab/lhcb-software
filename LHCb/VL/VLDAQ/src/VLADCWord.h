#ifndef VL_ADC_WORD_H
#define VL_ADC_WORD_H 1

/** @class VLADCWord
 *
 *  Decoding/encoding of VeloLite ADC words
 *
 */

class VLADCWord {

public:
  /// Constructors
  VLADCWord(unsigned int adc, bool centralStrip, bool endCluster) {
    unsigned int centre = (unsigned int)(centralStrip); 
    unsigned int end = (unsigned int)(endCluster); 
    const unsigned int adcMax = (1 << adcPrecision) - 1; 
    if (adc > adcMax) adc = adcMax;
    m_value = (adc << adcBits) + (centre << centreBits) + (end << endBits);
  }
  explicit VLADCWord(unsigned int value) : m_value(value) {}
  /// Destructor
  ~VLADCWord() {}

  unsigned char value() const {return m_value;}
  unsigned int adc() const {
    return (m_value & adcMask) >> adcBits;
  }
  bool centralStrip() const {
    return ((m_value & centreMask) >> centreBits != 0) ? true : false;
  }
  bool endCluster() const {
    return ((m_value & endMask) >> endBits != 0) ? true : false;
  }
 
private:
 
  unsigned char m_value;

  enum bits {
    adcBits      =  0,
    centreBits   =  6,
    endBits      =  7,
    adcPrecision =  6 
  };
  
  enum mask {
    adcMask    = 0x3f,
    centreMask = 0x40,
    endMask    = 0x80
  };

};
#endif

