// $Id: FilterTrueTracks.cpp,v 1.4 2008-07-08 12:12:10 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// LHCb
#include "MCInterfaces/IMCDecayFinder.h"
#include "Kernel/IWriteSelResult.h"

// local
#include "FilterTrueTracks.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FilterTrueTracks
//
// 2007-08-17 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( FilterTrueTracks );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FilterTrueTracks::FilterTrueTracks( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_mcDecFinder(0)
  , m_selResult(0)
{
  
  declareProperty( "TracksPath", m_tracksPath );
  declareProperty( "OutputPath", m_outputPath = "Undefined" );
  declareProperty( "MCParticlePath", m_mcParticlePath = LHCb::MCParticleLocation::Default);
  declareProperty( "VetoSignal", m_filterOut = false );

}
//=============================================================================
// Destructor
//=============================================================================
FilterTrueTracks::~FilterTrueTracks() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode FilterTrueTracks::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  debug() << "==> Initialize" << endmsg;

  if ( m_tracksPath.empty() ){
    if ( context() == "HLT" ) m_tracksPath.push_back( LHCb::TrackLocation::HltForward) ;
    else                      m_tracksPath.push_back( LHCb::TrackLocation::Default ) ;
  }
  info() << "Tracks will be taken from " ;
  for ( std::vector<std::string>::const_iterator t = m_tracksPath.begin() ;
        t!=m_tracksPath.end() ; ++t) info() << *t << " " ;
  info() << endmsg ;
  
  m_mcDecFinder = tool<IMCDecayFinder>("MCDecayFinder", this);
  m_selResult = tool<IWriteSelResult>("WriteSelResult", this);

  if ( m_outputPath == "Undefined" ){
    err() << "You must set the OutputPath " << endmsg ;
    return StatusCode::FAILURE;
  }  

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode FilterTrueTracks::execute() {

  debug() << "==> Execute" << endmsg;

  setFilterPassed(false);

  MCParts mcparts = finalStates();
  if ( msgLevel (MSG::DEBUG)) debug() << "Found " << mcparts.size() 
                                      << " true daughters" << endmsg ;
  counter("Signal particles") += mcparts.size();
  
  LHCb::Track::Container* newTracks = new LHCb::Track::Container() ;
  put( newTracks, m_outputPath);

  int nT = 0 ;
  for ( std::vector<std::string>::const_iterator p = m_tracksPath.begin() ;
        p!=m_tracksPath.end() ; ++p) {
   
    if ( !exist<LHCb::Track::Container>(*p)){
      Warning("No tracks at "+(*p),1);
    } else {
      LHCb::Track::Container* inTracks = get< LHCb::Track::Container>(*p);
      if (msgLevel(MSG::DEBUG)) debug() << "Container " << *p << " contains " 
                                        << inTracks->size() << " Tracks" << endmsg ;
      Asct assoc(evtSvc(),*p);
      const Table* table = assoc.direct();
      if ( NULL==table) {
        err() << "NO Table " << endmsg ;
        return StatusCode::FAILURE ;
      }

      if (msgLevel(MSG::DEBUG)) nT += inTracks->size();
      counter("All Tracks") += inTracks->size();
      for ( LHCb::Track::Container::const_iterator t = inTracks->begin() ;
            t != inTracks->end() ; ++t){
        const Range range = table->relations( *t) ;
        if (msgLevel(MSG::VERBOSE)) verbose() << "Track " << (*t)->key() 
                                              << " is associated to " 
                                              << range.size() << " MCP" << endmsg ;
        if ( isSignal(mcparts, range) ) {
          if (msgLevel(MSG::DEBUG)) debug() << "Track " << (*t)->key() 
                                            << " is associated to truth " << endmsg ;
          
          LHCb::Track* newT = (*t)->clone();
          if (msgLevel(MSG::VERBOSE)) verbose() << "Cloned Track " << (*t)->key() 
                                                << endmsg ;
          newTracks->insert(newT);
        }
      } 
    } 
  } 
  if (msgLevel(MSG::DEBUG)) debug() << "Saved " << newTracks->size() << " Tracks from "
                                    << nT << endmsg ;

  counter("Saved Tracks") += newTracks->size() ;

  for ( MCParts::const_iterator m = mcparts.begin() ; m != mcparts.end() ; ++m){
    if (m->second) counter("Found Particles")++ ;
  }
  
  if ( !newTracks->empty()) setFilterPassed(true);

  return m_selResult->write(name(), filterPassed());
}

//=========================================================================
//  get interesting MC Particles
//=========================================================================
MCParts FilterTrueTracks::finalStates ( ) const {
  const LHCb::MCParticle::Container* kmcparts = get<LHCb::MCParticle::Container>
    ( m_mcParticlePath );
  LHCb::MCParticle::ConstVector MCHead;
  const LHCb::MCParticle* imc = NULL; 
  while ( m_mcDecFinder->findDecay(*kmcparts, imc ) ){
    if (msgLevel(MSG::DEBUG)) debug() << "Found decay head " << imc->key() << " "
                                      << imc->particleID().pid() << endmsg ;
    MCHead.push_back(imc);
  }
  if (MCHead.empty()) Warning("Expected decay not found in this event") ;
  
  LHCb::MCParticle::ConstVector  decayMembers;
  LHCb::MCParticle::ConstVector::const_iterator ihead;
  for( ihead = MCHead.begin(); ihead != MCHead.end(); ++ihead){
    const LHCb::MCParticle* mc = *ihead;
    // final states must be flagged
    m_mcDecFinder->decayMembers(mc, decayMembers);
  }

  MCParts mcparts ;
  for ( LHCb::MCParticle::ConstVector::const_iterator i = decayMembers.begin();
        i != decayMembers.end() ; ++i ){
    mcparts.push_back(std::pair<const LHCb::MCParticle*,bool>(*i,false));
  }
  return mcparts ;
}
//=========================================================================
//  is it a signal track ?
//=========================================================================
bool FilterTrueTracks::isSignal( MCParts& mc, const Range& range) const {

  for ( MCParts::iterator m = mc.begin() ; m != mc.end() ; ++m){
    for ( iter rel = range.begin() ; rel != range.end() ; ++rel ){
      if ( m->first == rel->to() ){
        if (msgLevel(MSG::VERBOSE)) verbose() << "Track is associated to " 
                                              << m->first->particleID().pid() << endmsg ;
        m->second = true ; // found you
        return (!m_filterOut) ; // OK
      }
    }
  }
  
  return m_filterOut ;
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode FilterTrueTracks::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

