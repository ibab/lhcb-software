
//-----------------------------------------------------------------------------------------------
/** @file RichPhotonCreatorWithMCSignal.cpp
 *
 *  Implementation file for RICH reconstruction tool : RichPhotonCreatorWithMCSignal
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------------------------

// local
#include "RichPhotonCreatorWithMCSignal.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec::MC;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( PhotonCreatorWithMCSignal )

// Standard constructor
PhotonCreatorWithMCSignal::PhotonCreatorWithMCSignal( const std::string& type,
                                                      const std::string& name,
                                                      const IInterface* parent )
  : PhotonCreatorBase     ( type, name, parent ),
    m_mcRecTool           ( NULL ),
    m_mcPhotCr            ( NULL ),
    m_recoPhotCr          ( NULL ) { }

PhotonCreatorWithMCSignal::~PhotonCreatorWithMCSignal() { }

StatusCode PhotonCreatorWithMCSignal::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = PhotonCreatorBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRecMCTruthTool",    m_mcRecTool  );
  acquireTool( "RichPhotonCreatorMC",   m_mcPhotCr   );
  acquireTool( "RichPhotonCreatorReco", m_recoPhotCr );

  // Issue a warning, to make sure user knows they are MC cheating
  Warning( "Using cheated photon creator that uses MC for signal photons", 
           StatusCode::SUCCESS );

  return sc;
}

LHCb::RichRecPhoton *
PhotonCreatorWithMCSignal::buildPhoton( LHCb::RichRecSegment * segment,
                                        LHCb::RichRecPixel * pixel,
                                        const Rich::Rec::PhotonKey key ) const
{

  LHCb::RichRecPhoton * newPhoton = NULL;

  // See if there is a true cherenkov photon for this segment/pixel pair
  const LHCb::MCRichOpticalPhoton * mcPhoton = m_mcRecTool->trueOpticalPhoton(segment,pixel);

  if ( mcPhoton )
  {
    // Use MC creator
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Delegating photon creation to MC tool" << endmsg;
    }
    newPhoton = m_mcPhotCr->reconstructPhoton(segment,pixel);
  }
  else
  {
    // Use Reco creator
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Delegating photon creation to Reco tool" << endmsg;
    }
    newPhoton = m_recoPhotCr->reconstructPhoton(segment,pixel);
  }

  // Add to reference map
  if ( bookKeep() ) m_photonDone[key] = true;

  // Return pointer to this photon
  return newPhoton;

}
