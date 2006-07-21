// $Id: MCOTTimeCreator.cpp,v 1.12 2006-07-21 08:05:07 janos Exp $

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

using namespace LHCb;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MCOTTimeCreator );

MCOTTimeCreator::MCOTTimeCreator(const std::string& name, 
				 ISvcLocator* pSvcLocator) :
  GaudiAlgorithm(name, pSvcLocator)
{
  // constructor 
  // jobOptions - defaults
  /// Analog dead time is 25 ns. Digital dead time is 64*3 tdc counts
  declareProperty("DeadTime", m_deadTime = 25.0*Gaudi::Units::ns);
  declareProperty("countsPerBX", m_countsPerBX = 64);
  declareProperty("numberOfBX", m_numberOfBX = 3);
  declareProperty("timePerBX", m_timePerBX = 25.0*Gaudi::Units::ns);
  declareProperty("singleHitMode", m_singleHitMode = true);
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
  m_startReadOutGate  = aReadOutWindow->startReadOutGate();
  release( aReadOutWindow );
  return StatusCode::SUCCESS;
}


StatusCode MCOTTimeCreator::execute()
{
  // execute once per event
  StatusCode sc;

  // output container
  MCOTTimes* timeCont = new MCOTTimes();

  // create times
  debug() << "Time size before =" << timeCont->size() << endmsg;
  sc = createTimes( timeCont );
  if (sc.isFailure()) return Error("problems applying dead time", sc);
  debug() <<"Time size after dead time="<< timeCont->size() << endmsg;
  
  put(timeCont, MCOTTimeLocation::Default);

  return StatusCode::SUCCESS;
}

StatusCode MCOTTimeCreator::createTimes( MCOTTimes* times )
{
  // retrieve deposits
  MCOTDeposits* depositCont = get<MCOTDeposits>(MCOTDepositLocation::Default);

  /// This only works because the deposits are sorted less by time and channel
  /// in MCOTDepositCreator.
  MCOTDeposits::const_iterator iterDep = depositCont->begin();
  MCOTDeposits::const_iterator jterDep = iterDep;
  // apply dead time - Analog deadtime
  while (iterDep != depositCont->end()){
    SmartRefVector<MCOTDeposit> depositVector;
    /// apply analog deadtime. Add first deposit to tmp deposit vector and 
    /// check whether consecutive deposits are inside the analog dead 
    /// time window. If so also add them to the tmp deposit vector.
    do {
      depositVector.push_back(*jterDep);
      ++jterDep;
    } while (jterDep != depositCont->end() && AnalogDeadTime(*iterDep, *jterDep));
    
    // Calculate TDC-time
    int tdcTime = calculateTDCTime( *iterDep );
    
    // Apply read out window
    if ( insideReadOutWindow( tdcTime ) ) {    
      /// apply digital dead time. If consecutive deposits are inside the digital
      /// dead time window add them to the temp deposit vector (in singlehit mode).
      if (m_singleHitMode) {
        while (jterDep != depositCont->end() && DigitalDeadTime(*iterDep, *jterDep)) {
          depositVector.push_back( *jterDep );
          ++jterDep;
        }
      }
      
      // Get OTChannelID  
      OTChannelID channel = (*iterDep)->channel();
          
      // Add time to OTChannelID  
      channel.setTdcTime( tdcTime );
      
      // make a new MCOTTime and add it to the vector !!!!
      MCOTTime* newTime = new MCOTTime( channel, depositVector);
      times->insert(newTime);
    }

    iterDep = jterDep;
    
  } // iterDep

  return StatusCode::SUCCESS;
}

