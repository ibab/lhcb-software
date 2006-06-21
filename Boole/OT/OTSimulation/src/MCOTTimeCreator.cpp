// $Id: MCOTTimeCreator.cpp,v 1.11 2006-06-21 14:36:29 janos Exp $

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
  GaudiAlgorithm(name, pSvcLocator),
  m_tempTimeCont(0)
{
  // constructor 
  // jobOptions - defaults
  declareProperty("DeadTime", m_deadTime = 25.0*Gaudi::Units::ns);
  declareProperty("countsPerBX", m_countsPerBX = 64);
  declareProperty("numberOfBX", m_numberOfBX = 3);
  declareProperty("timePerBX", m_timePerBX = 25*Gaudi::Units::ns);
  declareProperty("singleHitMode", m_singleHitMode = true);

  // container for temporary time storage 
  m_tempTimeCont = new MCOTTimeVec();

  // reserve some space
  m_tempTimeCont->reserve(8000);

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
  put(timeCont, MCOTTimeLocation::Default);

  // create times
  debug() << "Time size before =" << timeCont->size() << endmsg;
  sc = createTimes( timeCont );
  if (sc.isFailure()){
    return Error ("problems applying dead time",sc);
  }  
  debug() <<"Time size after dead time="<< timeCont->size() << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode MCOTTimeCreator::createTimes( MCOTTimes* times )
{
  // retrieve deposits
  MCOTDeposits* depositCont = get<MCOTDeposits>(MCOTDepositLocation::Default);

  MCOTDeposits::const_iterator iterDep = depositCont->begin();
  MCOTDeposits::const_iterator jterDep = iterDep;

  // apply dead time - Analog deadtime
  while (iterDep != depositCont->end()){
    SmartRefVector<MCOTDeposit> depositVector;
    do {
      depositVector.push_back(*jterDep);
      ++jterDep;
    } while (jterDep != depositCont->end() && AnalogDeadTime(*iterDep, *jterDep));
    
    // Calculate TDC-time
    int tdcTime = calculateTDCTime( *iterDep );
    
    // Apply read out window
    if ( insideReadOutWindow( tdcTime ) ) {    
      // Kill deposits in single hit mode (digital deadtime)
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


bool MCOTTimeCreator::AnalogDeadTime(const MCOTDeposit* firstDep,
                                     const MCOTDeposit* secondDep) const 
{ 
  // check whether to continue adding deposits
  return (firstDep->channel() == secondDep->channel() && 
          calculateTDCTime(secondDep) - 
          calculateTDCTime(firstDep) <= std::abs( m_deadTime ) );
}

bool MCOTTimeCreator::DigitalDeadTime(const MCOTDeposit* firstDep,
                                      const MCOTDeposit* secondDep) const 
{ 
  // check whether to continue killing deposits
  return (firstDep->channel() == secondDep->channel()  && 
          calculateTDCTime(secondDep) < (m_countsPerBX * m_numberOfBX));
}


int MCOTTimeCreator::calculateTDCTime(const MCOTDeposit* firstDeposit) const
{
  // center around zero
  unsigned stationNum = ( (firstDeposit)->channel() ).station();
  double tdcTime =  firstDeposit->time() - m_startReadOutGate[stationNum-1] ;

  // Conversion to TDC counts
  tdcTime *=  double(m_countsPerBX) / m_timePerBX ;
  
  return int(tdcTime);
}

inline bool MCOTTimeCreator::insideReadOutWindow( int tdcTime ) const
{
  return ( tdcTime < (m_countsPerBX * m_numberOfBX) && tdcTime >= 0 ) ;
}

