// $Id: RichRecMCTruthTool.cpp,v 1.2 2004-02-02 14:24:41 jonesc Exp $

// local
#include "RichRecMCTruthTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRecMCTruthTool
//
// 15/03/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichRecMCTruthTool>          s_factory ;
const        IToolFactory& RichRecMCTruthToolFactory = s_factory ;

// Standard constructor
RichRecMCTruthTool::RichRecMCTruthTool( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : RichRecToolBase( type, name, parent ) {

  declareInterface<IRichRecMCTruthTool>(this);

  declareProperty( "MCRichDigitsLocation",
                   m_mcRichDigitsLocation = MCRichDigitLocation::Default );
  declareProperty( "MCRichOpticalPhotonsLocation",
                   m_mcRichPhotonsLocation = MCRichOpticalPhotonLocation::Default );
  declareProperty( "MCRichTracksLocation",
                   m_mcRichTracksLocation = MCRichTrackLocation::Default );
  declareProperty( "MCRichSegmentsLocation",
                   m_mcRichSegmentsLocation = MCRichSegmentLocation::Default );
  declareProperty( "TrackAsctName", m_trAsctName = "TrackToMCP" );
  declareProperty( "TrackAsctType",
                   m_trAsctType = "AssociatorWeighted<TrStoredTrack,MCParticle,double>");

}

StatusCode RichRecMCTruthTool::initialize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Initialize" << endreq;

  // Sets up various tools and services
  if ( !RichRecToolBase::initialize() ) return StatusCode::FAILURE;

  // Get pointer to Track MC truth
  if ( !toolSvc()->retrieveTool(m_trAsctType,m_trAsctName,m_trackToMCP) ) {
    msg << MSG::ERROR << m_trAsctName+" not found" << endreq;
    return StatusCode::FAILURE;
  }

  // Retrieve particle property service
  IParticlePropertySvc* ppSvc = 0;
  if ( !service( "ParticlePropertySvc", ppSvc ) ) {
    msg << MSG::WARNING << "Unable to retrieve ParticlePropertySvc" << endreq;
    return StatusCode::FAILURE;
  } else {
    // Setup the PDG code mappings
    m_localID[ 0 ] = Rich::Unknown;
    m_localID[ abs(ppSvc->find("e+")->jetsetID()) ]  = Rich::Electron;
    m_localID[ abs(ppSvc->find("mu+")->jetsetID()) ] = Rich::Muon;
    m_localID[ abs(ppSvc->find("pi+")->jetsetID()) ] = Rich::Pion;
    m_localID[ abs(ppSvc->find("K+")->jetsetID()) ]  = Rich::Kaon;
    m_localID[ abs(ppSvc->find("p+")->jetsetID()) ]  = Rich::Proton;
    ppSvc->release();
  }

  // Setup incident services
  IIncidentSvc * incSvc;
  if ( !serviceLocator()->service( "IncidentSvc", incSvc, true ) ) {
    msg << MSG::ERROR << "IncidentSvc not found" << endreq;
    return StatusCode::FAILURE;
  } else {
    incSvc->addListener( this, "BeginEvent" ); // Informed of a new event
    incSvc->release();
  }

  return StatusCode::SUCCESS;
}

StatusCode RichRecMCTruthTool::finalize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Finalize" << endreq;

  // release tools and services
  releaseTool( m_trackToMCP );

  // Execute base class method
  return RichRecToolBase::finalize();
}

// Method that handles various Gaudi "software events"
void RichRecMCTruthTool::handle ( const Incident& incident ) {

  if ( "BeginEvent" == incident.type() ) {

    // New event
    m_mcRichDigitsDone   = false;
    m_mcRichOptPhotsDone = false;
    m_mcRichSegmentsDone = false;
    m_mcRichTracksDone   = false;

  }

}

const MCRichDigits * RichRecMCTruthTool::mcRichDigits() {

  if ( !m_mcRichDigitsDone ) {
    m_mcRichDigitsDone = true;

    SmartDataPtr<MCRichDigits> tdsMCDigits( eventSvc(),
                                            m_mcRichDigitsLocation );
    if ( tdsMCDigits ) {
      m_mcRichDigits = tdsMCDigits;
      if ( msgLevel(MSG::DEBUG) ) {
        MsgStream msg( msgSvc(), name() );
        msg << MSG::DEBUG << "Successfully located " << m_mcRichDigits->size()
            << " MCRichDigits at " << m_mcRichDigitsLocation << endreq;
      }
    } else {
      m_mcRichDigits = NULL;
      if ( msgLevel(MSG::DEBUG) ) {
        MsgStream msg( msgSvc(), name() );
        msg << MSG::DEBUG << "Failed to locate MCRichDigits at "
            << m_mcRichDigitsLocation << endreq;
      }
    }

  }

  return m_mcRichDigits;
}

