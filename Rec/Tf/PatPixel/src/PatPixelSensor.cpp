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
PatPixelSensor::PatPixelSensor( DeVeloPixSensor* sensor ) : 
  m_previous(-1),
  m_sensor( sensor ) 
{
  m_number  = sensor->sensorNumber();
  m_isRight = sensor->isRight();
  DeVeloPixSquareType* sqDet = static_cast<const DeVeloPixSquareType*>( sensor );
  for ( unsigned int chipNum = 0 ; 12 > chipNum ; ++chipNum ) {
    if ( 0 > sqDet->WhichLadder(chipNum ) ) break;
    LHCb::VeloPixChannelID id0( m_number, chipNum,   1,   1 );
    LHCb::VeloPixChannelID idl( m_number, chipNum, 101,   1 );
    LHCb::VeloPixChannelID idh( m_number, chipNum,   1, 101 );
    std::pair<double,double> offsets(0.,0.);
    Gaudi::XYZPoint p0 = sqDet->globalXYZ( id0, offsets );
    Gaudi::XYZPoint pl = sqDet->globalXYZ( idl, offsets );
    Gaudi::XYZPoint ph = sqDet->globalXYZ( idh, offsets );
    Gaudi::XYZVector slx = 0.01 * (pl-p0);
    Gaudi::XYZVector sly = 0.01 * (ph-p0);
    p0 = p0 - slx - sly;
    m_chips.push_back( ChipParams( p0, slx, sly ) );
  }
  m_z       = sensor->z();
  reset();
}
//=============================================================================
