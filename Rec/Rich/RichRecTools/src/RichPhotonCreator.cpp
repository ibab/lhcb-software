
//-----------------------------------------------------------------------------
/** @file RichPhotonCreator.cpp
 *
 *  Implementation file for tool : RichPhotonCreator
 *
 *  CVS Log :-
 *  $Id: RichPhotonCreator.cpp,v 1.25 2005-05-13 15:20:38 jonrob Exp $
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
  : RichRecToolBase   ( type, name, parent ),
    m_photonPredictor ( 0 ),
    m_photonSignal    ( 0 ),
    m_photonReco      ( 0 ),
    m_photons         ( 0 ),
    m_bookKeep        ( false ),
    m_photCount       ( Rich::NRadiatorTypes, 0 ),
    m_photCountLast   ( Rich::NRadiatorTypes, 0 ),
    m_Nevts           ( 0                       ),
    m_hasBeenCalled   ( false                   )
{

  declareInterface<IRichPhotonCreator>(this);

  // job options

  declareProperty( "DoBookKeeping", m_bookKeep );

  declareProperty( "RichRecPhotonLocation",
                   m_richRecPhotonLocation = RichRecPhotonLocation::Default );

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

StatusCode RichPhotonCreator::initialize() 
{
  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichDetPhotonReco",     m_photonReco      );
  acquireTool( "RichPhotonSignal",      m_photonSignal    );
  acquireTool( "RichPhotonPredictor",   m_photonPredictor );

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );
  incSvc()->addListener( this, IncidentType::EndEvent   );

  return sc;
}

StatusCode RichPhotonCreator::finalize()
{

  // statistical tool
  RichStatDivFunctor occ("%10.2f +-%8.2f");

  // Print out final stats
  info() << "------------------------------------------------------" << endreq
         << "  Photon Candidate Summary : " << m_Nevts << " events :-" << endreq
         << "    Aerogel   " << occ(m_photCount[Rich::Aerogel],m_Nevts) << "  photons/event" << endreq
         << "    C4F10     " << occ(m_photCount[Rich::C4F10],m_Nevts)   << "  photons/event" << endreq
         << "    CF4       " << occ(m_photCount[Rich::CF4],m_Nevts)     << "  photons/event" << endreq
         << "------------------------------------------------------" << endreq;

  // Execute base class method
  return RichRecToolBase::finalize();
}

// Method that handles various Gaudi "software events"
void RichPhotonCreator::handle ( const Incident& incident )
{
  // Update prior to start of event. Used to re-initialise data containers
  if ( IncidentType::BeginEvent == incident.type() ) 
  { 
    InitEvent(); 
  }
  // end of event
  else if ( IncidentType::EndEvent == incident.type() )
  {
    FinishEvent();
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Created " << richPhotons()->size() << " RichRecPhotons : Aerogel=" 
              << m_photCount[Rich::Aerogel]-m_photCountLast[Rich::Aerogel]
              << " C4F10=" << m_photCount[Rich::C4F10]-m_photCountLast[Rich::C4F10]
              << " CF4=" << m_photCount[Rich::CF4]-m_photCountLast[Rich::CF4] << endreq;
    }
  }
}

RichRecPhoton*
RichPhotonCreator::reconstructPhoton( RichRecSegment * segment,
                                      RichRecPixel * pixel ) const 
{

  // check photon is possible before proceeding
  if ( !m_photonPredictor->photonPossible(segment, pixel) ) return NULL;

  // Form the key for this photon
  const RichRecPhotonKey photonKey(pixel->key(),segment->key());

  // See if this photon already exists
  if ( m_bookKeep && m_photonDone[ photonKey ] ) {
    return static_cast<RichRecPhoton*>(richPhotons()->object(photonKey));
  } else {
    return buildPhoton( segment, pixel, photonKey );
  }

}

RichRecPhoton * RichPhotonCreator::buildPhoton( RichRecSegment * segment,
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

    const Rich::RadiatorType rad = segment->trackSegment().radiator();
    if ( ( geomPhoton.CherenkovTheta() > 0. ||
           geomPhoton.CherenkovPhi() > 0. ) &&
         geomPhoton.CherenkovTheta() < m_maxCKtheta[rad] &&
         geomPhoton.CherenkovTheta() > m_minCKtheta[rad] ) {

      // give photon same smart ID as pixel
      geomPhoton.setSmartID( pixel->smartID() );

      // make new RichRecPhoton
      newPhoton = new RichRecPhoton( geomPhoton, segment,
                                     segment->richRecTrack(), pixel );

      // check photon has significant probability to be signal for any
      // hypothesis. If not then reject and delete
      bool keepPhoton = false;
      for ( int iHypo = 0; iHypo < Rich::NParticleTypes; ++iHypo ) {
        if ( m_photonSignal->predictedPixelSignal(newPhoton,
                                                  static_cast<Rich::ParticleIDType>(iHypo) )
             > m_minPhotonProb[rad] ) { keepPhoton = true; break; }
      }

      if ( keepPhoton ) 
      {
        
        if ( msgLevel(MSG::VERBOSE) ) 
        {
          verbose() << "Reconstructed a photon candidate for segment " << segment->key()
                    << " and pixel " << pixel->key() << endreq;
        }
        
        richPhotons()->insert( newPhoton, key );

        // count
        ++m_photCount[rad];

        // Build cross-references between objects
        segment->addToRichRecPixels( pixel );
        segment->addToRichRecPhotons( newPhoton );
        segment->richRecTrack()->addToRichRecPhotons( newPhoton );
        pixel->addToRichRecPhotons( newPhoton );
        pixel->addToRichRecTracks( segment->richRecTrack() );
        RichRecTrack::Pixels & tkPixs = segment->richRecTrack()->richRecPixels();
        bool notThere = true;
        for ( RichRecTrack::Pixels::iterator pix = tkPixs.begin();
              pix != tkPixs.end(); ++pix ) 
        {
          if ( (RichRecPixel*)(*pix) == pixel ) { notThere = false; break; }
        }
        if ( notThere ) 
        {
          segment->richRecTrack()->addToRichRecPixels( pixel );
        }
        
      } else 
      {
        delete newPhoton;
        newPhoton = NULL;
      }

    }

  }

  // Add to reference map
  if ( m_bookKeep ) m_photonDone[ key ] = true;

  // Return pointer to this photon
  return newPhoton;

}

void RichPhotonCreator::reconstructPhotons() const
{

  // flag this tool as having been called
  m_hasBeenCalled = true;

  const bool noPhots = richPhotons()->empty();

  // make a rough guess at a size to reserve based on number of pixels
  if ( noPhots ) richPhotons()->reserve( 5 * pixelCreator()->richPixels()->size() );

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
          //for ( RichRecPixels::const_iterator iPixel = pixelCreator()->richPixels()->begin();
          //      iPixel != pixelCreator()->richPixels()->end();
          //      ++iPixel ) 
          const Rich::DetectorType rich = segment->trackSegment().rich();
          for ( RichRecPixels::const_iterator iPixel = pixelCreator()->begin(rich);
                iPixel != pixelCreator()->end(rich); ++iPixel ) 
          {
            RichRecPixel * pixel = *iPixel;

            // If container was empty, skip checks for whether photon already exists
            if ( noPhots ) 
            {
              if ( m_photonPredictor->photonPossible( segment, pixel ) ) 
              {
                buildPhoton( segment, pixel,
                             RichRecPhotonKey(pixel->key(),segment->key()) );
              }
            } 
            else 
            {
              reconstructPhoton( segment, pixel );
            }

          } // pixel loop

          segment->setAllPhotonsDone(true);
        }

      } // segment loop

      track->setAllPhotonsDone(true);
    }

  } // track loop

}

const RichRecTrack::Photons &
RichPhotonCreator::reconstructPhotons( RichRecTrack * track ) const
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
RichPhotonCreator::reconstructPhotons( RichRecSegment * segment ) const
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
RichPhotonCreator::reconstructPhotons( RichRecPixel * pixel ) const
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
RichPhotonCreator::reconstructPhotons( RichRecTrack * track,
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

RichRecPhotons * RichPhotonCreator::richPhotons() const
{
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
        debug() << "Found " << m_photons->size() << " pre-existing RichRecPhotons in TES at "
                << m_richRecPhotonLocation << endreq;
      }

      // Remake local photon reference map
      if ( m_bookKeep ) 
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
