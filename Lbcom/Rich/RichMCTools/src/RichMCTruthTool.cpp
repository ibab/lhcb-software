
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
using namespace Rich::MC;

DECLARE_TOOL_FACTORY( MCTruthTool )

// Standard constructor
MCTruthTool::MCTruthTool( const std::string& type,
                          const std::string& name,
                          const IInterface* parent )
  : Rich::ToolBase( type, name, parent )
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

MCTruthTool::~MCTruthTool() { }

StatusCode MCTruthTool::initialize()
{
  // Sets up various tools and services
  StatusCode sc = Rich::ToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // Retrieve particle property service
  LHCb::IParticlePropertySvc * ppSvc =
    svc<LHCb::IParticlePropertySvc>( "LHCb::ParticlePropertySvc", true );

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

// Method that handles various Gaudi "software events"
void MCTruthTool::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) { InitNewEvent(); }
}

bool MCTruthTool::mcParticles( const Rich::HPDPixelCluster& cluster,
                               std::vector<const LHCb::MCParticle*> & mcParts ) const
{
  mcParts.clear();
  for ( const auto& S : cluster.smartIDs() )
  {
    std::vector<const LHCb::MCParticle*> tmp_vect;
    mcParticles( S, tmp_vect );
    for ( const auto* P : tmp_vect ) { mcParts.push_back(P); }
  }
  return !mcParts.empty();
}

bool MCTruthTool::mcParticles( const LHCb::RichSmartID id,
                               std::vector<const LHCb::MCParticle*> & mcParts ) const
{
  // Clean vector
  mcParts.clear();

  // First try via direct MCParticles references in MCRichDigitSummarys
  const auto iEn = mcRichDigSumMap().find( id );
  if ( iEn != mcRichDigSumMap().end() )
  {
    for ( const auto* sum : (*iEn).second )
    {
      const auto * mcP = sum->mcParticle();
      // protect against null references
      if ( !mcP ) continue;
      // Add to vector, once per MCParticle
      const auto iFind = std::find( mcParts.begin(), mcParts.end(), mcP );
      if ( mcParts.end() == iFind ) mcParts.push_back( mcP );
    }
  }

  // return
  return !mcParts.empty();
}

const LHCb::MCRichDigit * MCTruthTool::mcRichDigit( const LHCb::RichSmartID id ) const
{
  _ri_debug << "Locating MCRichDigit for RichSmartID " << id << endmsg;
  const LHCb::MCRichDigit * mcDigit = 
    ( mcRichDigits() ? mcRichDigits()->object(id) : nullptr );
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
  return ( mcPart ? mcTrackLinks()->first( mcPart ) : nullptr );
}

const LHCb::MCRichOpticalPhoton *
MCTruthTool::mcOpticalPhoton( const LHCb::MCRichHit * mcHit ) const
{
  return ( mcPhotonLinks() ? mcPhotonLinks()->first(mcHit) : nullptr );
}

bool MCTruthTool::isBackground ( const Rich::HPDPixelCluster& cluster ) const
{
  for ( const auto& S : cluster.smartIDs() )
  {
    if ( !isBackground(S) ) return false;
  }
  return true;
}

bool MCTruthTool::isHPDReflection ( const Rich::HPDPixelCluster& cluster ) const
{
  for ( const auto& S : cluster.smartIDs() )
  {
    if ( !isHPDReflection(S) ) return false;
  }
  return true;
}

bool MCTruthTool::isSiBackScatter ( const Rich::HPDPixelCluster& cluster ) const
{
  for ( const auto& S : cluster.smartIDs() )
  {
    if ( !isSiBackScatter(S) ) return false;
  }
  return true;
}

bool MCTruthTool::isRadScintillation ( const Rich::HPDPixelCluster& cluster ) const
{
  for ( const auto& S : cluster.smartIDs() )
  {
    if ( !isRadScintillation(S) ) return false;
  }
  return true;
}

