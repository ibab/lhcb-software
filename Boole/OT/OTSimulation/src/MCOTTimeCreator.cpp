// $Id: MCOTTimeCreator.cpp,v 1.4 2004-12-10 08:09:13 jnardull Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"

// OTDAQ
#include "OTDAQ/IOTReadOutWindow.h"

// local
#include "MCOTTimeCreator.h"

static const AlgFactory<MCOTTimeCreator> s_Factory;
const IAlgFactory& MCOTTimeCreatorFactory = s_Factory;

/** @file MCOTTimeCreator.cpp
 *
 *  Implementation of MCOTimeCreator
 *  
 *  @author J. Nardulli and J. van Tilburg 
 *  @date   10/6/2004
 */

MCOTTimeCreator::MCOTTimeCreator(const std::string& name, 
                                   ISvcLocator* pSvcLocator) :
  GaudiAlgorithm(name, pSvcLocator),
  m_tempTimeCont(0)
{
  // constructor 
  // jobOptions - defaults
  declareProperty("deadTime", m_deadTime = 50.0*ns);
  declareProperty("countsPerBX", m_countsPerBX = 64);
  declareProperty("numberOfBX", m_numberOfBX = 2);
  declareProperty("timePerBX", m_timePerBX = 25*ns);

  // container for temporary time storage 
  m_tempTimeCont = new MCOTTimeVector();

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
  IOTReadOutWindow* aReadOutWindow = 0;
  aReadOutWindow = tool<IOTReadOutWindow>("OTReadOutWindow");
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
  msg () << "Time size before =" << timeCont->size() << endreq;
  sc = createTimes( timeCont );
  if (sc.isFailure()){
    return Error ("problems applying dead time",sc);
  }  
  msg () <<"Time size after dead time="<< timeCont->size() << endreq;
  return StatusCode::SUCCESS;
}

StatusCode MCOTTimeCreator::createTimes( MCOTTimes* times )
{
  // retrieve deposits
  MCOTDeposits* depositCont = 
    get<MCOTDeposits>(MCOTDepositLocation::Default);

  MCOTDeposits::const_iterator iterDep = depositCont->begin();
  MCOTDeposits::const_iterator jterDep = iterDep;

  // Multiple Hit Useful Definitions
  OTChannelID newChannel;
  bool multipleHit = false;
  int newTdc = 999999;

  // apply dead time
  while (iterDep != depositCont->end()){
    SmartRefVector<MCOTDeposit> depositVector;
    
    do {
      depositVector.push_back(*jterDep);
      jterDep++;
    } while ( (jterDep != depositCont->end()) && 
              ( keepAdding(*iterDep, *jterDep) == true) );

    // Calculate TDC-time
    int tdcTime = this->calculateTDCTime( *iterDep );

    // Apply read out window
    if ( insideReadOutWindow( tdcTime ) ) {

      // Get OTChannelID  
      OTChannelID channel = (*iterDep)->channel();
      
      // Multiple Hit Check, before adding the tdcTime
      if(newChannel == channel) multipleHit = true;
      newChannel = channel;

      // Multiple Hit Checks !
      if(multipleHit == true ){ 
        if(tdcTime == newTdc){
          tdcTime ++;   // We add 1 to the value of the TdcTime
        }
      }
      // Add time to OTChannelID  
      channel.setTdcTime( tdcTime );

      //Copy of the Tdc Time Value
      newTdc = tdcTime;

      // make a new MCOTTime and add it to the vector !!!!
      MCOTTime* newTime = new MCOTTime( channel, depositVector);
      times->insert(newTime);
    }

    multipleHit = false; 
    iterDep = jterDep;
  } // iterDep

  return StatusCode::SUCCESS;
}


bool MCOTTimeCreator::keepAdding( const MCOTDeposit* firstDep,
                                  const MCOTDeposit* secondDep) const 
{ 
  // check whether to continue adding deposits
  if ( firstDep->channel() == secondDep->channel() && 
       secondDep->time() - firstDep->time() <= m_deadTime ) {
    return true;
  }
  return false;
}


int MCOTTimeCreator::calculateTDCTime( MCOTDeposit* firstDeposit )
{
  // center around zero
  unsigned stationNum = ( (firstDeposit)->channel() ).station();
  double tdcTime =  firstDeposit->time() - m_startReadOutGate[stationNum-1] ;

  // Conversion to TDC counts
  tdcTime *=  double(m_countsPerBX) / m_timePerBX ;
  
  return (int) tdcTime;
}

bool MCOTTimeCreator::insideReadOutWindow( int tdcTime )
{
  if ( tdcTime < (m_countsPerBX * m_numberOfBX) && tdcTime >= 0 ) {
    return true;
  }
  return false;
}

