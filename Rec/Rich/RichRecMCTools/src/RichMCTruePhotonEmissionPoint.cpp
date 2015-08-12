
//-----------------------------------------------------------------------------
/** @file RichMCTruePhotonEmissionPoint.cpp
 *
 *  Implementation file for tool : Rich::Rec::MC::TruePhotonEmissionPoint
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichMCTruePhotonEmissionPoint.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec::MC;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( TruePhotonEmissionPoint )

// Standard constructor
TruePhotonEmissionPoint::TruePhotonEmissionPoint ( const std::string& type,
                                                   const std::string& name,
                                                   const IInterface* parent )
  : Rich::Rec::ToolBase ( type, name, parent ),
    m_mcRecTool         ( NULL )
{
  // tool interface
  declareInterface<IPhotonEmissionPoint>(this);
}

TruePhotonEmissionPoint::~TruePhotonEmissionPoint() { }

StatusCode TruePhotonEmissionPoint::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = Rich::Rec::ToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRecMCTruthTool", m_mcRecTool );

  return sc;
}

bool TruePhotonEmissionPoint::emissionPoint( const LHCb::RichRecSegment * segment,
                                             const LHCb::RichRecPixel * pixel,
                                             Gaudi::XYZPoint & emissPnt ) const
{
  if ( segment )
  {
    const LHCb::MCRichOpticalPhoton * mcPhoton = m_mcRecTool->trueOpticalPhoton(segment,pixel);
    if ( mcPhoton )
    {
      emissPnt = mcPhoton->emissionPoint();
    }
    else
    {
      emissPnt = segment->trackSegment().bestPoint();
    }
    return true;
  }
  return false;
}

bool TruePhotonEmissionPoint::emissionPoint( const LHCb::RichRecSegment * segment,
                                             const LHCb::RichRecPixel * pixel,
                                             const double fracDist,
                                             Gaudi::XYZPoint & emissPnt ) const
{
  if ( segment )
  {
    const LHCb::MCRichOpticalPhoton * mcPhoton = m_mcRecTool->trueOpticalPhoton(segment,pixel);
    if ( mcPhoton )
    {
      emissPnt = mcPhoton->emissionPoint();
    }
    else
    {
      emissPnt = segment->trackSegment().bestPoint(fracDist);
    }
    return true;
  }
  return false;
}
