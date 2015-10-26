
//-----------------------------------------------------------------------------
/** @file RichPhotonCreatorBase.cpp
 *
 *  Implementation file for tool base class : Rich::Rec::PhotonCreatorBase
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   20/05/2005
 */
//-----------------------------------------------------------------------------

// local
#include "RichRecBase/RichPhotonCreatorBase.h"

// boost
#include "boost/format.hpp"

//-----------------------------------------------------------------------------

using namespace Rich::Rec;

// Standard constructor, initializes variables
PhotonCreatorBase::PhotonCreatorBase( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : ToolBase            ( type, name, parent ),
    m_hasBeenCalled     ( false ),
    m_photonPredictor   ( NULL ),
    m_photonSignal      ( NULL ),
    m_ckAngle           ( NULL ),
    m_ckRes             ( NULL ),
    m_richPartProp      ( NULL ),
    m_Nevts             ( 0    ),
    m_photons           ( NULL ),
    m_photCount         ( Rich::NRadiatorTypes, 0 ),
    m_photCountLast     ( Rich::NRadiatorTypes, 0 )
{
  // Define the interface
  declareInterface<IPhotonCreator>(this);

  // defaults         Aero   R1Gas  R2Gas
  m_minCKtheta     = {0.075, 0.005, 0.005} ;
  m_maxCKtheta     = {0.320, 0.075, 0.035} ;
  m_minPhotonProb  = {1e-15, 1e-15, 1e-15} ;
  //if ( contextContains("HLT") )
  //{
  //  m_nSigma = { 3.5, 2.8, 3.0 } ;
  //}
  //else // Offline settings
  //{
  m_nSigma         = { 3.8,  3.3,   3.3  } ;
  //}

  // set properties
  declareProperty( "RichRecPhotonLocation",
                   m_richRecPhotonLocation = contextSpecificTES(LHCb::RichRecPhotonLocation::Default),
                   "The TES location for the transient RichRecPhoton objects" );
  declareProperty( "DoBookKeeping", m_bookKeep = true,
                   "Turn on/off the book keeping of which pixels have been made" );
  declareProperty( "PhotonPredictor", m_photPredName = "RichPhotonPredictor",
                   "The photon predictor(pre-selection) tool name" );
  declareProperty( "MinAllowedCherenkovTheta", m_minCKtheta,
                   "The minimum allowed CK theta values for each radiator (Aero/R1Gas/R2Gas)" );
  declareProperty( "MaxAllowedCherenkovTheta", m_maxCKtheta,
                   "The maximum allowed CK theta values for each radiator (Aero/R1Gas/R2Gas)" );
  declareProperty( "MinPhotonProbability", m_minPhotonProb,
                   "The minimum allowed photon probability values for each radiator (Aero/R1Gas/R2Gas)" );
  declareProperty( "NSigma", m_nSigma,
                   "The CK theta # sigma selection range for each radiator (Aero/R1Gas/R2Gas)");
  declareProperty( "MaxPhotons", m_maxPhotons = 9999999,
                   "The maximum number of photon candidates to allow per event" );
  declareProperty( "MaxPhotDetOcc", m_maxHPDOccForReco = {15, 99999, 99999},
                   "Max photon detector occupancy for each radiator (Aero/R1Gas/R2Gas)" );
  declareProperty( "RejectAeroPhotonsIfGas", m_rejAeroPhotsIfGas = false );

}

const std::vector<std::string> & PhotonCreatorBase::photonCreatorJobOptions() const
{
  static const std::vector<std::string> jos =
   {{"RichRecPhotonLocation"},
    {"DoBookKeeping"},
    {"PhotonPredictor"},
    {"MinAllowedCherenkovTheta"},
    {"MaxAllowedCherenkovTheta"},
    {"MinPhotonProbability"},
    {"NSigma"},
    {"MaxPhotons"},
    {"MaxPhotDetOcc"},
    {"RejectAeroPhotonsIfGas"}};
  return jos;
}

StatusCode PhotonCreatorBase::initialize()
{
  // base class initilize
  const StatusCode sc = ToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  if ( m_richRecPhotonLocation.empty() )
  {
    return Error( "RichRecPhoton location is undefined. Please set" );
  }
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "RichRecPhoton location : " << m_richRecPhotonLocation << endmsg;
  }

  // get tools
  acquireTool( m_photPredName, "Predictor", m_photonPredictor, this );
  acquireTool( "RichPhotonSignal", m_photonSignal );
  acquireTool( "RichCherenkovAngle",  m_ckAngle   );
  acquireTool( "RichCherenkovResolution", m_ckRes );
  acquireTool( "RichParticleProperties",  m_richPartProp );

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );
  incSvc()->addListener( this, IncidentType::EndEvent   );

  // info printout
  for ( int rad = 0; rad < Rich::NRadiatorTypes; ++rad )
  {
    std::string trad = Rich::text((Rich::RadiatorType)rad);
    trad.resize(8,' ');
    if (  msgLevel(MSG::DEBUG) )
      debug() << trad << " : CK theta range " << boost::format("%5.3f") % m_minCKtheta[rad]
              << " -> " << boost::format("%5.3f") % m_maxCKtheta[rad]
              << " rad : Tol. " << boost::format("%5.3f") % m_nSigma[rad] << " sigma "
              << endmsg;
  }

  m_pidTypes = m_richPartProp->particleTypes();

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Maximum HPD pixels (Aero/R1Gas/R2Gas) = " << m_maxHPDOccForReco << endmsg;
    debug() << "Particle types considered = " << m_pidTypes << endmsg;
    debug() << "Maximum number of photon candidates per event = " << m_maxPhotons << endmsg;
    if ( m_rejAeroPhotsIfGas )
      debug() << "Will reject Aerogel photons if pixel has a Rich1 Gas photon" << endmsg;
  }

  return sc;
}

