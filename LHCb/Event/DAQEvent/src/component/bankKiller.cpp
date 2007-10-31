// $Id: bankKiller.cpp,v 1.3 2007-10-31 14:33:41 odescham Exp $

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

  // Map to be completed if needed
  // Calo 'Online' banks
  m_bankMap["EcalPacked"]= LHCb::RawBank::EcalPacked ;
  m_bankMap["HcalPacked"]= LHCb::RawBank::HcalPacked ;
  m_bankMap["PrsPacked"]= LHCb::RawBank::PrsPacked ;
  // L0 stuff
  m_bankMap["L0Calo"]= LHCb::RawBank::L0Calo ;
  m_bankMap["L0Muon"]= LHCb::RawBank::L0Muon ;
  m_bankMap["L0PU"]= LHCb::RawBank::L0PU ;
  m_bankMap["L0DU"]= LHCb::RawBank::L0DU ;

  // default : Calo packed banks
  m_bankTypes.push_back( "EcalPacked" );
  m_bankTypes.push_back( "HcalPacked" );
  m_bankTypes.push_back( "PrsPacked"  );
  m_bankTypes.push_back( "L0DU"  );
  m_bankTypes.push_back( "L0Calo"  );
  m_bankTypes.push_back( "L0Muon"  );
  m_bankTypes.push_back( "L0PU"  );

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
  

  for(std::vector< std::string >::iterator ityp = m_bankTypes.begin() ; 
      ityp!= m_bankTypes.end() ; ++ ityp ){
    info() << "All banks of type '" << *ityp 
	   << "' (" << m_bankMap[*ityp] <<") will be removed " <<endreq;
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

    const std::vector<LHCb::RawBank*> banks = rawEvt->banks( m_bankMap[*ityp] );

    if( 0 == banks.size() ){
      warning() << " No bank of type '" << *ityp 
		<< "' ("<<  m_bankMap[*ityp] << ")  has been found - nothing to be removed"<< endreq;
      continue;
    }

    debug() << "All banks of type '" << *ityp 
		<< "' ("<<  m_bankMap[*ityp] << ")  are to be removed - banks size =  " << banks.size() << endreq;

    
    for(std::vector<LHCb::RawBank*>::const_iterator itB = banks.begin() ; itB !=  banks.end() ; ++itB ) {

      bool success = rawEvt -> removeBank ( *itB );

      if( !success )warning() << "The bank " << * itB << " of type '" << *ityp 
                              << "' ("<<  m_bankMap[*ityp] << ") has not been found to be removed " << endreq;
      
    }
    debug() << "All banks of type '" << *ityp 
            << "' ("<<  m_bankMap[*ityp] << ") have been removed  - " 
            << " banks size is now : " << ( rawEvt->banks( m_bankMap[*ityp] ) ).size() << endreq;

  }
  return StatusCode::SUCCESS;
};

//=============================================================================
