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
  // DeVPSquareType* sqDet = static_cast<const DeVPSquareType*>( sensor );
  const DeVPSquareType* sqDet = sensor->squareType();
  for ( unsigned int chipNum = 0 ; 12 > chipNum ; ++chipNum )  // loop over chips
  { if ( 0 > sqDet->WhichLadder(chipNum ) ) break;
    LHCb::VPChannelID id0( m_number, chipNum,   1,   1 );      // ChannelID corresponding to this chip pixel (  1,   1)
    LHCb::VPChannelID idl( m_number, chipNum, 101,   1 );      // ChannelID corresponding to this chip pixel (101,   1)
    LHCb::VPChannelID idh( m_number, chipNum,   1, 101 );      // ChannelID corresponding to this chip pixel (  1, 101)
    std::pair<double,double> offsets(0.0, 0.0);
    Gaudi::XYZPoint p0 = sqDet->globalXYZ( id0, offsets );     // global X,Y,Z position of chip reference point = pixel(1,1)
    Gaudi::XYZPoint pl = sqDet->globalXYZ( idl, offsets );
    Gaudi::XYZPoint ph = sqDet->globalXYZ( idh, offsets );
    Gaudi::XYZVector slx = 0.01 * (pl-p0);                     // vector for axis along the low-resolution edge of the chip
    Gaudi::XYZVector sly = 0.01 * (ph-p0);                     // vector for axis along	the high-resolution edge	of the chip
    p0 = p0 - slx - sly;                                       // correct the reference point so it corresponds to pixel (0,0)
    m_chips.push_back( ChipParams( p0, slx, sly ) );           // add this chip to the list
  }
  m_z = sensor->z();
  reset();
}

//=============================================================================
