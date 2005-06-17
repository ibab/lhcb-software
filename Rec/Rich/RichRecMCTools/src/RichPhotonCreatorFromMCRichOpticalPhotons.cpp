
//-----------------------------------------------------------------------------------------------
/** @file RichPhotonCreatorFromMCRichOpticalPhotons.cpp
 *
 *  Implementation file for RICH reconstruction tool : RichPhotonCreatorFromMCRichOpticalPhotons
 *
 *  CVS Log :-
 *  $Id: RichPhotonCreatorFromMCRichOpticalPhotons.cpp,v 1.8 2005-06-17 15:28:34 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------------------------

// local
#include "RichPhotonCreatorFromMCRichOpticalPhotons.h"

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichPhotonCreatorFromMCRichOpticalPhotons>          s_factory ;
const        IToolFactory& RichPhotonCreatorFromMCRichOpticalPhotonsFactory = s_factory ;

// Standard constructor
RichPhotonCreatorFromMCRichOpticalPhotons::
RichPhotonCreatorFromMCRichOpticalPhotons( const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent )
  : RichPhotonCreatorBase ( type, name, parent ),
    m_mcRecTool           ( 0 ) {  }

StatusCode RichPhotonCreatorFromMCRichOpticalPhotons::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichPhotonCreatorBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRecMCTruthTool", m_mcRecTool    );

  return sc;
}

StatusCode RichPhotonCreatorFromMCRichOpticalPhotons::finalize()
{
  // Execute base class method
  return RichPhotonCreatorBase::finalize();
}

RichRecPhoton *
RichPhotonCreatorFromMCRichOpticalPhotons::buildPhoton( RichRecSegment * segment,
                                                        RichRecPixel * pixel,
                                                        const RichRecPhotonKey key ) const
{

  RichRecPhoton * newPhoton = NULL;

  // See if there is a true cherenkov photon for this segment/pixel pair
  const MCRichOpticalPhoton * mcPhoton = m_mcRecTool->trueOpticalPhoton(segment,pixel);
  if ( mcPhoton )
  {

    // Check angles are reasonable
    if ( ( mcPhoton->cherenkovTheta() > 0. ||
           mcPhoton->cherenkovPhi() > 0. ) &&
         checkAngleInRange( segment, mcPhoton->cherenkovTheta() ) )
    {

      // make a photon object from MC info
      RichGeomPhoton geomPhot( mcPhoton->cherenkovTheta(),
                               mcPhoton->cherenkovPhi(),
                               mcPhoton->emissionPoint(),
                               mcPhoton->pdIncidencePoint(),
                               mcPhoton->sphericalMirrorReflectPoint(),
                               mcPhoton->flatMirrorReflectPoint(),
                               pixel->smartID(),
                               1 );

      // make new RichRecPhoton from the MC information
      newPhoton = new RichRecPhoton( geomPhot,
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

