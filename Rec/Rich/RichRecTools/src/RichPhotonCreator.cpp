
//-----------------------------------------------------------------------------
/** @file RichPhotonCreator.cpp
 *
 *  Implementation file for tool : RichPhotonCreator
 *
 *  CVS Log :-
 *  $Id: RichPhotonCreator.cpp,v 1.26 2005-05-28 13:10:53 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichPhotonCreator.h"

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichPhotonCreator>          s_factory ;
const        IToolFactory& RichPhotonCreatorFactory = s_factory ;

// Standard constructor
RichPhotonCreator::RichPhotonCreator( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : RichPhotonCreatorBase ( type, name, parent ),
    m_photonReco          ( 0 ) { }

StatusCode RichPhotonCreator::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichPhotonCreatorBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichDetPhotonReco", m_photonReco );

  return sc;
}

StatusCode RichPhotonCreator::finalize()
{
  // Execute base class method
  return RichPhotonCreatorBase::finalize();
}

RichRecPhoton * 
RichPhotonCreator::buildPhoton( RichRecSegment * segment,
                                RichRecPixel * pixel,
                                const RichRecPhotonKey key ) const
{

  RichRecPhoton * newPhoton = NULL;

  // Reconstruct the geometrical photon
  RichGeomPhoton geomPhoton;
  if ( m_photonReco->reconstructPhoton( segment->trackSegment(),
                                        pixel->globalPosition(),
                                        geomPhoton ) != 0 )
  {

    // Which radiator
    const Rich::RadiatorType rad = segment->trackSegment().radiator();
    
    // Check angles are reasonable
    if ( ( geomPhoton.CherenkovTheta() > 0. ||
           geomPhoton.CherenkovPhi() > 0. ) &&
         geomPhoton.CherenkovTheta() < maxCKTheta(rad) &&
         geomPhoton.CherenkovTheta() > minCKTheta(rad) ) 
    {

      // give photon same smart ID as pixel
      geomPhoton.setSmartID( pixel->smartID() );

      // make new RichRecPhoton
      newPhoton = new RichRecPhoton( geomPhoton, segment,
                                     segment->richRecTrack(), pixel );

      // shall we keep this photon ?
      if ( keepPhoton( newPhoton ) )
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
  if ( bookKeep() ) m_photonDone[ key ] = true;

  // Return pointer to this photon
  return newPhoton;

}
