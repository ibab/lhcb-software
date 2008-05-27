// $Id: TrueMCFilterCriterion.cpp,v 1.24 2008-05-27 13:32:56 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

#include "MCInterfaces/IMCDecayFinder.h"
#include "Kernel/Particle2MCLinker.h"

// local
#include "TrueMCFilterCriterion.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrueMCFilterCriterion
//
// 2004-09-13 : Patrick KOPPENBURG
// 
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( TrueMCFilterCriterion );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrueMCFilterCriterion::TrueMCFilterCriterion( const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent )
  : FilterCriterionBase ( type, name , parent )
  , m_pMCDecFinder(0)
  , m_pLinker(0)
  , m_mcParticles(0)
{
  declareInterface<IFilterCriterion>(this);
  declareProperty( "VetoSignal", m_filterOut = false );
  declareProperty( "ParticlePath", m_particlePaths );
  declareProperty( "MCParticlePath", m_mcParticlePath = LHCb::MCParticleLocation::Default);
  declareProperty( "ExpectSignal", m_complain = false );
  declareProperty( "AssociationMethod", m_method = Particle2MCMethod::Chi2 );
  
  m_decayMembers.clear();
}
//=============================================================================
// Destructor
//=============================================================================
TrueMCFilterCriterion::~TrueMCFilterCriterion() {}; 
//=============================================================================
// Initialize
//=============================================================================
StatusCode TrueMCFilterCriterion::initialize( ){

  const StatusCode sc = FilterCriterionBase::initialize() ;
  if ( !sc) return sc ;

  m_pMCDecFinder = tool<IMCDecayFinder>("MCDecayFinder", this); 

  std::string MCDecay = m_pMCDecFinder->decay();
  if (MCDecay == "")  {
    fatal() << "MC decay is empty! " << endmsg;
    return StatusCode::FAILURE;
  } else info() << "MC decay is " << MCDecay << endmsg ;

  info() << "Associating using method " << m_method << endmsg ;

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );

  if (msgLevel(MSG::VERBOSE)) verbose() << "Initialised happily" << endmsg ;
  setActive(); 

  return sc ;
}

//=========================================================================
// handle method for incident service 
//=========================================================================
void TrueMCFilterCriterion::handle( const Incident& incident) {
  // Update prior to start of event. Used to re-initialise data containers
  if ( IncidentType::BeginEvent == incident.type() ){
    if ( 0!= m_mcParticles ){
      m_mcParticles = 0;
      if (msgLevel(MSG::DEBUG)) debug() << "New event, resetting mc Particles" << endmsg ;
    }
    if (0!=m_pLinker){
      delete m_pLinker ;
      if (msgLevel(MSG::VERBOSE)) verbose() << "Deleted Linker" << endmsg ;
      m_pLinker = 0 ;
    }
  }
}
//=============================================================================
// Test if filter is satisfied
//=============================================================================
bool TrueMCFilterCriterion::testParticle( const LHCb::Particle* const & part ) {
 
  if (msgLevel(MSG::VERBOSE)) verbose() << "TrueMCFilterCriterion" << endmsg ;

  bool signal = false;

  if (!preloadMCParticles()){ // preload particles
    if (msgLevel(MSG::DEBUG)) debug() << "Signal not found" << endmsg ;
    signal = false ;
  } else {    

    const LHCb::MCParticle* MC = m_pLinker->firstMCP( part );
  
    if( NULL == MC ){
      if (msgLevel(MSG::DEBUG)) debug() << "Empty association range" << endmsg;
      if (msgLevel(MSG::DEBUG)) debug() << "No association for " << part->key() << " "
                                        << part->particleID().pid() << " " 
                                        << part->momentum()  << endmsg ;
      return m_filterOut ; // true if one wants to kill all, false else
    }
    
    if ( msgLevel ( MSG::DEBUG )) debug() << "Particle      " << part->key() << " " 
                                          << part->particleID().pid() << " " 
                                          << part->momentum() 
                                          << endmsg ;
    
    while ( NULL != MC ){
      
      if (msgLevel(MSG::DEBUG)) debug() << "Associated to " << MC->key() << " " 
                                        << MC->particleID().pid() << " " 
                                        << MC->momentum() << endmsg ;
      signal = findMCParticle(MC);
      
      if(signal) if (msgLevel(MSG::DEBUG)) debug() << "which is a signal particle" << endmsg;
      if(signal) break; // just take one associated MCParticle
      MC = m_pLinker->nextMCP();
    }
  }
  
  return ( signal ) ? (!m_filterOut) : m_filterOut ;
  
}

