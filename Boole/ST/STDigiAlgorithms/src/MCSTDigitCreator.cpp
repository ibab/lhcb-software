// Gaudi
#include "GaudiKernel/AlgFactory.h"

// Event
#include "Event/MCSTDeposit.h"
#include "Event/MCSTDigit.h"


// local
#include "MCSTDigitCreator.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( MCSTDigitCreator )

MCSTDigitCreator::MCSTDigitCreator(const std::string& name,
                                             ISvcLocator* pSvcLocator):
  ST::AlgBase(name, pSvcLocator)
{
  // MCSTDigitCreator constructor

  declareSTConfigProperty("InputLocation" , m_inputLocation,
		  MCSTDepositLocation::TTDeposits);
  declareSTConfigProperty("OutputLocation" , m_outputLocation,
		  MCSTDigitLocation::TTDigits);

}

MCSTDigitCreator::~MCSTDigitCreator()
{
  // MCSTDigitCreator destructor
}


StatusCode MCSTDigitCreator::execute()
{
  // retrieve Deposits
  const MCSTDeposits* depositCont = get<MCSTDeposits>(m_inputLocation);

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
    const STChannelID aChan = (*iterDep)->channelID();
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
