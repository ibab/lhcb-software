// $Id: RawEventSelectiveCopy.cpp,v 1.1 2009/06/22 15:12:04 tskwarni Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

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

  declareProperty("RawBanksToCopy",m_banksToCopy);
  declareProperty("InputRawEventLocation",
    m_inputLocation =  RawEventLocation::Default );  
  declareProperty("OutputRawEventLocation",
    m_outputLocation = RawEventLocation::Copied );

}
//=============================================================================
// Destructor
//=============================================================================
RawEventSelectiveCopy::~RawEventSelectiveCopy() {};

StatusCode RawEventSelectiveCopy::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;


  // convert bankNames to bankTypes
  m_bankTypes.clear();  
  const std::vector<std::string>& theNameVector = m_banksToCopy.value();

  // all banks?
  if( theNameVector.size()==1 ){
    if( theNameVector[0] == "all" || theNameVector[0] == "All" || theNameVector[0] == "ALL" ){
      for(int i = 0 ; i != (int) RawBank::LastType; i++){
        m_bankTypes.push_back( (RawBank::BankType) i );
      }
      verbose() << " All RawBank types will be copied from input to output RawEvent " << endmsg;      
      return sc;      
    }
  }
  
  // selective banks
  for( std::vector<std::string>::const_iterator bankName = theNameVector.begin();
       bankName != theNameVector.end(); ++bankName ){
    bool found = false;
    for(int i = 0 ; i != (int) RawBank::LastType; i++){
      const std::string name = RawBank::typeName( (RawBank::BankType) i );
      if( name == *bankName){
        found = true;
        m_bankTypes.push_back( (RawBank::BankType) i );        
        break;        
      }
    }
    if( !found) warning() << "Requested bank '" << *bankName << "' is not a valid name" << endmsg;
  }
  if( msgLevel(MSG::VERBOSE) ){
      verbose() << " RawBank types to be copied= ";
    for( std::vector<RawBank::BankType>::const_iterator ib = m_bankTypes.begin();
         ib!=m_bankTypes.end();++ib){
      verbose() << RawBank::typeName( *ib ) << " ";
    }
    verbose() << endmsg;
  }

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode RawEventSelectiveCopy::execute() {

  debug() << "==> Execute" << endmsg;

  // get input RawEvent
  if( !exist<RawEvent>(m_inputLocation.value()) ){    
    return Error(" No RawEvent at " + m_inputLocation.value(),StatusCode::SUCCESS, 20 );
  }  
  RawEvent* rawEvent = get<RawEvent>(m_inputLocation.value());


  // create empty output RawEvent
  RawEvent* rawEventCopy = new RawEvent();

  // copy selected banks
  for( std::vector<RawBank::BankType>::const_iterator ib = m_bankTypes.begin();
       ib!=m_bankTypes.end();++ib){

       const std::vector<RawBank*> banks= rawEvent->banks( *ib );
       for( std::vector<RawBank*>::const_iterator b=banks.begin();b!=banks.end();++b){
         
         const RawBank & bank = **b;         
         rawEventCopy->adoptBank( rawEventCopy->createBank( bank.sourceID(), *ib, bank.version(), bank.size(),bank.data() ), 
                                  true );
         if( msgLevel(MSG::VERBOSE) ){
           verbose() << " Copied RawBank type=" << RawBank::typeName( *ib )
                     << " version= " << bank.version()
                     << " sourceID= " << bank.sourceID()  
                     << " size (bytes) = " << bank.size()
                     << endmsg;
         }         
         
       }
  }
  
  // put output RawEvent into its location
  if( msgLevel(MSG::VERBOSE) ){ verbose() << " Saving Copied RawEvent into new locations " << endmsg;  }  
  put( rawEventCopy, m_outputLocation.value() );

  return StatusCode::SUCCESS;
}

//=============================================================================
