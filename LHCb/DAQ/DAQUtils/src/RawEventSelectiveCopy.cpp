// Include files

// local
#include "Event/RawEvent.h"
#include "RawEventSelectiveCopy.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RawEventSelectiveCopy
//
// 2009-06-22 : Tomasz Skwarnicki
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(RawEventSelectiveCopy)

using namespace LHCb;
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RawEventSelectiveCopy::RawEventSelectiveCopy( const std::string& name,
                                            ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  
  declareProperty("RawBanksToCopy",   m_banksToCopy,  
                  "Create a new RawEvent copying only these banks");
  declareProperty("RawBanksToRemove", m_banksToRemove,
                  "Create a RawEvent copy, with these banks removed");
  declareProperty("InputRawEventLocation",
    m_inputLocation =  RawEventLocation::Default );  
  declareProperty("OutputRawEventLocation",
    m_outputLocation = RawEventLocation::Copied );

}
//=============================================================================
// Destructor
//=============================================================================
RawEventSelectiveCopy::~RawEventSelectiveCopy() {}

StatusCode RawEventSelectiveCopy::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  const std::vector<std::string>& copyVector   = m_banksToCopy.value();
  const std::vector<std::string>& removeVector = m_banksToRemove.value();

  if( copyVector.size() > 0 && removeVector.size() > 0 ) {
    return Error( "Inconsistent properties, set only one of RawBanksToCopy and RawBanksToRemove" );
  }
  
  m_bankTypes.clear();

  if( copyVector.size() > 0 ) {
    // all banks?
    if( copyVector.size()==1 ){
      if( copyVector[0] == "all" || 
          copyVector[0] == "All" || 
          copyVector[0] == "ALL" ){
        for(int i = 0 ; i != (int) RawBank::LastType; i++){
          m_bankTypes.push_back( (RawBank::BankType) i );
        }
        if ( msgLevel(MSG::VERBOSE) )
          verbose() << " All RawBank types will be copied from input to output RawEvent " << endmsg;      
        return sc;      
      }
    }
    // convert bankNames to bankTypes
    for( std::vector<std::string>::const_iterator bankName = copyVector.begin();
         bankName != copyVector.end(); ++bankName ){
      bool found = false;
      for(int i = 0 ; i != (int) RawBank::LastType; i++){
        const std::string name = RawBank::typeName( (RawBank::BankType) i );
        if( name == *bankName){
          found = true;
          m_bankTypes.push_back( (RawBank::BankType) i );        
          break;        
        }
      }
      if( !found) {
        error() << "Requested bank '" << *bankName << "' is not a valid name" << endmsg;
        return Error( "Invalid bank name requested" );
      }
    }
  }

  else if( removeVector.size() > 0 ) {
    // all banks?
    if( removeVector.size()==1 ){
      if( removeVector[0] == "all" ||
          removeVector[0] == "All" ||
          removeVector[0] == "ALL" ){
        return Error("Requested to remove ALL banks from copied event!" );
      }
    }

    // Check inputs
    for( std::vector<std::string>::const_iterator bankName = removeVector.begin();
         bankName != removeVector.end(); ++bankName ){
      bool found = false;
      for(int i = 0 ; i != (int) RawBank::LastType; i++){
        const std::string name = RawBank::typeName( (RawBank::BankType) i );
        if( name == *bankName){
          found = true;
          break;        
        }
      }
      if( !found) {
        error() << "Requested bank '" << *bankName << "' is not a valid name" << endmsg;
        return Error( "Invalid bank name requested" );
      }
    }

    // convert bankNames to bankTypes
    for(int i = 0 ; i != (int) RawBank::LastType; i++){
      const std::string name = RawBank::typeName( (RawBank::BankType) i );
      bool found = false;
      for( std::vector<std::string>::const_iterator bankName = removeVector.begin();
         bankName != removeVector.end(); ++bankName ){
        if( name == *bankName){
          found = true;
          break;        
        }
      }
      if( !found) m_bankTypes.push_back( (RawBank::BankType) i );
    }
  }

  else {
    return Warning( "Neither RawBanksToCopy nor RawBanksToRemove are set, algorithm has no effect", sc );
  }
  


  if( msgLevel(MSG::DEBUG) ){
    debug() << " RawBank types to be copied = ";
    for( std::vector<RawBank::BankType>::const_iterator ib = m_bankTypes.begin();
         ib!=m_bankTypes.end();++ib){
      debug() << RawBank::typeName( *ib ) << " ";
    }
    debug() << endmsg;
  }

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode RawEventSelectiveCopy::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // do nothing of output location already exists (e.g. reprocessing from (S)DST)
  if( exist<RawEvent>(m_outputLocation.value()) ){    
    return Warning(" Output location " + m_outputLocation.value() + " already exists, do nothing"
                   ,StatusCode::SUCCESS, 20 );
  }

  // get input RawEvent
  RawEvent* rawEvent = getIfExists<RawEvent>(m_inputLocation.value());
  if( rawEvent == NULL ){    
    return Error(" No RawEvent at " + m_inputLocation.value(),StatusCode::SUCCESS, 20 );
  }  


  // create empty output RawEvent
  RawEvent* rawEventCopy = new RawEvent();

  // copy selected banks
  for( std::vector<RawBank::BankType>::const_iterator ib = m_bankTypes.begin();
       ib!=m_bankTypes.end();++ib){

    const std::vector<RawBank*> banks= rawEvent->banks( *ib );
    if( banks.size() > 0 ) {
      for( std::vector<RawBank*>::const_iterator b=banks.begin();b!=banks.end();++b){
        const RawBank & bank = **b;         
        rawEventCopy->adoptBank( rawEventCopy->createBank( bank.sourceID(), *ib, bank.version(), bank.size(),bank.data() ),
                                 true );
        if( msgLevel(MSG::VERBOSE) ){
          verbose() << " Copied RawBank type= " << RawBank::typeName( *ib )
                    << " version= " << bank.version()
                    << " sourceID= " << bank.sourceID()  
                    << " size (bytes) = " << bank.size()
                    << endmsg;
        }         
      }
    }
    else
      if( msgLevel(MSG::VERBOSE) ){
        verbose() << " No banks found of type= " << RawBank::typeName( *ib ) << endmsg;
      }
  }
  

  // put output RawEvent into its location
  if( msgLevel(MSG::DEBUG) ){ debug() << " Saving Copied RawEvent into new locations " << endmsg;  }  
  put( rawEventCopy, m_outputLocation.value() );

  return StatusCode::SUCCESS;
}

//=============================================================================
