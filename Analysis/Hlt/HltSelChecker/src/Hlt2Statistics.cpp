// $Id: Hlt2Statistics.cpp,v 1.5 2009-10-16 11:29:32 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/GenHeader.h" 
#include "Kernel/ParticleID.h" 
#include "Event/HltDecReports.h"
#include "Event/HltSelReports.h"
#include "Kernel/IANNSvc.h"
// local
#include "Hlt2Statistics.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Hlt2Statistics
//
// 2008-06-23 : Patrick Koppenburg
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Hlt2Statistics::Hlt2Statistics( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : Hlt2StatisticsBase ( name , pSvcLocator )
{

}

//=============================================================================
// Destructor
//=============================================================================
Hlt2Statistics::~Hlt2Statistics() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode Hlt2Statistics::initialize()
{
  StatusCode sc = Hlt2StatisticsBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by Hlt2StatisticsBase

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_cats.push_back("t");
  m_cats.push_back("b");
  m_cats.push_back("c");
  m_cats.push_back("s");
  m_cats.push_back("ud");
  m_cats.push_back("1pp");
  m_cats.push_back("2pp");
  m_cats.push_back("3pp");
  m_cats.push_back("4pp");
  m_cats.push_back("5pp");
  m_cats.push_back("Mpp");

  sc =  m_algoCorr->algorithms(m_cats);
  if (!sc) return sc;
  strings sels = getSelections();
  sels.insert(sels.begin(),"Hlt2");
  sc =  m_algoCorr->algorithmsRow(sels);

  return sc ;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode Hlt2Statistics::execute() {
 
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg; 
  if (!fillQuarks()) return StatusCode::FAILURE;
  if (!fillHlt()) return StatusCode::FAILURE;  
  if ( msgLevel(MSG::VERBOSE) ) verbose() << "End of event" << endmsg;
  return m_algoCorr->endEvent();

}

//=========================================================================
//  fill Hlt
//=========================================================================
StatusCode Hlt2Statistics::fillHlt ( std::string level) 
{
  const LHCb::HltDecReports* decReports = 
    getIfExists<LHCb::HltDecReports>( LHCb::HltDecReportsLocation::Default );
  
  if( NULL!=decReports )
  { 
    
    if( !m_algoCorr->fillResult( level+"Global", (decReports->decReport(level+"Global"))? 
                        (decReports->decReport(level+"Global")->decision()):0 )) return StatusCode::FAILURE;
    unsigned int nsel = 0 ;
    std::vector<IANNSvc::minor_key_type> names = svc<IANNSvc>("HltANNSvc")->keys(level+"SelectionID");
    for ( std::vector<IANNSvc::minor_key_type>::const_iterator n = names.begin() ; n!= names.end() ; ++n){
      bool found = false ;
      // individual Hlt trigger lines
      for(LHCb::HltDecReports::Container::const_iterator it=decReports->begin();
          it!=decReports->end();++it){
        if ( ( it->first == *n ) ){
          if (msgLevel(MSG::DEBUG))  debug() << " Hlt trigger name= " << it->first  
                                             << " decision= " << it->second.decision() << endmsg;
          found = it->second.decision() ;
        }
      }
      if (msgLevel(MSG::VERBOSE)) verbose() << "Added " << *n << " " << found 
                                            << " to " << nsel << endmsg ;
      bool isDecision = ( std::string(*n).find("Decision") == std::string(*n).length()-8  ) ; // 8 is length of Decision
      if (isDecision && found) nsel++ ;
      if (!m_algoCorr->fillResult(*n, found )) return StatusCode::FAILURE;
    }
  } else Warning("No HltDecReports at "+LHCb::HltDecReportsLocation::Default,StatusCode::FAILURE,1);
  
  if (msgLevel(MSG::DEBUG)) debug() << "Done " << level << endmsg ;
  return StatusCode::SUCCESS ;
}
//=========================================================================
//  Fill Quarks
//=========================================================================
StatusCode Hlt2Statistics::fillQuarks( ) {
 // number of pp collisions
  const LHCb::GenHeader* mch = get<LHCb::GenHeader>(LHCb::GenHeaderLocation::Default);
  std::string npp ;
  switch ( mch->collisions().size() ){
  case 1 : npp = "1pp" ; break ;
  case 2 : npp = "2pp" ; break ;
  case 3 : npp = "3pp" ; break ;
  case 4 : npp = "4pp" ; break ;
  case 5 : npp = "5pp" ; break ;
  default : npp = "Mpp" ;
  }
  if (msgLevel(MSG::VERBOSE)) verbose() << mch->collisions().size() << " gets " << npp << endmsg ;

  if (!m_algoCorr->fillResult(npp, true )) return StatusCode::FAILURE;
  // quarks
  for ( SmartRefVector< LHCb::GenCollision >::const_iterator ic = mch->collisions().begin() ;
        ic != mch->collisions().end() ; ++ic){
    if (0==*ic) Exception("Null collision pointer");
    const LHCb::HepMCEvent* gene = (*ic)->event() ;
    if (0==*ic) Exception("Null event pointer");
    for ( HepMC::GenEvent::particle_const_iterator p = gene->pGenEvt()->particles_begin();
          p != gene->pGenEvt()->particles_end();   ++p ) {
      LHCb::ParticleID pid( (*p)->pdg_id()) ;     
      if (msgLevel(MSG::VERBOSE)) verbose() << "Gen particle " << (*p)->pdg_id() << " "
                                            << pid.hasQuark(LHCb::ParticleID::bottom)<< " " 
                                            << pid.hasQuark(LHCb::ParticleID::charm) << endmsg ;
      if (pid.hasQuark(LHCb::ParticleID::top)) {
        if (!m_algoCorr->fillResult("t", true ) ) return StatusCode::FAILURE;
        break ;
      } else if (pid.hasQuark(LHCb::ParticleID::bottom)) {
        if (!m_algoCorr->fillResult("b", true )) return StatusCode::FAILURE;
      } else if (pid.hasQuark(LHCb::ParticleID::charm)) {
        if (!m_algoCorr->fillResult("c", true )) return StatusCode::FAILURE;
      } else if (pid.hasQuark(LHCb::ParticleID::strange)){
        if (!m_algoCorr->fillResult("s", true )) return StatusCode::FAILURE;
      } else if (!m_algoCorr->fillResult("ud", true )) return StatusCode::FAILURE;
    }
    if (msgLevel(MSG::VERBOSE)) verbose() << "Process type is " << (*ic)->processType() << endmsg ;
  }
  return StatusCode::SUCCESS ;
}

//=============================================================================

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( Hlt2Statistics )
