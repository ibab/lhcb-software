
//-----------------------------------------------------------------------------
/** @file RichMCTruthTool.cpp
 *
 * Implementation file for class : RichMCTruthTool
 *
 * CVS Log :-
 * $Id: RichMCTruthTool.cpp,v 1.13 2005-02-20 18:45:13 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 14/01/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichMCTruthTool.h"

// Declaration of the Tool Factory
static const  ToolFactory<RichMCTruthTool>          s_factory ;
const        IToolFactory& RichMCTruthToolFactory = s_factory ;

// Standard constructor
RichMCTruthTool::RichMCTruthTool( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : RichToolBase       ( type, name, parent ),
    m_mcRichDigitsDone ( false ),
    m_mcRichHitsDone   ( false ),
    m_mcRichDigits     ( 0     ),
    m_mcRichHits       ( 0     ),
    m_mcTrackLinks     ( 0     ),
    m_mcPhotonLinks    ( 0     ),
    m_trgTrToMCPLinks  ( 0     ),
    m_trackToMCP       ( 0     )
{

  declareInterface<IRichMCTruthTool>(this);

  declareProperty( "MCRichDigitsLocation",
                   m_mcRichDigitsLocation = MCRichDigitLocation::Default );
  declareProperty( "MCRichHitsLocation",
                   m_mcRichHitsLocation = MCRichHitLocation::Default );
  declareProperty( "TrackAsctName", m_trAsctName = "TrackToMCP" );
  declareProperty( "TrackAsctType",
                   m_trAsctType = "AssociatorWeighted<TrStoredTrack,MCParticle,double>");

}


StatusCode RichMCTruthTool::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // Retrieve particle property service
  IParticlePropertySvc * ppSvc = svc<IParticlePropertySvc>( "ParticlePropertySvc", true );

  // Setup the PDG code mappings
  m_localID[ 0 ]                                   = Rich::Unknown;
  m_localID[ abs(ppSvc->find("e+")->jetsetID()) ]  = Rich::Electron;
  m_localID[ abs(ppSvc->find("mu+")->jetsetID()) ] = Rich::Muon;
  m_localID[ abs(ppSvc->find("pi+")->jetsetID()) ] = Rich::Pion;
  m_localID[ abs(ppSvc->find("K+")->jetsetID()) ]  = Rich::Kaon;
  m_localID[ abs(ppSvc->find("p+")->jetsetID()) ]  = Rich::Proton;

  // release service since it is no longer needed
  release(ppSvc);

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );

  // Make sure we are ready for a new event
  InitNewEvent();

  return sc;
}

StatusCode RichMCTruthTool::finalize()
{
  // clean up linkers
  cleanUpLinkers();

  // Execute base class method
  return RichToolBase::finalize();
}

// Method that handles various Gaudi "software events"
void RichMCTruthTool::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) { InitNewEvent(); }
}

const MCRichDigits * RichMCTruthTool::mcRichDigits() const
{
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
      Warning( "Failed to locate MCRichDigits at "+m_mcRichDigitsLocation );
    }

  }

  return m_mcRichDigits;
}

const MCRichHits * RichMCTruthTool::mcRichHits() const
{
  if ( !m_mcRichHitsDone ) {
    m_mcRichHitsDone = true;

    SmartDataPtr<MCRichHits> tdsMCHits( evtSvc(),
                                            m_mcRichHitsLocation );
    if ( tdsMCHits ) {
      m_mcRichHits = tdsMCHits;
      debug() << "Successfully located " << m_mcRichHits->size()
              << " MCRichHits at " << m_mcRichHitsLocation << endreq;
    } else {
      m_mcRichHits = NULL;
      Warning( "Failed to locate MCRichHits at "+m_mcRichHitsLocation );
    }

  }

  return m_mcRichHits;
}

const MCParticle *
RichMCTruthTool::mcParticle( const TrStoredTrack * track ) const
{
  if ( track ) {
    return trackAsct()->associatedFrom(track);
  } else {
    Warning ( "::mcParticle : NULL TrStoredTrack pointer" );
    return NULL;
  }
}

const MCParticle *
RichMCTruthTool::mcParticle( const TrgTrack * track ) const
{
  if ( track ) {
    return trgTrackToMCPLinks()->first(track);
  } else {
    Warning ( "::mcParticle : NULL TrgTrack pointer" );
    return NULL;
  }
}

const MCRichDigit * RichMCTruthTool::mcRichDigit( const RichDigit * digit ) const
{
  if ( !digit ) return NULL;

  // Try fast method
  MCRichDigit * mcDigit = MCTruth<MCRichDigit>(digit);

  // If failed, try accessing MCRichDigit container directly via key
  if ( !mcDigit && mcRichDigits() ) mcDigit = mcRichDigits()->object(digit->key());

  // if still failed, issue a warning
  if ( !mcDigit ) Warning("Failed to find MCRichDigit for RichDigit");

  return mcDigit;
}

const MCRichDigit * RichMCTruthTool::mcRichDigit( const RichSmartID id ) const
{
  return ( mcRichDigits() ? mcRichDigits()->object(id) : 0 );
}

Rich::ParticleIDType
RichMCTruthTool::mcParticleType( const TrStoredTrack * track ) const
{
  return mcParticleType( mcParticle(track) );
}

Rich::ParticleIDType
RichMCTruthTool::mcParticleType( const TrgTrack * track ) const
{
  return mcParticleType( mcParticle(track) );
}

Rich::ParticleIDType
RichMCTruthTool::mcParticleType( const MCParticle * mcPart ) const
{
  return (mcPart ? m_localID[abs(mcPart->particleID().pid())] : Rich::Unknown);
}

const MCRichTrack *
RichMCTruthTool::mcRichTrack( const TrStoredTrack * track ) const
{
  return mcRichTrack( mcParticle(track) );
}

const MCRichTrack *
RichMCTruthTool::mcRichTrack( const TrgTrack * track ) const
{
  return mcRichTrack( mcParticle(track) );
}

const MCRichTrack *
RichMCTruthTool::mcRichTrack( const MCParticle * mcPart ) const
{
  return ( mcPart ? mcTrackLinks()->first( mcPart ) : 0 );
}

const MCRichOpticalPhoton *
RichMCTruthTool::mcOpticalPhoton( const MCRichHit * mcHit ) const
{
  return ( mcPhotonLinks() ? mcPhotonLinks()->first(mcHit) : 0 );
}

bool RichMCTruthTool::isBackground( const MCRichDigit * digit ) const
{
  // Check digit is OK
  if ( !digit ) return true;
  for ( SmartRefVector<MCRichHit>::const_iterator iHit = digit->hits().begin();
        iHit != digit->hits().end(); ++iHit ) {
    if ( *iHit && !isBackground(*iHit) ) return false;
  }
  return true;
}

bool RichMCTruthTool::isBackground( const MCRichHit * hit ) const
{
  return ( hit && ( hit->scatteredPhoton() ||
                    hit->chargedTrack() ||
                    hit->backgroundHit() ) );
}

RichMCTruthTool::MCRichHitToPhoton * RichMCTruthTool::mcPhotonLinks() const
{
  if ( !m_mcPhotonLinks ) {
    m_mcPhotonLinks =
      new MCRichHitToPhoton( evtSvc(), msgSvc(),
                             MCRichOpticalPhotonLocation::LinksFromMCRichHits );
    if ( m_mcPhotonLinks->notFound() ) {
      Warning( "Linker for MCRichHits to MCRichOpticalPhotons not found for '" +
               MCRichOpticalPhotonLocation::LinksFromMCRichHits + "'" );
    }
  }
  return m_mcPhotonLinks;
}

RichMCTruthTool::MCPartToRichTracks * RichMCTruthTool::mcTrackLinks() const
{
  if ( !m_mcTrackLinks ) {
    m_mcTrackLinks =
      new MCPartToRichTracks( evtSvc(), msgSvc(),
                              MCRichTrackLocation::LinksFromMCParticles );
    if ( m_mcTrackLinks->notFound() ) {
      Warning( "Linker for MCParticles to MCRichTracks not found for '" +
               MCRichTrackLocation::LinksFromMCParticles + "'" );
    }
  }
  return m_mcTrackLinks;
}

RichMCTruthTool::TrgTrackToMCP * RichMCTruthTool::trgTrackToMCPLinks() const
{
  if ( !m_trgTrToMCPLinks ) {
    m_trgTrToMCPLinks =
      new TrgTrackToMCP( evtSvc(), msgSvc(), TrgTrackLocation::Long );
    if ( m_trgTrToMCPLinks->notFound() ) {
      Warning( "Linker for TrgTracks to MCParticles not found for " +
               TrgTrackLocation::Long + "'" );
    }
  }
  return m_trgTrToMCPLinks;
}

bool RichMCTruthTool::isSpillover ( const MCRichHit * hit ) const
{
  // Compare parent container to known "main event" container
  return ( hit && mcRichHits() && (hit->parent() != mcRichHits()) );
}

bool RichMCTruthTool::isSpillover ( const MCRichDigit * digit ) const
{
  // Check digit
  if ( !digit ) return false;

  // iterate over hits and see if any are from signal events
  for ( SmartRefVector<MCRichHit>::const_iterator iHit = digit->hits().begin();
        iHit != digit->hits().end(); ++iHit ) 
  {
    if ( !isSpillover(*iHit) ) return false;
  }

  // all are from spillover
  return true;
}