const MCRichOpticalPhotons * RichRecMCTruthTool::mcRichOpticalPhotons() {

  if ( !m_mcRichOptPhotsDone ) {
    m_mcRichOptPhotsDone = true;

    SmartDataPtr<MCRichOpticalPhotons> tdsMCPhots( eventSvc(),
                                                   m_mcRichPhotonsLocation );
    if ( tdsMCPhots ) {
      m_mcRichOpticalPhotons = tdsMCPhots;
      if ( msgLevel(MSG::DEBUG) ) {
        MsgStream msg( msgSvc(), name() );
        msg << MSG::DEBUG
            << "Successfully located " << m_mcRichOpticalPhotons->size()
            << " MCRichOpticalPhotons at " << m_mcRichPhotonsLocation << endreq;
      }
    } else {
      m_mcRichOpticalPhotons = NULL;
      if ( msgLevel(MSG::DEBUG) ) {
        MsgStream msg( msgSvc(), name() );
        msg << MSG::DEBUG << "Failed to locate MCRichOpticalPhotons at "
            << m_mcRichPhotonsLocation << endreq;
      }
    }

  }

  return m_mcRichOpticalPhotons;
}

const MCRichTracks * RichRecMCTruthTool::mcRichTracks() {

  if ( !m_mcRichTracksDone ) {
    m_mcRichTracksDone = true;

    SmartDataPtr<MCRichTracks> tdsMCTracks( eventSvc(),
                                            m_mcRichTracksLocation );
    if ( tdsMCTracks ) {
      m_mcRichTracks = tdsMCTracks;
      if ( msgLevel(MSG::DEBUG) ) {
        MsgStream msg( msgSvc(), name() );
        msg << MSG::DEBUG << "Successfully located " << m_mcRichTracks->size()
            << " MCRichTracks at " << m_mcRichTracksLocation << endreq;
      }
    } else {
      m_mcRichTracks = NULL;
      if ( msgLevel(MSG::DEBUG) ) {
        MsgStream msg( msgSvc(), name() );
        msg << MSG::DEBUG << "Failed to locate MCRichTracks at "
            << m_mcRichTracksLocation << endreq;
      }
    }

  }

  return m_mcRichTracks;
}

const MCRichSegments * RichRecMCTruthTool::mcRichSegments() {

  if ( !m_mcRichSegmentsDone ) {
    m_mcRichSegmentsDone = true;

    SmartDataPtr<MCRichSegments> tdsMCSegments( eventSvc(),
                                                m_mcRichSegmentsLocation );
    if ( tdsMCSegments ) {
      m_mcRichSegments = tdsMCSegments;
      if ( msgLevel(MSG::DEBUG) ) {
        MsgStream msg( msgSvc(), name() );
        msg << MSG::DEBUG << "Successfully located " << m_mcRichSegments->size()
            << " MCRichSegments at " << m_mcRichSegmentsLocation << endreq;
      }
    } else {
      m_mcRichSegments = NULL;
      if ( msgLevel(MSG::DEBUG) ) {
        MsgStream msg( msgSvc(), name() );
        msg << MSG::DEBUG << "Failed to locate MCRichSegments at "
            << m_mcRichSegmentsLocation << endreq;
      }
    }

  }

  return m_mcRichSegments;
}

const MCParticle *
RichRecMCTruthTool::mcParticle( const RichRecTrack * richTrack ) const {

  const ContainedObject * obj = richTrack->parentTrack();
  if ( !obj ) return NULL;

  // Try TrStoredTrack
  const TrStoredTrack * track = dynamic_cast<const TrStoredTrack*>(obj);
  if ( track ) return m_trackToMCP->associatedFrom( track );

  // else Try MCParticle
  const MCParticle * mcPart = dynamic_cast<const MCParticle*>(obj);
  if ( mcPart ) return mcPart;

  return NULL;
}

const SmartRefVector<MCRichHit> &
RichRecMCTruthTool::mcRichHits( const RichRecPixel * richPixel )
{
  return mcRichHits( mcRichDigit(richPixel) );
}

const SmartRefVector<MCRichHit> &
RichRecMCTruthTool::mcRichHits( const MCRichDigit * mcDigit )
{
  return ( mcDigit ? mcDigit->hits() : m_emptyContainer );
}

