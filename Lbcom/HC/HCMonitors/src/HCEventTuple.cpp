// LHCb
#include "Event/ODIN.h"
#include "Event/HCDigit.h"

// Local
#include "HCEventTuple.h"

DECLARE_ALGORITHM_FACTORY(HCEventTuple)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HCEventTuple::HCEventTuple(const std::string& name, ISvcLocator* pSvcLocator)
    : HCMonitorBase(name, pSvcLocator), m_tag(0) {

  declareProperty("DigitLocation", 
                  m_digitLocation = LHCb::HCDigitLocation::Default);
  declareProperty("CorrectedDigitLocation", 
                  m_correctedDigitLocation = "");

}

//=============================================================================
// Destructor
//=============================================================================
HCEventTuple::~HCEventTuple() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode HCEventTuple::initialize() {

  StatusCode sc = HCMonitorBase::initialize();
  if (sc.isFailure()) return sc;


  const std::string tmp = name();
  size_t posPrev = tmp.find("Prev");
  size_t posNext = tmp.find("Next");
  std::string tae = "";
  if (std::string::npos != posPrev) {
    tae = tmp.substr(posPrev, 5);
    if (!isdigit(tae.back())) {
      warning() << "Invalid TAE slot (" << tae << ")" << endmsg;
      tae = "";
    } else {
      m_tag = -1 * int(tae.back());
    }
  } else if (std::string::npos != posNext) {
    tae = tmp.substr(posNext, 5);
    if (!isdigit(tae.back())) {
      warning() << "Invalid TAE slot (" << tae << ")" << endmsg;
      tae = "";
    } else {
      m_tag = int(tae.back());
    }
  }

  if (tae != "") {
    m_digitLocation = tae + "/" + m_digitLocation;
  }
  m_corrected = !m_correctedDigitLocation.empty();


  IUpdateManagerSvc* mgrSvc=svc<IUpdateManagerSvc>("UpdateManagerSvc", true);  
  m_condition_path = "Conditions/Online/LHC";
  try
  {
    mgrSvc->registerCondition(this, m_condition_path,
                              &HCEventTuple::myCallBackFunction,
                              m_FillNumberCondition); 
  }
  catch (GaudiException &e)
  { 
    Error("Can not register Condition "+m_condition_path, StatusCode::SUCCESS,50).ignore(); 
    return StatusCode::SUCCESS;
  }

  m_saved = 0;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HCEventTuple::execute() {

  const LHCb::ODIN* odin = getIfExists<LHCb::ODIN>(LHCb::ODINLocation::Default);
  if (!odin) {
    return Error("Cannot retrieve ODIN.", StatusCode::SUCCESS);
  }
  if (msgLevel(MSG::DEBUG)) {
    debug() << "Run " << odin->runNumber() << ", Event " << odin->eventNumber()
            << endmsg;
  }
  const unsigned int runodin = odin->runNumber();
  const ulonglong eventodin = odin->eventNumber();
  const ulonglong orbitodin = odin->orbitNumber();
  const unsigned int bunchid = odin->bunchId() + m_tag;
  const ulonglong evTimeGps = odin->gpsTime();
  //const int step = odin->calibrationStep();
  const LHCb::HCDigits* digits = getIfExists<LHCb::HCDigits>(m_digitLocation);
  if (!digits) {
    return Error("No digits in " + m_digitLocation, StatusCode::SUCCESS);
  }
  const LHCb::HCDigits* correctedDigits = nullptr;
  if (m_corrected){
    correctedDigits = getIfExists<LHCb::HCDigits>(m_correctedDigitLocation);
    if (!correctedDigits) {
      return Error("No digits in " + m_correctedDigitLocation, StatusCode::SUCCESS);
    }
  }
  
  std::vector<double> adcB(64, 0.);
  std::vector<double> adcF(64, 0.);
  for (const LHCb::HCDigit* digit : *digits) {
    const int channel = int(digit->cellID().cellID() & 0x3F);
    if (digit->cellID().crate() == 0) {
      adcB[channel] = digit->adc();
    } else {
      adcF[channel] = digit->adc();
    }
  }



  Tuple tuple = nTuple(name(), "Herschel digits");
  if( m_saved == 0) 
  {
    tuple->column("run", runodin);
    tuple->column("eventID", eventodin);
    tuple->column("orbitID", orbitodin);
    tuple->column("bxID", bunchid);
    tuple->column("odinTime", evTimeGps);
    //tuple->column("slot", m_tag);
    //tuple->column("step", step);
    //tuple->array("adc_B", adcB.begin(), adcB.end());
    //tuple->array("adc_F", adcF.begin(), adcF.end());
    const Gaudi::Time gtime = odin->eventTime();
    tuple->column( "GpsYear",  gtime.year(false) );
    tuple->column( "GpsMonth", gtime.month(false) );
    tuple->column( "GpsDay", gtime.day(false) );
    tuple->column( "GpsHour", gtime.hour(false) );
    tuple->column( "GpsMinute", gtime.minute(false) );
    tuple->column( "GpsSecond", gtime.second(false) + (gtime.nsecond()/1e9) );
    //Add FillNumber                                                                                                                                                                    
    m_FillNumber = -1;
    myCallBackFunction();  
    ////////////////      
    tuple->column("fill", m_FillNumber);
    tuple->write();
    m_saved = 1;
  }  



  const std::vector<std::string> stations = {"B0", "B1", "B2", "F1", "F2"};
  const unsigned int nStations = 5;
  for (unsigned int i = 0; i < nStations; ++i) {
    for (unsigned int j = 0; j < 4; ++j) {
      // Skip masked channels.
      if (m_masked[i][j]) continue;
      const std::string ch = stations[i] + std::to_string(j);
      LHCb::HCCellID id(m_channels[i][j]);
      const LHCb::HCDigit* digit = digits->object(id);
      if (!digit) {
        warning() << "Cannot retrieve digit for " << ch << endmsg;
        continue;
      }
      tuple->column(ch, digit->adc());
      LHCb::HCCellID refid(m_references[i][j]);
      const LHCb::HCDigit* refdigit = digits->object(refid);
      if (!refdigit) {
        warning() << "Cannot retrieve reference digit for " << ch << endmsg;
        continue;
      }
      tuple->column(ch + "_reference", refdigit->adc());
      if (!m_corrected) continue;
      const LHCb::HCDigit* cordigit = correctedDigits->object(id);
      if (!cordigit) {
        warning() << "Cannot retrieve corrected digit for " << ch << endmsg;
        continue;
      }
      tuple->column(ch + "_cor", cordigit->adc());
    }
  }
 
  return StatusCode::SUCCESS;
}




//========================================================================                                                                                                               
// call back functions for condDB                                                                                                                                                        
//========================================================================                                                                                                               
StatusCode HCEventTuple::myCallBackFunction()
{
  m_FillNumber = m_FillNumberCondition->param<int>("FillNumber");
  //info()<<"Fill Number = "<<m_FillNumber<<endmsg;
  return StatusCode::SUCCESS; 
}







