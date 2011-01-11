// $Id: TisTosParticleTagger.cpp,v 1.2 2010-08-09 14:17:47 tskwarni Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "TisTosParticleTagger.h"

#include "Event/Particle.h"
#include "Event/HltDecReports.h"
#include "Event/HltSelReports.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TisTosParticleTagger
//
// 2010-07-22 : Tomasz Skwarnicki
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TisTosParticleTagger );

using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TisTosParticleTagger::TisTosParticleTagger( const std::string& name,
                                            ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator )
{
  declareProperty("TisTosSpecs",m_tistosSpecs);
  declareProperty("PassOnAll",m_passOnAll=false);
  declareProperty("NoRegex",m_noRegex=false);
  declareProperty("TriggerTisTosName",m_TriggerTisTosName="");  

  declareProperty("HltDecReportsInputLocation", m_decReportsLocation = "" );
  declareProperty("HltSelReportsInputLocation", m_selReportsLocation = "" );

  // pass these to the TisTos tool
  declareProperty("ProjectTracksToCalo", m_projectTracksToCalo=true );  
  declareProperty("CaloClustForCharged", m_caloClustForCharged=true );
  declareProperty("CaloClustForNeutral", m_caloClustForNeutral=true );
   
  declareProperty("CompositeTPSviaPartialTOSonly", m_compositeTPSviaPartialTOSonly=false );

  declareProperty("NoHitTypes", m_noHitTypes = false);
  // if empty use defaults (vector of pairs <kHitType,value>) type 0=Velo,1=T,2=TT,3=Mu,4=Ecal,5=Hcal
  declareProperty("TOSFrac",m_TOSFrac);
  declareProperty("TISFrac",m_TISFrac);
  

}
//=============================================================================
// Destructor
//=============================================================================
TisTosParticleTagger::~TisTosParticleTagger() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TisTosParticleTagger::initialize() {
  StatusCode sc = DVAlgorithm::initialize(); 
  if ( sc.isFailure() ) return sc;

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  if( m_TriggerTisTosName != "" ){
    // Specific TriggerTisTos tool 
    verbose() << " Allocating TriggerTisTos tool specified by user=" << m_TriggerTisTosName << endmsg;
    m_tistostool = tool<ITriggerTisTos>(  m_TriggerTisTosName, m_TriggerTisTosName+"Tool" ,this); 
    if( m_TriggerTisTosName.find("TES") == 0 ){      
      m_checkDecReport=false;
      m_checkSelReport=false;  
    } else {
      m_checkDecReport=true;
      if( m_TriggerTisTosName.find("L0") == 0 ){
        m_decReportLoc="HltLikeL0/DecReports";
        m_checkSelReport=true;
        m_selReportLoc="HltLikeL0/SelReports";
      } else {
        m_decReportLoc=HltDecReportsLocation::Default;
        m_checkSelReport=false;
      }
    }
  } else if( onOffline() ){
    if( onOffline()->online() ){
      verbose() << " Allocating TriggerTisTosInHlt tool " << endmsg;
      m_tistostool = tool<ITriggerTisTos>(  "TriggerTisTosInHlt" , "TriggerTisTosInHltTool" ,this);
      m_checkDecReport=true;
      m_decReportLoc=HltDecReportsLocation::Default;
      m_checkSelReport=false;  
    } else {
      verbose() << " Allocating TriggerTisTos tool " << endmsg;
      m_tistostool = tool<ITriggerTisTos>(  "TriggerTisTos" , "TriggerTisTosTool" ,this);
      m_checkDecReport=true;
      m_decReportLoc=HltDecReportsLocation::Default;
      m_checkSelReport=true;  
      m_selReportLoc=HltSelReportsLocation::Default;
    }
  } else {
    verbose() << " Allocating TriggerTisTos tool " << endmsg;
    m_tistostool = tool<ITriggerTisTos>(  "TriggerTisTos" , "TriggerTisTosTool" ,this);
    m_checkDecReport=true;
    m_decReportLoc=HltDecReportsLocation::Default;
    m_checkSelReport=true;  
    m_selReportLoc=HltSelReportsLocation::Default;
  }
  if( !m_tistostool )return Error("Could not allocate TriggerTisTos tool", StatusCode::FAILURE);  


  if( m_decReportsLocation != "" ){
    if( m_decReportsLocation == "None" ){
      m_checkDecReport=false;
    } else {
      m_checkDecReport=true;
      m_decReportLoc=m_decReportsLocation;
    }
  }
  if( m_selReportsLocation != "" ){
    if( m_selReportsLocation == "None" ){
      m_checkSelReport=false;
    } else {
      m_checkSelReport=true;
      m_selReportLoc=m_selReportsLocation;
    }
  }
  


  m_tusSpecs.clear();
  m_tosSpecs.clear();
  m_tisSpecs.clear();  
  if( m_tistosSpecs.empty() ){
    error() <<
    "TisTosSpecs not set - example: = { 'Hlt1.*Hadron.*Decision%TUS':12345, 'Hlt1.*Mu.*Decision%TOS':12346 }"
            << endmsg;
    return StatusCode::FAILURE;
  } else {
    for ( std::map<std::string,int>::const_iterator entry = m_tistosSpecs.begin(); entry != m_tistosSpecs.end(); ++entry ) {
      const std::string& spec = entry->first  ;
      std::size_t i = spec.find("%");      
      if( i == std::string::npos )
      {
        error() << " TisTosSpec contains no InputTrigger-spec separator" << spec << endmsg;
        return StatusCode::FAILURE;
      } else {
        std::string triggerInput = spec.substr(0,i);
        std::string tisTosSpec = spec.substr(i+1);
        if( tisTosSpec == "TUS" || tisTosSpec == "tus" ){
          m_tusSpecs.insert( std::pair<std::string,int>( triggerInput,entry->second ) );
        } else if( tisTosSpec == "TOS" || tisTosSpec == "tos" ){
          m_tosSpecs.insert( std::pair<std::string,int>( triggerInput,entry->second ) );
        } else if( tisTosSpec == "TIS" || tisTosSpec == "tis" ){
          m_tisSpecs.insert( std::pair<std::string,int>( triggerInput,entry->second ) );
        } else {
          error() << " unrecognized TisTos spec in " << spec << endmsg;
          return StatusCode::FAILURE;
        }
      }
    }
  }
  if( m_tusSpecs.empty() && m_tosSpecs.empty() && m_tisSpecs.empty() ){
    error() << "No TisTos specs definied " << endmsg;
    return StatusCode::FAILURE;
  }

  // configure TisTos tool  
  m_tistostool->setProjectTracksToCalo(m_projectTracksToCalo);
  m_tistostool->setCaloClustForCharged(m_caloClustForCharged);
  m_tistostool->setCaloClustForNeutral(m_caloClustForNeutral);
  m_tistostool->setCompositeTPSviaPartialTOSonly(m_compositeTPSviaPartialTOSonly);
  m_tistostool->setNoHitTypes(m_noHitTypes);
  for( std::map<int,double>::const_iterator entry=m_TOSFrac.begin();entry!=m_TOSFrac.end();++entry){
    m_tistostool->setTOSFrac( entry->first, entry->second );
  }
  for( std::map<int,double>::const_iterator entry=m_TISFrac.begin();entry!=m_TISFrac.end();++entry){
    m_tistostool->setTISFrac( entry->first, entry->second );
  }

  if ( msgLevel(MSG::DEBUG) ) {
    if( m_noRegex ){
      debug() << "No regex expressions in trigger names allowed - regex patterns will be ignored " << endmsg;
    }    
    debug() << " Registered TUS conditions size=" << m_tusSpecs.size() ;    
    for ( std::map<std::string,int>::const_iterator entry=m_tusSpecs.begin();entry!= m_tusSpecs.end();++entry){
      debug() << " , " << entry->first << " " << entry->second ;
    }
    debug() << endmsg;
    debug() << " Registered TOS conditions size=" << m_tosSpecs.size() ;    
    for ( std::map<std::string,int>::const_iterator entry=m_tosSpecs.begin();entry!= m_tosSpecs.end();++entry){
      debug() << " , " << entry->first << " " << entry->second ;
    }
    debug() << endmsg;
    debug() << " Registered TIS conditions size=" << m_tisSpecs.size() ;    
    for ( std::map<std::string,int>::const_iterator entry=m_tisSpecs.begin();entry!= m_tisSpecs.end();++entry){
      debug() << " , " << entry->first << " " << entry->second ;
    }          
    debug() << endmsg;
    if( m_passOnAll ){
      debug() << " always setFilterPassed(true); pass all input particles to output " << endmsg;
    } else {
      debug() << " setFilterPassed(true) only if at least one condition satisfied by at least one particle " << endmsg;
      debug() << " Pass to output only particles which satisfied at least one condition. " << endmsg;
    }
    if( m_checkDecReport ) debug() << " We will check for presence of HltDecReports at " + m_decReportLoc << endmsg;
    if( m_checkSelReport ) debug() << " We will check for presence of HltSelReports at " + m_selReportLoc << endmsg;    
  }

  return StatusCode::SUCCESS;

}


