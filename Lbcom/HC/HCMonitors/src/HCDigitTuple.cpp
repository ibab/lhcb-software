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
  return StatusCode::SUCCESS;
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

  for(std::map< std::string , unsigned int>::iterator it = m_channelsFromName.begin(); it != m_channelsFromName.end(); ++it) {
    unsigned int adc = digits->object(it->second)->adc();
    std::string chName = it->first;
    unsigned int adc_ref = digits->object(m_refChannelsFromName[chName])->adc();
    //unsigned int channel = digits->object(it->second)->cellID().channel();
    tuple->column(chName , adc );
    tuple->column(chName+"_reference" , adc_ref );
    if (!m_thetaConfig.empty()){
      tuple->column(chName+"_Cor" ,correctChannel(chName,adc,adc_ref, bunchid% 2)  );
    }
  } 
  tuple->write();
  return StatusCode::SUCCESS;
}