bool MCTruthTool::isBackground ( const LHCb::RichSmartID id ) const
{
  // try via summary objects
  const auto iEn = mcRichDigSumMap().find( id );
  if ( iEn != mcRichDigSumMap().end() )
  {
    // loop over summaries
    for ( const auto * sum : (*iEn).second )
    {
      // returns true if hit is only background (no signal contribution)
      if ( sum->history().hasSignal() ) return false;
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
  const auto iEn = mcRichDigSumMap().find( id );
  if ( iEn != mcRichDigSumMap().end() )
  {
    // loop over summaries
    for ( const auto * sum : (*iEn).second )
    {
      if ( sum->history().isSignal()      ) isSignal  = true;
      if ( sum->history().hpdReflection() ) isHPDRefl = true;
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
  const auto iEn = mcRichDigSumMap().find( id );
  if ( iEn != mcRichDigSumMap().end() )
  {
    // loop over summaries
    for ( const auto * sum : (*iEn).second )
    {
      if ( sum->history().isSignal()         ) isSignal = true;
      if ( sum->history().hpdSiBackscatter() ) isSiRefl = true;
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
  const auto iEn = mcRichDigSumMap().find( id );
  if ( iEn != mcRichDigSumMap().end() )
  {
    // loop over summaries
    for ( const auto * sum : (*iEn).second )
    {
      if ( sum->history().isSignal()         ) isSignal = true;
      if ( sum->history().radScintillation() ) isRadScint = true;
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
  for ( const auto& S : cluster.smartIDs() )
  {
    if ( isCherenkovRadiation(S,rad) ) return true;
  }
  return false;
}

bool
MCTruthTool::isCherenkovRadiation( const LHCb::RichSmartID id,
                                   const Rich::RadiatorType rad ) const
{

  // first, try via summary objects
  const auto iEn = mcRichDigSumMap().find( id );
  if ( iEn != mcRichDigSumMap().end() )
  {
    // loop over summaries
    for ( const auto * sum : (*iEn).second )
    {
      const LHCb::MCRichDigitHistoryCode & code = sum->history();
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
  for ( const auto& S : cluster.smartIDs() )
  {
    std::vector<const LHCb::MCRichDigitSummary*> tmp_hists;
    OK = ( getMcHistories(S,tmp_hists) || OK );
    //std::for_each( tmp_hists.begin(), tmp_hists.end(), histories.push_back( _1 ) );
    for ( const auto * sum : tmp_hists ) { histories.push_back(sum); }
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
    const auto iEn = mcRichDigSumMap().find( id );
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
  if ( UNLIKELY(!m_mcRichDigitsDone) )
  {
    m_mcRichDigitsDone = true;
    // try and load MCRichDigits
    m_mcRichDigits = getIfExists<LHCb::MCRichDigits>(m_mcRichDigitsLocation);
    if ( m_mcRichDigits )
    {
      _ri_debug << "Successfully located " << m_mcRichDigits->size()
                << " MCRichDigits at " << m_mcRichDigitsLocation << endmsg;
    }
    else
    {
      _ri_debug << "Failed to locate MCRichDigits at " << m_mcRichDigitsLocation
                << endmsg;
    }
  }

  return m_mcRichDigits;
}

const LHCb::MCRichDigitSummarys *
MCTruthTool::mcRichDigitSummaries() const
{
  if ( UNLIKELY(!m_mcRichDigitSumsDone) )
  {
    m_mcRichDigitSumsDone = true;
    // try and load MCRichDigitSummarys
    m_mcRichDigitSums = getIfExists<LHCb::MCRichDigitSummarys>(m_mcRichDigitSumsLocation);
    if ( m_mcRichDigitSums )
    {
      _ri_debug << "Successfully located " << m_mcRichDigitSums->size()
                << " MCRichDigitSummaries at " << m_mcRichDigitSumsLocation << endmsg;
    }
    else
    {
      _ri_debug << "Failed to locate MCRichDigitSummaries at " << m_mcRichDigitSumsLocation
                << endmsg;
    }
  }
  return m_mcRichDigitSums;
}

const MCTruthTool::MCRichDigitSummaryMap &
MCTruthTool::mcRichDigSumMap() const
{
  if ( UNLIKELY(!m_mcRichDigSumMapDone) )
  {
    m_mcRichDigSumMapDone = true;

    // clear current map
    m_mcRichDigSumMap.clear();

    // loop over summaries
    if ( mcRichDigitSummaries() )
    {
      for ( const auto * sum : *mcRichDigitSummaries() )
      {
        m_mcRichDigSumMap[sum->richSmartID()].push_back( sum );
      }
    }

    _ri_debug << "Built RichSmartID->MCRichDigitSummary Map for " << m_mcRichDigSumMap.size()
              << " RichSmartIDs" << endmsg;

  }
  return m_mcRichDigSumMap;
}

const LHCb::MCRichHits * MCTruthTool::mcRichHits() const
{
  if ( UNLIKELY(!m_mcRichHitsDone) )
  {
    m_mcRichHitsDone = true;

    // Try and load MC Rich Hits
    m_mcRichHits = getIfExists<LHCb::MCRichHits>(m_mcRichHitsLocation);
    if ( m_mcRichHits )
    {
      _ri_debug << "Successfully located " << m_mcRichHits->size()
                << " MCRichHits at " << m_mcRichHitsLocation << endmsg;
    }
    else
    {
      _ri_debug << "Failed to locate MCRichHits at " << m_mcRichHitsLocation << endmsg;
    }

  }

  return m_mcRichHits;
}

MCTruthTool::MCRichHitToPhoton *
MCTruthTool::mcPhotonLinks() const
{
  if ( !m_mcPhotonLinks.get() )
  {
    const std::string loc = LHCb::MCRichHitLocation::Default+"2MCRichOpticalPhotons";
    m_mcPhotonLinks.reset( new MCRichHitToPhoton( evtSvc(), msgSvc(), loc ) );
    if ( m_mcPhotonLinks->notFound() )
    {
      _ri_debug << "Linker for MCRichHits to MCRichOpticalPhotons not found at '" << loc
                << "'" << endmsg;
    }
  }
  return m_mcPhotonLinks.get();
}

MCTruthTool::MCPartToRichTracks *
MCTruthTool::mcTrackLinks() const
{
  if ( !m_mcTrackLinks.get() )
  {
    const std::string loc = LHCb::MCParticleLocation::Default+"2MCRichTracks";
    m_mcTrackLinks.reset( new MCPartToRichTracks( evtSvc(), msgSvc(), loc ) );
    if ( m_mcTrackLinks->notFound() )
    {
      _ri_debug << "Linker for MCParticles to MCRichTracks not found at '"
                << loc << "'" << endmsg;
    }
  }
  return m_mcTrackLinks.get();
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
      for ( const auto * hit : *mcRichHits() )
      {
        const auto * mcP = hit->mcParticle();
        if ( mcP )
        {
          _ri_verbo << "Adding MCRichHit to list for MCParticle " << mcP->key() << endmsg;
          m_mcPToHits[mcP].push_back( hit );
        }
      }
      _ri_debug << "Built MCParticle->MCRichHit Map for " << m_mcPToHits.size()
                << " RichSmartIDs" << endmsg;
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
      for ( const auto * hit : *mcRichHits() )
      {
        // For the moment, strip sub-pixel information
        // in the longer term, should do something more clever
        const auto pixelID = hit->sensDetID().pixelID();
        _ri_verbo << "Adding MCRichHit to list for Pixel ID " << pixelID << endmsg;
        m_smartIDsToHits[pixelID].push_back( hit );
        // Add another entry for the HPD
        const auto hpdID = pixelID.pdID();
        _ri_verbo << "Adding MCRichHit to list for HPD ID " << hpdID << endmsg;
        m_smartIDsToHits[hpdID].push_back( hit );
      }
      _ri_debug << "Built RichSmartID->MCRichHit Map for " << m_smartIDsToHits.size()
                << " RichSmartIDs" << endmsg;
    }
  }
  return m_smartIDsToHits;
}

const SmartRefVector<LHCb::MCRichHit> &
MCTruthTool::mcRichHits( const LHCb::MCParticle * mcp ) const
{
  const auto i = mcPartToMCRichHitsMap().find( mcp );
  return ( i != mcPartToMCRichHitsMap().end() ? (*i).second : m_emptyContainer );
}

void
MCTruthTool::mcRichHits( const Rich::HPDPixelCluster& cluster,
                         SmartRefVector<LHCb::MCRichHit> & hits ) const
{
  for ( const auto& S : cluster.smartIDs() )
  {
    const auto & chits = mcRichHits(S);
    hits.reserve( hits.size() + chits.size() );
    for ( const auto& H : chits ) { hits.push_back(H); }
  }
}

const SmartRefVector<LHCb::MCRichHit> &
MCTruthTool::mcRichHits( const LHCb::RichSmartID smartID ) const
{
  // try working backwards
  const auto i = smartIDToMCRichHitsMap().find( smartID );
  if ( i != smartIDToMCRichHitsMap().end() )
  {
    _ri_verbo << "Found " << (*i).second.size()
              << " MCRichHits for PixelID " << smartID << endmsg;
    return (*i).second;
  }

  // MC association failed
  _ri_debug << "Failed to find MCRichHits associated to RichSmartID "
            << smartID << endmsg;
  return m_emptyContainer;
}

bool MCTruthTool::richMCHistoryAvailable() const
{
  return ( nullptr != mcRichDigitSummaries() );
}

bool MCTruthTool::extendedMCAvailable() const
{
  return ( nullptr != mcPhotonLinks() && nullptr != mcTrackLinks() );
}
