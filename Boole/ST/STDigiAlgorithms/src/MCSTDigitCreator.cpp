// $Id: MCSTDigitCreator.cpp,v 1.1.1.1 2008-02-15 13:18:48 cattanem Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"

// Event
#include "Event/MCSTDeposit.h"
#include "Event/MCSTDigit.h"
#include "Kernel/STDetSwitch.h"

// local
#include "MCSTDigitCreator.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( MCSTDigitCreator );

MCSTDigitCreator::MCSTDigitCreator(const std::string& name,
                                             ISvcLocator* pSvcLocator):
  GaudiAlgorithm(name, pSvcLocator)
{
  // MCSTDigitCreator constructor
  declareProperty("DetType", m_detType = "TT"); 

  m_inputLocation = MCSTDepositLocation::TTDeposits;
  m_outputLocation = MCSTDigitLocation::TTDigits; 
}

MCSTDigitCreator::~MCSTDigitCreator()
{
  // MCSTDigitCreator destructor
}

StatusCode MCSTDigitCreator::initialize()
{
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  STDetSwitch::flip(m_detType,m_inputLocation);
  STDetSwitch::flip(m_detType,m_outputLocation);

  return StatusCode::SUCCESS;
}

StatusCode MCSTDigitCreator::execute()
{
  // retrieve Deposits
  MCSTDeposits* depositCont = get<MCSTDeposits>(m_inputLocation);

  // digits container
  MCSTDigits* digitCont = new MCSTDigits();
  digitCont->reserve(depositCont->size());

  // Collect all deposits on 1 strip (= MCSTDigit)
  // this requires the list of deposits to be sorted by STChannelID
  MCSTDeposits::const_iterator iterDep = depositCont->begin();
  MCSTDeposits::const_iterator jterDep = iterDep;
  while (iterDep != depositCont->end()){

    SmartRefVector<MCSTDeposit> depositVector;  
    do {
      depositVector.push_back(*jterDep);
      ++jterDep;
    } while ( (jterDep != depositCont->end())&&
              (keepAdding(*iterDep,*jterDep) == true) );

    // make a new MCSTDigit and add it to the vector
    MCSTDigit* newDigit = new MCSTDigit();
    newDigit->setMcDeposit(depositVector);
    STChannelID aChan = (*iterDep)->channelID();
    digitCont->insert(newDigit,aChan);

    iterDep = jterDep;

  } // iterDep

  // register MCSTDigits in the transient data store
  put(digitCont, m_outputLocation );
 
  return StatusCode::SUCCESS;
}

bool MCSTDigitCreator::keepAdding(const MCSTDeposit* firstDep,
                                  const MCSTDeposit* secondDep) const
{ 
  // check whether to continueing adding deposits to vector
  return (firstDep->channelID() == secondDep->channelID());
}
