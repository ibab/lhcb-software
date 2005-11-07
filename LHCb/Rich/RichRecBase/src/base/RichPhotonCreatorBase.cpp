
//---------------------------------------------------------------------------------
/** @file RichPhotonCreatorBase.cpp
 *
 *  Implementation file for tool base class : RichPhotonCreatorBase
 *
 *  CVS Log :-
 *  $Id: RichPhotonCreatorBase.cpp,v 1.6 2005-11-07 09:32:45 jonrob Exp $
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
  : RichRecToolBase         ( type, name, parent ),
    m_hasBeenCalled         ( false ),
    m_photonPredictor       ( 0 ),
    m_photonSignal          ( 0 ),
    m_ckAngle               ( 0 ),
    m_ckRes                 ( 0 ),
    m_Nevts                 ( 0 ),
    m_bookKeep              ( false ),
    m_photons               ( 0 ),
    m_richRecPhotonLocation ( RichRecPhotonLocation::Default ),
    m_photPredName          ( "RichPhotonPredictor" ),
    m_photCount             ( Rich::NRadiatorTypes, 0 ),
    m_photCountLast         ( Rich::NRadiatorTypes, 0 )
{

  // Define the interface
  declareInterface<IRichPhotonCreator>(this);

  // job options

  declareProperty( "DoBookKeeping", m_bookKeep );

  declareProperty( "PhotonPredictor", m_photPredName );

  m_CKTol.push_back( 0.005 );   // aerogel
  m_CKTol.push_back( 0.004 );      // c4f10
  m_CKTol.push_back( 0.003 );      // cf4
  declareProperty( "CherenkovThetaTolerence", m_CKTol );

  m_minCKtheta.push_back( 0 );   // aerogel
  m_minCKtheta.push_back( 0 );      // c4f10
  m_minCKtheta.push_back( 0 );      // cf4
  declareProperty( "MinAllowedCherenkovTheta", m_minCKtheta );

  m_maxCKtheta.push_back( 999 ); // aerogel
  m_maxCKtheta.push_back( 999 ); // c4f10
  m_maxCKtheta.push_back( 999 ); // cf4
  declareProperty( "MaxAllowedCherenkovTheta", m_maxCKtheta );

  m_minPhotonProb.push_back( 1e-15 ); // aerogel
  m_minPhotonProb.push_back( 1e-15 ); // c4f10
  m_minPhotonProb.push_back( 1e-15 ); // cf4
  declareProperty( "MinPhotonProbability", m_minPhotonProb );

  if      ( context() == "Offline" )
  {
    m_richRecPhotonLocation = RichRecPhotonLocation::Offline;
  }
  else if ( context() == "HLT" )
  {
    m_richRecPhotonLocation = RichRecPhotonLocation::HLT;
  }

}

StatusCode RichPhotonCreatorBase::initialize()
{
  // base class initilize
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "RichRecPhoton location : " << m_richRecPhotonLocation << endreq;
  }

  // get tools
  acquireTool( m_photPredName, m_photonPredictor  );
  acquireTool( "RichPhotonSignal", m_photonSignal );
  acquireTool( "RichCherenkovAngle",  m_ckAngle   );
  acquireTool( "RichCherenkovResolution", m_ckRes );

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );
  incSvc()->addListener( this, IncidentType::EndEvent   );

  // info printout
  for ( int rad = 0; rad < Rich::NRadiatorTypes; ++rad )
  {
    std::string trad = Rich::text((Rich::RadiatorType)rad);
    trad.resize(8,' ');
    info() << trad << " : CK theta range " << format("%5.3f",m_minCKtheta[rad])
           << " -> " << format("%5.3f",m_maxCKtheta[rad])
           << " rad : Tol. " << format("%5.3f",m_CKTol[rad]) << " mrad" << endreq;
  }

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

  if ( nEvents() > 0
       && !( m_photCount[Rich::Aerogel] == 0 &&
             m_photCount[Rich::C4F10]   == 0 &&
             m_photCount[Rich::CF4]     == 0  ) )
  {

    // statistical tool
    const RichStatDivFunctor occ("%10.2f +-%7.2f");

    // Print out final stats
    info() << "=================================================================" << endreq
           << "  Photon candidate summary : " << nEvents() << " events :-" << endreq
           << "    Aerogel   : "
           << occ(m_photCount[Rich::Aerogel],nEvents()) << "  photons/event" << endreq
           << "    C4F10     : "
           << occ(m_photCount[Rich::C4F10],nEvents())   << "  photons/event" << endreq
           << "    CF4       : "
           << occ(m_photCount[Rich::CF4],nEvents())     << "  photons/event" << endreq
           << "=================================================================" << endreq;

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
          RichRecPixels::const_iterator iPixel( pixelCreator()->begin(rich) );
          RichRecPixels::const_iterator endPix( pixelCreator()->end(rich)   );
          for ( ; iPixel != endPix; ++iPixel )
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

RichRecPhoton *
RichPhotonCreatorBase::checkForExistingPhoton( RichRecSegment * segment,
                                               RichRecPixel * pixel ) const
{
  // Form the key for this photon
  const RichRecPhotonKey photonKey( pixel->key(), segment->key() );

  // Try and find the photon
  return richPhotons()->object( photonKey );
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

void RichPhotonCreatorBase::buildCrossReferences( RichRecPhoton * photon ) const
{

  // Pointers to the segment and pixel
  RichRecSegment * segment = photon->richRecSegment();
  RichRecPixel * pixel     = photon->richRecPixel();

  // Add this pixel to the segment list of pixels that have a reconstructed photon
  segment->addToRichRecPixels( pixel );

  // Add this photon to the segment list of reconstructed photons
  segment->addToRichRecPhotons( photon );

  // Add this photon to the track list of reconstructed photons
  segment->richRecTrack()->addToRichRecPhotons( photon );

  // Add this photon to the pixel list of reconstructed photons
  pixel->addToRichRecPhotons( photon );

  // Add the track to the pixel list of associated tracks (those with valid photons)
  pixel->addToRichRecTracks( segment->richRecTrack() );

  // Add pixel to track list. Need to check if not already there for RICH1
  if ( Rich::Rich1 == segment->trackSegment().rich() )
  {
    RichRecTrack::Pixels & tkPixs = segment->richRecTrack()->richRecPixels();
    RichRecTrack::Pixels::iterator iPix = std::find( tkPixs.begin(), tkPixs.end(), pixel );
    if ( tkPixs.end() == iPix ) segment->richRecTrack()->addToRichRecPixels( pixel );
  }
  else // RICH2 - only one radiator type so no need to test
  {
    segment->richRecTrack()->addToRichRecPixels( pixel );
  }

}

void RichPhotonCreatorBase::InitNewEvent()
{
  m_hasBeenCalled = false;
  if ( bookKeep() ) m_photonDone.clear();
  m_photons = 0;
  m_photCountLast = m_photCount;
}

void RichPhotonCreatorBase::FinishEvent()
{
  if ( m_hasBeenCalled ) ++m_Nevts;
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Created " << richPhotons()->size() << " RichRecPhotons : Aerogel="
            << m_photCount[Rich::Aerogel]-m_photCountLast[Rich::Aerogel]
            << " C4F10=" << m_photCount[Rich::C4F10]-m_photCountLast[Rich::C4F10]
            << " CF4=" << m_photCount[Rich::CF4]-m_photCountLast[Rich::CF4] << endreq;
  }
}
