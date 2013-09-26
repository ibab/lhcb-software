// STD
#include <algorithm>

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// OTDAQ
#include "OTDAQ/IOTReadOutWindow.h"

// MCEvent
#include "Event/MCOTDeposit.h"
#include "Event/MCOTTime.h"

// local
#include "MCOTTimeCreator.h"

/** @file MCOTTimeCreator.cpp
 *
 *  Implementation of MCOTimeCreator
 *  
 *  @author J. Nardulli and J. van Tilburg 
 *  @date   10/6/2004
 */

namespace MCOTTimeHelpers {
  struct CompareDeposits {
    bool operator()( LHCb::MCOTDeposit* lhs, LHCb::MCOTDeposit* rhs ) {
      if ( ( lhs->channel() ).sameGeometry( rhs->channel() ) ) return lhs->time() < rhs->time();
      return lhs->channel().geometry() < rhs->channel().geometry();
    };
  };  
}


using namespace LHCb;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MCOTTimeCreator )

MCOTTimeCreator::MCOTTimeCreator(const std::string& name, 
                                 ISvcLocator* pSvcLocator) :
  GaudiAlgorithm(name, pSvcLocator),
  m_nsToTDCcounts(0.),
  m_readoutWindow(0.)
{
  // constructor 
  // jobOptions - defaults
  /// Analog dead time is 25 ns. Digital dead time is 64*3 tdc counts
  declareProperty("DeadTime"     , m_deadTime      = 25.0*Gaudi::Units::ns);
  declareProperty("countsPerBX"  , m_countsPerBX   = 64                   );
  declareProperty("numberOfBX"   , m_numberOfBX    = 3                    );
  declareProperty("timePerBX"    , m_timePerBX     = 25.0*Gaudi::Units::ns);
  declareProperty("singleHitMode", m_singleHitMode = true                 );
}

MCOTTimeCreator::~MCOTTimeCreator()
{
  // destructor
}

StatusCode MCOTTimeCreator::initialize()
{

  StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  // Read out window tool
  IOTReadOutWindow* aReadOutWindow = tool<IOTReadOutWindow>("OTReadOutWindow");
  m_startReadOutGate               = aReadOutWindow->startReadOutGate();
  release(aReadOutWindow);

  m_nsToTDCcounts = double(m_countsPerBX)/m_timePerBX;
  m_readoutWindow = m_countsPerBX * m_numberOfBX; /// = 192

  return StatusCode::SUCCESS;
}


StatusCode MCOTTimeCreator::execute()
{
  // execute once per event
  StatusCode sc;

  // output container
  MCOTTimes* timeCont = new MCOTTimes();

  // create times
  if (msgLevel(MSG::DEBUG)) debug() << "Size of MCOTTimes container before = " << timeCont->size() << endmsg;
  sc = createTimes(timeCont);
  if (sc.isFailure()) return Error("Failed to create MCOTTimes", sc);
  if (msgLevel(MSG::DEBUG)) debug() << "Size of MCOTTimes container before = " << timeCont->size() << endmsg;
  
  put(timeCont, MCOTTimeLocation::Default);

  return StatusCode::SUCCESS;
}

StatusCode MCOTTimeCreator::createTimes( MCOTTimes* times )
{
  // retrieve deposits
  MCOTDeposits* depositCont = get<MCOTDeposits>(MCOTDepositLocation::Default);

  /// This only works because the deposits are sorted less by time and channel
  /// in MCOTDepositCreator.

  /// Set tdc time of MCOTDeposit
  for ( MCOTDeposits::const_iterator dep = depositCont->begin(), depEnd = depositCont->end(); 
	dep != depEnd; ++dep ) {
    OTChannelID& channel = (*dep)->channel();
    const int tdcTime = calculateTDCTime( (*dep) );
    channel.setTdcTime( tdcTime );
  }

  /// OK, now sort the deposits according to time (not tdc time!!!) and channel
  std::sort( depositCont->begin(), depositCont->end(), MCOTTimeHelpers::CompareDeposits() );

  if (msgLevel(MSG::DEBUG)) {
    debug() << "Sorted channels are" << endmsg;
    for ( MCOTDeposits::const_iterator dep = depositCont->begin(), depEnd = depositCont->end(); 
	  dep != depEnd; ++dep ) {
      debug() << "time = " << (*dep)->time() << " channel = " << (*dep)->channel() << endmsg;
    }
  }

  MCOTDeposits::const_iterator iterDep     = depositCont->begin();
  MCOTDeposits::const_iterator jterDep     = iterDep;
  MCOTDeposits::const_iterator depositsEnd = depositCont->end();

  // apply dead time - Analog deadtime
  while (iterDep != depositsEnd){
    SmartRefVector<MCOTDeposit> depositVector;
    /// apply analog deadtime. Add first deposit to tmp deposit vector and 
    /// check whether consecutive deposits are inside the analog dead 
    /// time window of the first deposit. If so also add them to the tmp 
    /// deposit vector.
    do {
      depositVector.push_back((*jterDep));
      ++jterDep;
    } while (jterDep != depositsEnd && insideAnalogDeadTime((*iterDep), (*jterDep)));
    
    if (msgLevel(MSG::DEBUG)) {
      debug() << "Applied analog deadtime" << endmsg;
      debug() << "Pulse from " << (*iterDep)->channel() << "\n";
      debug() << "Number of deposits inside dead time: " << depositVector.size() <<"\n";
      debug() << "Channels are " << endmsg;
      for ( SmartRefVector<MCOTDeposit>::const_iterator dep = depositVector.begin(), depEnd = depositVector.end(); 
	    dep != depEnd; ++dep ) {
	debug() << "time = " << dep->target()->time() << " channel = " << dep->target()->channel() << endmsg;
      }
    }

    // Calculate TDC-time
    /// Need to check whether it falls inside the readout window of 2^3*3 = 192 == 75ns
    /// So we calculate it again
    const int tdcTime = calculateTDCTime( (*iterDep) );
    // Apply read out window
    if ( insideReadoutWindow( tdcTime ) ) {    
      /// apply digital dead time. If consecutive deposits are inside the digital
      /// dead time window add them to the temp deposit vector (in singlehit mode).
      if (m_singleHitMode) {
        while (jterDep != depositsEnd && insideDigitalDeadTime((*iterDep), (*jterDep))) {
          depositVector.push_back((*jterDep));
          ++jterDep;
        }
      }
     
      if (msgLevel(MSG::DEBUG)) {
	debug() << "Applied digital deadtime" << endmsg;
	debug() << "Pulse from " << (*iterDep)->channel() << "\n";
	debug() << "Number of deposits inside dead time: " << depositVector.size() <<"\n";
	debug() << "Channels are " << endmsg;
	for ( SmartRefVector<MCOTDeposit>::const_iterator dep = depositVector.begin(), depEnd = depositVector.end(); 
	      dep != depEnd; ++dep ) {
	  debug() << "time = " << dep->target()->time() << " channel = " << dep->target()->channel() << endmsg;
	}
      }
      
      // Get OTChannelID  
      const OTChannelID& channel = (*iterDep)->channel();
      // make a new MCOTTime and add it to the vector
      MCOTTime* newTime = new MCOTTime(channel, depositVector);
      times->insert(newTime);
    }

    /// Move on to the next deposit 
    iterDep = jterDep;

  } // iterDep

  return StatusCode::SUCCESS;
}

