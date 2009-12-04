// $Id: VeloPixDigitCreator.cpp,v 1.1.1.1 2009-12-04 14:16:52 marcin Exp $
// Include files:
// STL
#include <string>
#include <vector>
// GSL
#include "gsl/gsl_sf_erf.h"
// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"
// LHCbKernel
#include "Kernel/VeloPixChannelID.h"
#include "Kernel/VeloPixDataFunctor.h"
#include "LHCbMath/LHCbMath.h"
// Event
#include "Event/MCVeloPixDigit.h"
#include "Event/VeloPixDigit.h"
// Boost
#include "boost/assign/list_of.hpp"  
#include "boost/numeric/conversion/bounds.hpp"
// Local
#include "VeloPixDigitCreator.h"

using namespace LHCb;

//------------------------------------------------------------
// Implementation file for class : VeloPixDigitCreator
//
// 20/09/2009 : Marcin Kucharczyk
//------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(VeloPixDigitCreator);

//=============================================================================
// Constructor
//=============================================================================
VeloPixDigitCreator::VeloPixDigitCreator(const std::string& name, 
                                         ISvcLocator* pSvcLocator)
  : GaudiAlgorithm(name, pSvcLocator)
{
  declareProperty("InputLocation", m_inputLocation = 
                  "MC/VeloPix/Digits");
  declareProperty("OutputLocation", m_outputLocation = 
                  "VeloPix/Digits");
  declareProperty("SamplesVector", m_sampleNames = 
         boost::assign::list_of("/")("/Prev/")("/PrevPrev/")("/Next/"));
  declareProperty("SpillVector", m_spillNames = 
         boost::assign::list_of("/")("/Prev/")("/PrevPrev/")("/Next/"));
  declareProperty("Threshold", m_threshold = 1000.0);  // Si thickness = 150um
  declareProperty("Conversion", m_conversion = 0.00048);
  declareProperty("nBits",m_nBits = 3u); // 4 bits
}

//=============================================================================
// Destructor
//=============================================================================
VeloPixDigitCreator::~VeloPixDigitCreator(){};

//=============================================================================
// Initialisation
//=============================================================================
StatusCode VeloPixDigitCreator::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if(sc.isFailure()) return sc;
  m_maxCharge = double(2 << m_nBits) - 1;
  m_isDebug = msgLevel(MSG::DEBUG);
  m_isVerbose = msgLevel(MSG::VERBOSE);
  if(m_isDebug) debug() << "==> Initialise" << endmsg;
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Execution
//=============================================================================
StatusCode VeloPixDigitCreator::execute() {
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
    if(exist<MCVeloPixDigits>(m_spillPaths[iSpill]) == false) {
      debug() << "Unable to retrieve " + m_spillPaths[iSpill] << endmsg;
    }
    else {
      const MCVeloPixDigits* digitsMC = 
                             get<MCVeloPixDigits>(m_spillPaths[iSpill]);
      // Create VeloPixDigits
      LHCb::VeloPixDigits* digitsCont = new LHCb::VeloPixDigits();
      createDigits(digitsMC,digitsCont);
      // Register VeloPixDigits in the transient data store
      put(digitsCont,m_outPaths[iSpill]);
    }
  }
 
  return StatusCode::SUCCESS;
}


//============================================================================
// Create VeloPixDigits
//============================================================================
void VeloPixDigitCreator::createDigits(const MCVeloPixDigits* digitsMC, 
                                       LHCb::VeloPixDigits* digitsCont)
{
  MCVeloPixDigits::const_iterator iterMC = digitsMC->begin();
  for(; iterMC != digitsMC->end(); ++iterMC) {
    const SmartRefVector<MCVeloPixDeposit> depositsCont =
                                           (*iterMC)->mcDeposit();
    double totCharge = std::accumulate(depositsCont.begin(),
                                       depositsCont.end(), 0.0,
           VeloPixDataFunctor::Accumulate_Charge<const MCVeloPixDeposit*>());
    if(totCharge > m_threshold) {
      int tot = convertToTDC(totCharge);
      int timeStamp = 0; // temporary assumption
      LHCb::VeloPixDigit* newDigit =
                          new LHCb::VeloPixDigit(tot,timeStamp);
      digitsCont->insert(newDigit,(*iterMC)->channelID());
    }
  }
}


//============================================================================
// Convert to TDC
//============================================================================
int VeloPixDigitCreator::convertToTDC(double charge)
{
  int tot = int(ceil(charge * m_conversion));
  if(tot > int(m_maxCharge)) tot = int(m_maxCharge);
  return tot;
}


//============================================================================
StatusCode VeloPixDigitCreator::finalize() {

  return GaudiAlgorithm::finalize();

}