StatusCode PhotonCreatorBase::finalize()
{
  // print stats
  printStats();

  // base class finalize
  return ToolBase::finalize();
}

// Method that handles various Gaudi "software events"
void PhotonCreatorBase::handle ( const Incident & incident )
{
  // Update prior to start of event. Used to re-initialise data containers
  if ( IncidentType::BeginEvent == incident.type() )
  {
    InitNewEvent();
  }
  // End of event stuff
  else if ( IncidentType::EndEvent == incident.type() )
  {
    FinishEvent();
  }
}

void PhotonCreatorBase::printStats() const
{
  if ( nEvents() > 0 )
  {

    // statistical tool
    const StatDivFunctor occ("%10.2f +-%7.2f");

    // Print out final stats
    info() << "=================================================================" << endmsg
           << "  Photon candidate summary : " << nEvents() << " events :-" << endmsg;
    if ( m_photCount[Rich::Aerogel]>0 )
      info() << "    Aerogel   : "
             << occ(m_photCount[Rich::Aerogel],nEvents())  << "  photons/event" << endmsg;
    if ( m_photCount[Rich::Rich1Gas]>0 )
      info() << "    Rich1Gas  : "
             << occ(m_photCount[Rich::Rich1Gas],nEvents()) << "  photons/event" << endmsg;
    if ( m_photCount[Rich::Rich2Gas]>0 )
      info() << "    Rich2Gas  : "
             << occ(m_photCount[Rich::Rich2Gas],nEvents()) << "  photons/event" << endmsg;
    info() << "=================================================================" << endmsg;

  }
}

