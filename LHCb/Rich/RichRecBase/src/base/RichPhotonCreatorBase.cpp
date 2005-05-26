
//---------------------------------------------------------------------------------
/** @file RichPhotonCreatorBase.cpp
 *
 *  Implementation file for tool base class : RichPhotonCreatorBase
 *
 *  CVS Log :-
 *  $Id: RichPhotonCreatorBase.cpp,v 1.1 2005-05-26 16:45:51 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   20/05/2005
 */
//---------------------------------------------------------------------------------

// local
#include "RichRecBase/RichPhotonCreatorBase.h"

//-----------------------------------------------------------------------------

// Standard constructor, initializes variables
RichPhotonCreatorBase::RichPhotonCreatorBase( const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent )
  : RichRecToolBase   ( type, name, parent ),
    m_hasBeenCalled   ( false ),
    m_photonPredictor ( 0 ),
    m_photonSignal    ( 0 ),
    m_Nevts           ( 0 ),
    m_bookKeep        ( false ),
    m_photons         ( 0 ),
    m_photCount       ( Rich::NRadiatorTypes, 0 ),
    m_photCountLast   ( Rich::NRadiatorTypes, 0 )
{

  // Define the interface
  declareInterface<IRichPhotonCreator>(this);

  // job options

  declareProperty( "RichRecPhotonLocation",
                   m_richRecPhotonLocation = RichRecPhotonLocation::Default );

  declareProperty( "DoBookKeeping", m_bookKeep );

  m_minCKtheta.push_back( 0.09 );   // aerogel
  m_minCKtheta.push_back( 0 );      // c4f10
  m_minCKtheta.push_back( 0 );      // cf4
  declareProperty( "MinCherenkovTheta", m_minCKtheta );

  m_maxCKtheta.push_back( 0.300 ); // aerogel
  m_maxCKtheta.push_back( 0.080 ); // c4f10
  m_maxCKtheta.push_back( 0.050 ); // cf4
  declareProperty( "MaxCherenkovTheta", m_maxCKtheta );

  m_minPhotonProb.push_back( 1e-15 ); // aerogel
  m_minPhotonProb.push_back( 1e-15 ); // c4f10
  m_minPhotonProb.push_back( 1e-15 ); // cf4
  declareProperty( "MinPhotonProbability", m_minPhotonProb );

}

StatusCode RichPhotonCreatorBase::initialize()
{
  // base class initilize
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // get tools
  acquireTool( "RichPhotonPredictor",   m_photonPredictor );
  acquireTool( "RichPhotonSignal",      m_photonSignal    );

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );
  incSvc()->addListener( this, IncidentType::EndEvent   );

  return sc;
}

StatusCode RichPhotonCreatorBase::finalize()
{
  // print stats
  printStats();

  // base class finalize
  return RichRecToolBase::finalize();
}

// Method that handles various Gaudi "software events"
void RichPhotonCreatorBase::handle ( const Incident& incident )
{
  // Update prior to start of event. Used to re-initialise data containers
  if ( IncidentType::BeginEvent == incident.type() )
  {
    InitNewEvent();
  }
  // Debug printout at the end of each event
  else if ( IncidentType::EndEvent == incident.type() )
  {
    FinishEvent();
  }
}

void RichPhotonCreatorBase::printStats() const
{

  if ( m_Nevts > 0
       && !( m_photCount[Rich::Aerogel] == 0 &&
             m_photCount[Rich::C4F10]   == 0 &&
             m_photCount[Rich::CF4]     == 0  ) )
  {

    // statistical tool
    RichStatDivFunctor occ("%10.2f +-%7.2f");

    // Print out final stats
    info() << "=======================================================" << endreq
           << "  Photon Candidate Summary : " << m_Nevts << " events :-" << endreq
           << "    Aerogel   "
           << occ(m_photCount[Rich::Aerogel],m_Nevts) << "  photons/event" << endreq
           << "    C4F10     "
           << occ(m_photCount[Rich::C4F10],m_Nevts)   << "  photons/event" << endreq
           << "    CF4       "
           << occ(m_photCount[Rich::CF4],m_Nevts)     << "  photons/event" << endreq
           << "=======================================================" << endreq;

  }
  else
  {
    Warning( "No photon candidates produced", StatusCode::SUCCESS );
  }

}

