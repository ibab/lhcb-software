
//-----------------------------------------------------------------------------
/** @file RichPhotonCreator.cpp
 *
 *  Implementation file for tool : Rich::Rec::PhotonCreator
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichPhotonCreator.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

// Standard constructor
PhotonCreator::PhotonCreator( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : PhotonCreatorBase ( type, name, parent )
{
  // job options
  declareProperty( "PhotonRecoTool", m_photonRecoName = "PhotonParams" );
}

PhotonCreator::~PhotonCreator() { }

StatusCode PhotonCreator::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = PhotonCreatorBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( m_photonRecoName, m_photonReco, this );

  return sc;
}

LHCb::RichRecPhoton *
PhotonCreator::buildPhoton( LHCb::RichRecSegment * segment,
                            LHCb::RichRecPixel * pixel,
                            const Rich::Rec::PhotonKey key ) const
{

  LHCb::RichRecPhoton * newPhoton = nullptr;

  // Reconstruct the geometrical photon
  LHCb::RichGeomPhoton * geomPhoton = new LHCb::RichGeomPhoton();
  if ( ( m_photonReco->reconstructPhoton( segment,
                                          pixel,
                                          *geomPhoton ).isSuccess() ) &&
       // Check photon is OK
       ( ( geomPhoton->CherenkovTheta() > 0. ||
           geomPhoton->CherenkovPhi()   > 0. ) &&
         checkAngleInRange( segment, geomPhoton->CherenkovTheta() ) ) )
  {

    // make new RichRecPhoton ( NB will own geomPhoton )
    newPhoton = new LHCb::RichRecPhoton( geomPhoton, segment,
                                         segment->richRecTrack(), pixel );

    // check photon signal probability
    if ( checkPhotonProb( newPhoton ) )
    {
      if ( msgLevel(MSG::VERBOSE) )
      {
        verbose() << "  -> RichRecPhoton selected" << endmsg;
      }
      // save this photon to TES
      savePhoton( newPhoton, key );

      // Build cross-references between objects
      buildCrossReferences( newPhoton );
    }
    else
    {
      if ( msgLevel(MSG::VERBOSE) )
      {
        verbose() << "  -> RichRecPhoton FAILED probability checks" << endmsg;
      }
      delete newPhoton; // also deletes geomPhoton
      newPhoton = nullptr;
    }

  }
  else
  {
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "   -> RichGeomPhoton reconstruction FAILED" << endmsg;
    }
    delete geomPhoton;
  }

  // Add to reference map
  if ( bookKeep() ) m_photonDone[ key ] = true;

  // Return pointer to this photon
  return newPhoton;

}

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( PhotonCreator )

//-----------------------------------------------------------------------------
