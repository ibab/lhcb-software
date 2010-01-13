// $Id: bankKiller.cpp,v 1.8 2010/01/11 17:01:42 cattanem Exp $

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

  declareProperty("BankTypes"     , m_bankTypes      ) ;
  declareProperty("DefaultIsKill" , m_defaultIsKill = false  ) ;

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
  
  if( m_defaultIsKill ) {
    if(m_bankTypes.size() == 0)warning() << "bankKiller : nothing specified to be kept - all will be removed " 
					 << endmsg;
    
    for(std::vector< std::string >::iterator ityp = m_bankTypes.begin() ; 
	ityp!= m_bankTypes.end() ; ++ ityp ){
      always() << "bankKiller : all banks of type '" << *ityp << "' will be kept." <<endmsg;
    }
  } else {
    if(m_bankTypes.size() == 0)warning() << "bankKiller : nothing to be removed " << endmsg;
    
    for(std::vector< std::string >::iterator ityp = m_bankTypes.begin() ; 
	ityp!= m_bankTypes.end() ; ++ ityp ){
      always() << "bankKiller : all banks of type '" << *ityp << "' will be removed." <<endmsg;
    }
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode bankKiller::execute() {

  m_rawEvt = get<LHCb::RawEvent> ( LHCb::RawEventLocation::Default );
  if( m_defaultIsKill ) {
    for( unsigned int ibank = 0 ; ibank < (unsigned int) LHCb::RawBank::LastType ; ++ibank){
      LHCb::RawBank::BankType bankType = (LHCb::RawBank::BankType) ibank;
      bool keep = false;
      for(std::vector<std::string>::const_iterator ityp = m_bankTypes.begin(); 
	  ityp != m_bankTypes.end(); ++ityp ){
	if( *ityp == LHCb::RawBank::typeName( bankType ) ) {
	  keep = true;
	  break;
	}
      }
      if ( !keep ) {
	// kill al banks of this type
	killBankType( bankType, false );
      }
    }
  } else {
    for(std::vector<std::string>::const_iterator ityp = m_bankTypes.begin(); 
	ityp != m_bankTypes.end(); ++ityp ){
      
      LHCb::RawBank::BankType bankType = LHCb::RawBank::LastType;
      for( unsigned int ibank = 0 ; ibank < (unsigned int) LHCb::RawBank::LastType ; ++ibank){
	bankType = (LHCb::RawBank::BankType) ibank;
	if( *ityp == LHCb::RawBank::typeName( bankType ) )break;
      }
      // kill al banks of this type
      killBankType( bankType, true );
	}
  }
  return StatusCode::SUCCESS;
};

//=============================================================================

void  bankKiller::killBankType( LHCb::RawBank::BankType bankType, 
				bool warningmsg ) {
  const std::string bankTypeName = LHCb::RawBank::typeName( bankType );
  // look for all banks of this type and remove them 
  std::vector<LHCb::RawBank*> banks = m_rawEvt->banks( bankType );
  if( bankType == LHCb::RawBank::LastType || 0 == banks.size() ){
    if (warningmsg) {
      std::stringstream s("");
      s<< " No bank of type '" << bankTypeName		<< "' has been found - nothing to be removed";
      Error(s.str()).ignore();
    }
    return;
  }
  
  debug() << "All banks of type '" << bankTypeName << "'  are to be removed - banks size =  " << banks.size() << endmsg;

  
  for(std::vector<LHCb::RawBank*>::const_iterator itB = banks.begin() ; itB !=  banks.end() ; ++itB ) {
    
    bool success = m_rawEvt -> removeBank ( *itB );
    if( !success && warningmsg) 
      warning() << "The bank " << * itB << " of type '" << bankTypeName << "' has not been found to be removed " << endmsg;
    
  }
  debug() << "All banks of type '" << bankTypeName 
	  << "' have been removed  - " 
	  << " banks size is now : " << ( m_rawEvt->banks( bankType ) ).size() << endmsg;
  
};
