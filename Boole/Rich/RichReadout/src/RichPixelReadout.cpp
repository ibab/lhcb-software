
#include "RichPixelReadout.h"
#include "RichShape.h"
#include "RichShape_flat.h"
#include "RichNoisifier.h"
#include "RichFrontEndDigitiser.h"

RichPixelReadout::RichPixelReadout()
  :  m_shape     (0),
     m_shape_flat(0),
     m_noisifier (0),
     m_frontEnd  (0)
{

  // Need to work out how to handle this data
  //  m_frameSize       = 100;
  m_frameSize       = 25;
  double peakTime   = 25.0;
  double peakTime2   = 25.0;
  m_sigmaElecNoise  = 0.9;
  double calib      = 4.42;
  double threshold  = 8.8;
  m_baseline        = 50;

  m_shape      = new RichShape( peakTime, 2.7 );
  m_shape_flat = new RichShape_flat( peakTime2, 2.7 );
  m_noisifier  = new RichNoisifier();
  m_frontEnd   = new RichFrontEndDigitiser( threshold * m_sigmaElecNoise,
                                            calib );

}

RichPixelReadout::~RichPixelReadout() { cleanUp(); }

void RichPixelReadout::cleanUp() {

  if ( m_shape )     { delete m_shape;     m_shape = 0; }
  if ( m_shape_flat ){ delete m_shape_flat; m_shape_flat = 0; }
  if ( m_noisifier ) { delete m_noisifier; m_noisifier = 0; }
  if ( m_frontEnd )  { delete m_frontEnd;  m_frontEnd = 0; }

}

