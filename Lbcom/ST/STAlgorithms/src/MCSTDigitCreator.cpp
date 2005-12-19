// $Id: MCSTDigitCreator.cpp,v 1.1.1.1 2005-12-19 15:43:15 mneedham Exp $
//
// This File contains the implementation of the MCSTDigitCreator
// C++ code for 'LHCb Tracking package(s)'
//

// stl
#include <algorithm>

#include "GaudiKernel/AlgFactory.h"

#include "Event/MCSTDeposit.h"

// IT includes
#include "MCSTDigitCreator.h"

#include "Kernel/STDetSwitch.h"

using namespace LHCb;

static const AlgFactory<MCSTDigitCreator> s_factory;
const IAlgFactory& MCSTDigitCreatorFactory = s_factory;
//_________________________________________________
// ITDigitCreator
//
// An TrDigitCreator is a Gaudi sub algorithm that creates Inner tracker Digit 
// from the digitizations and stores them in the correct layer
// 
MCSTDigitCreator::MCSTDigitCreator(const std::string& name,
                                             ISvcLocator* pSvcLocator):
  GaudiAlgorithm(name, pSvcLocator)
{
  // MCSTDigitCreator constructor
  declareProperty("detType", m_detType = "TT"); 

  m_inputLocation = MCSTDepositLocation::TTDeposits;
  m_outputLocation = MCSTDigitLocation::TTDigits; 
}

MCSTDigitCreator::~MCSTDigitCreator()
{
  // MCSTDigitCreator destructor
}


StatusCode MCSTDigitCreator::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

  STDetSwitch::flip(m_detType,m_inputLocation);
  STDetSwitch::flip(m_detType,m_outputLocation);

  return StatusCode::SUCCESS;
}

StatusCode MCSTDigitCreator::execute()
{
  // Executes MCSTDigitCreator for one event.
  
 
  // retrieve Deposits
  MCSTDeposits* depositCont = get<MCSTDeposits>(m_inputLocation);

  // digits container
  MCSTDigits* digitCont = new MCSTDigits();
  digitCont->reserve(depositCont->size());

  // collect all deposits on 1 strip = MCSTDigit
  MCSTDeposits::const_iterator iterDep = depositCont->begin();
  MCSTDeposits::const_iterator jterDep = iterDep;

  while (iterDep != depositCont->end()){

    SmartRefVector<MCSTDeposit> depositVector;
  
    do {
      depositVector.push_back(*jterDep);
      ++jterDep;
    } while ((jterDep != depositCont->end())&&(keepAdding(*iterDep,*jterDep) == true));

    // make a new MCSTDigit and add it to the vector !!!!
    MCSTDigit* newDigit = new MCSTDigit();
    newDigit->setMcDeposit(depositVector);
    STChannelID aChan = (*iterDep)->channelID();
    digitCont->insert(newDigit,aChan);

    iterDep = jterDep;

  } // iterDep

  //register layers in the transient data store.....
  put(digitCont, m_outputLocation );
 
  return StatusCode::SUCCESS;
}

bool MCSTDigitCreator::keepAdding(const MCSTDeposit* firstDep,
                                  const MCSTDeposit* secondDep) const{
 
  // check whether to continueing adding deposits to vector
  return (firstDep->channelID() == secondDep->channelID());
}
















