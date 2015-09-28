// Gaudi
#include "GaudiUtils/HistoLabels.h"
#include "GaudiUtils/Aida2ROOT.h"

// LHCb
// Event/DAQEvent
#include "Event/ODIN.h"
// Event/DigiEvent
#include "Event/HCDigit.h"

#include <stdlib.h>
// Local
#include "HCDelayScan.h"
using namespace Gaudi::Utils::Histos;

DECLARE_ALGORITHM_FACTORY(HCDelayScan)

//=============================================================================
// Standard constructor
//=============================================================================
HCDelayScan::HCDelayScan(const std::string& name, ISvcLocator* pSvcLocator)
    : HCMonitorBase(name, pSvcLocator),
      m_odin(NULL),
  m_minStep(0),
  m_BxRange({-1,10000}),
  m_parAdc("", -0.5, 1023.5, 1023) {

  declareProperty("DigitLocation", m_digitLocation = LHCb::HCDigitLocation::Default);
  declareProperty("ParametersADC", m_parAdc);
  declareProperty("MinimumStepNr", m_minStep);
  declareProperty("MaximumStepNr", m_maxStep);
  declareProperty("VFEClkPitch", m_VFEClkPitch);
  declareProperty("ADCClkPitch", m_ADCClkPitch);
  declareProperty("BxRange", m_BxRange);
  declareProperty("BxPrev", m_BxPrev);
  declareProperty("BxCentral", m_BxCentral);
  declareProperty("BxNext", m_BxNext);
}

//=============================================================================
// Destructor
//=============================================================================
HCDelayScan::~HCDelayScan() {}

//=============================================================================
// Initialisation
//=============================================================================
StatusCode HCDelayScan::initialize() {

  // Initialise the base class.
  StatusCode sc = HCMonitorBase::initialize();
  if (sc.isFailure()) return sc;

  // Get ODIN.
  m_odin = tool<IEventTimeDecoder>("OdinTimeDecoder", "OdinDecoder", this);

  // Setup the histograms.
  std::vector<std::string> m_slots = {"Central","Previous",  "Next"};
  for(std::map< std::string , unsigned int>::iterator it = m_channelsFromName.begin(); it != m_channelsFromName.end(); ++it) {
    std::vector<TProfile*> myAdcs;
    std::vector<TProfile*> myAdcsCor;
    std::vector<std::vector<TH1D*>> myAdcs_step;
    std::vector<std::vector<TH1D*>> myAdcs_stepCor;
    std::string station = it->first;
    station.erase(2,1);
    std::string quad = it->first;
    quad.erase(0,2);
    for (unsigned int j = 0 ; j < m_slots.size() ; ++j){
	std::string sl = m_slots[j];
	std::string name = "ADC/"+sl+"/" + station+"/"+quad;
	std::string nameCor = "ADCCor/"+sl+"/" + station+"/"+quad;
	myAdcs.push_back(Gaudi::Utils::Aida2ROOT::aida2root(bookProfile1D(name, name, m_minStep-0.5, m_maxStep+0.5, m_maxStep-m_minStep+1)));
	if (!m_thetaConfig.empty()) myAdcsCor.push_back(Gaudi::Utils::Aida2ROOT::aida2root(bookProfile1D(nameCor, nameCor, m_minStep-0.5, m_maxStep+0.5, m_maxStep-m_minStep+1)));
	std::vector<TH1D*> perstep;
	std::vector<TH1D*> perstepCor;
	for (int kk = m_minStep; kk < m_maxStep ; kk++){
	  std::string u  = std::to_string(kk);
	  std::string name = "ADC/"+u+"/"+station+"/"+quad+"/"+sl;
	  std::string nameCor = "ADCCor/"+u+"/"+station+"/"+quad+"/"+sl;
	  perstep.push_back(Gaudi::Utils::Aida2ROOT::aida2root(book1D(name, name, -0.5, 1023+0.5, 1024)));
	  if (!m_thetaConfig.empty())perstepCor.push_back(Gaudi::Utils::Aida2ROOT::aida2root(book1D(nameCor, nameCor, -200.5, 1223+0.5, 1424)));
	}
	myAdcs_step.push_back(perstep);
	if (!m_thetaConfig.empty())myAdcs_stepCor.push_back(perstepCor);
    }
    m_adcs[it->first]=myAdcs;
    m_adcsPerStep[it->first]=myAdcs_step;
    if (!m_thetaConfig.empty()){
      m_adcsCor[it->first]=myAdcsCor;
      m_adcsPerStepCor[it->first]=myAdcs_stepCor;
    }
  }
  return StatusCode::SUCCESS;
}

