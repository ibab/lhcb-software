#ifndef RichFrontEndDigitiser_h
#define RichFrontEndDigitiser_h 1

#include "RichReadout/PixelReadout.h"
#include "RichReadout/RichTimeSample.h"
#include "RichReadout/RichCoder.h"

#include "Event/MCRichDigit.h"

class RichTimeSample;
class PixelReadout;

class RichFrontEndDigitiser
{

public:

  RichFrontEndDigitiser(double t, double c, int o)
    :  threshold_(t), calib_(c), offset_(o), adc_cut_(int(t*c+o))
  {
    m_coder = new RichCoder();
  };

  virtual ~RichFrontEndDigitiser(){};

  StatusCode process ( RichTimeSample ) const;

private:

  double threshold_;
  double calib_;
  int offset_;
  int adc_cut_;

  RichCoder* m_coder;

};

#endif