const MCRichDigit * RichRecMCTruthTool::mcRichDigit( const RichDigit * digit ) {

  // Try fast method
  MCRichDigit * mcDigit = MCTruth<MCRichDigit>(digit);

  // If failed, try accessing MCRichDigit container directly
  if ( !mcDigit &&
       mcRichDigits() ) mcDigit = m_mcRichDigits->object( digit->key() );

  if ( !mcDigit ) {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::WARNING
        << "Failed to find MCRichDigit for RichDigit : "
        << digit->key() << endreq;
  }

  return mcDigit;
}

const MCRichDigit *
RichRecMCTruthTool::mcRichDigit( const RichRecPixel * richPixel )
{
  const RichDigit * digit =
    dynamic_cast<const RichDigit*>( richPixel->parentPixel() );
  if ( !digit ) {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::WARNING << "RichRecPixel has no associated RichDigit" << endreq;
    return NULL;
  }

  return mcRichDigit( digit );
}

bool RichRecMCTruthTool::mcParticle( const RichRecPixel * richPixel,
                                     std::vector<const MCParticle*> & mcParts )
{

  // Clean vector
  mcParts.clear();

  // Loop over all MCRichHits associated to the pixel
  const SmartRefVector<MCRichHit> & hits = mcRichHits( richPixel );
  if ( hits.empty() ) return false;
  for ( SmartRefVector<MCRichHit>::const_iterator iHit = hits.begin();
        iHit != hits.end(); ++iHit ) {
    if ( !(*iHit) ) continue; // protect against bad hits
    const MCParticle * mcPart = (*iHit)->mcParticle();
    if ( !mcPart ) continue;
    std::vector<const MCParticle*>::const_iterator iFind =
      std::find( mcParts.begin(), mcParts.end(), mcPart );
    if ( mcParts.end() == iFind ) mcParts.push_back( mcPart );
  }

  return true;
}

const MCParticle *
RichRecMCTruthTool::trueRecPhoton( const RichRecPhoton * photon )
{
  return trueRecPhoton( photon->richRecSegment(), photon->richRecPixel() );
}

const MCParticle * RichRecMCTruthTool::trueRecPhoton( const RichRecSegment * segment,
                                                      const RichRecPixel * pixel )
{
  const RichRecTrack * track = segment->richRecTrack();
  const MCParticle * mcTrack = ( track ? mcParticle(track) : NULL );
  if ( !mcTrack ) return NULL;

  // Loop over all MCRichHits associated to the pixel
  const SmartRefVector<MCRichHit> & hits = mcRichHits( pixel );
  for ( SmartRefVector<MCRichHit>::const_iterator iHit = hits.begin();
        iHit != hits.end(); ++iHit ) {
    if ( (*iHit) && ( mcTrack == (*iHit)->mcParticle() ) ) return mcTrack;
  }

  return NULL;
}

const MCParticle *
RichRecMCTruthTool::trueCherenkovPhoton( const RichRecPhoton * photon )
{
  return trueCherenkovPhoton( photon->richRecSegment(), photon->richRecPixel() );
}

const MCParticle *
RichRecMCTruthTool::trueCherenkovPhoton( const RichRecSegment * segment,
                                         const RichRecPixel * pixel )
{
  const MCParticle * mcPart = trueRecPhoton( segment, pixel );
  return ( !mcPart ? NULL :
           trueCherenkovRadiation( pixel, segment->trackSegment().radiator() ) );
}

const MCParticle *
RichRecMCTruthTool::trueCherenkovRadiation( const RichRecPixel * pixel,
                                            Rich::RadiatorType rad )
{

  // Loop over all MCRichHits for this pixel
  const SmartRefVector<MCRichHit> & hits = mcRichHits( pixel );
  for ( SmartRefVector<MCRichHit>::const_iterator iHit = hits.begin();
        iHit != hits.end(); ++iHit ) {
    if ( !(*iHit) ) continue; // protect against bad hits
    // Experiment with different criteria.....
    //if ( (unsigned int)rad == (*iHit)->radiator() ) return (*iHit)->mcParticle();
    if ( rad == (*iHit)->radiator() &&
         !(*iHit)->scatteredPhoton() &&
         !(*iHit)->backgroundHit() ) return (*iHit)->mcParticle();
    //if ( rad == (*iHit)->radiator() &&
    //     !(*iHit)->scatteredPhoton() &&
    //     !(*iHit)->chargedTrack() &&
    //     !(*iHit)->backgroundHit() ) return (*iHit)->mcParticle();
  }

  return NULL;
}

