#ifndef VL_HEADER_WORD_H
#define VL_HEADER_WORD_H 1

/** @class VLHeaderWord
 *
 *  Decoding/encoding of VeloLite header words
 *  (based on SiHeaderWord)
 *
 */

class VLHeaderWord {

public:
  /// Constructors
  VLHeaderWord(unsigned int nClusters, unsigned int pcn, bool hasError) {
    unsigned int bError = (unsigned int)(hasError);
    m_value = (nClusters << clusterBits) + 
              (pcn << pcnBits) + (bError << errorBits);
  }
  explicit VLHeaderWord(unsigned int value) : m_value(value) {}
  /// Destructor
  ~VLHeaderWord() {}

  unsigned int value() const {return m_value;}

  unsigned int nClusters() const {
    return (m_value & clusterMask) >> clusterBits;
  }
  unsigned int pcn() const {
    return (m_value & pcnMask) >> pcnBits;
  }
  bool hasError() const {
    return ((m_value & errorMask) >> errorBits != 0 ? true : false);
  }

private:
 
  unsigned int m_value;

  enum bits {
    clusterBits =  0,
    pcnBits    =  16,
    errorBits  =  24 
  };
  
  enum mask {
    clusterMask = 0x00ffff,
    pcnMask     = 0xff0000,
    errorMask   = 0x10000000
  };

};
#endif

