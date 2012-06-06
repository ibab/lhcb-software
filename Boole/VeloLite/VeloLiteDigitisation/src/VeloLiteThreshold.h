#ifndef VELOLITETHRESHOLD_H 
#define VELOLITETHRESHOLD_H 1

#include "Event/MCVeloLiteDigit.h"

/** @class VeloLiteThreshold VeloLiteThreshold.h
 *
 */

class VeloLiteThreshold: public std::unary_function<LHCb::MCVeloLiteDigit*, bool> {

public: 
  /// Constructors
  VeloLiteThreshold() : m_threshold(0.) {}
  VeloLiteThreshold(const double thr) : m_threshold(thr) {}
  /// Destructor
  virtual ~VeloLiteThreshold() {} 
  bool operator()(LHCb::MCVeloLiteDigit* digit) {
    return (digit->adc() < m_threshold);
  }

private:

  double m_threshold;   

};
#endif
