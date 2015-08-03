// local
#include "bankKiller.h"

// ============================================================================
/** @file bankKiller.cpp
 *
 *    @author: Olivier Deschamps
 *    @date:   O6 September 2006
 */
// ============================================================================

DECLARE_ALGORITHM_FACTORY( bankKiller )

//=============================================================================
// Standard creator, initializes variables
//=============================================================================
bankKiller::bankKiller( const std::string& name, ISvcLocator* pSvcLocator)
  : Decoder::AlgBase       ( name , pSvcLocator            )
  , m_bankTypes()
  , m_rawEvt(0)
{
  m_rawEventLocations={LHCb::RawEventLocation::Default};
  initRawEventSearch();
  
  declareProperty("BankTypes"     , m_bankTypes, "List of bank names"    ) ;
  declareProperty("KillFromAll" , m_killFromAll = false, 
                  "Main behaviour switch. If false (default), kill only banks in the first location found in the search string. If false, kill *all* banks found in the search string." ) ;
  declareProperty("DefaultIsKill" , m_defaultIsKill = false, 
                  "Main behaviour switch. If false (default), kill only given banks. If true, kill all BUT given banks." ) ;
  

}

//=============================================================================
// Standard destructor
//=============================================================================
bankKiller::~bankKiller() {}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode bankKiller::initialize() {

  StatusCode sc = Decoder::AlgBase::initialize();
  if( sc.isFailure() ) return sc;
  
  if( m_defaultIsKill ) {
    if(m_bankTypes.empty()) warning() << "bankKiller : nothing specified to be kept - all will be removed " 
                                         << endmsg;
    
    for(std::vector< std::string >::iterator ityp = m_bankTypes.begin() ; 
        ityp!= m_bankTypes.end() ; ++ ityp ){
      always() << "bankKiller : all banks of type '" << *ityp << "' will be kept." <<endmsg;
    }
  } else {
    if(m_bankTypes.empty()) warning() << "bankKiller : nothing to be removed " << endmsg;
    
    for(std::vector< std::string >::iterator ityp = m_bankTypes.begin() ; 
        ityp!= m_bankTypes.end() ; ++ ityp ){
      always() << "bankKiller : all banks of type '" << *ityp << "' will be removed." <<endmsg;
    }
  }
  if (m_rawEventLocations.empty()) return Error("You didn't give me a list of RawEventLocations to look through",StatusCode::FAILURE);

  if (m_killFromAll) 
  {
    
    info() << "Killing banks from everything in '" << m_rawEventLocations <<"'" << endmsg;
  }
  else if (m_rawEventLocations.size()!=1  or m_rawEventLocations[0]!=LHCb::RawEventLocation::Default)
  {
    info() << "Killing banks from first raw event in '" << m_rawEventLocations <<"'" << endmsg;
  }
  
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode bankKiller::execute() {

  m_rawEvt = NULL;
  if (!m_killFromAll)
  {
    m_rawEvt = findFirstRawEvent();
    if( m_rawEvt == NULL ) return Error("Failed to find raw data");
    auto sc=killFromRawEvent();
    return sc;
  }
  
  for (auto loc : m_rawEventLocations) 
  {
    m_rawEvt = tryEventAt(loc);
    if( m_rawEvt != NULL ) 
    {
      auto sc=killFromRawEvent();
      if (sc.isFailure()) return sc;
    }
    
  }
  return StatusCode::SUCCESS;
  
}

StatusCode  bankKiller::killFromRawEvent()
{

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
        if( *ityp == LHCb::RawBank::typeName( bankType ) ){
          // kill all banks of this type
          killBankType( bankType, true );
          break;
        }
      }
    }
  }
  return StatusCode::SUCCESS;
}

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
      Warning(s.str(),StatusCode::SUCCESS,0).ignore();
    }
    return;
  }
  
  if (msgLevel(MSG::DEBUG)) debug() << "All banks of type '" << bankTypeName 
                                    << "'  are to be removed - banks size =  " << banks.size() << endmsg;
  
  
  for(std::vector<LHCb::RawBank*>::const_iterator itB = banks.begin() ; itB !=  banks.end() ; ++itB ) {
    
    bool success = m_rawEvt -> removeBank ( *itB );
    if( !success && warningmsg){
      if (msgLevel(MSG::DEBUG)) debug() << "The bank " << * itB << " of type '" << bankTypeName 
                                        << "' has not been found to be removed " << endmsg;
      Warning("The bank of type '"+bankTypeName+"' has not been found",StatusCode::SUCCESS,0).ignore();
    }
  }
  if (msgLevel(MSG::DEBUG)) debug() << "All banks of type '" << bankTypeName 
                                    << "' have been removed  - " 
                                    << " banks size is now : " << ( m_rawEvt->banks( bankType ) ).size() << endmsg;
  
}
