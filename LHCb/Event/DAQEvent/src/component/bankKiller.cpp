// $Id: bankKiller.cpp,v 1.5 2008-01-23 18:24:25 odescham Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"
// local
#include "bankKiller.h"

// ============================================================================
/** @file bankKiller.cpp
 *
 *    @author: Olivier Deschamps
 *    @date:   O6 Septembre 2006
 */
// ============================================================================

DECLARE_ALGORITHM_FACTORY( bankKiller );

//=============================================================================
// Standard creator, initializes variables
//=============================================================================
bankKiller::bankKiller( const std::string& name, ISvcLocator* pSvcLocator)
  : GaudiAlgorithm       ( name , pSvcLocator            )
    ,m_bankTypes()
{


  declareProperty("BankTypes"   , m_bankTypes      ) ;

};

//=============================================================================
// Standard destructor
//=============================================================================
bankKiller::~bankKiller() {};

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode bankKiller::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if( sc.isFailure() ) return sc;
  
  if(m_bankTypes.size() == 0)warning() << "bankKiller : nothing to be removed " << endreq;
  
  for(std::vector< std::string >::iterator ityp = m_bankTypes.begin() ; 
      ityp!= m_bankTypes.end() ; ++ ityp ){
    info() << "bankKiller : all banks of type '" << *ityp << "' will be removed." <<endreq;
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode bankKiller::execute() {

  LHCb::RawEvent* rawEvt = get<LHCb::RawEvent> ( LHCb::RawEventLocation::Default );

  for(std::vector< std::string >::iterator ityp = m_bankTypes.begin() ; 
      ityp!= m_bankTypes.end() ; ++ ityp ){

    LHCb::RawBank::BankType bankType = LHCb::RawBank::LastType;
    for( unsigned int ibank = 0 ; ibank < (unsigned int) LHCb::RawBank::LastType ; ++ibank){
      bankType = (LHCb::RawBank::BankType) ibank;
      if( *ityp == LHCb::RawBank::typeName( bankType ) )break;
    }

    std::vector<LHCb::RawBank*> banks = rawEvt->banks( bankType );
    if( bankType == LHCb::RawBank::LastType || 0 == banks.size() ){
      std::stringstream s("");
      s<< " No bank of type '" << *ityp		<< "' has been found - nothing to be removed";
      Error(s.str()).ignore();
      continue;
    }

    debug() << "All banks of type '" << *ityp << "'  are to be removed - banks size =  " << banks.size() << endreq;


    for(std::vector<LHCb::RawBank*>::const_iterator itB = banks.begin() ; itB !=  banks.end() ; ++itB ) {

      bool success = rawEvt -> removeBank ( *itB );
      if( !success )warning() << "The bank " << * itB << " of type '" << *ityp << "' has not been found to be removed " << endreq;
      
    }
    debug() << "All banks of type '" << *ityp 
            << "' have been removed  - " 
            << " banks size is now : " << ( rawEvt->banks( bankType ) ).size() << endreq;
    
  }
  return StatusCode::SUCCESS;
};

//=============================================================================
