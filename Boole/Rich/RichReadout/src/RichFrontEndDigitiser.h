#ifndef RichFrontEndDigitiser_h
#define RichFrontEndDigitiser_h 1

#include "RichTimeSample.h"

class RichFrontEndDigitiser {

public:

  RichFrontEndDigitiser(double threshold, double calib )
    :  m_adc_cut   ( threshold*calib )
  {
    m_adc_cut = 35; // temporary hack
  }

  ~RichFrontEndDigitiser() { }

  bool process ( RichTimeSample & sample ) const;

private:

  double m_adc_cut;

};

inline bool RichFrontEndDigitiser::process (RichTimeSample & sample) const
{
  for ( unsigned int bin = 0; bin < sample.size(); ++bin ) {
    if ( sample[bin] >= m_adc_cut ) return true;
  }
  return false;
}

#endif
