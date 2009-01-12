// $Id: SpotLight.cpp,v 1.2 2009-01-12 13:49:15 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Kernel/IPrintDecay.h"
// local
#include "SpotLight.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SpotLight
//
// 2008-09-22 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( SpotLight );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SpotLight::SpotLight( const std::string& name,
                      ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator )
    , m_mother()
    , m_intermediate()
    , m_final()
    , m_print()
    , m_selResults(LHCb::SelResultLocation::Default)
{
  

  declareProperty("Algorithms", m_algos, "The actual list of algos to look at");
}
//=============================================================================
// Destructor
//=============================================================================
SpotLight::~SpotLight() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode SpotLight::initialize() {
  //=== The following two lines should be commented for DC04 algorithms ! ===
  StatusCode sc = DVAlgorithm::initialize(); 
  if ( sc.isFailure() ) return sc;

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  if ( m_algos.empty()) {
    err() << "No algorithms given" << endmsg ;
    return StatusCode::FAILURE ;
  }

  info() << "Will be looking at algorithms " ;
  for ( std::vector<std::string>::const_iterator a = m_algos.begin() ; a!= m_algos.end() ; ++a) info() << *a << " " ;
  info() << endmsg ;
  

  m_mother = tool<IFilterCriterion>("LoKi::Hybrid::FilterCriterion", "MotherFilter", this ) ;
  m_intermediate = tool<IFilterCriterion>("LoKi::Hybrid::FilterCriterion", "IntermediateFilter", this ) ;
  m_final = tool<IFilterCriterion>("LoKi::Hybrid::FilterCriterion", "FinalFilter", this ) ;
  m_print = tool<IPrintDecay>("PrintDecayTreeTool/PrintDecay");

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode SpotLight::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // code goes here  
  setFilterPassed(false);  // Mandatory. Set to true if event is accepted.

  LHCb::Particle::ConstVector candidates = headCandidates() ;
  counter("Candidates")+=candidates.size() ;
  unsigned int nC = 0 ;
  for ( LHCb::Particle::ConstVector::const_iterator i = candidates.begin() ;
        i != candidates.end() ; ++i){
    
    if (msgLevel(MSG::DEBUG)) m_print->printTree(*i);
    if (spotLight(*i, 0)){
      info() << "Found a hot candidate" << endmsg ;
      m_print->printTree(*i);
      nC++ ;
      setFilterPassed(true);
    }
  }
  counter("Hot candidates")+=nC;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode SpotLight::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return DVAlgorithm::finalize(); //=== For DC04, return StatusCode::SUCCESS;
}

//=========================================================================
//  find the good guys
//=========================================================================
bool SpotLight::spotLight( const LHCb::Particle* p, int level) const {
  // 
  if (msgLevel(MSG::DEBUG)) {
    const LHCb::ParticleProperty* pp = ppSvc()->find(p->particleID());
    if (0!=pp) debug() << pp->particle() 
                       << " M: " << p->measuredMass() 
                       << " level : " << level << endmsg ;
    else debug() << "What is " << p->particleID().pid() << endmsg ;
  }
  
  bool pass = false ;
  if ( 0 == level ) { // mother
    pass = m_mother->isSatisfied(p);
    if (msgLevel(MSG::DEBUG)) debug() << "Mother " << pass << endmsg ;
    counter("Passed Mothers")+=pass ;
  } else if ( p->daughters().empty() ){
    pass = m_final->isSatisfied(p);
    counter("Passed Finals")+=pass ;
    if (msgLevel(MSG::DEBUG)) debug() << "Final " << pass << endmsg ;
  } else {
    pass = m_intermediate->isSatisfied(p);
    if (msgLevel(MSG::DEBUG)) debug() << "Intermediate " << pass << endmsg ;
    counter("Passed Intermediates")+=pass;
  }  
  if (!pass) return pass ;

  // get daughters
  const LHCb::Particle::ConstVector dauts = p->daughtersVector();
  for ( LHCb::Particle::ConstVector::const_iterator i = dauts.begin() ;
        i != dauts.end() ; ++i){
    if (!spotLight(*i,level+1)) return false ;
  }
  return true ;
}

//=========================================================================
// Some containers contain intermediates as well. Kill them.
//=========================================================================
LHCb::Particle::ConstVector  SpotLight::headCandidates() const {

  LHCb::Particle::ConstVector goodcands ;
  if (!exist<LHCb::SelResults>(m_selResults)) return goodcands ;
  LHCb::SelResult::Container* SelResCtr = get<LHCb::SelResults>(m_selResults);
  if (msgLevel(MSG::VERBOSE)) verbose() << "SelResults: " << SelResCtr->size() << endmsg ;

  for ( LHCb::SelResults::const_iterator iselRes  = SelResCtr->begin() ;
        iselRes != SelResCtr->end(); iselRes++ ) {
    //      if (msgLevel(MSG::VERBOSE)) verbose() << "Got " << (*iselRes)->location() << endmsg ;
    if (!(*iselRes)->found()) continue ;
    for ( std::vector<std::string>::const_iterator a = m_algos.begin() ; a != m_algos.end() ; ++a){
    std::string location = "/Event/Phys/"+(*a) ;
    //    if (msgLevel(MSG::VERBOSE)) verbose() << "Looking for " << location << endmsg ;
      if ( location != (*iselRes)->location()) continue ;
      // now do the work
      if (!exist<LHCb::Particles>(location+"/Particles")) {
        Warning("No particles at "+location,StatusCode::FAILURE,1);
        continue ;
      }
      LHCb::Particle::Container* candidates = get<LHCb::Particles>(location+"/Particles") ;      
      if ((msgLevel(MSG::VERBOSE)) && (!candidates->empty())) verbose() << "Cleaning " 
                                                                        << candidates->size() << " candidates in " 
                                                                        << location << endmsg ;
      for ( LHCb::Particle::Container::const_iterator i = candidates->begin() ;
            i != candidates->end() ; ++i){
        if (msgLevel(MSG::VERBOSE)) m_print->printTree(*i);
        // check if the candidate is a daughter of another one. In that case ignore. Need for some LoKi algos
        // @todo remove that once there are no such cases anymore.
        bool isdaut = false ;
        for ( LHCb::Particle::Container::const_iterator j = candidates->begin() ;
              j != candidates->end() ; ++j){
          for ( LHCb::Particle::ConstVector::const_iterator k = (*j)->daughtersVector().begin() ;
                k != (*j)->daughtersVector().end() ; ++k){
            if ( (*k)==(*i)) {
              if (msgLevel(MSG::VERBOSE)) verbose() << (*i)->particleID().pid() << " is a daughter of a " 
                                                    << (*j)->particleID().pid() << endmsg ;
              isdaut = true ;
              break ;
            }
          }
          if (isdaut) break ;
        }
        if (msgLevel(MSG::VERBOSE)) verbose() << "Keeping " << (*i)->particleID().pid() << endmsg ;
        if (!isdaut) goodcands.push_back(*i);
      }
    }
  }
  
  return goodcands ;
  
}
//=============================================================================
