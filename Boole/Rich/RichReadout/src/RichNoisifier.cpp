
#include "RichReadout/RichNoisifier.h"
#include "RichReadout/PixelReadout.h"
#include "RichReadout/Pixel.h"



RichNoisifier::RichNoisifier() {}

RichNoisifier::~RichNoisifier(){}

RichTimeSample RichNoisifier::noisify( const Pixel* id ) const
{

  int dim = id->Readout()->FrameSize() ;
  //double sigma = id->Readout()->SigmaElecNoise() ;
  
  return RichTimeSample(id, dim, 0.0);
}

