
//-----------------------------------------------------------------------------
/** @file RichPhotonCreator.cpp
 *
 *  Implementation file for tool : RichPhotonCreator
 *
 *  CVS Log :-
 *  $Id: RichPhotonCreator.cpp,v 1.28 2005-11-15 13:38:10 jonrob Exp $
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
    m_photonReco          ( 0 ),
    m_photonRecoName      ( "RichDetPhotonReco" )
{
  declareProperty( "PhotonRecoTool", m_photonRecoName );
}

StatusCode RichPhotonCreator::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichPhotonCreatorBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( m_photonRecoName, m_photonReco, this );

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
  RichGeomPhoton * geomPhoton = new RichGeomPhoton();
  if ( ( m_photonReco->reconstructPhoton( segment->trackSegment(),
                                          pixel->globalPosition(),
                                          *geomPhoton ).isSuccess() ) &&
       // Check photon is OK
       ( ( geomPhoton->CherenkovTheta() > 0. ||
           geomPhoton->CherenkovPhi()   > 0. ) &&
         checkAngleInRange( segment, geomPhoton->CherenkovTheta() ) ) )
  {

    // give photon same smart ID as pixel
    geomPhoton->setSmartID( pixel->smartID() );

    // make new RichRecPhoton ( NB will own geomPhoton )
    newPhoton = new RichRecPhoton( geomPhoton, segment,
                                   segment->richRecTrack(), pixel );

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
      delete newPhoton; // also deletes geomPhoton
      newPhoton = NULL;
    }

  }
  else
  {
    delete geomPhoton;
  }

  // Add to reference map
  if ( bookKeep() ) m_photonDone[ key ] = true;

  // Return pointer to this photon
  return newPhoton;

}
