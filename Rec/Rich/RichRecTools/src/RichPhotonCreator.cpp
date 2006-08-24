
//-----------------------------------------------------------------------------
/** @file RichPhotonCreator.cpp
 *
 *  Implementation file for tool : RichPhotonCreator
 *
 *  CVS Log :-
 *  $Id: RichPhotonCreator.cpp,v 1.33 2006-08-24 12:26:35 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichPhotonCreator.h"

// namespaces
using namespace LHCb;

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
                                          *geomPhoton,
                                          pixel->smartID() ).isSuccess() ) &&
       // Check photon is OK
       ( ( geomPhoton->CherenkovTheta() > 0. ||
           geomPhoton->CherenkovPhi()   > 0. ) &&
         checkAngleInRange( segment, geomPhoton->CherenkovTheta() ) ) )
  {

    // make new RichRecPhoton ( NB will own geomPhoton )
    newPhoton = new RichRecPhoton( geomPhoton, segment,
                                   segment->richRecTrack(), pixel );

    // check photon signal probability
    if ( checkPhotonProb( newPhoton ) )
    {
      if ( msgLevel(MSG::VERBOSE) )
      {
        verbose() << "  -> RichRecPhoton selected" << endreq;
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
        verbose() << "  -> RichRecPhoton FAILED probability checks" << endreq;
      }
      delete newPhoton; // also deletes geomPhoton
      newPhoton = NULL;
    }

  }
  else
  {
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "   -> RichGeomPhoton reconstruction FAILED" << endreq;
    }
    delete geomPhoton;
  }

  // Add to reference map
  if ( bookKeep() ) m_photonDone[ key ] = true;

  // Return pointer to this photon
  return newPhoton;

}
