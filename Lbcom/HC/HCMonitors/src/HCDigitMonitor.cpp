// Gaudi
#include "GaudiUtils/HistoLabels.h"

// LHCb
// Event/DAQEvent
#include "Event/ODIN.h"
// Event/DigiEvent
#include "Event/HCDigit.h"
#include "Kernel/HCCellID.h"

// Local
#include "HCDigitMonitor.h"

using namespace Gaudi::Utils::Histos;

DECLARE_ALGORITHM_FACTORY(HCDigitMonitor)

//=============================================================================
// Standard constructor
//=============================================================================
HCDigitMonitor::HCDigitMonitor(const std::string& name, ISvcLocator* pSvcLocator)
    : HCMonitorBase(name, pSvcLocator) {
  declareProperty("DigitLocation", m_digitLocation = LHCb::HCDigitLocation::Default);
}

//=============================================================================
// Destructor
//=============================================================================
HCDigitMonitor::~HCDigitMonitor() {}

//=============================================================================
// Initialisation
//=============================================================================
StatusCode HCDigitMonitor::initialize() {

  // Initialise the base class.
  StatusCode sc = HCMonitorBase::initialize();
  if (sc.isFailure()) return sc;

  // Setup the histograms.
  m_stations = {"B0", "B1", "B2", "F1", "F2"};
  const unsigned int nStations = 5;
  for (unsigned int i = 0; i < nStations; ++i) {
    // Book histograms for ADC sum distributions for each station.
    const unsigned int bins = m_variableBins ? 256 : 4096;
    const double low = -0.5;
    const double high = 4095.5;
    const std::string st = m_stations[i];
    std::string name = "ADC/Sum/" + st;
    m_hAdcSum.push_back(book1D(name, st, low, high, bins));
    m_hAdcSumEven.push_back(book1D("ADC/Sum/Even/" + st, st, low, high, bins));
    m_hAdcSumOdd.push_back(book1D("ADC/Sum/Odd/" + st, st, low, high, bins));
    m_hAdcSumNoBeam.push_back(book1D("ADC/Sum/NoBeam/" + st, st, low, high, bins));
    m_hAdcSumBeam.push_back(book1D("ADC/Sum/Beam/" + st, st, low, high, bins));
    setAxisLabels(m_hAdcSum[i], "Sum ADC", "Entries");
    setAxisLabels(m_hAdcSumEven[i], "Sum ADC", "Entries");
    setAxisLabels(m_hAdcSumOdd[i], "Sum ADC", "Entries");
    setAxisLabels(m_hAdcSumNoBeam[i], "Sum ADC", "Entries");
    setAxisLabels(m_hAdcSumBeam[i], "Sum ADC", "Entries");
    // Book profile histograms of average ADC vs. quadrant for each station.
    name = "ADC/" + st + "/Average";
    m_hAdcVsQuadrant.push_back(bookProfile1D(name, st, -0.5, 3.5, 4));
    name = "ADC/" + st + "/Even/Average";
    m_hAdcVsQuadrantEven.push_back(bookProfile1D(name, st, -0.5, 3.5, 4));
    name = "ADC/" + st + "/Odd/Average";
    m_hAdcVsQuadrantOdd.push_back(bookProfile1D(name, st, -0.5, 3.5, 4));
    name = "ADC/" + st + "/NoBeam/Average";
    m_hAdcVsQuadrantNoBeam.push_back(bookProfile1D(name, st, -0.5, 3.5, 4));
    name = "ADC/" + st + "/Beam/Average";
    m_hAdcVsQuadrantBeam.push_back(bookProfile1D(name, st, -0.5, 3.5, 4));
    setAxisLabels(m_hAdcVsQuadrant[i], "Quadrant", "ADC");
    setAxisLabels(m_hAdcVsQuadrantEven[i], "Quadrant", "ADC");
    setAxisLabels(m_hAdcVsQuadrantOdd[i], "Quadrant", "ADC");
    setAxisLabels(m_hAdcVsQuadrantNoBeam[i], "Quadrant", "ADC");
    setAxisLabels(m_hAdcVsQuadrantBeam[i], "Quadrant", "ADC");

    if (!m_thetaConfig.empty()){
      const double lowCor = -1000.5;
      const double highCor = 5095.5;
      const unsigned int binsCor = 6096;
      std::string name = "ADCCor/Sum/" + st;
      m_hAdcSumCor.push_back(book1D(name, st, lowCor, highCor, binsCor));
      m_hAdcSumEvenCor.push_back(book1D("ADCCor/Sum/Even/" + st, st, lowCor, highCor, binsCor));
      m_hAdcSumOddCor.push_back(book1D("ADCCor/Sum/Odd/" + st, st, lowCor, highCor, binsCor));
      m_hAdcSumNoBeamCor.push_back(book1D("ADCCor/Sum/NoBeam/" + st, st, lowCor, highCor, binsCor));
      m_hAdcSumBeamCor.push_back(book1D("ADCCor/Sum/Beam/" + st, st, lowCor, highCor, binsCor));
      setAxisLabels(m_hAdcSumCor[i], "Sum ADCCor", "Entries");
      setAxisLabels(m_hAdcSumEvenCor[i], "Sum ADCCor", "Entries");
      setAxisLabels(m_hAdcSumOddCor[i], "Sum ADCCor", "Entries");
      setAxisLabels(m_hAdcSumNoBeamCor[i], "Sum ADCCor", "Entries");
      setAxisLabels(m_hAdcSumBeamCor[i], "Sum ADCCor", "Entries");
      // Book profile histograms of average ADC vs. quadrant for each station.
      name = "ADCCor/" + st + "/Average";
      m_hAdcVsQuadrantCor.push_back(bookProfile1D(name, st, -0.5, 3.5, 4));
      name = "ADCCor/" + st + "/Even/Average";
      m_hAdcVsQuadrantEvenCor.push_back(bookProfile1D(name, st, -0.5, 3.5, 4));
      name = "ADCCor/" + st + "/Odd/Average";
      m_hAdcVsQuadrantOddCor.push_back(bookProfile1D(name, st, -0.5, 3.5, 4));
      name = "ADCCor/" + st + "/NoBeam/Average";
      m_hAdcVsQuadrantNoBeamCor.push_back(bookProfile1D(name, st, -0.5, 3.5, 4));
      name = "ADCCor/" + st + "/Beam/Average";
      m_hAdcVsQuadrantBeamCor.push_back(bookProfile1D(name, st, -0.5, 3.5, 4));
      setAxisLabels(m_hAdcVsQuadrantCor[i], "Quadrant", "ADCCor");
      setAxisLabels(m_hAdcVsQuadrantEvenCor[i], "Quadrant", "ADCCor");
      setAxisLabels(m_hAdcVsQuadrantOddCor[i], "Quadrant", "ADCCor");
      setAxisLabels(m_hAdcVsQuadrantNoBeamCor[i], "Quadrant", "ADCCor");
      setAxisLabels(m_hAdcVsQuadrantBeamCor[i], "Quadrant", "ADCCor");
    }
  }

  const unsigned int nChannels = 64;
  for (unsigned int i = 0; i < nChannels; ++i) {
    // Book histograms for ADC distributions for each channel.
    const std::string ch = "Channel" + std::to_string(i);
    if (m_variableBins) {
      m_hAdcB.push_back(book1D("ADC/B/" + ch, ch, m_edges));
      m_hAdcF.push_back(book1D("ADC/F/" + ch, ch, m_edges));
      m_hAdcEvenB.push_back(book1D("ADC/B/Even/" + ch, ch, m_edges));
      m_hAdcEvenF.push_back(book1D("ADC/F/Even/" + ch, ch, m_edges));
      m_hAdcOddB.push_back(book1D("ADC/B/Odd/" + ch, ch, m_edges));
      m_hAdcOddF.push_back(book1D("ADC/F/Odd/" + ch, ch, m_edges));
    } else {
      const double low = -0.5;
      const double high = 1023.5;
      const unsigned int bins = 1024;
      m_hAdcB.push_back(book1D("ADC/B/" + ch, ch, low, high, bins));
      m_hAdcF.push_back(book1D("ADC/F/" + ch, ch, low, high, bins));
      m_hAdcEvenB.push_back(book1D("ADC/B/Even/" + ch, ch, low, high, bins));
      m_hAdcEvenF.push_back(book1D("ADC/F/Even/" + ch, ch, low, high, bins));
      m_hAdcOddB.push_back(book1D("ADC/B/Odd/" + ch, ch, low, high, bins));
      m_hAdcOddF.push_back(book1D("ADC/F/Odd/" + ch, ch, low, high, bins));
    }
    setAxisLabels(m_hAdcB[i], "ADC", "Entries");
    setAxisLabels(m_hAdcF[i], "ADC", "Entries");
    setAxisLabels(m_hAdcEvenB[i], "ADC", "Entries");
    setAxisLabels(m_hAdcEvenF[i], "ADC", "Entries");
    setAxisLabels(m_hAdcOddB[i], "ADC", "Entries");
    setAxisLabels(m_hAdcOddF[i], "ADC", "Entries");

    
    if (!m_thetaConfig.empty()){
      const double lowCor = -200.5;
      const double highCor = 1223.5;
      const unsigned int binsCor = 1424;
      m_hAdcBCor.push_back(book1D("ADCCor/B/" + ch, ch, lowCor, highCor, binsCor));
      m_hAdcFCor.push_back(book1D("ADCCor/F/" + ch, ch, lowCor, highCor, binsCor));
      m_hAdcEvenBCor.push_back(book1D("ADCCor/B/Even/" + ch, ch, lowCor, highCor, binsCor));
      m_hAdcEvenFCor.push_back(book1D("ADCCor/F/Even/" + ch, ch, lowCor, highCor, binsCor));
      m_hAdcOddBCor.push_back(book1D("ADCCor/B/Odd/" + ch, ch, lowCor, highCor, binsCor));
      m_hAdcOddFCor.push_back(book1D("ADCCor/F/Odd/" + ch, ch, lowCor, highCor, binsCor));
    
      setAxisLabels(m_hAdcBCor[i], "ADCCor", "Entries");
      setAxisLabels(m_hAdcFCor[i], "ADCCor", "Entries");
      setAxisLabels(m_hAdcEvenBCor[i], "ADCCor", "Entries");
      setAxisLabels(m_hAdcEvenFCor[i], "ADCCor", "Entries");
      setAxisLabels(m_hAdcOddBCor[i], "ADCCor", "Entries");
      setAxisLabels(m_hAdcOddFCor[i], "ADCCor", "Entries");
    }
  }

  for (unsigned int i = 0; i < 4; ++i) {
    // Histogram range and number of bins (in case of uniform binning).
    const double low = -0.5;
    const double high = 1023.5;
    const unsigned int bins = 1024;
    const double lowCor = -200.5;
    const double highCor = 1223.5;
    const unsigned int binsCor = 1424;
    // Book histograms for ADC distributions for each quadrant.
    const std::string qu = "Quadrant" + std::to_string(i);
    // Bx ID binning
    const double bxlow = -0.5;
    const double bxhigh = 4095.5;
    const unsigned int bxbins = 4096;
    const double bxlowCor =  -0.5;
    const double bxhighCor =  4095.5;
    const unsigned int bxbinsCor = 4096;
    std::vector<double> bxedges;
    for (unsigned int j = 0; j < bxbins + 1; ++j) {
      bxedges.push_back(j - 0.5);
    }
    for (unsigned int j = 0; j < nStations; ++j) {
      const std::string name = "ADC/" + m_stations[j] + "/" + qu;
      const std::string nameEven = "ADC/" + m_stations[j] + "/Even/" + qu;
      const std::string nameOdd = "ADC/" + m_stations[j] + "/Odd/" + qu;
      const std::string nameNoBeam = "ADC/" + m_stations[j] + "/NoBeam/" + qu;
      const std::string nameBeam = "ADC/" + m_stations[j] + "/Beam/" + qu;
      const std::string nameBx = "ADCvsBX/" + m_stations[j] + "/" + qu;
      if (m_variableBins) {
        m_hAdcQuadrant.push_back(book1D(name, qu, m_edges));
        m_hAdcQuadrantEven.push_back(book1D(nameEven, qu, m_edges));
        m_hAdcQuadrantOdd.push_back(book1D(nameOdd, qu, m_edges));
        m_hAdcQuadrantNoBeam.push_back(book1D(nameNoBeam, qu, m_edges));
        m_hAdcQuadrantBeam.push_back(book1D(nameBeam, qu, m_edges));
        m_hAdcVsBx.push_back(book2D(nameBx, qu, bxedges, m_edges));
      } else {
        m_hAdcQuadrant.push_back(book1D(name, qu, low, high, bins));
        m_hAdcQuadrantEven.push_back(book1D(nameEven, qu, low, high, bins));
        m_hAdcQuadrantOdd.push_back(book1D(nameOdd, qu, low, high, bins));
        m_hAdcQuadrantNoBeam.push_back(book1D(nameNoBeam, qu, low, high, bins));
        m_hAdcQuadrantBeam.push_back(book1D(nameBeam, qu, low, high, bins));
        m_hAdcVsBx.push_back(book2D(nameBx, qu, bxlow, bxhigh, bxbins, 
                                    low, high, bins));
      }
      const unsigned int index = i * nStations + j;
      setAxisLabels(m_hAdcQuadrant[index], "ADC", "Entries");
      setAxisLabels(m_hAdcQuadrantEven[index], "ADC", "Entries");
      setAxisLabels(m_hAdcQuadrantOdd[index], "ADC", "Entries");
      setAxisLabels(m_hAdcQuadrantNoBeam[index], "ADC", "Entries");
      setAxisLabels(m_hAdcQuadrantBeam[index], "ADC", "Entries");
      setAxisLabels(m_hAdcVsBx[index], "BX", "ADC");
      if (!m_thetaConfig.empty()){
	const std::string nameCor = "ADCCor/" + m_stations[j] + "/" + qu;
	const std::string nameEvenCor = "ADCCor/" + m_stations[j] + "/Even/" + qu;
	const std::string nameOddCor = "ADCCor/" + m_stations[j] + "/Odd/" + qu;
	const std::string nameNoBeamCor = "ADCCor/" + m_stations[j] + "/NoBeam/" + qu;
	const std::string nameBeamCor = "ADCCor/" + m_stations[j] + "/Beam/" + qu;
	const std::string nameBxCor = "ADCCorvsBX/" + m_stations[j] + "/" + qu;
	m_hAdcQuadrantCor.push_back(book1D(nameCor, qu, lowCor, highCor, bins));
	m_hAdcQuadrantEvenCor.push_back(book1D(nameEvenCor, qu, lowCor, highCor, bins));
	m_hAdcQuadrantOddCor.push_back(book1D(nameOddCor, qu, lowCor, highCor, bins));
	m_hAdcQuadrantNoBeamCor.push_back(book1D(nameNoBeamCor, qu, lowCor, highCor, bins));
	m_hAdcQuadrantBeamCor.push_back(book1D(nameBeamCor, qu, lowCor, highCor, bins));
	m_hAdcVsBxCor.push_back(book2D(nameBxCor, qu, bxlow, bxhighCor, bxbins, 
				       lowCor, highCor, bins));
	
	const unsigned int index = i * nStations + j;
	setAxisLabels(m_hAdcQuadrantCor[index], "ADCCor", "Entries");
	setAxisLabels(m_hAdcQuadrantEvenCor[index], "ADCCor", "Entries");
	setAxisLabels(m_hAdcQuadrantOddCor[index], "ADCCor", "Entries");
	setAxisLabels(m_hAdcQuadrantNoBeamCor[index], "ADCCor", "Entries");
	setAxisLabels(m_hAdcQuadrantBeamCor[index], "ADCCor", "Entries");
	setAxisLabels(m_hAdcVsBxCor[index], "BX", "ADCCor");
      }
    }
  }
 
  // Book profile histograms of average ADC vs. channel number.
  m_hAdcVsChannelB = bookProfile1D("ADC/B/Average", "B", -0.5, 63.5, 64);
  m_hAdcVsChannelF = bookProfile1D("ADC/F/Average", "F", -0.5, 63.5, 64);
  m_hAdcVsChannelEvenB = bookProfile1D("ADC/B/Even/Average", "B", -0.5, 63.5, 64);
  m_hAdcVsChannelEvenF = bookProfile1D("ADC/F/Even/Average", "F", -0.5, 63.5, 64);
  m_hAdcVsChannelOddB = bookProfile1D("ADC/B/Odd/Average", "B", -0.5, 63.5, 64);
  m_hAdcVsChannelOddF = bookProfile1D("ADC/F/Odd/Average", "F", -0.5, 63.5, 64);
  setAxisLabels(m_hAdcVsChannelB, "Channel", "ADC");
  setAxisLabels(m_hAdcVsChannelF, "Channel", "ADC");
  setAxisLabels(m_hAdcVsChannelEvenB, "Channel", "ADC");
  setAxisLabels(m_hAdcVsChannelEvenF, "Channel", "ADC");
  setAxisLabels(m_hAdcVsChannelOddB, "Channel", "ADC");
  setAxisLabels(m_hAdcVsChannelOddF, "Channel", "ADC");

  if (!m_thetaConfig.empty()){
    m_hAdcVsChannelBCor = bookProfile1D("ADCCor/B/Average", "B", -0.5, 63.5, 64);
    m_hAdcVsChannelFCor = bookProfile1D("ADCCor/F/Average", "F", -0.5, 63.5, 64);
    m_hAdcVsChannelEvenBCor = bookProfile1D("ADCCor/B/Even/Average", "B", -0.5, 63.5, 64);
    m_hAdcVsChannelEvenFCor = bookProfile1D("ADCCor/F/Even/Average", "F", -0.5, 63.5, 64);
    m_hAdcVsChannelOddBCor = bookProfile1D("ADCCor/B/Odd/Average", "B", -0.5, 63.5, 64);
    m_hAdcVsChannelOddFCor = bookProfile1D("ADCCor/F/Odd/Average", "F", -0.5, 63.5, 64);
    setAxisLabels(m_hAdcVsChannelBCor, "Channel", "ADCCor");
    setAxisLabels(m_hAdcVsChannelFCor, "Channel", "ADCCor");
    setAxisLabels(m_hAdcVsChannelEvenBCor, "Channel", "ADCCor");
    setAxisLabels(m_hAdcVsChannelEvenFCor, "Channel", "ADCCor");
    setAxisLabels(m_hAdcVsChannelOddBCor, "Channel", "ADCCor");
    setAxisLabels(m_hAdcVsChannelOddFCor, "Channel", "ADCCor");
  }


  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HCDigitMonitor::execute() {

  // Get event information from ODIN.
  m_odin->getTime();
  const LHCb::ODIN* odin = getIfExists<LHCb::ODIN>(LHCb::ODINLocation::Default);
  if (!odin) {
    return Error("Cannot retrieve ODIN", StatusCode::SUCCESS);
  }
  const unsigned int bxid = odin->bunchId();
  // Skip events with out-of-range bunch-crossing ID. 
  if (bxid < m_bxMin || bxid > m_bxMax) return StatusCode::SUCCESS;
  const bool even = (bxid % 2 == 0);
  // Get the bunch-crossing type.
  const auto bxtype = odin->bunchCrossingType();

  // Grab the digits.
  const LHCb::HCDigits* digits = getIfExists<LHCb::HCDigits>(m_digitLocation);
  if (!digits) {
    return Error("No digits in " + m_digitLocation, StatusCode::FAILURE);
  }


  const unsigned int nStations = 5;
  std::vector<double> sum(nStations, 0.);
  std::vector<double> sumCor(nStations, 0.);
  // Loop over all digits.
  for (LHCb::HCDigit* digit : *digits) {
    const unsigned int crate = digit->cellID().crate();
    const unsigned int channel = digit->cellID().channel();
    const double adc = fadc(digit->adc());
    if (crate == m_crateB) {
      m_hAdcB[channel]->fill(adc);
      m_hAdcVsChannelB->fill(channel, adc); 
    }
    else if (crate == m_crateF) {
      m_hAdcF[channel]->fill(adc);
      m_hAdcVsChannelF->fill(channel, adc); 
    }
    else {
      warning() << "Unexpected crate number (" << crate << ")" << endmsg;
    }
  }
  // Get only the mapped ones
  for(std::map< std::string , unsigned int>::iterator it = m_channelsFromName.begin(); it != m_channelsFromName.end(); ++it) {
    unsigned int adc = digits->object(LHCb::HCCellID(it->second))->adc();
    unsigned int channel = digits->object(LHCb::HCCellID(it->second))->cellID().channel();
    std::string chName = it->first;
    const unsigned int crate = digits->object(LHCb::HCCellID(it->second))->cellID().crate();
    unsigned int adc_ref = digits->object(m_refChannelsFromName[chName])->adc();
    float adcCor = 0.;
    if (!m_thetaConfig.empty()){
      adcCor = correctChannel(chName,adc,adc_ref, bxid% 2);
    }

    if (crate == m_crateB) {
      m_hAdcB[channel]->fill(adc);
      m_hAdcVsChannelB->fill(channel, adc); 
      if (!m_thetaConfig.empty()){
	m_hAdcFCor[channel]->fill(adcCor);
	m_hAdcVsChannelFCor->fill(channel, adcCor); 
      }
      if (even) {
	m_hAdcEvenB[channel]->fill(adc);
	m_hAdcVsChannelEvenB->fill(channel, adc);
	if (!m_thetaConfig.empty()){
	  m_hAdcEvenBCor[channel]->fill(adcCor);
	  m_hAdcVsChannelEvenBCor->fill(channel, adcCor);
	}
      } 
      else {
	m_hAdcOddB[channel]->fill(adc);
	m_hAdcVsChannelOddB->fill(channel, adc);
	if (!m_thetaConfig.empty()){
	  m_hAdcOddBCor[channel]->fill(adcCor);
	  m_hAdcVsChannelOddBCor->fill(channel, adcCor);
	}
      }
    } 
    else if (crate == m_crateF) {
      m_hAdcF[channel]->fill(adc);
      m_hAdcVsChannelF->fill(channel, adc); 
      if (!m_thetaConfig.empty()){
	m_hAdcFCor[channel]->fill(adcCor);
	m_hAdcVsChannelFCor->fill(channel, adcCor); 
      }
      if (even) {
	m_hAdcEvenF[channel]->fill(adc);
	m_hAdcVsChannelEvenF->fill(channel, adc);
	if (!m_thetaConfig.empty()){
	  m_hAdcEvenFCor[channel]->fill(adcCor);
	  m_hAdcVsChannelEvenFCor->fill(channel, adcCor); 
	}
      } 
      else {
	m_hAdcOddF[channel]->fill(adc);
	m_hAdcVsChannelOddF->fill(channel, adc);
	if (!m_thetaConfig.empty()){
	  m_hAdcOddFCor[channel]->fill(adcCor);
	  m_hAdcVsChannelOddFCor->fill(channel, adcCor); 
	}
      }
    }
    else {
      warning() << "Unexpected crate number (" << crate << ")" << endmsg;
    }
  }
  // Mapped but by quadrant only
  for (unsigned int s = 0 ; s < m_stations.size(); ++s){
    for ( int quadrant = 0 ; quadrant < 4 ; ++quadrant){
      const unsigned int index = quadrant * nStations + s;
      std::string chName = m_stations[s]+std::to_string(quadrant);
      unsigned int channelID = m_channelsFromName[chName];
      float adc = digits->object(channelID)->adc();
      unsigned int adc_ref = digits->object(m_refChannelsFromName[chName])->adc();
      float adcCor = 0.;
      if (!m_thetaConfig.empty()){
	adcCor = correctChannel(chName,adc,adc_ref, bxid% 2);
      }
      m_hAdcVsQuadrant[s]->fill(quadrant, adc);
      m_hAdcQuadrant[index]->fill(adc);
      m_hAdcVsBx[index]->fill(bxid, adc);
      if (!m_thetaConfig.empty()){
	m_hAdcVsQuadrantCor[s]->fill(quadrant, adcCor);
	m_hAdcQuadrantCor[index]->fill(adcCor);
	m_hAdcVsBxCor[index]->fill(bxid, adcCor);
      }
      if (even) {
	m_hAdcVsQuadrantEven[s]->fill(quadrant, adc);
	m_hAdcQuadrantEven[index]->fill(adc);
	if (!m_thetaConfig.empty()){
	  m_hAdcVsQuadrantEvenCor[s]->fill(quadrant, adcCor);
	  m_hAdcQuadrantEvenCor[index]->fill(adcCor);
	}
      } else { 
	m_hAdcVsQuadrantOdd[s]->fill(quadrant, adc);
	m_hAdcQuadrantOdd[index]->fill(adc);
	if (!m_thetaConfig.empty()){
	  m_hAdcVsQuadrantOddCor[s]->fill(quadrant, adcCor);
	  m_hAdcQuadrantOddCor[index]->fill(adcCor);
	}
      }
      if (bxtype == LHCb::ODIN::NoBeam) {
	m_hAdcVsQuadrantNoBeam[s]->fill(quadrant, adc);
	m_hAdcQuadrantNoBeam[index]->fill(adc);
	if (!m_thetaConfig.empty()){
	  m_hAdcVsQuadrantNoBeamCor[s]->fill(quadrant, adcCor);
	  m_hAdcQuadrantNoBeamCor[index]->fill(adcCor);
	}
      } else if (bxtype == LHCb::ODIN::BeamCrossing) {
	m_hAdcVsQuadrantBeam[s]->fill(quadrant, adc);
	m_hAdcQuadrantBeam[index]->fill(adc);
	if (!m_thetaConfig.empty()){
	  m_hAdcVsQuadrantBeamCor[s]->fill(quadrant, adcCor);
	  m_hAdcQuadrantBeamCor[index]->fill(adcCor);
	}
      }
      sum[s] += adc;
      if (!m_thetaConfig.empty())sumCor[s] += adcCor;
    }
    m_hAdcSum[s]->fill(sum[s]);
    if (!m_thetaConfig.empty())m_hAdcSumCor[s]->fill(sumCor[s]);
    if (even) {
      m_hAdcSumEven[s]->fill(sum[s]);
	if (!m_thetaConfig.empty())m_hAdcSumEvenCor[s]->fill(sumCor[s]);
    } else {
      m_hAdcSumOdd[s]->fill(sum[s]);
	if (!m_thetaConfig.empty()) m_hAdcSumOddCor[s]->fill(sumCor[s]);
    }
    if (bxtype == LHCb::ODIN::NoBeam) {
      m_hAdcSumNoBeam[s]->fill(sum[s]);
	if (!m_thetaConfig.empty())m_hAdcSumNoBeamCor[s]->fill(sumCor[s]);
    } else if (bxtype == LHCb::ODIN::BeamCrossing) {
      m_hAdcSumBeam[s]->fill(sum[s]);
	if (!m_thetaConfig.empty())m_hAdcSumBeamCor[s]->fill(sumCor[s]);
    }
  }
  

  return StatusCode::SUCCESS;
}

//=============================================================================
// Finalisation
//=============================================================================
StatusCode HCDigitMonitor::finalize() {

  if (!m_variableBins) return HCMonitorBase::finalize();
  const unsigned int nChannels = 64;
  for (unsigned int i = 0; i < nChannels; ++i) {
    scale(m_hAdcB[i]);
    scale(m_hAdcF[i]);
    scale(m_hAdcEvenB[i]);
    scale(m_hAdcEvenF[i]);
    scale(m_hAdcOddB[i]);
    scale(m_hAdcOddF[i]);
  }
  const unsigned int nStations = 5;
  for (unsigned int i = 0; i < 4; ++i) {
    for (unsigned int j = 0; j < nStations; ++j) {
      const unsigned int index = i * nStations + j;
      scale(m_hAdcQuadrant[index]);
      scale(m_hAdcQuadrantEven[index]);
      scale(m_hAdcQuadrantOdd[index]);
      scale(m_hAdcQuadrantNoBeam[index]);
      scale(m_hAdcQuadrantBeam[index]);
    }
  }
  return HCMonitorBase::finalize();
}

