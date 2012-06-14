#ifndef VL_THRESHOLD_H 
#define VL_THRESHOLD_H 1

#include "Event/MCVLDigit.h"

/** @class VLThreshold VLThreshold.h
 *
 */

class VLThreshold: public std::unary_function<LHCb::MCVLDigit*, bool> {

public: 
  /// Constructors
  VLThreshold() : m_threshold(0.) {}
  VLThreshold(const double thr) : m_threshold(thr) {}
  /// Destructor
  virtual ~VLThreshold() {} 
  bool operator()(LHCb::MCVLDigit* digit) {
    return (digit->adc() < m_threshold);
  }

private:

  double m_threshold;   

};
#endif
