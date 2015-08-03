// $Id: RawEventMapCombiner.cpp,v 1.1 2009/06/22 15:12:04 tskwarni Exp $
// Include files

// local
#include "Event/RawEvent.h"
#include "RawEventMapCombiner.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RawEventMapCombiner
//
// 2009-06-22 : Tomasz Skwarnicki
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(RawEventMapCombiner)

using namespace LHCb;
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RawEventMapCombiner::RawEventMapCombiner( const std::string& name,
                                            ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  ,m_banksToCopy()
{

  declareProperty("RawBanksToCopy",m_banksToCopy);
  declareProperty("OutputRawEventLocation",
    m_outputLocation = RawEventLocation::Default );

}
//=============================================================================
// Destructor
//=============================================================================
RawEventMapCombiner::~RawEventMapCombiner() {}

StatusCode RawEventMapCombiner::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;


  // convert bankNames to bankTypes
  m_bankTypes.clear();
  //const std::vector<std::string>& theNameVector = m_banksToCopy.value();


  // selective banks
  for( std::map<std::string, std::string>::const_iterator aBank = m_banksToCopy.begin();
       aBank != m_banksToCopy.end(); ++aBank ){
    bool found = false;
    for(int i = 0 ; i != (int) RawBank::LastType; i++){
      const std::string name = RawBank::typeName( (RawBank::BankType) i );
      if( name == aBank->first){
        found = true;
        m_bankTypes[( (RawBank::BankType) i )]=aBank->second;
        break;
      }
    }
    if( !found) warning() << "Requested bank '" << aBank->first << "' is not a valid name" << endmsg;
  }
  if( msgLevel(MSG::VERBOSE) ){
      verbose() << " RawBank types to be copied= ";
    for( std::map<LHCb::RawBank::BankType, std::string>::const_iterator ib = m_bankTypes.begin();
         ib!=m_bankTypes.end();++ib)
    {
      verbose() << RawBank::typeName( ib->first ) << " from " << ib->second << " , ";
    }
    verbose() << endmsg;
  }

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode RawEventMapCombiner::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  std::map<LHCb::RawBank::BankType, RawEvent*> foundRawEvents; //translation of m_banksToCopy

  //std::vector<RawEvent*> foundRawEvents(0);
  //const std::vector<std::string> & lookFor=m_inputLocations.value();

  //find all the raw events
  for( std::map<LHCb::RawBank::BankType, std::string>::const_iterator ib = m_bankTypes.begin();
         ib!=m_bankTypes.end();++ib)
  {
    RawEvent* rawEvent =getIfExists<RawEvent>(ib->second);//try with RootInTes

    if( rawEvent==NULL )  //try without RootInTes
    {
      rawEvent = getIfExists<RawEvent>(ib->second, false);
    }

    if (rawEvent==NULL) //if still not found it's a problem
    {
      return Error(" No RawEvent at " + (ib->second),StatusCode::SUCCESS, 20 );
    }
    foundRawEvents[ib->first]=rawEvent;
  }

  // create empty output RawEvent
  RawEvent* rawEventCopy = new RawEvent();

  //copy selected raw banks
  for( std::map<LHCb::RawBank::BankType, RawEvent*>::const_iterator ib = foundRawEvents.begin();ib!=foundRawEvents.end();++ib)
  {

      const std::vector<RawBank*> banks= (ib->second)->banks( ib->first );
      for( std::vector<RawBank*>::const_iterator b=banks.begin();b!=banks.end();++b)
      {
        if (*b == NULL) continue;
        const RawBank & bank = **b;
        rawEventCopy->adoptBank( rawEventCopy->createBank( bank.sourceID(), ib->first, bank.version(), bank.size(),bank.data() ), true );
        if( msgLevel(MSG::VERBOSE) )
        {
          verbose() << " Copied RawBank type=" << RawBank::typeName( ib->first )
                    << " version= " << bank.version()
                    << " sourceID= " << bank.sourceID()
                    << " size (bytes) = " << bank.size()
                    << endmsg;
        }

      }
  }


  // put output RawEvent into its location
  if( msgLevel(MSG::VERBOSE) ){ verbose() << " Saving Copied RawEvent into new locations " << endmsg;  }
  put( rawEventCopy, m_outputLocation );
  if( msgLevel(MSG::VERBOSE) ){ verbose() << " Saved Copied RawEvent into new locations " << endmsg;  }

  return StatusCode::SUCCESS;
}

//=============================================================================
