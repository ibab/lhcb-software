// $Id: RichMCTruthTool.cpp,v 1.1 2004-06-02 21:37:22 jonesc Exp $

// local
#include "RichMCTruthTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichMCTruthTool
//
// 15/03/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichMCTruthTool>          s_factory ;
const        IToolFactory& RichMCTruthToolFactory = s_factory ;

// Standard constructor
RichMCTruthTool::RichMCTruthTool( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : RichToolBase( type, name, parent ),
    m_mcRichDigitsDone ( false ),
    m_mcRichDigits     ( 0     ),
    m_mcTrackLinks     ( 0     ),
    m_mcPhotonLinks    ( 0     )
{

  declareInterface<IRichMCTruthTool>(this);

  declareProperty( "MCRichDigitsLocation",
                   m_mcRichDigitsLocation = MCRichDigitLocation::Default );

  declareProperty( "TrackAsctName", m_trAsctName = "TrackToMCP" );
  declareProperty( "TrackAsctType",
                   m_trAsctType = "AssociatorWeighted<TrStoredTrack,MCParticle,double>");

}


StatusCode RichMCTruthTool::initialize()
{
  // Sets up various tools and services
  StatusCode sc = RichToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // Get pointer to Track MC truth
  m_trackToMCP = tool<TrackAsct>( m_trAsctType, m_trAsctName );

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
  if ( IncidentType::BeginEvent == incident.type() ) 
    {
      // New event
      m_mcRichDigitsDone   = false;
      
      // Get new linkers for this event
      cleanUpLinkers();
    }
}

RichMCTruthTool::MCRichHitToPhoton * RichMCTruthTool::mcPhotonLinks() const
{
  if ( !m_mcPhotonLinks ) {
    m_mcPhotonLinks =
      new MCRichHitToPhoton( evtSvc(), msgSvc(),
                             MCRichOpticalPhotonLocation::LinksFromMCRichHits );
  }
  return m_mcPhotonLinks;
}

RichMCTruthTool::MCPartToRichTracks * RichMCTruthTool::mcTrackLinks() const
{
  if ( !m_mcTrackLinks ) {
    m_mcTrackLinks =
      new MCPartToRichTracks( evtSvc(), msgSvc(),
                              MCRichTrackLocation::LinksFromMCParticles );
  }
  return m_mcTrackLinks;
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
      debug() << "Failed to locate MCRichDigits at "
              << m_mcRichDigitsLocation << endreq;
    }

  }

  return m_mcRichDigits;
}

const MCParticle *
RichMCTruthTool::mcParticle( const TrStoredTrack * track ) const
{
  if ( track ) {
    const MCParticle* mcPart = m_trackToMCP->associatedFrom(track);
    return mcPart;
  } else { return NULL; }
}

const SmartRefVector<MCRichHit> &
RichMCTruthTool::mcRichHits( const MCRichDigit * mcDigit ) const
{
  return ( mcDigit ? mcDigit->hits() : m_emptyContainer );
}

const MCRichDigit * RichMCTruthTool::mcRichDigit( const RichDigit * digit ) const
{

  // Try fast method
  MCRichDigit * mcDigit = MCTruth<MCRichDigit>(digit);

  // If failed, try accessing MCRichDigit container directly
  if ( !mcDigit && mcRichDigits() ) mcDigit = m_mcRichDigits->object(digit->key());

  // if still failed, issue a warning
  if ( !mcDigit ) Warning("Failed to find MCRichDigit for RichDigit");

  return mcDigit;
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
RichMCTruthTool::mcRichTrack( const MCParticle * mcPart ) const
{
  return ( mcPart ? mcTrackLinks()->first( mcPart ) : 0 );
}

const MCRichOpticalPhoton * 
RichMCTruthTool::mcOpticalPhoton( const MCRichHit * mcHit ) const
{
  return ( mcPhotonLinks() ? mcPhotonLinks()->first(mcHit) : 0 );
}
