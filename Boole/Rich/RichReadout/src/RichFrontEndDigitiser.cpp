
#include "RichReadout/RichFrontEndDigitiser.h"
#include "RichReadout/RichTimeSample.h"
#include "Event/MCRichDigit.h"


StatusCode RichFrontEndDigitiser::process (RichTimeSample sample) const {

  StatusCode sc = StatusCode::FAILURE;

  for ( unsigned int bin = 0 ; bin < sample.size() ; bin++ ) {
    int value = int( sample[bin]*calib_ ) + offset_;

    if ( value >= adc_cut_ ) sc = StatusCode::SUCCESS;
  }

  return sc;
}