StatusCode PhotonCreatorBase::reconstructPhotons() const
{

  // Check for event aborts. If already aborted stop here
  if ( !procStatus()->aborted() )
  {

    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Found " << richSegments()->size()
              << " RichRecSegments and " << richPixels()->size()
              << " RichRecPixels" << endmsg;
    }
    if ( !trackCreator()->richTracks()->empty() &&
         !pixelCreator()->richPixels()->empty() )
    {

      // make a rough guess at a size to reserve based on number of pixels
      if ( richPhotons()->empty() )
        richPhotons()->reserve( ( richSegments()->size() *
                                  richPixels()->size() ) / 40 );

      // abort flag for too many photons
      bool abortPhotonReco(false);

      // Need to loop twice over segments, first over gas and then over aerogel
      // in order to allow aerogel photons to be rejected based on the presence
      // of a good gas photon for the same pixel

      // Iterate over all gas segments
      for ( auto * seg : *richSegments() )
      {

        // is this a gas radiator segment ?
        if ( Rich::Aerogel != seg->trackSegment().radiator() )
        {

          // only make photons if reco has not been aborted
          if ( !abortPhotonReco ) { reconstructPhotons( seg ); }

          // check on size of photon container
          abortPhotonReco = ( richPhotons()->size() > m_maxPhotons );

          // flag this segment as having all its photons reconstructed
          seg->setAllPhotonsDone(true);

        } // gas radiator segment

      } // segment loop

      // Iterate over all aerogel segments
      for ( auto * seg : *richSegments() )
      {

        // is this a aerogel radiator segment ?
        if ( Rich::Aerogel == seg->trackSegment().radiator() )
        {

          // only make photons if reco has not been aborted
          if ( !abortPhotonReco ) { reconstructPhotons( seg ); }

          // check on size of photon container
          abortPhotonReco = ( richPhotons()->size() > m_maxPhotons );

          // flag this segment as having all its photons reconstructed
          seg->setAllPhotonsDone(true);

        } // aerogel radiator segment

      } // segment loop

      // Finally, set all tracks as having all their photons reconstructed
      for ( auto * tk : *richTracks() ) { tk->setAllPhotonsDone(true); }

      // If abort flag was set, clean up and issue warnings etc.
      if ( abortPhotonReco )
      {
        procStatus()->addAlgorithmStatus( name(), "RICH", "ReachedRichPhotonLimit",
                                          Rich::Rec::ReachedRichPhotonLimit, true );
        std::ostringstream mess;
        mess << "Number of photon candidates exceeds maximum of "
             << m_maxPhotons << " -> Processing aborted";
        Warning( mess.str(), StatusCode::SUCCESS, 0 ).ignore();
        // remove all references to the photons from tracks, segments and pixels
        deleteAllCrossReferences();
        // remove all photon candidates
        richPhotons()->clear();
      }

    } // have tracks and pixels

  } // ProcStat OK

  return StatusCode::SUCCESS;
}

LHCb::RichRecPhoton *
PhotonCreatorBase::checkForExistingPhoton( LHCb::RichRecSegment * segment,
                                           LHCb::RichRecPixel * pixel ) const
{
  // Form the key for this photon
  const Rich::Rec::PhotonKey photonKey( pixel->key(), segment->key() );

  // Try and find the photon
  return richPhotons()->object( photonKey );
}

LHCb::RichRecPhoton*
PhotonCreatorBase::reconstructPhoton( LHCb::RichRecSegment * segment,
                                      LHCb::RichRecPixel * pixel ) const
{
  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "Trying photon reco. with segment " << segment->key()
              << " and pixel " << pixel->key() << " " << pixel->hpdPixelCluster()
              << endmsg;
  }

  // flag this tool as having been called
  m_hasBeenCalled = true;

  // Check HPD occupancy for this pixel
  if ( pixel->photonDetOccupancy() >
       m_maxHPDOccForReco[segment->trackSegment().radiator()] )
  {
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "   -> FAILED HPD occupancy check -> reject" << endmsg;
    }
    return NULL;
  }

  // check photon is possible before proceeding
  if ( !m_photonPredictor->photonPossible(segment,pixel) )
  {
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "   -> FAILED predictor check -> reject" << endmsg;
    }
    return NULL;
  }
  else if (  msgLevel(MSG::VERBOSE) )
  {
    verbose() << "   -> PASSED predictor check" << endmsg;
  }

  // If aerogel, and configured to do so, reject if the pixel already has
  // a good Rich1Gas photon associated with it.
  if ( m_rejAeroPhotsIfGas &&
       Rich::Aerogel == segment->trackSegment().radiator() )
  {
    bool reject = false;
    const LHCb::RichRecPixel::Photons & assocPhots = pixel->richRecPhotons();
    for ( const auto * P : assocPhots )
    {
      if ( Rich::Rich1Gas == P->richRecSegment()->trackSegment().radiator() )
      {
        reject = true;
        break;
      }
    }
    if ( reject )
    {
      if ( msgLevel(MSG::VERBOSE) )
      {
        verbose() << "   -> FAILED Aerogel/Gas check -> reject" << endmsg;
      }
      return NULL;
    }
  }

  // Form the key for this photon
  const PhotonKey photonKey( pixel->key(), segment->key() );

