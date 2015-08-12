
//-----------------------------------------------------------------------------------------------
/** @file RichPhotonCreatorWithMCHitPosition.cpp
 *
 *  Implementation file for RICH reconstruction tool : RichPhotonCreatorWithMCHitPosition
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   06/11/2007
 */
//-----------------------------------------------------------------------------------------------

// local
#include "RichPhotonCreatorWithMCHitPosition.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec::MC;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( PhotonCreatorWithMCHitPosition )

// Standard constructor
PhotonCreatorWithMCHitPosition::PhotonCreatorWithMCHitPosition( const std::string& type,
                                                                const std::string& name,
                                                                const IInterface* parent )
  : PhotonCreatorBase ( type, name, parent ),
    m_mcRecTool       ( NULL ),
    m_recoPhotCr      ( NULL ) { }

PhotonCreatorWithMCHitPosition::~PhotonCreatorWithMCHitPosition() { }

StatusCode PhotonCreatorWithMCHitPosition::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = PhotonCreatorBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRecMCTruthTool",    m_mcRecTool  );
  acquireTool( "RichPhotonCreatorReco", m_recoPhotCr );

  // Issue a warning, to make sure user knows they are MC cheating
  Warning( "Using cheated photon creator with MC HPD hit positions", 
           StatusCode::SUCCESS );

  return sc;
}

LHCb::RichRecPhoton *
PhotonCreatorWithMCHitPosition::buildPhoton( LHCb::RichRecSegment * segment,
                                             LHCb::RichRecPixel * pixel,
                                             const Rich::Rec::PhotonKey key ) const
{

  LHCb::RichRecPhoton * newPhoton = NULL;

  // See if there is a true cherenkov photon for this segment/pixel pair
  const LHCb::MCRichOpticalPhoton * mcPhoton = m_mcRecTool->trueOpticalPhoton(segment,pixel);

  // Cache the original global position, so it can be put at after making the photon
  const Gaudi::XYZPoint original_point = pixel->globalPosition();

  // If we have a true photon, cheat the position
  if ( mcPhoton )
  {
    // Update the hit position to the MC one for this photon
    pixel->setGlobalPosition( mcPhoton->pdIncidencePoint() );
  }
  
  // make the photon
  newPhoton = m_recoPhotCr->reconstructPhoton(segment,pixel);

  // if need be, set the position back as before
  if ( mcPhoton )
  {
    pixel->setGlobalPosition( original_point );
  }
  
  // Add to reference map
  if ( bookKeep() ) m_photonDone[key] = true;
  
  // Return pointer to this photon
  return newPhoton;

}
