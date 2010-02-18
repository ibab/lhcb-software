
#include "RichPixelReadout.h"
#include "RichShape.h"
#include "RichFrontEndDigitiser.h"

using namespace Rich::MC::Digi;

RichPixelReadout::RichPixelReadout()
  :  m_shape          ( new RichShape( 25, 2.7 )       ),
     m_frontEnd       ( new RichFrontEndDigitiser()    ),
     m_frameSize      ( 25                             ),
     m_baseline       ( 50                             ),
     m_sigmaElecNoise ( 0.9                            )
{

  //const double peakTime    = 25.0;
  //const double calib       = 4.42;
  //const double threshold   = 8.8;
  //m_shape      = new RichShape( peakTime, 2.7 );
  //m_frontEnd   = new RichFrontEndDigitiser( threshold * m_sigmaElecNoise, calib );

}

void RichPixelReadout::cleanUp()
{
  if ( m_shape )     { delete m_shape;      m_shape = 0;      }
  if ( m_frontEnd )  { delete m_frontEnd;   m_frontEnd = 0;   }
}

