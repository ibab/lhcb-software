// $Id: RichRecMCTruthTool.cpp,v 1.4 2004-04-19 23:01:24 jonesc Exp $

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
  : RichRecToolBase( type, name, parent ),
    m_mcRichDigitsDone ( false ),
    m_mcRichDigits     ( 0     ),
    m_mcTrackLinks     ( 0     ),
    m_mcPhotonLinks    ( 0     )
{

  declareInterface<IRichRecMCTruthTool>(this);

  declareProperty( "MCRichDigitsLocation",
                   m_mcRichDigitsLocation = MCRichDigitLocation::Default );

  declareProperty( "TrackAsctName", m_trAsctName = "TrackToMCP" );
  declareProperty( "TrackAsctType",
                   m_trAsctType = "AssociatorWeighted<TrStoredTrack,MCParticle,double>");

}


StatusCode RichRecMCTruthTool::initialize()
{

  // Sets up various tools and services
  if ( !RichRecToolBase::initialize() ) return StatusCode::FAILURE;

  // Get pointer to Track MC truth
  m_trackToMCP = tool<TrackAsct>( m_trAsctType,m_trAsctName );

  // Retrieve particle property service
  IParticlePropertySvc * ppSvc = svc<IParticlePropertySvc>( "ParticlePropertySvc" );

  // Setup the PDG code mappings
  m_localID[ 0 ]                                   = Rich::Unknown;
  m_localID[ abs(ppSvc->find("e+")->jetsetID()) ]  = Rich::Electron;
  m_localID[ abs(ppSvc->find("mu+")->jetsetID()) ] = Rich::Muon;
  m_localID[ abs(ppSvc->find("pi+")->jetsetID()) ] = Rich::Pion;
  m_localID[ abs(ppSvc->find("K+")->jetsetID()) ]  = Rich::Kaon;
  m_localID[ abs(ppSvc->find("p+")->jetsetID()) ]  = Rich::Proton;
  release(ppSvc);

  // Setup incident services
  IIncidentSvc * incSvc = svc<IIncidentSvc>( "IncidentSvc", true );
  incSvc->addListener( this, IncidentType::BeginEvent );
  release(incSvc);

  return StatusCode::SUCCESS;
}

StatusCode RichRecMCTruthTool::finalize() 
{
  // clean up linkers
  cleanUpLinkers();

  // Execute base class method
  return RichRecToolBase::finalize();
}

// Method that handles various Gaudi "software events"
void RichRecMCTruthTool::handle ( const Incident& incident ) {

  if ( IncidentType::BeginEvent == incident.type() ) {

    // New event
    m_mcRichDigitsDone   = false;

    // Get new linkers for this event
    cleanUpLinkers();

  }

}

RichRecMCTruthTool::MCRichHitToPhoton * RichRecMCTruthTool::mcPhotonLinks() const
{
  if ( !m_mcPhotonLinks ) {
    m_mcPhotonLinks =
      new MCRichHitToPhoton( evtSvc(), msgSvc(),
                             MCRichOpticalPhotonLocation::LinksFromMCRichHits );
    //if ( m_mcPhotonLinks->notFound() ) {
    //  delete m_mcPhotonLinks; 
    //  m_mcPhotonLinks = 0;
    //}
  }
  return m_mcPhotonLinks;
}

RichRecMCTruthTool::MCPartToRichTracks * RichRecMCTruthTool::mcTrackLinks() const
{
  if ( !m_mcTrackLinks ) {
    m_mcTrackLinks =
      new MCPartToRichTracks( evtSvc(), msgSvc(),
                              MCRichTrackLocation::LinksFromMCParticles );
    //if ( m_mcTrackLinks->notFound() ) {
    //  delete m_mcTrackLinks;
    //  m_mcTrackLinks = 0;
    //}
  }
  return m_mcTrackLinks;
}

const MCRichDigits * RichRecMCTruthTool::mcRichDigits() {

  if ( !m_mcRichDigitsDone ) {
    m_mcRichDigitsDone = true;

    SmartDataPtr<MCRichDigits> tdsMCDigits( evtSvc(),
                                            m_mcRichDigitsLocation );
    if ( tdsMCDigits ) {
      m_mcRichDigits = tdsMCDigits;
      debug() << "Successfully located " << m_mcRichDigits->size()
              << " MCRichDigits at " << m_mcRichDigitsLocation << endreq;
    } else {
      m_mcRichDigits = NULL;
      debug() << "Failed to locate MCRichDigits at "
              << m_mcRichDigitsLocation << endreq;
    }

  }

  return m_mcRichDigits;
}

const MCParticle *
RichRecMCTruthTool::mcParticle( const RichRecTrack * richTrack ) const {

  const ContainedObject * obj = richTrack->parentTrack();
  if ( !obj ) return NULL;

  // Try TrStoredTrack
  const TrStoredTrack * track = dynamic_cast<const TrStoredTrack*>(obj);
  if ( track ) return mcParticle( track );

  // else Try MCParticle
  const MCParticle * mcPart = dynamic_cast<const MCParticle*>(obj);
  if ( mcPart ) return mcPart;

  return NULL;
}

