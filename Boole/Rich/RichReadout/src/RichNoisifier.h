
#ifndef RICHREADOUT_RICHNOSIFIER_H
#define RICHREADOUT_RICHNOSIFIER_H 1

#include <vector>

#include "RichTimeSample.h"
#include "RichPixel.h"

#include "CLHEP/Random/RandGauss.h"

class RichNoisifier {

public:

  RichNoisifier() {}

  ~RichNoisifier() {}

  RichTimeSample noisify ( const RichPixel* ) const;

private:

  RandGauss* gaussDist;

};

inline RichTimeSample RichNoisifier::noisify( const RichPixel* id ) const
{
  //double sigma = id->Readout()->SigmaElecNoise() ;
  return RichTimeSample( id, id->Readout()->FrameSize(), 0 );
}

#endif // RICHREADOUT_RICHNOSIFIER_H