//=============================================================================
// Main execution
//=============================================================================
StatusCode TisTosParticleTagger::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  //Get the particles from the local storage
  const LHCb::Particle::ConstVector& particles = this->i_particles();


  // Useful for MC: check if necessary reports exists
  if( m_checkDecReport ){
    if( !exist<LHCb::HltDecReports>(m_decReportLoc) ){
      setFilterPassed(m_passOnAll);  // Mandatory. Set to true if event is accepted. 
      return StatusCode::SUCCESS;
    }
  }
  if( m_checkSelReport ){
    if( !exist<LHCb::HltSelReports>(m_selReportLoc) ){
      setFilterPassed(m_passOnAll);  // Mandatory. Set to true if event is accepted. 
      return StatusCode::SUCCESS;
    }
  }

  std::vector<LHCb::Particle> outparts;

 
  bool passed(m_passOnAll);

  for (LHCb::Particle::ConstVector::const_iterator particle = particles.begin(); particle != particles.end(); ++particle) {
    if( !particleOK(*particle) )continue;          
    Particle* candi = const_cast<Particle*>(*particle);    
    // This is a slow part of TisTos tool - do it only once
    m_tistostool->setOfflineInput(*candi);

    bool accept(false);    
    if( m_passOnAll ){
      accept=true;
    }    
    for ( std::map<std::string,int>::const_iterator entry = m_tusSpecs.begin(); entry != m_tusSpecs.end(); ++entry ) {
      const std::string& triggerInput = entry->first  ;
      const int          index    = entry->second ;      
      bool acc;      
      if( m_noRegex) {
        acc = m_tistostool->tusSelection( triggerInput );
      } else {
        m_tistostool->setTriggerInput(triggerInput);
        acc=m_tistostool->tusTrigger();
      }
      if( acc ){
        accept =true;
      }
      if ( msgLevel(MSG::DEBUG) ) debug() << triggerInput + " TUS " << acc << endmsg;      
      if( index )if( !storeTag(candi,index,acc) ){
        std::ostringstream mess;
        mess << triggerInput + " TUS index=" << index << " used for other types or illegal " << endmsg;
        Warning( mess.str(),StatusCode::SUCCESS, 10 ).setChecked();
      }
    }
    for ( std::map<std::string,int>::const_iterator entry = m_tosSpecs.begin(); entry != m_tosSpecs.end(); ++entry ) {
      const std::string& triggerInput = entry->first  ;
      const int          index    = entry->second ; 
      bool acc;      
      if( m_noRegex) {
        acc = m_tistostool->tosSelection( triggerInput );
      } else {
        m_tistostool->setTriggerInput(triggerInput);
        acc=m_tistostool->tosTrigger();
      }
      if( acc ){
        accept =true;
      }
      if ( msgLevel(MSG::DEBUG) ) debug() << triggerInput + " TOS " << acc << endmsg;      
      if( index )if( !storeTag(candi,index,acc) ){
        std::ostringstream mess;
        mess << triggerInput + " TOS index=" << index << " used for other types or illegal " << endmsg;
        Warning( mess.str(),StatusCode::SUCCESS, 10 ).setChecked();
      }
    }
    for ( std::map<std::string,int>::const_iterator entry = m_tisSpecs.begin(); entry != m_tisSpecs.end(); ++entry ) {
      const std::string& triggerInput = entry->first  ;
      const int          index    = entry->second ; 
      bool acc;      
      if( m_noRegex) {
        acc = m_tistostool->tisSelection( triggerInput );
      } else {
        m_tistostool->setTriggerInput(triggerInput);
        acc=m_tistostool->tisTrigger();
      }
      if( acc ){
        accept =true;
      }
      if ( msgLevel(MSG::DEBUG) ) debug() << triggerInput + " TIS " << acc << endmsg;      
      if( index )if( !storeTag(candi,index,acc) ){
        std::ostringstream mess;
        mess << triggerInput + " TIS index=" << index << " used for other types or illegal " << endmsg;
        Warning( mess.str(),StatusCode::SUCCESS, 10 ).setChecked();
      }
    }
    if( accept  ){
      outparts.push_back(*candi);
      passed = true;      
    }     
  }

  for( std::vector<LHCb::Particle>::iterator i = outparts.begin(); i != outparts.end(); i++ ) this->markTree( &(*i) );

   
  setFilterPassed(passed);  // Mandatory. Set to true if event is accepted. 

  return StatusCode::SUCCESS;
}


