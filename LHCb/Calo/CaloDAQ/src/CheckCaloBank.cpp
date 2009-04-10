// $Id: 


// Gaudi
#include "GaudiKernel/AlgFactory.h"
// local
#include "CheckCaloBank.h"

// ============================================================================
/** @file CheckCaloBank.cpp
 *
 *    @author: Olivier Deschamps
 *    @date:   O6 Septembre 2006
 */
// ============================================================================

// MANDATORY!!!
DECLARE_ALGORITHM_FACTORY( CheckCaloBank );

//=============================================================================
// Standard creator, initializes variables
//=============================================================================
CheckCaloBank::CheckCaloBank( const std::string& name, ISvcLocator* pSvcLocator)
  : GaudiAlgorithm       ( name , pSvcLocator            )
  , m_trig(LHCb::RawBank::LastType)
  , m_bank(LHCb::RawBank::LastType)
  , m_typ("Invalid")
{
  if( "Prs" == name.substr( 0 , 3 ) ){
    m_trig = LHCb::RawBank::PrsTrig;
    m_bank = LHCb::RawBank::PrsE;
    m_typ  = "Prs";
  }else if( "Ecal" == name.substr( 0 , 4 ) ){
    m_trig = LHCb::RawBank::EcalTrig;
    m_bank = LHCb::RawBank::EcalE;
    m_typ  = "Ecal";  
  }else if( "Hcal" == name.substr( 0 , 4 ) ){
    m_trig = LHCb::RawBank::HcalTrig;
    m_bank = LHCb::RawBank::HcalE;
    m_typ  = "Hcal";  
  }

};

//=============================================================================
// Standard destructor
//=============================================================================
CheckCaloBank::~CheckCaloBank() {};

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode CheckCaloBank::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if( sc.isFailure() ) return sc;  

  if( "Prs" != m_typ && "Ecal" != m_typ && "Hcal" != m_typ ){
    error() << "Undefined Calo type : " << m_typ << endreq;    
    return StatusCode::FAILURE;
  }else{
    info() << " Check the 0-suppressed banks exist for " << m_typ << endreq;
    info() << " if not (MC) re-create them from packed bank and delete packed " << endreq;
  }
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode CheckCaloBank::execute() {

  LHCb::RawEvent* rawEvt = get<LHCb::RawEvent> ( LHCb::RawEventLocation::Default );
  
  bool trig = ( 0 == (rawEvt->banks( m_trig )).size() ) ?  false : true ;
  bool bank = ( 0 == (rawEvt->banks( m_bank )).size() ) ?  false : true ;
  
  debug() << "TYPE of Calo Bank to be checked : " << m_typ << " " << trig << " " << bank <<endreq;
  if(trig ||  bank ){
    debug() << " Calo Banks with 'compressed' format exist - Do nothing" << endreq;
  }else{
    debug() << " Calo Banks with 'compressed' format DOESN'T exist - Will be created from PACKED banks" << endreq;
  }
  
  if(trig && bank)setFilterPassed(false);// no need to rebuilt 'compressed' bank
  else if(!trig && !bank)setFilterPassed(true); // re-Built compressed banks and (delete packed bank)
  else {
    warning() << " Compressed bank incomplete !! Do nothing" << endreq;    
    setFilterPassed(false);
  }
  
  return StatusCode::SUCCESS;
};

//=============================================================================
