
//-----------------------------------------------------------------------------
/** @file RichPhotonPredictorUsingMCRichOpticalPhotons.cpp
 *
 *  Implementation file for RICH reconstruction tool : 
 *        Rich::Rec::PhotonPredictorUsingMCRichOpticalPhotons
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichPhotonPredictorUsingMCRichOpticalPhotons.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec::MC;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( PhotonPredictorUsingMCRichOpticalPhotons )

// Standard constructor
PhotonPredictorUsingMCRichOpticalPhotons::
PhotonPredictorUsingMCRichOpticalPhotons( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : ToolBase    ( type, name, parent ),
    m_mcRecTool ( NULL )
{
  declareInterface<IPhotonPredictor>(this);
}

// Destructor
PhotonPredictorUsingMCRichOpticalPhotons::~PhotonPredictorUsingMCRichOpticalPhotons() {}

StatusCode PhotonPredictorUsingMCRichOpticalPhotons::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = ToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRecMCTruthTool", m_mcRecTool );

  return sc;
}

// fast decision on whether a photon is possible
bool PhotonPredictorUsingMCRichOpticalPhotons::
photonPossible( LHCb::RichRecSegment * segment,
                LHCb::RichRecPixel * pixel ) const
{
  // Are they in the same Rich detector ?
  if ( segment->trackSegment().rich() != pixel->detector() ) return false;

  // Check if this combination due to a true cherenkov hit using MC
  const LHCb::MCRichOpticalPhoton * mcPhoton = m_mcRecTool->trueOpticalPhoton(segment,pixel);
  return ( mcPhoton ? true : false );
}