bool TisTosParticleTagger::particleOK(const Particle* po) 
{
   if( !po )return false;
   const Particle & p = *po;   
   //if( !(p.endVertex()) )return false;
   const SmartRefVector<Particle> daughters = p.daughters();
   if( daughters.size() ){
     for( SmartRefVector<Particle>::const_iterator pp=daughters.begin(); pp!=daughters.end(); ++pp){
       if( !(pp->target()) )return false;
       const Particle* ppp= pp->target();
       if( !ppp )return false;
       if( !particleOK(ppp) )return false;
     }
   } else {
     if( !(p.proto()) )return false;
   }
   return true;   
   
}

bool TisTosParticleTagger::storeTag(Particle* po, int index,bool passedCondition) 
{
   if( !po )return false;
   double value = (passedCondition ? 1.0: 0.0);
   double svalue = po->info(index,-1.0);   
   if( svalue == -1.0 ){
     return po->addInfo(index,value);
   } else if( passedCondition ){
     if( svalue == 0.0 ){
       po->eraseInfo(index);
       return po->addInfo(index,value);
     } else if( svalue == value ){
       return true;
     } else {
       return false;
     }
   } else if(  svalue == value ){
       return true;
     } else {
       return false;
   }
}




//=============================================================================
//  Finalize
//=============================================================================
StatusCode TisTosParticleTagger::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return DVAlgorithm::finalize();
}

//=============================================================================