Rich::ParticleIDType
RichRecMCTruthTool::mcParticleType( const MCParticle * mcPart )
{
  return (mcPart ? m_localID[abs(mcPart->particleID().pid())] : Rich::Unknown);
}

Rich::ParticleIDType
RichRecMCTruthTool::mcParticleType( const RichRecTrack * richTrack )
{
  return mcParticleType( mcParticle(richTrack) );
}

Rich::ParticleIDType
RichRecMCTruthTool::mcParticleType( const RichRecSegment * richSegment )
{
  const MCParticle * mcPart = this->mcParticle( richSegment->richRecTrack() );
  return (mcPart ? m_localID[abs(mcPart->particleID().pid())] : Rich::Unknown);
}

const MCParticle *
RichRecMCTruthTool::mcParticle( const RichRecSegment * richSegment ) const
{
  return mcParticle( richSegment->richRecTrack() );
}

bool
RichRecMCTruthTool::mcRichOpticalPhoton( const RichRecPixel * richPixel,
                                         SmartRefVector<MCRichOpticalPhoton> & phots ) {

  phots.clear();
  if ( mcRichOpticalPhotons() ) {
    const SmartRefVector<MCRichHit> & hits = mcRichHits(richPixel);
    for ( SmartRefVector<MCRichHit>::const_iterator iHit = hits.begin();
          iHit != hits.end(); ++iHit ) {
      if ( !(*iHit) ) continue; // protect against bad hits
      MCRichOpticalPhoton * phot = m_mcRichOpticalPhotons->object( (*iHit)->key() );
      if ( phot ) phots.push_back( phot );
    }
    return true;
  }

  return false;
}

const MCRichSegment *
RichRecMCTruthTool::mcRichSegment( const RichRecSegment * segment )
{
  const MCRichTrack * mcTrack = mcRichTrack( segment );
  return ( mcTrack && mcRichSegments() ?
           mcTrack->segmentInRad(segment->trackSegment().radiator()) : 0 );
}

const SmartRefVector<MCRichSegment> *
RichRecMCTruthTool::mcRichSegments( const RichRecTrack * track )
{
  const MCRichTrack * mcTrack = mcRichTrack( track );
  return ( mcTrack ? &(mcTrack->mcSegments()) : 0 );
}

const MCRichTrack *
RichRecMCTruthTool::mcRichTrack( const RichRecSegment * segment )
{
  return ( segment ? mcRichTrack( segment->richRecTrack() ) : 0 );
}

const MCRichTrack *
RichRecMCTruthTool::mcRichTrack( const RichRecTrack * track )
{
  const MCParticle * mcPart = mcParticle(track);
  return ( mcPart && mcRichTracks() ?
           m_mcRichTracks->object(mcPart->key()) : 0 );
}

bool RichRecMCTruthTool::isBackground( const RichRecPixel * pixel )
{
  const SmartRefVector<MCRichHit> & hits = mcRichHits(pixel);
  for ( SmartRefVector<MCRichHit>::const_iterator iHit = hits.begin();
        iHit != hits.end(); ++iHit ) {
    if ( !(*iHit)->backgroundHit() ) return false;
  }

  return true;
}

const MCRichHit *
RichRecMCTruthTool::trueCherenkovHit( const RichRecPhoton * photon )
{

  // Track MCParticle
  const MCParticle * trackMCP = mcParticle( photon->richRecSegment() );
  if ( trackMCP ) {

    // Loop over all MCRichHits for the pixel associated to this photon
    const SmartRefVector<MCRichHit> & hits = mcRichHits( photon->richRecPixel() );
    for ( SmartRefVector<MCRichHit>::const_iterator iHit = hits.begin();
          iHit != hits.end(); ++iHit ) {
      const MCParticle * pixelMCP = (*iHit)->mcParticle();
      if ( pixelMCP == trackMCP ) return *iHit;
    }

  }

  return NULL;
}

const MCRichOpticalPhoton *
RichRecMCTruthTool::trueOpticalPhoton( const RichRecPhoton * photon )
{

  if ( mcRichOpticalPhotons() ) {
    // get true MCRichHit
    const MCRichHit * mchit = trueCherenkovHit( photon );
    if ( !mchit ) return NULL;
    // return associated MCRichOpticalPhoton
    return m_mcRichOpticalPhotons->object( mchit->key() );
  }

  return NULL;
}
