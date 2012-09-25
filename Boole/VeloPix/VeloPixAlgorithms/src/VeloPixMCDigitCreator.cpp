// $Id: VeloPixMCDigitCreator.cpp,v 1.1.1.1 2009-12-04 14:16:52 marcin Exp $
// Include files:
// STL
#include <string>
#include <vector>
// Gaudi
#include "GaudiKernel/AlgFactory.h"
// Event
#include "Event/MCVeloPixDeposit.h"
#include "Event/MCVeloPixDigit.h"
// Boost
#include "boost/assign/list_of.hpp"
// Local
#include "VeloPixMCDigitCreator.h"

using namespace LHCb;

//------------------------------------------------------------
// Implementation file for class : VeloPixMCDigitCreator
//
// 20/09/2009 : Marcin Kucharczyk
//------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(VeloPixMCDigitCreator);

//=============================================================================
// Constructor
//=============================================================================
VeloPixMCDigitCreator::VeloPixMCDigitCreator(const std::string& name, 
                                             ISvcLocator* pSvcLocator)
  : GaudiAlgorithm(name, pSvcLocator)
{
  declareProperty("InputLocation", m_inputLocation = 
                  "MC/VP/Deposits");
  declareProperty("OutputLocation", m_outputLocation = 
                  "MC/VP/Digits");
  declareProperty("SamplesVector", m_sampleNames = 
         boost::assign::list_of("/")("/Prev/")("/PrevPrev/")("/Next/"));
  declareProperty("SpillVector", m_spillNames = 
         boost::assign::list_of("/")("/Prev/")("/PrevPrev/")("/Next/"));
}

//=============================================================================
// Destructor
//=============================================================================
VeloPixMCDigitCreator::~VeloPixMCDigitCreator(){};

//=============================================================================
// Initialisation
//=============================================================================
StatusCode VeloPixMCDigitCreator::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if(sc.isFailure()) return sc;
  m_isDebug = msgLevel(MSG::DEBUG);
  m_isVerbose = msgLevel(MSG::VERBOSE);
  if(m_isDebug) debug() << "==> Initialise" << endmsg;
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Execution
//=============================================================================
StatusCode VeloPixMCDigitCreator::execute() {
  if(m_isDebug) debug() << "==> Execute" << endmsg;
  // Clear path vectors
  m_spillPaths.clear();
  m_outPaths.clear();
  std::vector<std::string>::const_iterator iSpillName = m_spillNames.begin() ;
  while (iSpillName != m_spillNames.end()) {
    m_spillPaths.push_back("/Event"+(*iSpillName)+m_inputLocation);
    ++iSpillName;
  }
  std::vector<std::string>::const_iterator iSampleName = m_sampleNames.begin();
  while (iSampleName != m_sampleNames.end()){
    m_outPaths.push_back("/Event"+(*iSampleName)+m_outputLocation);
    ++iSampleName;
  }
  // Loop over spills
  for(unsigned int iSpill = 0; iSpill < m_spillPaths.size(); ++iSpill) {
    if(exist<MCVeloPixDeposits>(m_spillPaths[iSpill]) == false) {
      debug() << "Unable to retrieve " + m_spillPaths[iSpill] << endmsg;
    }
    else {
      MCVeloPixDeposits* deposits = 
                         get<MCVeloPixDeposits>(m_spillPaths[iSpill]);
      // Create MCVeloPixDigits
      LHCb::MCVeloPixDigits* digitsCont = new LHCb::MCVeloPixDigits();
      createMCDigits(deposits,digitsCont);
      // Register MCVeloPixDigits in the transient data store
      put(digitsCont,m_outPaths[iSpill]);
    }
  }
 
  return StatusCode::SUCCESS;
}


//============================================================================
// Create MCVeloPixDigits
//============================================================================
void VeloPixMCDigitCreator::createMCDigits(
                            const MCVeloPixDeposits* depositCont,  // list of deposits is sorted accord. to channel id
                            LHCb::MCVeloPixDigits* digitsCont)     // otherwise the collection method would not work
{
  // printf("VeloPixMCDigitCreator::createMCDigits(): %d deposits\n", depositCont->size());
  // Collect all deposits that belong to the same pixel
  MCVeloPixDeposits::const_iterator iterDep = depositCont->begin();
  MCVeloPixDeposits::const_iterator jterDep = iterDep;
  while(iterDep != depositCont->end()) {                   // loop over ionization charge deposits
    int Count=0;
    SmartRefVector<MCVeloPixDeposit> depositVector;        // temporary storage
    do {
      depositVector.push_back(*jterDep);                   // keep adding to the temporary storage
      ++jterDep; Count++;
    } while ((jterDep != depositCont->end()) &&            // as long
             (keepAdding(*iterDep,*jterDep) == true));     // as same channel ID (= same pixel)
    MCVeloPixDigit* newDigit = new MCVeloPixDigit();
    newDigit->setMcDeposit(depositVector);
    const VeloPixChannelID aChan = (*iterDep)->channelID(); // channel id for this group

    // printf(" [%02d:%c, %02d:%03dx%03d]",                    // print channel ID
    //        aChan.station(), aChan.sidepos()?'R':'L',
    //        aChan.chip(), aChan.pixel_lp(), aChan.pixel_hp() );
    // printf(" => %d deposits\n", Count);                     // and how many deposits for this pixel

    digitsCont->insert(newDigit,aChan);                     // add this group with its channel id
    iterDep = jterDep;
  }
}


//============================================================================
// Keep adding charge assigned to the same channelID 
//============================================================================ 
bool VeloPixMCDigitCreator::keepAdding(const MCVeloPixDeposit* firstDep,
                                       const MCVeloPixDeposit* secondDep) const
{
  return (firstDep->channelID() == secondDep->channelID()); // return true if same channel ID (same pixel)
}

//============================================================================
StatusCode VeloPixMCDigitCreator::finalize() {

  return GaudiAlgorithm::finalize();

}