const MCParticle *
RichRecMCTruthTool::mcParticle( const TrStoredTrack * track ) const
{
  if ( track ) {
    const MCParticle* mcPart = m_trackToMCP->associatedFrom(track);
    return mcPart;
  } else { return NULL; }
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
  if ( !mcDigit && mcRichDigits() ) mcDigit = m_mcRichDigits->object(digit->key());

  if ( !mcDigit ) Warning("Failed to find MCRichDigit for RichDigit");

  return mcDigit;
}

const MCRichDigit *
RichRecMCTruthTool::mcRichDigit( const RichRecPixel * richPixel )
{
  const RichDigit * digit =
    dynamic_cast<const RichDigit*>( richPixel->parentPixel() );
  if ( !digit ) {
    Warning("RichRecPixel has no associated RichDigit");
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
  return ( !photon ? NULL :
           trueRecPhoton( photon->richRecSegment(), photon->richRecPixel() ) );
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
  return ( !photon ? NULL :
           trueCherenkovPhoton( photon->richRecSegment(), photon->richRecPixel() ) );
}

const MCParticle *
RichRecMCTruthTool::trueCherenkovPhoton( const RichRecSegment * segment,
                                         const RichRecPixel * pixel )
{
  if ( !segment || !pixel ) return NULL;
  const MCParticle * mcPart = trueRecPhoton( segment, pixel );
  return ( !mcPart ? NULL :
           trueCherenkovRadiation( pixel, segment->trackSegment().radiator() ) );
}

const MCParticle *
RichRecMCTruthTool::trueCherenkovRadiation( const RichRecPixel * pixel,
                                            const Rich::RadiatorType rad )
{

  // Loop over all MCRichHits for this pixel
  const SmartRefVector<MCRichHit> & hits = mcRichHits( pixel );
  for ( SmartRefVector<MCRichHit>::const_iterator iHit = hits.begin();
        iHit != hits.end(); ++iHit ) {
    if ( !(*iHit) ) continue; // protect against bad hits
    // Experiment with different criteria.....
    //if ( rad == (*iHit)->radiator() ) return (*iHit)->mcParticle();
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
  return ( richSegment ? mcParticleType( richSegment->richRecTrack() ) : Rich::Unknown );
}

const MCParticle *
RichRecMCTruthTool::mcParticle( const RichRecSegment * richSegment ) const
{
  return ( richSegment ? mcParticle( richSegment->richRecTrack() ) : NULL );
}

bool
RichRecMCTruthTool::mcRichOpticalPhoton( const RichRecPixel * richPixel,
                                         SmartRefVector<MCRichOpticalPhoton> & phots ) {

  phots.clear();
  if ( mcPhotonLinks() ) {
    const SmartRefVector<MCRichHit> & hits = mcRichHits(richPixel);
    for ( SmartRefVector<MCRichHit>::const_iterator iHit = hits.begin();
          iHit != hits.end(); ++iHit ) {
      if ( !(*iHit) ) continue; // protect against bad hits
      MCRichOpticalPhoton * phot = mcPhotonLinks()->first( *iHit );
      if ( phot ) phots.push_back( phot );
    }
    return true;
  }

  return false;
}

const MCRichSegment *
RichRecMCTruthTool::mcRichSegment( const RichRecSegment * segment )
{
  if ( !segment ) return NULL;
  const MCRichTrack * mcTrack = mcRichTrack( segment );
  return ( mcTrack ?
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
  return mcRichTrack( mcParticle(track) );
}

const MCRichTrack *
RichRecMCTruthTool::mcRichTrack( const TrStoredTrack * track )
{
  return mcRichTrack( mcParticle(track) );
}

const MCRichTrack *
RichRecMCTruthTool::mcRichTrack( const MCParticle * mcPart )
{
  return ( mcPart ? mcTrackLinks()->first( mcPart ) : 0 );
}

bool RichRecMCTruthTool::isBackground( const RichRecPixel * pixel )
{
  const SmartRefVector<MCRichHit> & hits = mcRichHits(pixel);
  for ( SmartRefVector<MCRichHit>::const_iterator iHit = hits.begin();
        iHit != hits.end(); ++iHit ) {
    if ( *iHit && !(*iHit)->backgroundHit() ) return false;
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
      if ( !(*iHit) ) continue;
      const MCParticle * pixelMCP = (*iHit)->mcParticle();
      if ( pixelMCP == trackMCP ) return *iHit;
    }

  }

  return NULL;
}

const MCRichOpticalPhoton *
RichRecMCTruthTool::trueOpticalPhoton( const RichRecPhoton * photon )
{

  if ( mcPhotonLinks() ) {
    // get true MCRichHit
    const MCRichHit * mchit = trueCherenkovHit( photon );
    if ( !mchit ) return NULL;
    // return associated MCRichOpticalPhoton
    return mcPhotonLinks()->first( mchit );
  }

  return NULL;
}