void RichPhotonCreatorBase::reconstructPhotons() const
{

  // make a rough guess at a size to reserve based on number of pixels
  if ( richPhotons()->empty() )
    richPhotons()->reserve( 10 * pixelCreator()->richPixels()->size() );

  // Iterate over all tracks
  for ( RichRecTracks::const_iterator iTrack =
          trackCreator()->richTracks()->begin();
        iTrack != trackCreator()->richTracks()->end();
        ++iTrack )
  {
    RichRecTrack * track = *iTrack;

    if ( !track->inUse() ) continue; // skip tracks not "on"
    if ( !track->allPhotonsDone() )
    {

      // Iterate over segments
      for ( RichRecTrack::Segments::const_iterator iSegment =
              track->richRecSegments().begin();
            iSegment != track->richRecSegments().end();
            ++iSegment)
      {
        RichRecSegment * segment = *iSegment;

        if ( !segment->allPhotonsDone() )
        {

          // Iterate over pixels in same RICH as this segment
          const Rich::DetectorType rich = segment->trackSegment().rich();
          for ( RichRecPixels::const_iterator iPixel = pixelCreator()->begin(rich);
                iPixel != pixelCreator()->end(rich); ++iPixel )
          {
            reconstructPhoton( segment, *iPixel );

          } // pixel loop

          segment->setAllPhotonsDone(true);
        }

      } // segment loop

      track->setAllPhotonsDone(true);
    }

  } // track loop

}

RichRecPhoton*
RichPhotonCreatorBase::reconstructPhoton( RichRecSegment * segment,
                                          RichRecPixel * pixel ) const
{
  // check photon is possible before proceeding
  if ( !m_photonPredictor->photonPossible(segment, pixel) ) return NULL;

  // flag this tool as having been called
  m_hasBeenCalled = true;

  // Form the key for this photon
  const RichRecPhotonKey photonKey( pixel->key(), segment->key() );

  // See if this photon already exists
  if ( bookKeep() && m_photonDone[ photonKey ] )
  {
    // return pre-made photon
    return static_cast<RichRecPhoton*>( richPhotons()->object(photonKey) );
  }
  else
  {
    // return brand new photon
    return buildPhoton( segment, pixel, photonKey );
  }

}

RichRecPhotons * RichPhotonCreatorBase::richPhotons() const
{

  // is this the first call this event ?
  if ( !m_photons )
  {

    if ( !exist<RichRecPhotons>(m_richRecPhotonLocation) )
    {

      // Reinitialise the Photon Container
      m_photons = new RichRecPhotons();

      // Register new RichRecPhoton container to Gaudi data store
      put( m_photons, m_richRecPhotonLocation );

    }
    else
    {

      // get photons from TES
      m_photons = get<RichRecPhotons>(m_richRecPhotonLocation);
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "Found " << m_photons->size()
                << " pre-existing RichRecPhotons in TES at "
                << m_richRecPhotonLocation << endreq;
      }

      // Remake local photon reference map
      if ( bookKeep() )
      {
        for ( RichRecPhotons::const_iterator iPhoton = m_photons->begin();
              iPhoton != m_photons->end();
              ++iPhoton )
        {
          m_photonDone[ (*iPhoton)->key() ] = true;
        }
      }

    }
  }

  return m_photons;
}


const RichRecTrack::Photons &
RichPhotonCreatorBase::reconstructPhotons( RichRecTrack * track ) const
{
  if ( !track->allPhotonsDone() )
  {

    // Iterate over segments
    for ( RichRecTrack::Segments::iterator segment =
            track->richRecSegments().begin();
          segment != track->richRecSegments().end(); ++segment )
    {
      reconstructPhotons( *segment );
    }

    track->setAllPhotonsDone(true);
  }

  return track->richRecPhotons();
}

const RichRecSegment::Photons &
RichPhotonCreatorBase::reconstructPhotons( RichRecSegment * segment ) const
{
  if ( !segment->allPhotonsDone() )
  {

    // Iterate over pixels
    for ( RichRecPixels::iterator pixel =
            pixelCreator()->richPixels()->begin();
          pixel != pixelCreator()->richPixels()->end();
          ++pixel )
    {
      reconstructPhoton( segment, *pixel );
    }

    segment->setAllPhotonsDone(true);
  }

  return segment->richRecPhotons();
}

const RichRecPixel::Photons &
RichPhotonCreatorBase::reconstructPhotons( RichRecPixel * pixel ) const
{
  // Iterate over tracks
  for ( RichRecTracks::iterator track =
          trackCreator()->richTracks()->begin();
        track != trackCreator()->richTracks()->end();
        ++track )
  {
    if ( !(*track)->inUse() ) continue;
    reconstructPhotons( *track, pixel );
  }

  return pixel->richRecPhotons();
}

// Note to self. Need to review what this method passes back
RichRecTrack::Photons
RichPhotonCreatorBase::reconstructPhotons( RichRecTrack * track,
                                           RichRecPixel * pixel ) const
{
  RichRecTrack::Photons photons;

  // Iterate over segments
  for ( RichRecTrack::Segments::iterator segment =
          track->richRecSegments().begin();
        segment != track->richRecSegments().end();
        ++segment )
  {
    RichRecPhoton * photon = reconstructPhoton( *segment, pixel );
    if ( photon ) photons.push_back( photon );
  }

  return photons;
}
