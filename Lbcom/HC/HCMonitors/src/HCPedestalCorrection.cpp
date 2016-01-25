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
  const std::vector<std::string> stations = {"B0", "B1", "B2", "F1", "F2"};
  const std::vector<std::string> par = {"Even", "Odd"};
  const unsigned int nStations = 5;
  m_hCorrelation.resize(nStations);
  for (unsigned int i = 0; i < nStations; ++i) {
    const std::string st = stations[i];
    m_hCorrelation[i].resize(4);
    for (unsigned int j = 0; j < 4; ++j) {
      const std::string qu = std::to_string(j);
      for (unsigned int k = 0; k < 2; ++k) {
        const std::string name = "Correlation/" + st + "/" + qu + "/" + par[k];
        //m_hCorrelation[i][j].push_back(book2D(name, qu, 50., 300., 50, 15., 200., 50));
        m_hCorrelation[i][j].push_back(book2D(name, qu, 0, 500., 250, 0, 500., 250));
        //lowX=0, highX=100, binsX=50, lowY=0, highY=100, binsY=50)
        setAxisLabels(m_hCorrelation[i][j][k], "Reference ADC", "ADC");
      }
    }
  }
  // TODO
  /*
      std::string titleS = it->first + parity[p] + "_gaus2dfit";
      char* cstr = new char[titleS.length() + 1];
      std::strcpy(cstr, titleS.c_str());
      m_fit[it->first + parity[p]] = new TF2(
          cstr,
          "[0]*exp(-(cos([3])*cos([3])/(2*[4]*[4])+sin([3])*sin([3])/"
          "(2*[5]*[5]))*(x-[1])*(x-[1])-2*(-sin(2*[3])/(4*[4]*[4])+sin(2*[3])/"
          "(4*[5]*[5]))*(x-[1])*(y-[2])-(sin([3])*sin([3])/"
          "(2*[4]*[4])+cos([3])*cos([3])/(2*[5]*[5]))*(y-[2])*(y-[2]))");
  */


  m_bxID = book1D("bxID", "bxID" , 0, 4000., 4000);
  setAxisLabels(m_bxID, "bxID", "Events");

  return StatusCode::SUCCESS;

}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HCPedestalCorrection::execute() {

  const LHCb::ODIN* odin = getIfExists<LHCb::ODIN>(LHCb::ODINLocation::Default);
  if (!odin) {
    return Error("Cannot retrieve ODIN.", StatusCode::SUCCESS);
  }
  if (msgLevel(MSG::DEBUG)) {
    debug() << "Run " << odin->runNumber() << ", Event " << odin->eventNumber()
            << endmsg;
  }
  const unsigned int bxID = odin->bunchId();
  const unsigned int parity = bxID % 2;


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
        info() << "Cannot retrieve digit." << endmsg;
        continue;
      }
      const int adc = digit->adc();
      LHCb::HCCellID refid(m_references[i][j]);
      const LHCb::HCDigit* refdigit = digits->object(refid);
      if (!refdigit) {
        info() << "Cannot retrieve reference digit." << endmsg;
        continue;
      }
      const int refadc = refdigit->adc();
      m_hCorrelation[i][j][parity]->fill(refadc, adc);
    }
  }



  /*
  //Tuple general Info                                                                                                                                                                  
  Tuple tuple = nTuple(name(), "General Info");
  const unsigned int runodin = odin->runNumber();
  const ulonglong eventodin = odin->eventNumber();
  const ulonglong orbitodin = odin->orbitNumber();
  const unsigned int bunchid = odin->bunchId();
  const ulonglong evTimeGps = odin->gpsTime();
  tuple->column("run", runodin);
  tuple->column("eventID", eventodin);
  tuple->column("orbitID", orbitodin);
  tuple->column("bxID", bunchid);
  tuple->column("odinTime", evTimeGps);
  */

  m_bxID->fill(odin->bunchId());

  return StatusCode::SUCCESS;
}

//=============================================================================
// Finalisation
//=============================================================================
StatusCode HCPedestalCorrection::finalize() {
  /*
  const unsigned int nStations = 5;

  std::vector<std::string> stations = {"B0", "B1", "B2", "F1", "F2"};
  std::vector<std::string> parity = {"Odd", "Even"};

  for(std::map< std::string , unsigned int>::iterator it =
  m_channelsFromName.begin(); it != m_channelsFromName.end(); ++it) {
      for (unsigned int p = 0 ; p < 2 ; ++p ){
        TH2D* profile =
  Gaudi::Utils::Aida2ROOT::aida2root(m_hCorrelation[it->first+parity[p]]);
        std::string  index = it->first+parity[p];
        double rmsX = profile->GetRMS(1);
        double rmsY = profile->GetRMS(2);
        double  theta = atan(rmsY/rmsX);
        int N = profile->GetEntries();
        m_fit[index]->SetParameter(0,N);
        double meanX = profile->GetMean(1);
        double meanY = profile->GetMean(2);
        m_fit[index]->SetParameter(1,meanX);
        m_fit[index]->SetParameter(2,meanY);
        m_fit[index]->SetParameter(3,theta);
        m_fit[index]->SetParLimits(3,0.5,1.5);
        m_fit[index]->SetParameter(4,5);
        m_fit[index]->SetParameter(5,20);
        m_fit[index]->SetParLimits(4,2.,15.);
        m_fit[index]->SetParLimits(5,10.,60.);
        m_fit[index]->SetContour(5);
        m_fit[index]->SetNpy(300);
        m_fit[index]->SetNpx(300);
        std::string titleS = stations[i]+ std::to_string(j)+
  parity[p]+"_gaus2dfit";
        char * cstr = new char [titleS.length()+1];
        std::strcpy (cstr, titleS.c_str());
        profile->Fit(cstr,"QS");
        theta=m_fit[index]->GetParameter(3);
        double x0=m_fit[index]->GetParameter(1);
        double y0=m_fit[index]->GetParameter(2);
        rmsX=m_fit[index]->GetParameter(4);
        rmsY=m_fit[index]->GetParameter(5);
        always()<<stations[i]+ std::to_string(j)+ parity[p]<<" "<<theta<<"
  "<<x0<<" "<<y0<<endreq;
      }
    }
    }  */
  return HCMonitorBase::finalize();
}
