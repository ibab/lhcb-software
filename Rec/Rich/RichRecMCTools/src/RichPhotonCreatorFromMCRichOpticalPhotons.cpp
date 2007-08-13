
//-----------------------------------------------------------------------------------------------
/** @file RichPhotonCreatorFromMCRichOpticalPhotons.cpp
 *
 *  Implementation file for RICH reconstruction tool : RichPhotonCreatorFromMCRichOpticalPhotons
 *
 *  CVS Log :-
 *  $Id: RichPhotonCreatorFromMCRichOpticalPhotons.cpp,v 1.16 2007-08-13 12:37:07 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "RichPhotonCreatorFromMCRichOpticalPhotons.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec::MC;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( PhotonCreatorFromMCRichOpticalPhotons );

// Standard constructor
PhotonCreatorFromMCRichOpticalPhotons::
PhotonCreatorFromMCRichOpticalPhotons( const std::string& type,
                                       const std::string& name,
                                       const IInterface* parent )
  : PhotonCreatorBase ( type, name, parent ),
    m_mcRecTool       ( NULL ) {  }

StatusCode PhotonCreatorFromMCRichOpticalPhotons::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = PhotonCreatorBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRecMCTruthTool", m_mcRecTool );

  return sc;
}

LHCb::RichRecPhoton *
PhotonCreatorFromMCRichOpticalPhotons::buildPhoton( LHCb::RichRecSegment * segment,
                                                    LHCb::RichRecPixel * pixel,
                                                    const RichRecPhotonKey key ) const
{

  LHCb::RichRecPhoton * newPhoton = NULL;

  // See if there is a true cherenkov photon for this segment/pixel pair
  const LHCb::MCRichOpticalPhoton * mcPhoton = m_mcRecTool->trueOpticalPhoton(segment,pixel);
  if ( mcPhoton )
  {

    // Check angles are reasonable
    if ( ( mcPhoton->cherenkovTheta() > 0. ||
           mcPhoton->cherenkovPhi() > 0. ) &&
         checkAngleInRange( segment, mcPhoton->cherenkovTheta() ) )
    {

      // make a photon object from MC info
      LHCb::RichGeomPhoton * geomPhot =
        new LHCb::RichGeomPhoton( mcPhoton->cherenkovTheta(),
                                  mcPhoton->cherenkovPhi(),
                                  mcPhoton->emissionPoint(),
                                  mcPhoton->pdIncidencePoint(),
                                  mcPhoton->sphericalMirrorReflectPoint(),
                                  mcPhoton->flatMirrorReflectPoint(),
                                  pixel->hpdPixelCluster().primaryID(),
                                  1 );

      // make new RichRecPhoton from the MC information
      newPhoton = new LHCb::RichRecPhoton( geomPhot,
                                           segment,
                                           segment->richRecTrack(),
                                           pixel );

      // check photon signal probability
      if ( checkPhotonProb( newPhoton ) )
      {

        // save this photons to TES
        savePhoton( newPhoton, key );

        // Build cross-references between objects
        buildCrossReferences( newPhoton );

      }
      else
      {
        delete newPhoton;
        newPhoton = NULL;
      }

    }

  }

  // Add to reference map
  if ( bookKeep() ) m_photonDone[key] = true;

  // Return pointer to this photon
  return newPhoton;

}

