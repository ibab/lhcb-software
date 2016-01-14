
//-----------------------------------------------------------------------------
/** @file RichPhotonEmissionPoint.cpp
 *
 *  Implementation file for tool : Rich::Rec::PhotonEmissionPoint
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichPhotonEmissionPoint.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( PhotonEmissionPoint )

// Standard constructor
PhotonEmissionPoint::PhotonEmissionPoint ( const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent )
  : ToolBase ( type, name, parent )
{
  // tool interface
  declareInterface<IPhotonEmissionPoint>(this);
}

PhotonEmissionPoint::~PhotonEmissionPoint() { }

bool PhotonEmissionPoint::emissionPoint( const LHCb::RichRecSegment * segment,
                                         const LHCb::RichRecPixel *,
                                         Gaudi::XYZPoint & emissPnt ) const
{
  if ( segment )
  {
    emissPnt = segment->trackSegment().bestPoint();
    return true;
  }
  return false;
}

bool PhotonEmissionPoint::emissionPoint( const LHCb::RichRecSegment * segment,
                                         const LHCb::RichRecPixel *,
                                         const double fracDist,
                                         Gaudi::XYZPoint & emissPnt ) const
{
  if ( segment )
  {
    emissPnt = segment->trackSegment().bestPoint(fracDist);
    return true;
  }
  return false;
}
