#ifndef VL_ADC_WORD_H
#define VL_ADC_WORD_H 1

/** @class VLADCWord
 *
 *  Decoding/encoding of VeloLite ADC words
 *  (based on SiADCWord)
 *
 */

class VLADCWord {

public:
  /// Constructors
  VLADCWord(double adc, bool endCluster) {
    unsigned int bEnd = (unsigned int)(endCluster); 
    unsigned char tAdc = char(adc);
    if (adc > ((1 << adcPrecision) - 1)) {
      tAdc = (1 << adcPrecision) - 1;
    }
    m_value = (tAdc << adcBits) + (bEnd << endBits);
  }
  explicit VLADCWord(unsigned int value) : m_value(value) {}
  /// Destructor
  ~VLADCWord() {}

  unsigned char value() const {return m_value;}
  double adc() const {
    return double((m_value & adcMask) >> adcBits);
  }
  bool endCluster() const {
    return ((m_value & endMask) >> endBits != 0u) ? true : false;
  }
 
private:
 
  unsigned char m_value;

  enum bits {
    adcBits      =  0,
    endBits      =  7,
    adcPrecision =  7 
  };
  
  enum mask {
    adcMask = 0x7f,
    endMask = 0x80
  };

};
#endif

