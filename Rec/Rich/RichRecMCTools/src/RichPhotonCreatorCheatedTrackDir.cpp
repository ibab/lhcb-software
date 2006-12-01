
//-----------------------------------------------------------------------------
/** @file RichPhotonCreatorCheatedTrackDir.cpp
 *
 *  Implementation file for tool : RichPhotonCreatorCheatedTrackDir
 *
 *  CVS Log :-
 *  $Id: RichPhotonCreatorCheatedTrackDir.cpp,v 1.3 2006-12-01 16:18:23 cattanem Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "RichPhotonCreatorCheatedTrackDir.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( RichPhotonCreatorCheatedTrackDir );

// Standard constructor
RichPhotonCreatorCheatedTrackDir::RichPhotonCreatorCheatedTrackDir( const std::string& type,
                                                                    const std::string& name,
                                                                    const IInterface* parent )
  : RichPhotonCreatorBase ( type, name, parent ),
    m_mcRecTool           ( NULL ),
    m_recoPhotCr          ( NULL ) { }

StatusCode RichPhotonCreatorCheatedTrackDir::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichPhotonCreatorBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRecMCTruthTool",   m_mcRecTool  );
  acquireTool( "RichDelPhotonCreator", m_recoPhotCr );

  return sc;
}

StatusCode RichPhotonCreatorCheatedTrackDir::finalize()
{
  // Execute base class method
  return RichPhotonCreatorBase::finalize();
}

RichRecPhoton *
RichPhotonCreatorCheatedTrackDir::buildPhoton( RichRecSegment * segment,
                                               RichRecPixel * pixel,
                                               const RichRecPhotonKey key ) const
{
  // Store state information
  const Gaudi::XYZPoint storedMidPoint = segment->trackSegment().middlePoint();
  const Gaudi::XYZVector storedMidMom  = segment->trackSegment().middleMomentum();
  const Gaudi::XYZPoint storedEntryPoint = segment->trackSegment().entryPoint();
  const Gaudi::XYZVector storedEntryMom  = segment->trackSegment().entryMomentum();
  const Gaudi::XYZPoint storedExitPoint = segment->trackSegment().exitPoint();
  const Gaudi::XYZVector storedExitMom  = segment->trackSegment().exitMomentum();
  
  // See if there is a true cherenkov photon for this segment/pixel pair
  const MCRichOpticalPhoton * mcPhoton = m_mcRecTool->trueOpticalPhoton(segment,pixel);
  bool updatedTk ( false );
  if ( mcPhoton )
  {
    updatedTk = true;
    segment->trackSegment().setEntryState ( storedEntryPoint,
                                            mcPhoton->parentMomentum() );
    segment->trackSegment().setMiddleState( /*mcPhoton->emissionPoint(),*/ storedMidPoint,
                                            mcPhoton->parentMomentum() );
    segment->trackSegment().setExitState  ( storedExitPoint,
                                            mcPhoton->parentMomentum() );
    segment->trackSegment().reset();
  }

  // Reconstruct the photon (uses the cheated middle point)
  //RichRecPhoton * newPhoton = ( mcPhoton ? m_recoPhotCr->reconstructPhoton(segment,pixel) : NULL );
  RichRecPhoton * newPhoton = m_recoPhotCr->reconstructPhoton(segment,pixel);

  // Add to reference map
  if ( bookKeep() ) m_photonDone[key] = true;

  // Reset the track back to original settings
  if ( updatedTk )
  {
    segment->trackSegment().setEntryState ( storedEntryPoint,
                                            storedEntryMom );
    segment->trackSegment().setMiddleState( storedMidPoint,
                                            storedMidMom );
    segment->trackSegment().setExitState  ( storedExitPoint,
                                            storedExitMom );
    segment->trackSegment().reset();
  }

  // Return pointer to this photon
  return newPhoton;

}
