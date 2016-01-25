// ROOT
#include "TProfile.h"

// Gaudi
#include "GaudiUtils/HistoLabels.h"
#include "GaudiUtils/Aida2ROOT.h"

// LHCb
// Event/DigiEvent
#include "Event/HCDigit.h"
#include "Event/ODIN.h"
// Local
#include "HCPedestalCorrectionTree.h"

using namespace Gaudi::Utils::Histos;

DECLARE_ALGORITHM_FACTORY(HCPedestalCorrectionTree)

//=============================================================================
// Standard constructor
//=============================================================================
HCPedestalCorrectionTree::HCPedestalCorrectionTree(const std::string& name,
                                           ISvcLocator* pSvcLocator)
  : HCMonitorBase(name, pSvcLocator) {
  declareProperty("DigitLocation", m_digitLocation = LHCb::HCDigitLocation::Default);

}

//=============================================================================
// Destructor
//=============================================================================
HCPedestalCorrectionTree::~HCPedestalCorrectionTree() {}

//=============================================================================
// Initialisation
//=============================================================================
StatusCode HCPedestalCorrectionTree::initialize() {

  // Initialise the base class.
  StatusCode sc = HCMonitorBase::initialize();
  if (sc.isFailure()) return sc;

  //m_bxID = book1D("bxID", "bxID" , 0, 4000., 4000);
  //setAxisLabels(m_bxID, "bxID", "Events");
  return StatusCode::SUCCESS;

}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HCPedestalCorrectionTree::execute() {

  const LHCb::ODIN* odin = getIfExists<LHCb::ODIN>(LHCb::ODINLocation::Default);
  if (!odin) {
    return Error("Cannot retrieve ODIN.", StatusCode::SUCCESS);
  }
  if (msgLevel(MSG::DEBUG)) {
    debug() << "Run " << odin->runNumber() << ", Event " << odin->eventNumber()
            << endmsg;
  }
  const unsigned int bxID = odin->bunchId();
  //const unsigned int parity = bxID % 2;
  ////
  if (bxID < m_bxMin || bxID > m_bxMax) return StatusCode::SUCCESS;
  ///
  

  // Grab the digits.
  const LHCb::HCDigits* digits = getIfExists<LHCb::HCDigits>(m_digitLocation);
  if (!digits) {
    return Error("No digits in " + m_digitLocation, StatusCode::FAILURE);
  }

  // Loop over the digits.
  const unsigned int nStations = 5;
  for (unsigned int i = 0; i < nStations; ++i) {
    for (unsigned int j = 0; j < 4; ++j) {
      LHCb::HCCellID id(m_channels[i][j]);
      const LHCb::HCDigit* digit = digits->object(id);
      if (!digit) {
        warning() << "Cannot retrieve digit." << endmsg;
        continue;
      }
      const int adc = digit->adc();
      LHCb::HCCellID refid(m_references[i][j]);
      const LHCb::HCDigit* refdigit = digits->object(refid);
      if (!refdigit) {
        warning() << "Cannot retrieve reference digit." << endmsg;
        continue;
      }
      const int refadc = refdigit->adc();
      m_sig[i][j] = adc;
      m_ref[i] = refadc;
    }
  }

  
  //Tuple general Info                                                                                                                                                                  
  Tuple tuple = nTuple("HerschelDigits", "HerschelDigits");
  tuple->column("run",odin->runNumber());
  tuple->column("bxID",odin->bunchId());
  const std::vector<std::string> stations2 = {      "B0", "B1", "B2", "F1", "F2"};
  const unsigned int nStations2 = 5;
  for (unsigned int i = 0; i < nStations2; ++i){
    const std::string st2 = stations2[i];
    const std::string st2ref = st2+"ref";
    tuple->column(st2ref, m_ref[i]);
    for (unsigned int j = 0; j < 4; ++j){
      const std::string qu = std::to_string(j);
      const std::string name = st2 + qu ;
      tuple->column(name,m_sig[i][j]);
    } 
  }
  tuple->write();

  return StatusCode::SUCCESS;
}

//=============================================================================
// Finalisation
//=============================================================================
StatusCode HCPedestalCorrectionTree::finalize() {
   return HCMonitorBase::finalize();
}
