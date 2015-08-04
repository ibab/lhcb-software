
//-----------------------------------------------------------------------------
/** @file RichMCTruthTool.cpp
 *
 * Implementation file for class : RichMCTruthTool
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 14/01/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichMCTruthTool.h"

// From PartProp
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

// namespace
//using namespace boost;
//using namespace boost::lambda;
using namespace Rich::MC;

DECLARE_TOOL_FACTORY( MCTruthTool )

// Standard constructor
MCTruthTool::MCTruthTool( const std::string& type,
                          const std::string& name,
                          const IInterface* parent )
  : Rich::ToolBase         ( type, name, parent ),
    m_mcRichDigitsDone     ( false ),
    m_mcRichDigitSumsDone  ( false ),
    m_mcRichDigSumMapDone  ( false ),
    m_mcRichHitsDone       ( false ),
    m_mcRichDigits         ( NULL  ),
    m_mcRichDigitSums      ( NULL  ),
    m_mcRichHits           ( NULL  ),
    m_mcTrackLinks         ( NULL  ),
    m_mcPhotonLinks        ( NULL  )
{
  // interface
  declareInterface<IMCTruthTool>(this);

  // job options
  declareProperty( "MCRichDigitsLocation",
                   m_mcRichDigitsLocation = LHCb::MCRichDigitLocation::Default );
  declareProperty( "MCRichDigitSummariesLocation",
                   m_mcRichDigitSumsLocation = LHCb::MCRichDigitSummaryLocation::Default );
  declareProperty( "MCRichHitsLocation",
                   m_mcRichHitsLocation = LHCb::MCRichHitLocation::Default );
}

MCTruthTool::~MCTruthTool() { cleanUpLinkers(); }

StatusCode MCTruthTool::initialize()
{
  // Sets up various tools and services
  StatusCode sc = Rich::ToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // Retrieve particle property service
  LHCb::IParticlePropertySvc * ppSvc = svc<LHCb::IParticlePropertySvc>( "LHCb::ParticlePropertySvc", true );

  // Setup the PDG code mappings
  m_localID[ 0 ]                                    = Rich::Unknown;
  m_localID[ abs(ppSvc->find("e+")->pid().pid()) ]  = Rich::Electron;
  m_localID[ abs(ppSvc->find("mu+")->pid().pid()) ] = Rich::Muon;
  m_localID[ abs(ppSvc->find("pi+")->pid().pid()) ] = Rich::Pion;
  m_localID[ abs(ppSvc->find("K+")->pid().pid()) ]  = Rich::Kaon;
  m_localID[ abs(ppSvc->find("p+")->pid().pid()) ]  = Rich::Proton;

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );

  // initialise
  m_emptyContainer.clear();

  // Make sure we are ready for a new event
  InitNewEvent();

  // release service since it is no longer needed
  sc = release(ppSvc);

  return sc;
}

StatusCode MCTruthTool::finalize()
{
  // clean up linkers
  cleanUpLinkers();

  // Execute base class method
  return Rich::ToolBase::finalize();
}

// Method that handles various Gaudi "software events"
void MCTruthTool::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) { InitNewEvent(); }
}

bool MCTruthTool::mcParticles( const Rich::HPDPixelCluster& cluster,
                               std::vector<const LHCb::MCParticle*> & mcParts ) const
{
  mcParts.clear();
  for ( LHCb::RichSmartID::Vector::const_iterator iS = cluster.smartIDs().begin();
        iS != cluster.smartIDs().end(); ++iS )
  {
    std::vector<const LHCb::MCParticle*> tmp_vect;
    mcParticles( *iS, tmp_vect );
    for ( std::vector<const LHCb::MCParticle*>::const_iterator iP = tmp_vect.begin();
          iP != tmp_vect.end(); ++iP ) { mcParts.push_back(*iP); }
  }
  return !mcParts.empty();
}

bool MCTruthTool::mcParticles( const LHCb::RichSmartID id,
                               std::vector<const LHCb::MCParticle*> & mcParts ) const
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
      const LHCb::MCParticle * mcP = (*iSum)->mcParticle();
      // protect against null references
      if ( !mcP ) continue;
      // Add to vector, once per MCParticle
      std::vector<const LHCb::MCParticle*>::const_iterator iFind =
        std::find( mcParts.begin(), mcParts.end(), mcP );
      if ( mcParts.end() == iFind ) mcParts.push_back( mcP );
    }
  }

  // return
  return !mcParts.empty();
}

const LHCb::MCRichDigit * MCTruthTool::mcRichDigit( const LHCb::RichSmartID id ) const
{
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Locating MCRichDigit for RichSmartID " << id << endmsg;
  }
  const LHCb::MCRichDigit * mcDigit = ( mcRichDigits() ? mcRichDigits()->object(id) : 0 );
  if ( !mcDigit && msgLevel(MSG::DEBUG) )
  {
    debug() << "Failed to locate MCRichDigit from RichSmartID " << id << endmsg;
  }
  return mcDigit;
}

Rich::ParticleIDType
MCTruthTool::mcParticleType( const LHCb::MCParticle * mcPart ) const
{
  return (mcPart ? m_localID[abs(mcPart->particleID().pid())] : Rich::Unknown);
}

const LHCb::MCRichTrack *
MCTruthTool::mcRichTrack( const LHCb::MCParticle * mcPart ) const
{
  return ( mcPart ? mcTrackLinks()->first( mcPart ) : 0 );
}

const LHCb::MCRichOpticalPhoton *
MCTruthTool::mcOpticalPhoton( const LHCb::MCRichHit * mcHit ) const
{
  return ( mcPhotonLinks() ? mcPhotonLinks()->first(mcHit) : 0 );
}

bool MCTruthTool::isBackground ( const Rich::HPDPixelCluster& cluster ) const
{
  for ( LHCb::RichSmartID::Vector::const_iterator iS = cluster.smartIDs().begin();
        iS != cluster.smartIDs().end(); ++iS )
  {
    if ( !isBackground(*iS) ) return false;
  }
  return true;
}

bool MCTruthTool::isHPDReflection ( const Rich::HPDPixelCluster& cluster ) const
{
  for ( LHCb::RichSmartID::Vector::const_iterator iS = cluster.smartIDs().begin();
        iS != cluster.smartIDs().end(); ++iS )
  {
    if ( !isHPDReflection(*iS) ) return false;
  }
  return true;
}

bool MCTruthTool::isSiBackScatter ( const Rich::HPDPixelCluster& cluster ) const
{
  for ( LHCb::RichSmartID::Vector::const_iterator iS = cluster.smartIDs().begin();
        iS != cluster.smartIDs().end(); ++iS )
  {
    if ( !isSiBackScatter(*iS) ) return false;
  }
  return true;
}

bool MCTruthTool::isRadScintillation ( const Rich::HPDPixelCluster& cluster ) const
{
  for ( LHCb::RichSmartID::Vector::const_iterator iS = cluster.smartIDs().begin();
        iS != cluster.smartIDs().end(); ++iS )
  {
    if ( !isRadScintillation(*iS) ) return false;
  }
  return true;
}

bool MCTruthTool::isBackground ( const LHCb::RichSmartID id ) const
{
  // try via summary objects
  MCRichDigitSummaryMap::const_iterator iEn = mcRichDigSumMap().find( id );
  if ( iEn != mcRichDigSumMap().end() )
  {
    // loop over summaries
    for ( MCRichDigitSummaries::const_iterator iSum = (*iEn).second.begin();
          iSum != (*iEn).second.end(); ++iSum )
    {
      // returns true if hit is only background (no signal contribution)
      if ( (*iSum)->history().hasSignal() ) return false;
    }
  }
  else if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Failed to find MC history for " << id << endmsg;
  }

  // if all else fails, assume background
  return true;
}

bool MCTruthTool::isHPDReflection ( const LHCb::RichSmartID id ) const
{
  // try via summary objects
  bool isSignal(false), isHPDRefl(false);
  MCRichDigitSummaryMap::const_iterator iEn = mcRichDigSumMap().find( id );
  if ( iEn != mcRichDigSumMap().end() )
  {
    // loop over summaries
    for ( MCRichDigitSummaries::const_iterator iSum = (*iEn).second.begin();
          iSum != (*iEn).second.end(); ++iSum )
    {
      if ( (*iSum)->history().isSignal()      ) isSignal  = true;
      if ( (*iSum)->history().hpdReflection() ) isHPDRefl = true;
    }
  }
  else if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Failed to find MC history for " << id << endmsg;
  }

  return (!isSignal && isHPDRefl);
}

bool MCTruthTool::isSiBackScatter ( const LHCb::RichSmartID id ) const
{
  // try via summary objects
  bool isSignal(false), isSiRefl(false);
  MCRichDigitSummaryMap::const_iterator iEn = mcRichDigSumMap().find( id );
  if ( iEn != mcRichDigSumMap().end() )
  {
    // loop over summaries
    for ( MCRichDigitSummaries::const_iterator iSum = (*iEn).second.begin();
          iSum != (*iEn).second.end(); ++iSum )
    {
      if ( (*iSum)->history().isSignal()         ) isSignal = true;
      if ( (*iSum)->history().hpdSiBackscatter() ) isSiRefl = true;
    }
  }
  else if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Failed to find MC history for " << id << endmsg;
  }

  return (!isSignal && isSiRefl);
}

bool MCTruthTool::isRadScintillation ( const LHCb::RichSmartID id ) const
{
  // try via summary objects
  bool isSignal(false), isRadScint(false);
  MCRichDigitSummaryMap::const_iterator iEn = mcRichDigSumMap().find( id );
  if ( iEn != mcRichDigSumMap().end() )
  {
    // loop over summaries
    for ( MCRichDigitSummaries::const_iterator iSum = (*iEn).second.begin();
          iSum != (*iEn).second.end(); ++iSum )
    {
      if ( (*iSum)->history().isSignal()         ) isSignal = true;
      if ( (*iSum)->history().radScintillation() ) isRadScint = true;
    }
  }
  else if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Failed to find MC history for " << id << endmsg;
  }

  return (!isSignal && isRadScint);
}

bool MCTruthTool::isCherenkovRadiation( const Rich::HPDPixelCluster& cluster,
                                        const Rich::RadiatorType rad ) const
{
  for ( LHCb::RichSmartID::Vector::const_iterator iS = cluster.smartIDs().begin();
        iS != cluster.smartIDs().end(); ++iS )
  {
    if ( isCherenkovRadiation(*iS,rad) ) return true;
  }
  return false;
}

bool
MCTruthTool::isCherenkovRadiation( const LHCb::RichSmartID id,
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
      const LHCb::MCRichDigitHistoryCode & code = (*iSum)->history();
      if ( code.hasSignal() )
      {
        if      ( Rich::Aerogel == rad && code.aerogelHit() ) { return true; }
        else if ( Rich::C4F10   == rad && code.c4f10Hit()   ) { return true; }
        else if ( Rich::CF4     == rad && code.cf4Hit()     ) { return true; }
      }
    }
    // if get here, must be background
    return false;
  }

  // finally, assume not
  return false;
}

bool
MCTruthTool::getMcHistories( const Rich::HPDPixelCluster& cluster,
                             std::vector<const LHCb::MCRichDigitSummary*> & histories ) const
{
  bool OK = false;
  for ( LHCb::RichSmartID::Vector::const_iterator iS = cluster.smartIDs().begin();
        iS != cluster.smartIDs().end(); ++iS )
  {
    std::vector<const LHCb::MCRichDigitSummary*> tmp_hists;
    OK = ( getMcHistories(*iS,tmp_hists) || OK );
    //std::for_each( tmp_hists.begin(), tmp_hists.end(), histories.push_back( _1 ) );
    for ( std::vector<const LHCb::MCRichDigitSummary*>::const_iterator iSum = tmp_hists.begin();
          iSum != tmp_hists.end(); ++iSum ) { histories.push_back(*iSum); }
  }
  return OK;
}

bool
MCTruthTool::getMcHistories( const LHCb::RichSmartID id,
                             std::vector<const LHCb::MCRichDigitSummary*> & histories ) const
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

const LHCb::MCRichDigits * MCTruthTool::mcRichDigits() const
{
  if ( !m_mcRichDigitsDone )
  {
    m_mcRichDigitsDone = true;

    // try and load MCRichDigits
    if ( exist<LHCb::MCRichDigits>(m_mcRichDigitsLocation) )
    {
      m_mcRichDigits = get<LHCb::MCRichDigits>(m_mcRichDigitsLocation);
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "Successfully located " << m_mcRichDigits->size()
                << " MCRichDigits at " << m_mcRichDigitsLocation << endmsg;
      }
    }
    else
    {
      m_mcRichDigits = NULL;
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "Failed to locate MCRichDigits at " << m_mcRichDigitsLocation
                << endmsg;
      }
    }

  }

  return m_mcRichDigits;
}

const LHCb::MCRichDigitSummarys *
MCTruthTool::mcRichDigitSummaries() const
{
  if ( !m_mcRichDigitSumsDone )
  {
    m_mcRichDigitSumsDone = true;

    // try and load MCRichDigitSummarys
    if ( exist<LHCb::MCRichDigitSummarys>(m_mcRichDigitSumsLocation) )
    {
      m_mcRichDigitSums = get<LHCb::MCRichDigitSummarys>(m_mcRichDigitSumsLocation);
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "Successfully located " << m_mcRichDigitSums->size()
                << " MCRichDigitSummaries at " << m_mcRichDigitSumsLocation << endmsg;
      }
    }
    else
    {
      m_mcRichDigitSums = NULL;
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "Failed to locate MCRichDigitSummaries at " << m_mcRichDigitSumsLocation
                << endmsg;
      }
    }

  }
  return m_mcRichDigitSums;
}

const MCTruthTool::MCRichDigitSummaryMap &
MCTruthTool::mcRichDigSumMap() const
{
  if ( !m_mcRichDigSumMapDone )
  {
    m_mcRichDigSumMapDone = true;

    // clear current map
    m_mcRichDigSumMap.clear();

    // loop over summaries
    if ( mcRichDigitSummaries() )
    {
      for ( LHCb::MCRichDigitSummarys::const_iterator iSum = mcRichDigitSummaries()->begin();
            iSum != mcRichDigitSummaries()->end(); ++iSum )
      {
        m_mcRichDigSumMap[(*iSum)->richSmartID()].push_back( *iSum );
      }
    }

    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Built RichSmartID->MCRichDigitSummary Map for " << m_mcRichDigSumMap.size()
              << " RichSmartIDs" << endmsg;
    }

  }
  return m_mcRichDigSumMap;
}

const LHCb::MCRichHits * MCTruthTool::mcRichHits() const
{
  if ( !m_mcRichHitsDone )
  {
    m_mcRichHitsDone = true;

    // Try and load MC Rich Hits
    if ( exist<LHCb::MCRichHits>(m_mcRichHitsLocation) )
    {
      m_mcRichHits = get<LHCb::MCRichHits>(m_mcRichHitsLocation);
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "Successfully located " << m_mcRichHits->size()
                << " MCRichHits at " << m_mcRichHitsLocation << endmsg;
      }
    }
    else
    {
      m_mcRichHits = NULL;
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "Failed to locate MCRichHits at " << m_mcRichHitsLocation << endmsg;
      }
    }

  }

  return m_mcRichHits;
}

MCTruthTool::MCRichHitToPhoton *
MCTruthTool::mcPhotonLinks() const
{
  if ( !m_mcPhotonLinks )
  {
    const std::string loc = LHCb::MCRichHitLocation::Default+"2MCRichOpticalPhotons";
    m_mcPhotonLinks = new MCRichHitToPhoton( evtSvc(), msgSvc(), loc );
    if ( m_mcPhotonLinks->notFound() )
    {
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "Linker for MCRichHits to MCRichOpticalPhotons not found at '" << loc
                << "'" << endmsg;
      }
    }
  }
  return m_mcPhotonLinks;
}

MCTruthTool::MCPartToRichTracks *
MCTruthTool::mcTrackLinks() const
{
  if ( !m_mcTrackLinks )
  {
    const std::string loc = LHCb::MCParticleLocation::Default+"2MCRichTracks";
    m_mcTrackLinks = new MCPartToRichTracks( evtSvc(), msgSvc(), loc );
    if ( m_mcTrackLinks->notFound() )
    {
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "Linker for MCParticles to MCRichTracks not found at '"
                << loc << "'" << endmsg;
      }
    }
  }
  return m_mcTrackLinks;
}

const MCTruthTool::MCPartToMCRichHits &
MCTruthTool::mcPartToMCRichHitsMap() const
{
  if ( m_mcPToHits.empty() )
  {
    // loop over all MCRichHits
    // only using signal event here for the time being. Should add spillovers sometime ...
    if ( mcRichHits() )
    {
      for ( LHCb::MCRichHits::const_iterator iHit = mcRichHits()->begin();
            iHit != mcRichHits()->end(); ++iHit )
      {
        const LHCb::MCParticle * mcP = (*iHit)->mcParticle();
        if ( mcP )
        {
          if ( msgLevel(MSG::VERBOSE) )
          {
            verbose() << "Adding MCRichHit to list for MCParticle " << mcP->key() << endmsg;
          }
          m_mcPToHits[mcP].push_back( *iHit );
        }
      }
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "Built MCParticle->MCRichHit Map for " << m_mcPToHits.size()
                << " RichSmartIDs" << endmsg;
      }
    }
  }
  return m_mcPToHits;
}

const MCTruthTool::SmartIDToMCRichHits &
MCTruthTool::smartIDToMCRichHitsMap() const
{
  if ( m_smartIDsToHits.empty() )
  {
    // loop over all MCRichHits
    // only using signal event here for the time being. Should add spillovers sometime
    if ( mcRichHits() )
    {
      for ( LHCb::MCRichHits::const_iterator iHit = mcRichHits()->begin();
            iHit != mcRichHits()->end(); ++iHit )
      {
        // For the moment, strip sub-pixel information
        // in the longer term, should do something more clever
        const LHCb::RichSmartID pixelID = (*iHit)->sensDetID().pixelID();
        if ( msgLevel(MSG::VERBOSE) )
        {
          verbose() << "Adding MCRichHit to list for Pixel ID " << pixelID << endmsg;
        }
        m_smartIDsToHits[pixelID].push_back( *iHit );
        // Add another entry for the HPD
        const LHCb::RichSmartID hpdID = pixelID.pdID();
        if ( msgLevel(MSG::VERBOSE) )
        {
          verbose() << "Adding MCRichHit to list for HPD ID " << hpdID << endmsg;
        }
        m_smartIDsToHits[hpdID].push_back( *iHit );
      }
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "Built RichSmartID->MCRichHit Map for " << m_smartIDsToHits.size()
                << " RichSmartIDs" << endmsg;
      }
    }
  }
  return m_smartIDsToHits;
}

const SmartRefVector<LHCb::MCRichHit> &
MCTruthTool::mcRichHits( const LHCb::MCParticle * mcp ) const
{
  MCPartToMCRichHits::const_iterator i = mcPartToMCRichHitsMap().find( mcp );
  return ( i != mcPartToMCRichHitsMap().end() ? (*i).second : m_emptyContainer );
}

void
MCTruthTool::mcRichHits( const Rich::HPDPixelCluster& cluster,
                         SmartRefVector<LHCb::MCRichHit> & hits ) const
{
  for ( LHCb::RichSmartID::Vector::const_iterator iS = cluster.smartIDs().begin();
        iS != cluster.smartIDs().end(); ++iS )
  {
    const SmartRefVector<LHCb::MCRichHit> & tmp_hits = mcRichHits(*iS);
    for ( SmartRefVector<LHCb::MCRichHit>::const_iterator iH = tmp_hits.begin();
          iH != tmp_hits.end(); ++iH )
    {
      hits.push_back(*iH);
    }
  }
}

const SmartRefVector<LHCb::MCRichHit> &
MCTruthTool::mcRichHits( const LHCb::RichSmartID smartID ) const
{
  // try working backwards
  SmartIDToMCRichHits::const_iterator i = smartIDToMCRichHitsMap().find( smartID );
  if ( i != smartIDToMCRichHitsMap().end() )
  {
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "Found " << (*i).second.size()
                << " MCRichHits for PixelID " << smartID << endmsg;
    }
    return (*i).second;
  }

  // MC association failed
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Failed to find MCRichHits associated to RichSmartID "
            << smartID << endmsg;
  }
  return m_emptyContainer;
}

bool MCTruthTool::richMCHistoryAvailable() const
{
  return ( NULL != mcRichDigitSummaries() );
}

bool MCTruthTool::extendedMCAvailable() const
{
  return ( NULL != mcPhotonLinks() && NULL != mcTrackLinks() );
}
