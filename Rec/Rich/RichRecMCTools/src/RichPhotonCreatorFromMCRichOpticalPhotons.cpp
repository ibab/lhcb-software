
/** @file RichPhotonCreatorFromMCRichOpticalPhotons.cpp
 *
 *  Implementation file for RICH reconstruction tool : RichPhotonCreatorFromMCRichOpticalPhotons
 *
 *  CVS Log :-
 *  $Id: RichPhotonCreatorFromMCRichOpticalPhotons.cpp,v 1.4 2004-07-27 16:14:11 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */

// local
#include "RichPhotonCreatorFromMCRichOpticalPhotons.h"

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichPhotonCreatorFromMCRichOpticalPhotons>          s_factory ;
const        IToolFactory& RichPhotonCreatorFromMCRichOpticalPhotonsFactory = s_factory ;

// Standard constructor
RichPhotonCreatorFromMCRichOpticalPhotons::
RichPhotonCreatorFromMCRichOpticalPhotons( const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent )
  : RichRecToolBase( type, name, parent ),
    m_photonSignal    ( 0 ),
    m_mcRecTool       ( 0 ),
    m_photons         ( 0 )
{

  declareInterface<IRichPhotonCreator>(this);

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

StatusCode RichPhotonCreatorFromMCRichOpticalPhotons::initialize() 
{
  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichPhotonSignal",      m_photonSignal    );
  acquireTool( "RichRecMCTruthTool",    m_mcRecTool       );

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );
  if (msgLevel(MSG::DEBUG)) incSvc()->addListener( this, IncidentType::EndEvent );

  // Make sure we are ready for a new event
  InitNewEvent();

  return StatusCode::SUCCESS;
}

StatusCode RichPhotonCreatorFromMCRichOpticalPhotons::finalize()
{
  // Execute base class method
  return RichRecToolBase::finalize();
}

// Method that handles various Gaudi "software events"
void RichPhotonCreatorFromMCRichOpticalPhotons::handle ( const Incident& incident )
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
RichPhotonCreatorFromMCRichOpticalPhotons::reconstructPhoton( RichRecSegment * segment,
                                                              RichRecPixel * pixel ) const
{
  // Form the key for this photon
  const RichRecPhotonKey photonKey(pixel->key(),segment->key());

  // See if this photon already exists
  if ( m_photonDone[static_cast<long>(photonKey)] ) {
    return static_cast<RichRecPhoton*>(richPhotons()->object(photonKey));
  } else {
    return buildPhoton( segment, pixel, photonKey );
  }

}

RichRecPhoton *
RichPhotonCreatorFromMCRichOpticalPhotons::buildPhoton( RichRecSegment * segment,
                                                        RichRecPixel * pixel,
                                                        const RichRecPhotonKey key ) const
{

  RichRecPhoton * newPhoton = NULL;

  // See if there is a true cherenkov photon for this segment/pixel pair
  const MCRichOpticalPhoton * mcPhoton = m_mcRecTool->trueOpticalPhoton(segment,pixel);
  if ( mcPhoton ) {

    const Rich::RadiatorType rad = segment->trackSegment().radiator();
    if ( ( mcPhoton->cherenkovTheta() > 0. ||
           mcPhoton->cherenkovPhi() > 0. ) &&
         mcPhoton->cherenkovTheta() < m_maxCKtheta[rad] &&
         mcPhoton->cherenkovTheta() > m_minCKtheta[rad] ) {

      // construct a photon from the MC information
      RichGeomPhoton geomPhoton( mcPhoton->cherenkovTheta(),
                                 mcPhoton->cherenkovPhi(),
                                 mcPhoton->emissionPoint(),
                                 mcPhoton->pdIncidencePoint(),
                                 mcPhoton->sphericalMirrorReflectPoint(),
                                 mcPhoton->flatMirrorReflectPoint(),
                                 pixel->smartID(),
                                 1 );

      // make new RichRecPhoton
      newPhoton = new RichRecPhoton( geomPhoton,
                                     segment,
                                     segment->richRecTrack(),
                                     pixel );

      // check photon has significant probability to be signal for any
      // hypothesis. If not then reject and delete
      bool keepPhoton = false;
      for ( int iHypo = 0; iHypo < Rich::NParticleTypes; ++iHypo ) {
        if ( m_photonSignal->predictedPixelSignal(newPhoton,
                                                  static_cast<Rich::ParticleIDType>(iHypo) )
             > m_minPhotonProb[rad] ) { keepPhoton = true; break; }
      }

      if ( keepPhoton ) {

        if ( msgLevel(MSG::VERBOSE) ) {
          verbose() << "Reconstructed a photon candidate for segment " << segment->key()
                    << " and pixel " << pixel->key() << endreq;
        }

        richPhotons()->insert( newPhoton, key );

        // Build cross-references between objects
        segment->addToRichRecPixels( pixel );
        segment->addToRichRecPhotons( newPhoton );
        segment->richRecTrack()->addToRichRecPhotons( newPhoton );
        pixel->addToRichRecPhotons( newPhoton );
        pixel->addToRichRecTracks( segment->richRecTrack() );
        RichRecTrack::Pixels & tkPixs = segment->richRecTrack()->richRecPixels();
        bool notThere = true;
        for ( RichRecTrack::Pixels::iterator pix = tkPixs.begin();
              pix != tkPixs.end(); ++pix ) {
          if ( (RichRecPixel*)(*pix) == pixel ) { notThere = false; break; }
        }
        if ( notThere ) {
          segment->richRecTrack()->addToRichRecPixels( pixel );
        }

      } else {
        delete newPhoton;
        newPhoton = NULL;
      }

    }

  }

  // Add to reference map
  m_photonDone[static_cast<long>(key)] = true;

  // Return pointer to this photon
  return newPhoton;

}

void RichPhotonCreatorFromMCRichOpticalPhotons::reconstructPhotons() const
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
RichPhotonCreatorFromMCRichOpticalPhotons::reconstructPhotons( RichRecTrack * track ) const
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
RichPhotonCreatorFromMCRichOpticalPhotons::reconstructPhotons( RichRecSegment * segment ) const
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
RichPhotonCreatorFromMCRichOpticalPhotons::reconstructPhotons( RichRecPixel * pixel ) const
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
RichPhotonCreatorFromMCRichOpticalPhotons::reconstructPhotons( RichRecTrack * track,
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

RichRecPhotons * RichPhotonCreatorFromMCRichOpticalPhotons::richPhotons() const
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
        m_photonDone[static_cast<long>((*iPhoton)->key())] = true;
      }

    }
  }
  return m_photons;
}
