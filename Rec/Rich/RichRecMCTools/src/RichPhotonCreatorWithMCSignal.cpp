
//-----------------------------------------------------------------------------------------------
/** @file RichPhotonCreatorWithMCSignal.cpp
 *
 *  Implementation file for RICH reconstruction tool : RichPhotonCreatorWithMCSignal
 *
 *  CVS Log :-
 *  $Id: RichPhotonCreatorWithMCSignal.cpp,v 1.5 2006-04-03 12:47:12 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------------------------

// local
#include "RichPhotonCreatorWithMCSignal.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichPhotonCreatorWithMCSignal>          s_factory ;
const        IToolFactory& RichPhotonCreatorWithMCSignalFactory = s_factory ;

// Standard constructor
RichPhotonCreatorWithMCSignal::
RichPhotonCreatorWithMCSignal( const std::string& type,
                               const std::string& name,
                               const IInterface* parent )
  : RichPhotonCreatorBase ( type, name, parent ),
    m_mcRecTool           ( NULL ),
    m_mcPhotCr            ( NULL ),
    m_recoPhotCr          ( NULL ) { }

StatusCode RichPhotonCreatorWithMCSignal::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichPhotonCreatorBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRecMCTruthTool",    m_mcRecTool  );
  acquireTool( "RichPhotonCreatorMC",   m_mcPhotCr   );
  acquireTool( "RichPhotonCreatorReco", m_recoPhotCr );

  return sc;
}

StatusCode RichPhotonCreatorWithMCSignal::finalize()
{
  // Execute base class method
  return RichPhotonCreatorBase::finalize();
}

RichRecPhoton *
RichPhotonCreatorWithMCSignal::buildPhoton( RichRecSegment * segment,
                                            RichRecPixel * pixel,
                                            const RichRecPhotonKey key ) const
{

  RichRecPhoton * newPhoton = NULL;

  // See if there is a true cherenkov photon for this segment/pixel pair
  const MCRichOpticalPhoton * mcPhoton = m_mcRecTool->trueOpticalPhoton(segment,pixel);

  if ( mcPhoton )
  {
    // Use MC creator
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Delegating photon creation to MC tool" << endreq;
    }
    newPhoton = m_mcPhotCr->reconstructPhoton(segment,pixel);
  }
  else
  {
    // Use Reco creator
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Delegating photon creation to Reco tool" << endreq;
    }
    newPhoton = m_recoPhotCr->reconstructPhoton(segment,pixel);
  }

  // Add to reference map
  if ( bookKeep() ) m_photonDone[key] = true;

  // Return pointer to this photon
  return newPhoton;

}
