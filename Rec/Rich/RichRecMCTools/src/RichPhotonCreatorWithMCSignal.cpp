
//-----------------------------------------------------------------------------------------------
/** @file RichPhotonCreatorWithMCSignal.cpp
 *
 *  Implementation file for RICH reconstruction tool : RichPhotonCreatorWithMCSignal
 *
 *  CVS Log :-
 *  $Id: RichPhotonCreatorWithMCSignal.cpp,v 1.1 2005-04-16 10:30:00 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------------------------

// local
#include "RichPhotonCreatorWithMCSignal.h"

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichPhotonCreatorWithMCSignal>          s_factory ;
const        IToolFactory& RichPhotonCreatorWithMCSignalFactory = s_factory ;

// Standard constructor
RichPhotonCreatorWithMCSignal::
RichPhotonCreatorWithMCSignal( const std::string& type,
                               const std::string& name,
                               const IInterface* parent )
  : RichRecToolBase  ( type, name, parent ),
    m_mcRecTool      ( 0 ),
    m_photons        ( 0 ),
    m_mcPhotCr       ( 0 ),
    m_recoPhotCr     ( 0 )
{

  declareInterface<IRichPhotonCreator>(this);

  declareProperty( "RichRecPhotonLocation",
                   m_richRecPhotonLocation = RichRecPhotonLocation::Default );

}

StatusCode RichPhotonCreatorWithMCSignal::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRecMCTruthTool",    m_mcRecTool  );
  acquireTool( "RichPhotonCreatorMC",   m_mcPhotCr   );
  acquireTool( "RichPhotonCreatorReco", m_recoPhotCr );

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );
  if (msgLevel(MSG::DEBUG)) incSvc()->addListener( this, IncidentType::EndEvent );

  // Make sure we are ready for a new event
  InitNewEvent();

  return sc;
}

StatusCode RichPhotonCreatorWithMCSignal::finalize()
{
  // Execute base class method
  return RichRecToolBase::finalize();
}

// Method that handles various Gaudi "software events"
void RichPhotonCreatorWithMCSignal::handle ( const Incident& incident )
{
  // Update prior to start of event. Used to re-initialise data containers
  if ( IncidentType::BeginEvent == incident.type() ) { InitNewEvent(); }
  // Debug printout at the end of each event
  else if ( msgLevel(MSG::DEBUG) && IncidentType::EndEvent == incident.type() )
  {
    debug() << "Created " << richPhotons()->size() << " RichRecPhotons" << endreq;
  }
}

RichRecPhoton*
RichPhotonCreatorWithMCSignal::reconstructPhoton( RichRecSegment * segment,
                                                  RichRecPixel * pixel ) const
{
  // Form the key for this photon
  const RichRecPhotonKey photonKey(pixel->key(),segment->key());

  // See if this photon already exists
  if ( m_photonDone[photonKey] ) {
    return static_cast<RichRecPhoton*>(richPhotons()->object(photonKey));
  } else {
    return buildPhoton( segment, pixel, photonKey );
  }

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
  { // Use MC creator
    debug() << "Delegating photon creation to MC tool" << endreq;
    newPhoton = m_mcPhotCr->reconstructPhoton(segment,pixel);
  } else 
  { // Use Reco creator
    debug() << "Delegating photon creation to Reco tool" << endreq;
    newPhoton = m_recoPhotCr->reconstructPhoton(segment,pixel);
  }

  // Add to reference map
  m_photonDone[key] = true;

  // Return pointer to this photon
  return newPhoton;

}

void RichPhotonCreatorWithMCSignal::reconstructPhotons() const
{
  const bool noPhots = richPhotons()->empty();

  // make a rough guess at a size to reserve based on number of pixels
  if ( noPhots ) richPhotons()->reserve( 5 * pixelCreator()->richPixels()->size() );

  // Iterate over all tracks
  for ( RichRecTracks::const_iterator iTrack =
          trackCreator()->richTracks()->begin();
        iTrack != trackCreator()->richTracks()->end();
        ++iTrack ) {
    RichRecTrack * track = *iTrack;
    if ( !track->inUse() ) continue; // skip tracks not "on"

    if ( !track->allPhotonsDone() ) {

      // Iterate over segments
      for ( RichRecTrack::Segments::const_iterator iSegment =
              track->richRecSegments().begin();
            iSegment != track->richRecSegments().end();
            ++iSegment) {
        RichRecSegment * segment = *iSegment;

        if ( !segment->allPhotonsDone() ) {

          // Iterate over pixels
          for ( RichRecPixels::const_iterator iPixel =
                  pixelCreator()->richPixels()->begin();
                iPixel != pixelCreator()->richPixels()->end();
                ++iPixel ) {
            RichRecPixel * pixel = *iPixel;

            // If container was empty, skip checks for whether photon already exists
            if ( noPhots ) {
              buildPhoton( segment, pixel,
                           RichRecPhotonKey(pixel->key(),segment->key()) );
            } else {
              reconstructPhoton( segment, pixel );
            }

          } // pixel loop

          segment->setAllPhotonsDone(true);
        }

      } // segment loop

      track->setAllPhotonsDone(true);
    }

  } // track loop

  debug() << "Created " << richPhotons()->size() << " RichRecPhotons" << endreq;

}

const RichRecTrack::Photons &
RichPhotonCreatorWithMCSignal::reconstructPhotons( RichRecTrack * track ) const
{
  if ( !track->allPhotonsDone() ) {

    // Iterate over segments
    for ( RichRecTrack::Segments::iterator segment =
            track->richRecSegments().begin();
          segment != track->richRecSegments().end(); ++segment ) {
      reconstructPhotons( *segment );
    }

    track->setAllPhotonsDone(true);
  }

  return track->richRecPhotons();
}

const RichRecSegment::Photons &
RichPhotonCreatorWithMCSignal::reconstructPhotons( RichRecSegment * segment ) const
{
  if ( !segment->allPhotonsDone() ) {

    // Iterate over pixels
    for ( RichRecPixels::iterator pixel =
            pixelCreator()->richPixels()->begin();
          pixel != pixelCreator()->richPixels()->end();
          ++pixel ) { reconstructPhoton( segment, *pixel ); }

    segment->setAllPhotonsDone(true);
  }

  return segment->richRecPhotons();
}

const RichRecPixel::Photons &
RichPhotonCreatorWithMCSignal::reconstructPhotons( RichRecPixel * pixel ) const
{
  // Iterate over tracks
  for ( RichRecTracks::iterator track =
          trackCreator()->richTracks()->begin();
        track != trackCreator()->richTracks()->end();
        ++track ) {
    if ( !(*track)->inUse() ) continue;
    reconstructPhotons( *track, pixel );
  }

  return pixel->richRecPhotons();
}

// Note to self. Need to review what this method passes back
RichRecTrack::Photons
RichPhotonCreatorWithMCSignal::reconstructPhotons( RichRecTrack * track,
                                                   RichRecPixel * pixel ) const
{
  RichRecTrack::Photons photons;

  // Iterate over segments
  for ( RichRecTrack::Segments::iterator segment =
          track->richRecSegments().begin();
        segment != track->richRecSegments().end();
        ++segment ) {

    RichRecPhoton * photon = reconstructPhoton( *segment, pixel );
    if ( photon ) photons.push_back( photon );

  }

  return photons;
}

RichRecPhotons * RichPhotonCreatorWithMCSignal::richPhotons() const
{
  if ( !m_photons ) {
    SmartDataPtr<RichRecPhotons> tdsPhotons( evtSvc(),
                                             m_richRecPhotonLocation );
    if ( !tdsPhotons ) {

      // Reinitialise the Photon Container
      m_photons = new RichRecPhotons();

      // Register new RichRecPhoton container to Gaudi data store
      put( m_photons, m_richRecPhotonLocation );

    } else {

      debug() << "Found " << tdsPhotons->size() << " pre-existing RichRecPhotons in TES at "
              << m_richRecPhotonLocation << endreq;

      // Set smartref to TES photon container
      m_photons = tdsPhotons;

      // Remake local photon reference map
      for ( RichRecPhotons::const_iterator iPhoton = tdsPhotons->begin();
            iPhoton != tdsPhotons->end();
            ++iPhoton ) {
        m_photonDone[(*iPhoton)->key()] = true;
      }

    }
  }
  return m_photons;
}
