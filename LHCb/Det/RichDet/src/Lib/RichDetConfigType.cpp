
// local
#include "RichDet/RichDetConfigType.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichDetConfigType
//-----------------------------------------------------------------------------

std::string Rich::text( const Rich::RichPhDetConfigType conf )
{
  switch ( conf )
  {
  case Rich::HPDConfig:          return "HPDs";
  case Rich::PMTConfig:          return "MaPMTs";
  case Rich::InvalidPDConfig:    return "Invalid PDConfig";
  default:                       return "SHOULD NEVER SEE THIS";
  }
}
