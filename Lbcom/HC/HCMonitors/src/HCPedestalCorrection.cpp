// ROOT
#include "TProfile.h"

// Gaudi
#include "GaudiUtils/HistoLabels.h"
#include "GaudiUtils/Aida2ROOT.h"

// LHCb
// Event/DigiEvent
#include "Event/HCDigit.h"

// Local
#include "HCPedestalCorrection.h"

using namespace Gaudi::Utils::Histos;

DECLARE_ALGORITHM_FACTORY(HCPedestalCorrection)

//=============================================================================
// Standard constructor
//=============================================================================
HCPedestalCorrection::HCPedestalCorrection(const std::string& name, 
                                           ISvcLocator* pSvcLocator)
    : HCMonitorBase(name, pSvcLocator) {

  declareProperty("DigitLocation", m_digitLocation = LHCb::HCDigitLocation::Default);
}

//=============================================================================
// Destructor
//=============================================================================
HCPedestalCorrection::~HCPedestalCorrection() {}

//=============================================================================
// Initialisation
//=============================================================================
StatusCode HCPedestalCorrection::initialize() {

  // Initialise the base class.
  StatusCode sc = HCMonitorBase::initialize();
  if (sc.isFailure()) return sc;

  // Setup the histograms.
  std::vector<std::string> stations = {"B0", "B1", "B2", "F1", "F2"};
  const unsigned int nStations = 5;
  for (unsigned int i = 0; i < 4; ++i) {
    // Histogram range and number of bins (in case of uniform binning).
    const double low = -0.5;
    const double high = 1023.5;
    const unsigned int bins = 1024;
    // Book histograms for ADC distributions for each quadrant.
    const std::string qu = "Quadrant" + std::to_string(i);
    for (unsigned int j = 0; j < nStations; ++j) {
      const std::string name = "Correlation/" + stations[j] + "/" + qu;
      if (m_variableBins) {
        m_hCorrelation.push_back(book2D(name, qu, m_edges, m_edges));
        m_hProfile.push_back(bookProfile1D(name + "Prof", qu, m_edges));
      } else {
        m_hCorrelation.push_back(book2D(name, qu, low, high, bins, 
                                        low, high, bins));
        m_hProfile.push_back(bookProfile1D(name + "Prof", qu, low, high, bins));
      }
      const unsigned int index = i * nStations + j;
      setAxisLabels(m_hCorrelation[index], "ADC Spare", 
                    "ADC Quadrant " + std::to_string(i));
      setAxisLabels(m_hProfile[index], "ADC Spare", 
                    "ADC Quadrant " + std::to_string(i));
    }
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HCPedestalCorrection::execute() {

  // Grab the digits.
  const LHCb::HCDigits* digits = getIfExists<LHCb::HCDigits>(m_digitLocation);
  if (!digits) {
    return Error("No digits in " + m_digitLocation, StatusCode::FAILURE);
  }
  const unsigned int nStations = 5;
  std::vector<std::vector<double> > adcs(nStations);
  for (unsigned int i = 0; i < nStations; ++i) {
    adcs[i].assign(5, 0.);
  }
  // Loop over the digits.
  for (LHCb::HCDigit* digit : *digits) {
    const unsigned int crate = digit->cellID().crate();
    const unsigned int channel = digit->cellID().channel();
    const double adc = fadc(digit->adc());
    unsigned int station = 0;
    unsigned int quadrant = 0;
    unsigned int offset = 0;
    if (crate == m_crateB) {
      if (!m_mappedB[channel]) continue;
      station = m_stationB[channel];
      quadrant = m_quadrantB[channel];
    } else if (crate == m_crateF) {
      if (!m_mappedF[channel]) continue;
      station = m_stationF[channel];
      quadrant = m_quadrantF[channel];
      offset = 2;
    } else {
      warning() << "Unexpected crate number (" << crate << ")" << endmsg;
    }
    adcs[station + offset][quadrant] = adc;
  }
  for (unsigned int i = 0; i < nStations; ++i) {
    for (unsigned int j = 0; j < 4; ++j) {
      const unsigned int index = j * nStations + i;
      m_hCorrelation[index]->fill(adcs[i][4], adcs[i][j]);
      m_hProfile[index]->fill(adcs[i][4], adcs[i][j]);
    }
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Finalisation
//=============================================================================
StatusCode HCPedestalCorrection::finalize() {

  const unsigned int nStations = 5;
  for (unsigned int i = 0; i < nStations; ++i) {
    for (unsigned int j = 0; j < 4; ++j) {
      const unsigned int index = j * nStations + i;
      auto profile = Gaudi::Utils::Aida2ROOT::aida2root(m_hProfile[index]);
      profile->Fit("pol1");
    }
  }     
  return HCMonitorBase::finalize();
}

