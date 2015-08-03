// Include files 

#include "Event/RawBankReadoutStatus.h"
#include "Event/RawBankReadoutStatus.h"
#include "Event/ProcStatus.h"
// local
#include "RawBankReadoutStatusConverter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RawBankReadoutStatusConverter
//
// 2012-03-30 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( RawBankReadoutStatusConverter )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RawBankReadoutStatusConverter::RawBankReadoutStatusConverter( const std::string& name,
                                                              ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_mask(0x0){
 declareProperty( "BankTypes" , m_types);
 declareProperty( "AbortStatus" , m_flags);
 declareProperty( "System"    , m_system="UNSET");
}
//=============================================================================
// Destructor 
//=============================================================================
RawBankReadoutStatusConverter::~RawBankReadoutStatusConverter() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode RawBankReadoutStatusConverter::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  // convert aborting status into mask
  m_mask = 0x0;
  for( std::vector<std::string>::iterator i = m_flags.begin() ; m_flags.end() != i ; ++ i){
    int word  = 1;
    while( word <= LHCb::RawBankReadoutStatus::Unknown ){      
      LHCb::RawBankReadoutStatus::Status stat = (LHCb::RawBankReadoutStatus::Status) word;
      std::ostringstream label("");
      label << stat;
      if( label.str() == *i)m_mask |= word;
      word *= 2;
    }
  }
  if ( msgLevel(MSG::DEBUG) )debug() << "Abort mask = " << m_mask << endmsg;
  if(m_types.empty())
    return Warning("No BankTypes requested in RawBankReadoutStatusConverter",StatusCode::SUCCESS);


  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode RawBankReadoutStatusConverter::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  if(m_types.empty())return StatusCode::SUCCESS;
  
  // Access RawBankReadoutStatus
  LHCb::RawBankReadoutStatuss* rStats = getIfExists<LHCb::RawBankReadoutStatuss>(LHCb::RawBankReadoutStatusLocation::Default);
  if( rStats == NULL || rStats->empty() ) return StatusCode::SUCCESS;  
  
  // Access procStatus
  LHCb::ProcStatus* pStat = getOrCreate<LHCb::ProcStatus,LHCb::ProcStatus>(LHCb::ProcStatusLocation::Default);

  // loop over rStats
  for(LHCb::RawBankReadoutStatuss::iterator it = rStats->begin() ; it != rStats->end() ; ++it ){    
    LHCb::RawBankReadoutStatus* rStat = *it;
    LHCb::RawBank::BankType type = rStat->key();
    std::string typeName = LHCb::RawBank::typeName( type );
    // loop over requested bank types
    for(std::vector<std::string>::iterator inam = m_types.begin() ; inam != m_types.end() ; ++inam){
      if( *inam != typeName)continue;
      int status = rStat->status();
      bool aborted = (status & m_mask) != 0;
      if ( msgLevel(MSG::DEBUG) ) debug() << m_system << " : " << status << " / " << m_mask  << " -> " << reason( status,typeName) 
                                          << " abort ? " << aborted << endmsg;
      if(status != LHCb::RawBankReadoutStatus::OK){
        pStat->addAlgorithmStatus(this->name(),m_system ,reason(status,typeName) , status , aborted);     
        counter(m_system + " readoutStatus : create a procStatus")+=1;
        if( aborted) counter(m_system + " readoutStatus : abort proccessing" )+=1;        
      }      
    }
  }
  return StatusCode::SUCCESS;
}

std::string RawBankReadoutStatusConverter::reason(int status,std::string typeName){
  std::ostringstream tag("");
  tag << "Bank = " << typeName << " - ReadoutStatus = " << status << " : | ";
  unsigned int word = 1;
  while( word <= LHCb::RawBankReadoutStatus::Unknown ){
    LHCb::RawBankReadoutStatus::Status stat = (LHCb::RawBankReadoutStatus::Status) word;
    if( (status & word) != 0 )tag << stat << " | ";
    word *= 2;
  }
  return tag.str();
}


//=============================================================================
//  Finalize
//=============================================================================
StatusCode RawBankReadoutStatusConverter::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