unsigned int HCDelayScan::randomizeADC( unsigned int adc ) {
  if( adc<128.5 ) return adc;
  else if (adc<255.5) return adc+(unsigned int)rand()%2;
  else if (adc<511.5) return adc+(unsigned int)rand()%8;
  else return adc+(unsigned int)rand()%16;
  
}


//=============================================================================
// Main execution
//=============================================================================
StatusCode HCDelayScan::execute() {

  // Get event information from ODIN.
  m_odin->getTime();
  const LHCb::ODIN* odin = getIfExists<LHCb::ODIN>(LHCb::ODINLocation::Default);
  if (!odin) {
    return Error("Cannot retrieve ODIN", StatusCode::SUCCESS);
  }
  const unsigned int bxid = odin->bunchId();

  std::string i_location = "Central";

  if (bxid == m_BxPrev)i_location = "Previous";
  if (bxid == m_BxNext)i_location = "Next";


  if ( (int) bxid < m_BxRange[0] || (int) bxid > m_BxRange[1]) return StatusCode::SUCCESS;
  const int step = odin->calibrationStep();

  if (step<m_minStep || step>m_maxStep) return StatusCode::SUCCESS;

  const unsigned int runodin = odin->runNumber();
  const ulonglong eventodin = odin->eventNumber();
  
  srand (runodin*eventodin);

  const LHCb::HCDigits* digits = getIfExists<LHCb::HCDigits>(m_digitLocation);
  if (!digits) {
    return Error("No digits in " + m_digitLocation, StatusCode::FAILURE);
  }
  // Loop over the digits.
  for(std::map< std::string , unsigned int>::iterator it = m_channelsFromName.begin(); it != m_channelsFromName.end(); ++it) {
    unsigned int adc = digits->object(it->second)->adc();
    std::string chName = it->first;
    unsigned int adc_ref = digits->object(m_refChannelsFromName[chName])->adc();
    int loc = 0;
    if (i_location == "Previous") loc = 1;
    if (i_location == "Next") loc = 2;
    m_adcs[chName][loc]->Fill(step,randomizeADC(adc));
    m_adcsPerStep[chName][loc][step-m_minStep]->Fill(randomizeADC(adc));
    if (!m_thetaConfig.empty()){
      float adc_Cor =  correctChannel(chName,adc,adc_ref, bxid% 2);
      m_adcsCor[chName][loc]->Fill(step,adc_Cor);
      m_adcsPerStepCor[chName][loc][step-m_minStep]->Fill(adc_Cor);
    }
  }
  return StatusCode::SUCCESS;
}


StatusCode HCDelayScan::finalize() {
  /*for (unsigned int i = 0; i < m_stations.size(); ++i) {
    for (unsigned int k = 0; k < m_quadrants.size(); ++k) {
      for (int ix = 0 ; ix< m_adcs[0][i][k]->GetNbinsX()+1;++ix){
	int step = (int)m_adcs[0][i][k]->GetXaxis()->GetBinCenter(ix) ;
	int vfeclk = (step+m_minStep) % ( 32/m_VFEClkPitch ) ;
	int adcclk = (step+m_minStep) / ( 32/m_ADCClkPitch ) ;
	double adcmax = -1.;
	int bestslot = -2;
	for (unsigned int j = 0 ; j < 3 ; ++j){
	  double val = m_adcs[j][i][k]->GetBinContent(m_adcs[j][i][k]->GetXaxis()->FindBin(step));
	  if (val>adcmax){
	    adcmax = val;
	    bestslot = j-1;
	  }
	  m_scanResults[i][k]->Fill(vfeclk,adcclk,adcmax);
	  m_scanOffset[i][k]->Fill(vfeclk,adcclk,bestslot);
	}
      }
    }
    }*/

  StatusCode sc = HCMonitorBase::finalize();
  if (sc.isFailure()) return sc;
  return StatusCode::SUCCESS;
}