//   info() << "Sizes     " << sizeof(pixel->key()) << " " << sizeof(segment->key())
//          << " " << sizeof(photonKey.key()) << endmsg;
//   info() << "Before    " << pixel->key() << " " << segment->key() << endmsg;
//   info() << "PhotonKey " << photonKey << endmsg;

  // See if this photon already exists
  LHCb::RichRecPhoton * phot = NULL;
  if ( UNLIKELY( bookKeep() && m_photonDone[ photonKey ] ) )
  {
    // return pre-made photon
    phot = static_cast<LHCb::RichRecPhoton*>( richPhotons()->object(photonKey) );
  }
  else
  {
    // return brand new photon
    phot = buildPhoton( segment, pixel, photonKey );
  }

//   if ( phot )
//   {
//     info() << "New Photon key " << sizeof(phot->key()) << " " << phot->key() << endmsg;
//   }

  return phot;
}

LHCb::RichRecPhotons * PhotonCreatorBase::richPhotons() const
{

  // is this the first call this event ?
  if ( !m_photons )
  {

    if ( !exist<LHCb::RichRecPhotons>(m_richRecPhotonLocation) )
    {

      // Reinitialise the Photon Container
      m_photons = new LHCb::RichRecPhotons();

      // Register new RichRecPhoton container to Gaudi data store
      put( m_photons, m_richRecPhotonLocation );

    }
    else
    {

      // get photons from TES
      m_photons = get<LHCb::RichRecPhotons>(m_richRecPhotonLocation);
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "Found " << m_photons->size()
                << " pre-existing RichRecPhotons in TES at "
                << m_richRecPhotonLocation << endmsg;
      }

      // Remake local photon reference map
      if ( bookKeep() )
      {
        for ( LHCb::RichRecPhotons::const_iterator iPhoton = m_photons->begin();
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

const LHCb::RichRecTrack::Photons &
PhotonCreatorBase::reconstructPhotons( LHCb::RichRecTrack * track ) const
{
  if ( !track->allPhotonsDone() && track->inUse() )
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << "Reconstructing all photons for track " << track->key() << endmsg;

    // Iterate over segments
    for ( LHCb::RichRecTrack::Segments::iterator segment =
            track->richRecSegments().begin();
          segment != track->richRecSegments().end(); ++segment )
    {
      reconstructPhotons( *segment );
    }

    track->setAllPhotonsDone(true);
  }

  return track->richRecPhotons();
}

const LHCb::RichRecSegment::Photons &
PhotonCreatorBase::reconstructPhotons( LHCb::RichRecSegment * segment ) const
{
  // Skip segments not on, or already have all their photons reconstructed
  if ( !segment->richRecTrack()->allPhotonsDone() &&
       !segment->allPhotonsDone()                 &&
       segment->richRecTrack()->inUse()            )
  {
    // Which Rich
    const Rich::DetectorType rich = segment->trackSegment().rich();

    // get appropriate pixel range
    const bool has1 = segment->hasPhotonsIn(Rich::top);
    const bool has2 = segment->hasPhotonsIn(Rich::bottom);
    const IPixelCreator::PixelRange range =
      ( has1 && has2 ? pixelCreator()->range(rich) :
        has1         ? pixelCreator()->range(rich,Rich::top) :
        pixelCreator()->range(rich,Rich::bottom) );

    // loop over pixels and reconstruct photons against this segment
    for ( auto * pixel : range ) { reconstructPhoton( segment, pixel ); }

    // flag this segment as having all its photons reconstructed
    segment->setAllPhotonsDone(true);
  }

  // return the list of photons for this segment
  return segment->richRecPhotons();
}

const LHCb::RichRecPixel::Photons &
PhotonCreatorBase::reconstructPhotons( LHCb::RichRecPixel * pixel ) const
{
  // Iterate over tracks
  for ( auto * track : *(trackCreator()->richTracks()) )
  {
    if ( !track->inUse() ) continue;
    reconstructPhotons( track, pixel );
  }

  return pixel->richRecPhotons();
}

// Note to self. Need to review what this method passes back
LHCb::RichRecTrack::Photons
PhotonCreatorBase::reconstructPhotons( LHCb::RichRecTrack * track,
                                       LHCb::RichRecPixel * pixel ) const
{
  LHCb::RichRecTrack::Photons photons;

  // Iterate over segments
  for ( auto * segment : track->richRecSegments() )
  {
    LHCb::RichRecPhoton * photon = reconstructPhoton( segment, pixel );
    if ( photon ) photons.push_back( photon );
  }

  return photons;
}

void PhotonCreatorBase::buildCrossReferences( LHCb::RichRecPhoton * photon ) const
{

  // Pointers to the segment and pixel
  LHCb::RichRecSegment * segment = photon->richRecSegment();
  LHCb::RichRecPixel * pixel     = photon->richRecPixel();

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
    LHCb::RichRecTrack::Pixels & tkPixs = segment->richRecTrack()->richRecPixels();
    LHCb::RichRecTrack::Pixels::iterator iPix = std::find( tkPixs.begin(), tkPixs.end(), pixel );
    if ( tkPixs.end() == iPix ) segment->richRecTrack()->addToRichRecPixels( pixel );
  }
  else // RICH2 - only one radiator type so no need to test
  {
    segment->richRecTrack()->addToRichRecPixels( pixel );
  }

}

void PhotonCreatorBase::deleteAllCrossReferences() const
{
  // loop over tracks
  for ( auto * tk : *richTracks() )
  {
    tk->setRichRecPixels  ( LHCb::RichRecTrack::Pixels()    );
    tk->setRichRecPhotons ( LHCb::RichRecTrack::Photons()   );
  }
  // loop over segments
  for ( auto * seg : *richSegments() )
  {
    seg->setRichRecPixels  ( LHCb::RichRecSegment::Pixels()  );
    seg->setRichRecPhotons ( LHCb::RichRecSegment::Photons() );
  }
  // loop over pixels
  for ( auto * pix : *richPixels() )
  {
    pix->richRecPhotons().clear();
    pix->richRecTracks().clear();
  }
}

bool
PhotonCreatorBase::checkAngleInRange( LHCb::RichRecSegment * segment,
                                      const double ckTheta ) const
{
  bool ok = false;

  // cache reference to track segment
  const auto & tkSeg = segment->trackSegment();

  // Just check overall absolute min - max range
  if ( ckTheta < absMaxCKTheta(tkSeg) && 
       ckTheta > absMinCKTheta(tkSeg) )
  {
    // Finer grained check, to be within tolerence of any mass hypothesis
    for ( const auto & hypo : m_pidTypes )
    {
      const double tmpT = m_ckAngle->avgCherenkovTheta( segment, hypo );
      ok = ( fabs(tmpT-ckTheta) < ckSearchRange(segment,tkSeg,hypo) );
      if ( ok ) break;
    }

  }
  if ( !ok && msgLevel(MSG::VERBOSE) )
  {
    verbose() << "    -> photon FAILED checkAngleInRange test" << endmsg;
  }
  return ok;
}

bool PhotonCreatorBase::checkPhotonProb( LHCb::RichRecPhoton * photon ) const
{
  // cache reference to track segment
  const auto & tkSeg = photon->richRecSegment()->trackSegment();

  // check photon has significant probability to be signal for any
  // hypothesis. If not then reject
  bool keepPhoton = false;
  for ( const auto & hypo : m_pidTypes )
  {
    const double predPixSig = m_photonSignal->predictedPixelSignal( photon, hypo );
    if ( predPixSig > m_minPhotonProb[tkSeg.radiator()] )
    {
      keepPhoton = true;
    }
    if ( keepPhoton ) break;
  }

  return keepPhoton;
}

void PhotonCreatorBase::InitNewEvent()
{
  m_hasBeenCalled = false;
  if ( bookKeep() ) m_photonDone.clear();
  m_photons = NULL;
  m_photCountLast = m_photCount;
}

void PhotonCreatorBase::FinishEvent()
{
  if ( m_hasBeenCalled ) ++m_Nevts;
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Created " << richPhotons()->size() << " RichRecPhotons : Aerogel="
            << m_photCount[Rich::Aerogel]-m_photCountLast[Rich::Aerogel]
            << " Rich1Gas=" << m_photCount[Rich::Rich1Gas]-m_photCountLast[Rich::Rich1Gas]
            << " Rich2Gas=" << m_photCount[Rich::Rich2Gas]-m_photCountLast[Rich::Rich2Gas]
            << endmsg;
    if ( ! richPhotons()->empty() )
      counter("PhotScaleFactor") += ( (double)( richSegments()->size() *
                                                richPixels()->size() ) /
                                      (double)(richPhotons()->size()) );
  }
}
