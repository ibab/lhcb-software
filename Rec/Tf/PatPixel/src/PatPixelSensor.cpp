// $Id: $
// Include files

// local
#include "PatPixelSensor.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatPixelSensor, from FastVelo
//
// 2012-01-05 : Olivier Callot
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatPixelSensor::PatPixelSensor( DeVPSensor* sensor ) : 
  m_previous(-1),
  m_sensor( sensor ) 
{
  m_number  = sensor->sensorNumber();
  m_isRight = sensor->isRight();
  // printf("PatPixelSensor(%02d:%c)\n", m_number, m_isRight?'R':'L' );
  for ( unsigned int chipNum = 0 ; 12 > chipNum ; ++chipNum )  // loop over chips
  { if ( 0 > sensor->ladderOfChip(chipNum ) ) break;
    LHCb::VPChannelID id0( m_number, chipNum,   1,   1 );      // ChannelID corresponding to this chip pixel (  1,   1)
    LHCb::VPChannelID idl( m_number, chipNum, 101,   1 );      // ChannelID corresponding to this chip pixel (101,   1)
    LHCb::VPChannelID idh( m_number, chipNum,   1, 101 );      // ChannelID corresponding to this chip pixel (  1, 101)
    std::pair<double,double> offsets(0.0, 0.0);
    Gaudi::XYZPoint p0 = sensor->channelToPoint( id0, offsets );     // global X,Y,Z position of chip reference point = pixel(1,1)
    Gaudi::XYZPoint pl = sensor->channelToPoint( idl, offsets );
    Gaudi::XYZPoint ph = sensor->channelToPoint( idh, offsets );
    Gaudi::XYZVector slx = 0.01 * (pl-p0);                     // vector for axis along the low-resolution edge of the chip
    Gaudi::XYZVector sly = 0.01 * (ph-p0);                     // vector for axis along	the high-resolution edge	of the chip
    p0 = p0 - slx - sly;                                       // correct the reference point so it corresponds to pixel (0,0)
    m_chips.push_back( ChipParams( p0, slx, sly ) );           // add this chip to the list
    // printf(" %2d: [%+7.3f,%+7.3f,%+8.3f] : [%+6.3f,%+6.3f,%+6.3f] x [%+6.3f,%+6.3f,%+6.3f]\n",
    //       chipNum, p0.x(), p0.y(), p0.z(), slx.x(), slx.y(), slx.z(), sly.x(), sly.y(), sly.z() );
  }
  m_z = sensor->z();
  // printf(" Z = %+8.3f\n", m_z);
  reset();
}

//=============================================================================