//=========================================================================
// Call DecayFinder only once per event
//=========================================================================
bool TrueMCFilterCriterion::preloadMCParticles ( ) {
  // MC list
  if ( 0 == m_mcParticles ){ // new event -> load particles

    // For Particle -> MCParticle association  
    if ( 0 != m_pLinker ) Exception("Linker should have been deleted");
    m_pLinker = new Particle2MCLinker(this,m_method,m_particlePaths);

    if (msgLevel(MSG::DEBUG)) debug() << "New event, get MC particles " << endmsg ;
    // this will issue an error if no MC particles present
    const LHCb::MCParticle::Container* kmcparts = 
      get<LHCb::MCParticle::Container>( m_mcParticlePath );
    counter("New Events")++;
    m_mcParticles = kmcparts ;
    m_decayMembers.clear() ; // clear decay members
    
    LHCb::MCParticle::ConstVector MCHead;
    const LHCb::MCParticle* imc = NULL;
    while ( m_pMCDecFinder->findDecay(*kmcparts, imc ) ){
      if (msgLevel(MSG::DEBUG)) debug() << "Found decay head " << imc->key() << " " 
                                        << imc->particleID().pid() << endmsg ;
      MCHead.push_back(imc);
    }
    if (MCHead.empty()) {
      if (m_complain)  warning() << "Expected decay not found in this event" << endmsg ;
    } else counter("Found Decay")++ ;
    LHCb::MCParticle::ConstVector::const_iterator ihead;
    for( ihead = MCHead.begin(); ihead != MCHead.end(); ++ihead){
      const LHCb::MCParticle* mc = *ihead;
      // final states must be flagged
      m_pMCDecFinder->decayMembers(mc, m_decayMembers); 
    }
    if (msgLevel(MSG::DEBUG)) debug() << "Found " << m_decayMembers.size() 
                                      << " MC particles from true decay" << endmsg ;
  } else if (msgLevel(MSG::DEBUG)) debug() << "Reusing " << m_decayMembers.size() 
                                           << " MC particles from true decay" << endmsg ;
  
  return !(m_decayMembers.empty()) ;  
  
}
//=============================================================================
// get MC particles
//=============================================================================
bool TrueMCFilterCriterion::findMCParticle( const LHCb::MCParticle* MC ) {

  if (msgLevel(MSG::VERBOSE)) verbose() << "TrueMCFilterCriterion find MC" << endmsg ;  
  if (m_decayMembers.empty()) return false ;

  if (msgLevel(MSG::VERBOSE)) verbose() << "Looking for " << MC->key() 
                                        << " " << MC->particleID().pid() << " " << MC->momentum() << endmsg ;  
  LHCb::MCParticle::ConstVector::iterator mcp ; // LF
  for ( mcp = m_decayMembers.begin() ; mcp != m_decayMembers.end() ; ++mcp ){
    if (msgLevel(MSG::VERBOSE)) verbose() << "Looping on  " << (*mcp)->key() 
                                          << " " << (*mcp)->particleID().pid() << " " 
                                          << (*mcp)->momentum() << endmsg ;  
    if ( (*mcp) == MC ) {
      if (msgLevel(MSG::VERBOSE)) verbose() << "Particle found" << endmsg;
      return true ; // found 
    }
  }
  if (msgLevel(MSG::VERBOSE)) verbose() << "Particle not found" << endmsg;
  return false ;
  
}
//=============================================================================
StatusCode TrueMCFilterCriterion::finalize(){
  
  if( NULL != m_pLinker ) delete m_pLinker;
  return GaudiTool::finalize() ;
  
}
