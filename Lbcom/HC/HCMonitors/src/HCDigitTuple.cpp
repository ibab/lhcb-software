// LHCb
#include "Event/ODIN.h"
#include "Event/HCDigit.h"

// Local
#include "HCDigitTuple.h"

DECLARE_ALGORITHM_FACTORY(HCDigitTuple)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HCDigitTuple::HCDigitTuple(const std::string& name,
                           ISvcLocator* pSvcLocator)
    : HCMonitorBase(name, pSvcLocator),
      m_tag(0) {

}

//=============================================================================
// Destructor
//=============================================================================
HCDigitTuple::~HCDigitTuple() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HCDigitTuple::initialize() {

  StatusCode sc = HCMonitorBase::initialize();
  if (sc.isFailure()) return sc;

  const std::string tmp = name();
  size_t posPrev = tmp.find("Prev");
  size_t posNext = tmp.find("Next");
  std::string tae = "";
  if (std::string::npos != posPrev) {
    tae = tmp.substr(posPrev, 5);
  } else if (std::string::npos != posNext) {
    tae = tmp.substr(posNext, 5);
  }
  if (tae == "") m_tag = 0;
  else if (tae == "Prev1") m_tag = -1;
  else if (tae == "Next1") m_tag =  1;
  else if (tae == "Prev2") m_tag = -2;
  else if (tae == "Next2") m_tag =  2;
  else if (tae == "Prev3") m_tag = -3;
  else if (tae == "Next3") m_tag =  3;
  else if (tae == "Prev4") m_tag = -4;
  else if (tae == "Next4") m_tag =  4;
  else if (tae == "Prev5") m_tag = -5;
  else if (tae == "Next5") m_tag =  5;
  else if (tae == "Prev6") m_tag = -6;
  else if (tae == "Next6") m_tag =  6;
  else if (tae == "Prev7") m_tag = -7;
  else if (tae == "Next7") m_tag =  7;

  m_digitLocation = LHCb::HCDigitLocation::Default; 
  if (tae != "") {
    m_digitLocation = tae + "/" + m_digitLocation;
  }

  addQuadrants(m_channelsB0, "B0", true); 
  addQuadrants(m_channelsB1, "B1", true); 
  addQuadrants(m_channelsB2, "B1", true); 
  addQuadrants(m_channelsF1, "F1", false); 
  addQuadrants(m_channelsF2, "F2", false); 
  return StatusCode::SUCCESS;
}

//=============================================================================
// Add the labels and channel numbers for one station to the list.
//=============================================================================
void HCDigitTuple::addQuadrants(const std::vector<unsigned int>& channels,
                                const std::string& station, const bool bwd) {

  const unsigned int nChannels = channels.size();
  if (nChannels != 5) {
    warning() << "Spare channel of station " << station << "is not defined." 
              << endmsg;
  }
  for (unsigned int i = 0; i < nChannels; ++i) {
    if (i == 4) {
      if (bwd) {
        m_labelsB.push_back(station + "S");
      } else {
        m_labelsF.push_back(station + "S");
      }
    } else {
      if (bwd) {
        m_labelsB.push_back(station + std::to_string(i));
      } else {
        m_labelsF.push_back(station + std::to_string(i));
      }
    }
    if (bwd) {
      m_channelsB.push_back(channels[i]);
    } else {
      m_channelsF.push_back(channels[i]);
    }
  }
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HCDigitTuple::execute() {

  const LHCb::ODIN* odin = getIfExists<LHCb::ODIN> (LHCb::ODINLocation::Default);
  if (!odin) {
    return Error("Cannot retrieve ODIN.", StatusCode::SUCCESS);
  }
  if (msgLevel(MSG::DEBUG)) { 
    debug() << "Run " << odin->runNumber() 
            << ", Event " << odin->eventNumber() << endmsg;
  }
  const unsigned int runodin = odin->runNumber();
  const ulonglong eventodin = odin->eventNumber();
  const ulonglong orbitodin = odin->orbitNumber();
  const unsigned int bunchid = odin->bunchId() + m_tag;
  const ulonglong evTimeGps = odin->gpsTime();
  const int step = odin->calibrationStep();

  const LHCb::HCDigits* digits = getIfExists<LHCb::HCDigits>(m_digitLocation);
  if (!digits) {
    return Error("No digits in " + m_digitLocation, StatusCode::SUCCESS);
  } 

  std::vector<double> adcB(64, 0);
  std::vector<double> adcF(64, 0);
  for (const LHCb::HCDigit* digit : *digits) {
    const int channel = int(digit->cellID().cellID() & 0x3F);
    if (digit->cellID().crate() == 0) {
      adcB[channel] = digit->adc();
    } else {
      adcF[channel] = digit->adc();
    }
  }

  Tuple tuple = nTuple("HCDigitNtuple", "Herschel digits");
  tuple->column("run", runodin);
  tuple->column("eventID", eventodin);
  tuple->column("orbitID", orbitodin);
  tuple->column("bxID", bunchid);
  tuple->column("odinTime", evTimeGps); 
  tuple->column("slot", m_tag); 
  tuple->column("step", step); 
  tuple->array("adc_B", adcB.begin(), adcB.end());
  tuple->array("adc_F", adcF.begin(), adcF.end());
  const unsigned int nChannelsB = m_labelsB.size();
  for (unsigned int i = 0; i < nChannelsB; ++i) {
    tuple->column(m_labelsB[i], adcB[m_channelsB[i]]);
  }
  const unsigned int nChannelsF = m_labelsF.size();
  for (unsigned int i = 0; i < nChannelsF; ++i) {
    tuple->column(m_labelsF[i], adcF[m_channelsF[i]]);
  }
  tuple->write();
  return StatusCode::SUCCESS;
}

