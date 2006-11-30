
//-----------------------------------------------------------------------------
/** @file RichMCTruthTool.cpp
 *
 * Implementation file for class : RichMCTruthTool
 *
 * CVS Log :-
 * $Id: RichMCTruthTool.cpp,v 1.31 2006-11-30 14:39:59 jonrob Exp $
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
    m_mcRichDigSumMapDone  ( false ),
    m_mcRichHitsDone       ( false ),
    m_mcRichDigits         ( 0     ),
    m_mcRichHits           ( 0     ),
    m_mcTrackLinks         ( 0     ),
    m_mcPhotonLinks        ( 0     )
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

}

RichMCTruthTool::~RichMCTruthTool() { cleanUpLinkers(); }

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

  // initialise
  m_emptyContainer.clear();

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
  MCRichDigitSummaryMap::const_iterator iEn = mcRichDigSumMap().find( id );
  if ( iEn != mcRichDigSumMap().end() )
  {
    for ( MCRichDigitSummaries::const_iterator iSum = (*iEn).second.begin();
          iSum != (*iEn).second.end(); ++iSum )
    {
      const MCParticle * mcP = (*iSum)->mcParticle();
      // protect against null references
      if ( !mcP ) continue;
      // Add to vector, once per MCParticle
      std::vector<const MCParticle*>::const_iterator iFind =
        std::find( mcParts.begin(), mcParts.end(), mcP );
      if ( mcParts.end() == iFind ) mcParts.push_back( mcP );
    }
  }

  // return
  return !mcParts.empty();
}

const MCRichDigit * RichMCTruthTool::mcRichDigit( const RichSmartID id ) const
{
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Locating MCRichDigit for RichSmartID " << id << endreq;
  }
  const MCRichDigit * mcDigit = ( mcRichDigits() ? mcRichDigits()->object(id) : 0 );
  if ( !mcDigit && msgLevel(MSG::DEBUG) )
  {
    debug() << "Failed to locate MCRichDigit from RichSmartID " << id << endreq;
  }
  return mcDigit;
}

Rich::ParticleIDType
RichMCTruthTool::mcParticleType( const MCParticle * mcPart ) const
{
  return (mcPart ? m_localID[abs(mcPart->particleID().pid())] : Rich::Unknown);
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

bool RichMCTruthTool::isBackground ( const RichSmartID id ) const
{

  // first, try via summary objects
  MCRichDigitSummaryMap::const_iterator iEn = mcRichDigSumMap().find( id );
  if ( iEn != mcRichDigSumMap().end() )
  {
    // loop over summaries
    for ( MCRichDigitSummaries::const_iterator iSum = (*iEn).second.begin();
          iSum != (*iEn).second.end(); ++iSum )
    {
      if ( (*iSum)->history().isBackground() ) return true;
    }
    // if get here, not background
    return false;
  }

  // if all else fails, assume background
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Failed to find MC history for " << id << endreq;
  }

  // if all else fails, assume background
  return true;
}

bool
RichMCTruthTool::isCherenkovRadiation( const RichSmartID id,
                                       const Rich::RadiatorType rad ) const
{

  // first, try via summary objects
  MCRichDigitSummaryMap::const_iterator iEn = mcRichDigSumMap().find( id );
  if ( iEn != mcRichDigSumMap().end() )
  {
    // loop over summaries
    for ( MCRichDigitSummaries::const_iterator iSum = (*iEn).second.begin();
          iSum != (*iEn).second.end(); ++iSum )
    {
      const MCRichDigitHistoryCode & code = (*iSum)->history();
      if      ( Rich::Aerogel == rad && code.aerogelHit() ) { return true; }
      else if ( Rich::C4F10   == rad && code.c4f10Hit()   ) { return true; }
      else if ( Rich::CF4     == rad && code.cf4Hit()     ) { return true; }
    }
    // if get here, must be background
    return false;
  }

  // finally, assume not
  return false;
}

bool
RichMCTruthTool::getMcHistories( const RichSmartID id,
                                 std::vector<const MCRichDigitSummary*> & histories ) const
{
  // clear histories
  histories.clear();
  if ( mcRichDigitSummaries() )
  {
    // try to find summaries
    MCRichDigitSummaryMap::const_iterator iEn = mcRichDigSumMap().find( id );
    if ( iEn != mcRichDigSumMap().end() )
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
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "Failed to locate MCRichDigits at " << m_mcRichDigitsLocation
                << endreq;
      }
    }

  }

  return m_mcRichDigits;
}

const MCRichDigitSummarys *
RichMCTruthTool::mcRichDigitSummaries() const
{
  if ( !m_mcRichDigitSumsDone )
  {
    m_mcRichDigitSumsDone = true;

    // try and load MCRichDigitSummarys
    if ( exist<MCRichDigitSummarys>(m_mcRichDigitSumsLocation) )
    {
      m_mcRichDigitSums = get<MCRichDigitSummarys>(m_mcRichDigitSumsLocation);
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "Successfully located " << m_mcRichDigitSums->size()
                << " MCRichDigitSummaries at " << m_mcRichDigitSumsLocation << endreq;
      }
    }
    else
    {
      m_mcRichDigitSums = NULL;
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "Failed to locate MCRichDigitSummaries at " << m_mcRichDigitSumsLocation
                << endreq;
      }
    }

  }
  return m_mcRichDigitSums;
}

const RichMCTruthTool::MCRichDigitSummaryMap & RichMCTruthTool::mcRichDigSumMap() const
{
  if ( !m_mcRichDigSumMapDone )
  {
    m_mcRichDigSumMapDone = true;

    // clear current map
    m_mcRichDigSumMap.clear();

    // loop over summaries
    if ( mcRichDigitSummaries() )
    {
      for ( MCRichDigitSummarys::const_iterator iSum = mcRichDigitSummaries()->begin();
            iSum != mcRichDigitSummaries()->end(); ++iSum )
      {
        m_mcRichDigSumMap[(*iSum)->richSmartID()].push_back( *iSum );
      }
    }

    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Built RichSmartID->MCRichDigitSummary Map for " << m_mcRichDigSumMap.size()
              << " RichSmartIDs" << endreq;
    }

  }
  return m_mcRichDigSumMap;
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
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "Successfully located " << m_mcRichHits->size()
                << " MCRichHits at " << m_mcRichHitsLocation << endreq;
      }
    }
    else
    {
      m_mcRichHits = NULL;
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "Failed to locate MCRichHits at " << m_mcRichHitsLocation << endreq;
      }
    }

  }

  return m_mcRichHits;
}

RichMCTruthTool::MCRichHitToPhoton * RichMCTruthTool::mcPhotonLinks() const
{
  if ( !m_mcPhotonLinks )
  {
    const std::string loc = MCRichHitLocation::Default+"2MCRichOpticalPhotons";
    m_mcPhotonLinks = new MCRichHitToPhoton( evtSvc(), msgSvc(), loc );
    if ( m_mcPhotonLinks->notFound() )
    {
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "Linker for MCRichHits to MCRichOpticalPhotons not found at '" << loc
                << "'" << endreq;
      }
    }
  }
  return m_mcPhotonLinks;
}

RichMCTruthTool::MCPartToRichTracks * RichMCTruthTool::mcTrackLinks() const
{
  if ( !m_mcTrackLinks )
  {
    const std::string loc = MCParticleLocation::Default+"2MCRichTracks";
    m_mcTrackLinks = new MCPartToRichTracks( evtSvc(), msgSvc(), loc );
    if ( m_mcTrackLinks->notFound() )
    {
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "Linker for MCParticles to MCRichTracks not found at '"
                << loc << "'" << endreq;
      }
    }
  }
  return m_mcTrackLinks;
}

const RichMCTruthTool::MCPartToMCRichHits &
RichMCTruthTool::mcPartToMCRichHitsMap() const
{
  if ( m_mcPToHits.empty() )
  {
    // loop over all MCRichHits
    // only using signal event here for the time being. Should add spillovers sometime ...
    if ( mcRichHits() )
    {
      for ( MCRichHits::const_iterator iHit = mcRichHits()->begin();
            iHit != mcRichHits()->end(); ++iHit )
      {
        const MCParticle * mcP = (*iHit)->mcParticle();
        if ( mcP )
        {
          if ( msgLevel(MSG::VERBOSE) )
          {
            verbose() << "Adding MCRichHit to list for MCParticle " << mcP->key() << endreq;
          }
          m_mcPToHits[mcP].push_back( *iHit );
        }
      }
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "Built MCParticle->MCRichHit Map for " << m_mcPToHits.size()
                << " RichSmartIDs" << endreq;
      }
    }
  }
  return m_mcPToHits;
}

const RichMCTruthTool::SmartIDToMCRichHits &
RichMCTruthTool::smartIDToMCRichHitsMap() const
{
  if ( m_smartIDsToHits.empty() )
  {
    // loop over all MCRichHits
    // only using signal event here for the time being. Should add spillovers sometime
    if ( mcRichHits() )
    {
      for ( MCRichHits::const_iterator iHit = mcRichHits()->begin();
            iHit != mcRichHits()->end(); ++iHit )
      {
        // For the moment, strip sub-pixel information
        // in the longer term, should do something more clever
        const RichSmartID pixelID = (*iHit)->sensDetID().pixelID();
        if ( msgLevel(MSG::VERBOSE) )
        {
          verbose() << "Adding MCRichHit to list for PixelID " << pixelID << endreq;
        }
        m_smartIDsToHits[pixelID].push_back( *iHit );
      }
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "Built RichSmartID->MCRichHit Map for " << m_smartIDsToHits.size()
                << " RichSmartIDs" << endreq;
      }
    }
  }
  return m_smartIDsToHits;
}

const SmartRefVector<MCRichHit> &
RichMCTruthTool::mcRichHits( const MCParticle * mcp ) const
{
  MCPartToMCRichHits::const_iterator i = mcPartToMCRichHitsMap().find( mcp );
  return ( i != mcPartToMCRichHitsMap().end() ? (*i).second : m_emptyContainer );
}

const SmartRefVector<MCRichHit> &
RichMCTruthTool::mcRichHits( const RichSmartID smartID ) const
{
  // try working backwards
  SmartIDToMCRichHits::const_iterator i = smartIDToMCRichHitsMap().find( smartID );
  if ( i != smartIDToMCRichHitsMap().end() )
  {
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "Found " << (*i).second.size()
                << " MCRichHits for PixelID " << smartID << endreq;
    }
    return (*i).second;
  }

  // MC association failed
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Failed to find MCRichHits associated to RichSmartID "
            << smartID << endreq;
  }
  return m_emptyContainer;
}

bool RichMCTruthTool::richMCHistoryAvailable() const
{
  return ( NULL != mcRichDigitSummaries() );
}

bool RichMCTruthTool::extendedMCAvailable() const
{
  return ( NULL != mcPhotonLinks() && NULL != mcTrackLinks() );
}
