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
#ifdef DEBUG_HISTO
  : GaudiTupleAlg(name, pSvcLocator)
#else
  : GaudiAlgorithm(name, pSvcLocator)
#endif

{
  declareProperty("InputLocation", m_inputLocation = 
                  "MC/VP/Digits");
  declareProperty("OutputLocation", m_outputLocation = 
                  "VP/PreDigits");
  declareProperty("SamplesVector", m_sampleNames = 
         boost::assign::list_of("/")("/Prev/")("/PrevPrev/")("/Next/"));
  declareProperty("SpillVector", m_spillNames = 
         boost::assign::list_of("/")("/Prev/")("/PrevPrev/")("/Next/"));
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
  m_isDebug = msgLevel(MSG::DEBUG);
  m_isVerbose = msgLevel(MSG::VERBOSE);
  if(m_isDebug) debug() << "==> Initialise" << endmsg;
#ifdef DEBUG_HISTO
  setHistoTopDir("VeloPix/");
#endif
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
{ // printf("VeloPixDigitCreator::createDigits() =>\n");
  MCVeloPixDigits::const_iterator iterMC = digitsMC->begin();
  for(; iterMC != digitsMC->end(); ++iterMC) {                                   // for every MC hit
    const SmartRefVector<MCVeloPixDeposit> depositsCont =
                                           (*iterMC)->mcDeposit();
    double totCharge = std::accumulate(depositsCont.begin(),                     // sum up the charge of all the deposits
                                       depositsCont.end(), 0.0,                  // simulated for this MC hit
           VeloPixDataFunctor::Accumulate_Charge<const MCVeloPixDeposit*>());

    const VeloPixChannelID aChan = (*iterMC)->channelID();                       // print the channel ID
    // printf(" %9.1f e [%02d:%c, %02d:%03dx%03d]\n", totCharge,                    // and the total change accumulated on this pixel
    //        aChan.station(), aChan.sidepos()?'R':'L',
    //        aChan.chip(), aChan.pixel_lp(), aChan.pixel_hp() );

    int Station = aChan.station()+1; if(aChan.sidepos()) Station = (-Station);

#ifdef DEBUG_HISTO
    if(totCharge>=500.0)
      int Chip    = aChan.chip();
      plot2D( Station, Chip, "HitsPerChip", "VeloPixDigitCreator: number of hits/chip (all events)",
             -24.0, 24.0, 0.0, 12.0, 49, 12);

    plot(totCharge, "ChargePerPixel", "VeloPixDigitCreator: charge/pixel [e]", 250.0, 25000.0, 99);
#endif

    if(totCharge > 0.0 ) {
      int tot = int( ceil(totCharge) );
      int timeStamp = 0; // temporary assumption                                 // for now time-stamp=0
      LHCb::VeloPixDigit* newDigit =
                          new LHCb::VeloPixDigit(tot,timeStamp);                 // store total charge and time-stamp
      digitsCont->insert(newDigit,(*iterMC)->channelID());
    }
  }
}



//============================================================================
StatusCode VeloPixDigitCreator::finalize() {

  return GaudiAlgorithm::finalize();

}
