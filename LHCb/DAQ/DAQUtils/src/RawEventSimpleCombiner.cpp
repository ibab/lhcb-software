// $Id: RawEventSimpleCombiner.cpp,v 1.1 2009/06/22 15:12:04 tskwarni Exp $
// Include files

// local
#include "Event/RawEvent.h"
#include "RawEventSimpleCombiner.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RawEventSimpleCombiner
//
// 2009-06-22 : Tomasz Skwarnicki
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(RawEventSimpleCombiner)

using namespace LHCb;
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RawEventSimpleCombiner::RawEventSimpleCombiner( const std::string& name,
                                            ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  ,m_banksToCopy(1,std::string("ALL"))
  ,m_inputLocations(0)
{

  declareProperty("RawBanksToCopy",m_banksToCopy);
  declareProperty("InputRawEventLocations", m_inputLocations );
  declareProperty("OutputRawEventLocation",
    m_outputLocation = RawEventLocation::Default );

}
//=============================================================================
// Destructor
//=============================================================================
RawEventSimpleCombiner::~RawEventSimpleCombiner() {}

StatusCode RawEventSimpleCombiner::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;


  // convert bankNames to bankTypes
  m_bankTypes.clear();
  //const std::vector<std::string>& theNameVector = m_banksToCopy.value();

  // all banks?
  if( m_banksToCopy.size()==1 ){
    if( m_banksToCopy[0] == "all" || m_banksToCopy[0] == "All" || m_banksToCopy[0] == "ALL" ){
      for(int i = 0 ; i != (int) RawBank::LastType; i++){
        m_bankTypes.push_back( (RawBank::BankType) i );
      }
      if ( msgLevel(MSG::VERBOSE) ) verbose() << " All RawBank types will be copied from input to output RawEvent " << endmsg;
      return sc;
    }
  }

  // selective banks
  for( std::vector<std::string>::const_iterator bankName = m_banksToCopy.begin();
       bankName != m_banksToCopy.end(); ++bankName ){
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
StatusCode RawEventSimpleCombiner::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  std::vector<RawEvent*> foundRawEvents(0);
  //const std::vector<std::string> & lookFor=m_inputLocations.value();

  // get input RawEvents
  for( std::vector<std::string>::const_iterator il=m_inputLocations.begin(); il!=m_inputLocations.end(); il++)
  {
    RawEvent* rawEvent =getIfExists<RawEvent>(*il); //try with RootInTes

    if( rawEvent==NULL )  //try without RootInTes
    {
      rawEvent = getIfExists<RawEvent>(*il, false);
    }

    if (rawEvent==NULL)
    {
      return Error(" No RawEvent at " + (*il),StatusCode::SUCCESS, 20 );
    }
    foundRawEvents.push_back(rawEvent);
  }



  // create empty output RawEvent
  RawEvent* rawEventCopy = new RawEvent();

  // Loop over locations I've found
  for (std::vector<RawEvent*>::const_iterator rawEvent=foundRawEvents.begin(); rawEvent!=foundRawEvents.end(); rawEvent++)
  {
    //copy selected raw banks
    for( std::vector<RawBank::BankType>::const_iterator ib = m_bankTypes.begin();ib!=m_bankTypes.end();++ib)
    {

      const std::vector<RawBank*> banks= (*rawEvent)->banks( *ib );
      for( std::vector<RawBank*>::const_iterator b=banks.begin();b!=banks.end();++b)
      {
        if (*b == NULL) continue;

        //const RawBank & bank = **b;
        rawEventCopy->adoptBank( rawEventCopy->createBank( (*b)->sourceID(), *ib, (*b)->version(), (*b)->size(),(*b)->data() ), true );
        if( msgLevel(MSG::VERBOSE) )
        {
          verbose() << " Copied RawBank type=" << RawBank::typeName( *ib )
                    << " version= " << (*b)->version()
                    << " sourceID= " << (*b)->sourceID()
                    << " size (bytes) = " << (*b)->size()
                    << endmsg;
        }

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
