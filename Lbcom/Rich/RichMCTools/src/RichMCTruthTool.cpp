
//-----------------------------------------------------------------------------
/** @file RichMCTruthTool.cpp
 *
 * Implementation file for class : RichMCTruthTool
 *
 * CVS Log :-
 * $Id: RichMCTruthTool.cpp,v 1.22 2005-10-18 12:42:26 jonrob Exp $
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
  : RichToolBase           ( type, name, parent ),
    m_mcRichDigitsDone     ( false ),
    m_mcRichDigitSumsDone  ( false ),
    m_summaryMapDone       ( false ),
    m_mcRichHitsDone       ( false ),
    m_mcRichDigits         ( 0     ),
    m_mcRichHits           ( 0     ),
    m_mcTrackLinks         ( 0     ),
    m_mcPhotonLinks        ( 0     ),
    m_trgTrToMCPLinks      ( 0     ),
    m_trStoredTrToMCPLinks ( 0     ),
    m_trToMCPLinks         ( 0     ),
    m_trackToMCP           ( 0     )
{

  // interface
  declareInterface<IRichMCTruthTool>(this);

  // job options
  declareProperty( "MCRichDigitsLocation",
                   m_mcRichDigitsLocation = MCRichDigitLocation::Default );
  declareProperty( "MCRichDigitSummariesLocation",
                   m_mcRichDigitSumsLocation = MCRichDigitSummaryLocation::Default );
  declareProperty( "MCRichHitsLocation",
                   m_mcRichHitsLocation = MCRichHitLocation::Default );
  declareProperty( "TrackAsctName", m_trAsctName = "TrackToMCP" );
  declareProperty( "TrackAsctType",
                   m_trAsctType = "AssociatorWeighted<TrStoredTrack,MCParticle,double>");
  declareProperty( "FollowMCChain", m_followMC = false );

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

bool RichMCTruthTool::mcParticles( const RichSmartID id,
                                   std::vector<const MCParticle*> & mcParts ) const
{
  // Clean vector
  mcParts.clear();

  // First try via direct MCParticles references in MCRichDigitSummarys
  RichSummaryMap::const_iterator iEn = summaryMap().find( id );
  if ( iEn != summaryMap().end() )
  {
    for ( MCRichDigitSummaries::const_iterator iSum = (*iEn).second.begin();
          iSum != (*iEn).second.end(); ++iSum )
    {
      MCParticle * mcP = (*iSum)->mcParticle();
      // protect against null references
      if ( !mcP ) continue;
      // Add to vector, once per MCParticle
      std::vector<const MCParticle*>::const_iterator iFind =
        std::find( mcParts.begin(), mcParts.end(), mcP );
      if ( mcParts.end() == iFind ) mcParts.push_back( mcP );
    }
  }

  // If MCParticles found, return
  if ( !mcParts.empty() ) return true;

  // if configured to try following the MC tree...
  if ( m_followMC )
  {

    // get MCRichDigit
    const MCRichDigit * mcDig = mcRichDigit( id );
    if ( mcDig )
    {

      // Loop over all MCRichHits associated
      const SmartRefVector<MCRichHit> & hits = mcDig->hits();
      for ( SmartRefVector<MCRichHit>::const_iterator iHit = hits.begin();
            iHit != hits.end(); ++iHit )
      {
        // protect against bad hits
        if ( !(*iHit) ) continue;

        // find MCParticle
        const MCParticle * mcPart = (*iHit)->mcParticle();
        if ( !mcPart ) continue;

        // Add to vector, once per MCParticle
        std::vector<const MCParticle*>::const_iterator iFind =
          std::find( mcParts.begin(), mcParts.end(), mcPart );
        if ( mcParts.end() == iFind ) mcParts.push_back( mcPart );
      } // end loop over hits

    } // end mcDig was found

  } // follow MC if

  return !mcParts.empty();
}

const MCParticle *
RichMCTruthTool::mcParticle( const Track * track ) const
{
  // Try with linkers
  if ( trackToMCPLinks() && !trackToMCPLinks()->notFound() )
  {
    return trackToMCPLinks()->first(track->key());
  }
  // If get here MC association failed
  Warning( "No MC association available for Tracks" );
  return NULL;
}

const MCParticle *
RichMCTruthTool::mcParticle( const TrStoredTrack * track ) const
{
  if ( !track )
  {
    Warning ( "::mcParticle : NULL TrStoredTrack pointer" );
    return NULL;
  }

  // First, try with linkers
  if ( trStoredTrackToMCPLinks() && !trStoredTrackToMCPLinks()->notFound() )
  {
    return trStoredTrackToMCPLinks()->first(track->key());
  }

  // Otherwise try using relations
  if ( trackAsct() )
  {
    return trackAsct()->associatedFrom(track);
  }

  // If get here, both failed
  Warning( "No MC association available for TrStoredTracks" );
  return NULL;
}

const MCParticle *
RichMCTruthTool::mcParticle( const TrgTrack * track ) const
{
  if ( track ) {
    return trgTrackToMCPLinks()->first(track->key());
  } else {
    Warning ( "::mcParticle : NULL TrgTrack pointer" );
    return NULL;
  }
}

const MCRichDigit * RichMCTruthTool::mcRichDigit( const RichDigit * digit ) const
{
  if ( !digit ) return NULL;

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Locating MCRichDigit for RichDigit " << (int)digit->key() << endreq;
  }

  // Try fast method
  const MCRichDigit * mcDigit = MCTruth<MCRichDigit>(digit);

  // If failed, try accessing MCRichDigit container directly via key
  if ( !mcDigit && mcRichDigits() ) mcDigit = mcRichDigit( digit->key() );

  // if still failed, issue a warning
  if ( !mcDigit ) Warning( "Failed to find MCRichDigit for RichDigit" );

  return mcDigit;
}

const MCRichDigit * RichMCTruthTool::mcRichDigit( const RichSmartID id ) const
{
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Locating MCRichDigit for RichSmartID " << id << endreq;
  }
  const MCRichDigit * mcDigit = ( mcRichDigits() ? mcRichDigits()->object(id) : 0 );
  if ( !mcDigit ) Warning( "Failed to locate MCRichDigit from RichSmartID" );
  return mcDigit;
}

Rich::ParticleIDType
RichMCTruthTool::mcParticleType( const Track * track ) const
{
  return mcParticleType( mcParticle(track) );
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
  return ( digit && ( digit->scatteredHit() ||
                      digit->chargedTrack() ||
                      digit->backgroundHit() ) );
}

bool RichMCTruthTool::isBackground ( const RichSmartID id ) const
{

  // first, try via summary objects
  RichSummaryMap::const_iterator iEn = summaryMap().find( id );
  if ( iEn != summaryMap().end() )
  {
    // loop over summaries
    for ( MCRichDigitSummaries::const_iterator iSum = (*iEn).second.begin();
          iSum != (*iEn).second.end(); ++iSum )
    {
      const MCRichDigitHistoryCode & code = (*iSum)->historyCode();
      if ( code.scatteredHit() ||
           code.chargedTrack() ||
           code.backgroundHit() ) return true;
    }
    // if get here, not background
    return false;
  }

  // if configured to try following the MC tree...
  if ( m_followMC )
  {
    const MCRichDigit * mcDig = mcRichDigit( id );
    return ( mcDig ? isBackground(mcDig) : true );
  }

  // if all else fails, assume background
  return true;
}

bool RichMCTruthTool::isBackground( const MCRichHit * hit ) const
{
  return ( hit && ( hit->scatteredPhoton() ||
                    hit->chargedTrack() ||
                    hit->backgroundHit() ) );
}

bool
RichMCTruthTool::isCherenkovRadiation( const RichSmartID id,
                                       const Rich::RadiatorType rad ) const
{

  // first, try via summary objects
  RichSummaryMap::const_iterator iEn = summaryMap().find( id );
  if ( iEn != summaryMap().end() )
  {
    // loop over summaries
    for ( MCRichDigitSummaries::const_iterator iSum = (*iEn).second.begin();
          iSum != (*iEn).second.end(); ++iSum )
    {
      const MCRichDigitHistoryCode & code = (*iSum)->historyCode();
      if      ( Rich::Aerogel == rad && code.aerogelHit() ) { return true; }
      else if ( Rich::C4F10   == rad && code.c4f10Hit()   ) { return true; }
      else if ( Rich::CF4     == rad && code.cf4Hit()     ) { return true; }
    }
    // if get here, must be background
    return false;
  }

  // try via MCRichDigit
  const MCRichDigit * mcDig = mcRichDigit( id );
  if ( mcDig )
  {
    if ( Rich::Aerogel == rad ) return mcDig->aerogelHit();
    if ( Rich::C4F10   == rad ) return mcDig->c4f10Hit();
    if ( Rich::CF4     == rad ) return mcDig->cf4Hit();
  }

  // finally, assume not
  return false;
}

bool RichMCTruthTool::getMcHistories( const RichSmartID id,
                                      std::vector<MCRichDigitSummary*> & histories ) const
{
  // clear histories
  histories.clear();
  if ( mcRichDigitSummaries() )
  {
    // try to find summaries
    RichSummaryMap::const_iterator iEn = summaryMap().find( id );
    if ( iEn != summaryMap().end() )
    {
      // set histories
      histories = (*iEn).second;
    }
  }
  // return if any were found
  return !histories.empty();
}

const MCRichDigits * RichMCTruthTool::mcRichDigits() const
{
  if ( !m_mcRichDigitsDone )
  {
    m_mcRichDigitsDone = true;

    // try and load MCRichDigits
    if ( exist<MCRichDigits>(m_mcRichDigitsLocation) )
    {
      m_mcRichDigits = get<MCRichDigits>(m_mcRichDigitsLocation);
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "Successfully located " << m_mcRichDigits->size()
                << " MCRichDigits at " << m_mcRichDigitsLocation << endreq;
      }
    }
    else
    {
      m_mcRichDigits = NULL;
      Warning( "Failed to locate MCRichDigits at "+m_mcRichDigitsLocation );
    }

  }

  return m_mcRichDigits;
}

const MCRichDigitSummaryVector *
RichMCTruthTool::mcRichDigitSummaries() const
{
  if ( !m_mcRichDigitSumsDone )
  {
    m_mcRichDigitSumsDone = true;

    // try and load MCRichDigitSummaryVector
    if ( exist<MCRichDigitSummaryVector>(m_mcRichDigitSumsLocation) )
    {
      m_mcRichDigitSums = get<MCRichDigitSummaryVector>(m_mcRichDigitSumsLocation);
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "Successfully located " << m_mcRichDigitSums->size()
                << " MCRichDigitSummaries at " << m_mcRichDigitSumsLocation << endreq;
      }
    }
    else
    {
      m_mcRichDigitSums = NULL;
      Warning( "Failed to locate MCRichDigitSummaries at "+m_mcRichDigitSumsLocation );
    }

  }
  return m_mcRichDigitSums;
}

const RichMCTruthTool::RichSummaryMap & RichMCTruthTool::summaryMap() const
{
  if ( !m_summaryMapDone )
  {
    m_summaryMapDone = true;

    // clear current map
    m_summaryMap.clear();

    // loop over summaries
    if ( mcRichDigitSummaries() )
    {
      for ( MCRichDigitSummaryVector::const_iterator iSum = mcRichDigitSummaries()->begin();
            iSum != mcRichDigitSummaries()->end(); ++iSum )
      {
        m_summaryMap[(*iSum)->richSmartID()].push_back( *iSum );
      }
    }

    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Built RichSummaryMap for " << m_summaryMap.size()
              << " RichSmartIDs" << endreq;
    }

  }
  return m_summaryMap;
}

const MCRichHits * RichMCTruthTool::mcRichHits() const
{
  if ( !m_mcRichHitsDone )
  {
    m_mcRichHitsDone = true;

    // Try and load MC Rich Hits
    if ( exist<MCRichHits>(m_mcRichHitsLocation) )
    {
      m_mcRichHits = get<MCRichHits>(m_mcRichHitsLocation);
      debug() << "Successfully located " << m_mcRichHits->size()
              << " MCRichHits at " << m_mcRichHitsLocation << endreq;
    }
    else
    {
      m_mcRichHits = NULL;
      Warning( "Failed to locate MCRichHits at "+m_mcRichHitsLocation );
    }

  }

  return m_mcRichHits;
}

RichMCTruthTool::MCRichHitToPhoton * RichMCTruthTool::mcPhotonLinks() const
{
  if ( !m_mcPhotonLinks )
  {
    m_mcPhotonLinks =
      new MCRichHitToPhoton( evtSvc(), msgSvc(),
                             MCRichOpticalPhotonLocation::LinksFromMCRichHits );
    if ( m_mcPhotonLinks->notFound() )
    {
      Warning( "Linker for MCRichHits to MCRichOpticalPhotons not found for '" +
               MCRichOpticalPhotonLocation::LinksFromMCRichHits + "'" );
    }
  }
  return m_mcPhotonLinks;
}

RichMCTruthTool::MCPartToRichTracks * RichMCTruthTool::mcTrackLinks() const
{
  if ( !m_mcTrackLinks )
  {
    m_mcTrackLinks =
      new MCPartToRichTracks( evtSvc(), msgSvc(),
                              MCRichTrackLocation::LinksFromMCParticles );
    if ( m_mcTrackLinks->notFound() )
    {
      Warning( "Linker for MCParticles to MCRichTracks not found for '" +
               MCRichTrackLocation::LinksFromMCParticles + "'" );
    }
  }
  return m_mcTrackLinks;
}

RichMCTruthTool::TrgTrackToMCP * RichMCTruthTool::trgTrackToMCPLinks() const
{
  if ( !m_trgTrToMCPLinks )
  {
    m_trgTrToMCPLinks =
      new TrgTrackToMCP( evtSvc(), msgSvc(), TrgTrackLocation::Long );
    if ( m_trgTrToMCPLinks->notFound() )
    {
      Warning( "Linker for TrgTracks to MCParticles not found for '" +
               TrgTrackLocation::Long + "'" );
    }
  }
  return m_trgTrToMCPLinks;
}

RichMCTruthTool::TrStoredTrackToMCP *
RichMCTruthTool::trStoredTrackToMCPLinks() const
{
  if ( !m_trStoredTrToMCPLinks )
  {
    m_trStoredTrToMCPLinks =
      new TrStoredTrackToMCP( evtSvc(), msgSvc(), TrStoredTrackLocation::Default );
    if ( m_trStoredTrToMCPLinks->notFound() )
    {
      Warning( "Linker for TrStoredTracks to MCParticles not found for '" +
               TrStoredTrackLocation::Default + "'" );
    }
  }
  return m_trStoredTrToMCPLinks;
}

RichMCTruthTool::TrackToMCP *
RichMCTruthTool::trackToMCPLinks() const
{
  if ( !m_trToMCPLinks )
  {
    m_trToMCPLinks =
      new TrackToMCP( evtSvc(), msgSvc(), TrackLocation::Default );
    if ( m_trToMCPLinks->notFound() )
    {
      Warning( "Linker for Tracks to MCParticles not found for '" +
               TrackLocation::Default + "'" );
    }
  }
  return m_trToMCPLinks;
}
