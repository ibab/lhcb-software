
//-----------------------------------------------------------------------------
/** @file RichPhotonCreatorCheatedTrackDir.cpp
 *
 *  Implementation file for tool : RichPhotonCreatorCheatedTrackDir
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichPhotonCreatorCheatedTrackDir.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec::MC;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( PhotonCreatorCheatedTrackDir )

// Standard constructor
PhotonCreatorCheatedTrackDir::PhotonCreatorCheatedTrackDir( const std::string& type,
                                                            const std::string& name,
                                                            const IInterface* parent )
  : PhotonCreatorBase ( type, name, parent ),
    m_mcRecTool       ( NULL ),
    m_recoPhotCr      ( NULL ) 
{
  declareProperty( "CheatDirections", m_cheatDir = true  );
  declareProperty( "CheatPositions",  m_cheatPos = true  );
}

StatusCode PhotonCreatorCheatedTrackDir::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = PhotonCreatorBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRecMCTruthTool",   m_mcRecTool  );
  acquireTool( "RichDelPhotonCreator", m_recoPhotCr );

  return sc;
}

LHCb::RichRecPhoton *
PhotonCreatorCheatedTrackDir::buildPhoton( LHCb::RichRecSegment * segment,
                                           LHCb::RichRecPixel * pixel,
                                           const Rich::Rec::PhotonKey key ) const
{
  LHCb::RichRecPhoton * newPhoton            = NULL;
  const LHCb::MCRichOpticalPhoton * mcPhoton = NULL;
  
  if ( (m_cheatDir || m_cheatPos) && 
       (mcPhoton = m_mcRecTool->trueOpticalPhoton(segment,pixel)) != NULL )
  {

    // Store initial state information
    const Gaudi::XYZPoint storedMidPoint   = segment->trackSegment().middlePoint();
    const Gaudi::XYZVector storedMidMom    = segment->trackSegment().middleMomentum();
    const Gaudi::XYZPoint storedEntryPoint = segment->trackSegment().entryPoint();
    const Gaudi::XYZVector storedEntryMom  = segment->trackSegment().entryMomentum();
    const Gaudi::XYZPoint storedExitPoint  = segment->trackSegment().exitPoint();
    const Gaudi::XYZVector storedExitMom   = segment->trackSegment().exitMomentum();

    // cheat track direction info
    if ( m_cheatDir )
    {
      segment->trackSegment().setEntryState ( storedEntryPoint,
                                              mcPhoton->parentMomentum() );
      segment->trackSegment().setExitState  ( storedExitPoint,
                                              mcPhoton->parentMomentum() );
    }
    segment->trackSegment().setMiddleState( (m_cheatPos ? mcPhoton->emissionPoint()  : storedMidPoint),
                                            (m_cheatDir ? mcPhoton->parentMomentum() : storedMidMom)   );
    segment->trackSegment().reset();

    // Reconstruct the photon (uses the cheated track info)
    newPhoton = m_recoPhotCr->reconstructPhoton(segment,pixel);

    // Reset the track back to original settings
    segment->trackSegment().setEntryState ( storedEntryPoint,
                                            storedEntryMom );
    segment->trackSegment().setMiddleState( storedMidPoint,
                                            storedMidMom );
    segment->trackSegment().setExitState  ( storedExitPoint,
                                            storedExitMom );
    segment->trackSegment().reset();

  }
  else
  {
    // just reconstruct as per normal
    newPhoton = m_recoPhotCr->reconstructPhoton(segment,pixel);
  }

  // Add to reference map
  if ( bookKeep() ) m_photonDone[key] = true;

  // Return pointer to this photon
  return newPhoton;
}
