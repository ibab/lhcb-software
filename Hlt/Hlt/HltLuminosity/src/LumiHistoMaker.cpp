// $Id: LumiHistoMaker.cpp,v 1.8 2008-12-19 17:35:58 graven Exp $
// Include files 
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IAlgManager.h"

#include "HltBase/ANNSvc.h"

#include "LumiHistoMaker.h"


//-----------------------------------------------------------------------------
// Implementation file for class : LumiHistoMaker
//
// 2008-07-24 : Antonio Perez-Calero
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( LumiHistoMaker );



LumiHistoMaker::LumiHistoMaker( const std::string& name,
                            ISvcLocator* pSvcLocator )
  : HltBaseAlg ( name , pSvcLocator )
  , m_counterEntries("Entries")
  , m_counterHistoInputs("HistoInputs")
{
  
  declareProperty("HistogramUpdatePeriod" , m_histogramUpdatePeriod = 1 );
  declareProperty("InputDataContainer",  m_DataName = LHCb::HltLumiSummaryLocation::Default );
  declareProperty("InputVariables",  m_Variables );
  declareProperty("MaxBins",         m_MaxBins );
  declareProperty("Thresholds",      m_Thresholds );
  declareProperty("MaxBin",          m_MaxBin = 500 );
  declareProperty("Threshold",       m_Threshold = 5 );
}

LumiHistoMaker::~LumiHistoMaker()
{
  //delete m_outputSelection;
} 


StatusCode LumiHistoMaker::initialize() {

  StatusCode sc = HltBaseAlg::initialize(); 
  if ( sc.isFailure() ) return sc;

  IANNSvc* annSvc = svc<IANNSvc>("LumiANNSvc");

  // book a histogram for each recognized counter name
  m_Histos.clear();
  m_names.clear();
  m_keys.clear();
  
  if (m_Variables.size()!=m_MaxBins.size() && !m_MaxBins.empty()) {
    error() << "wrong numer of histogram maximum bin values" << endmsg;
    return StatusCode::FAILURE;
  }
  if (m_Variables.size()!=m_Thresholds.size() && !m_Thresholds.empty()) {
    error() << "wrong numer of threshold values" << endmsg;
    return StatusCode::FAILURE;
  }

  // set the default threshold
  if (m_Thresholds.empty()) {
    for(std::vector< std::string >::iterator ivar = m_Variables.begin() ; 
	ivar!= m_Variables.end() ; ++ivar ){  
      m_Thresholds.push_back(m_Threshold);
    }
  }

  // create the histograms
  bool useMaxBins = (m_Variables.size()==m_MaxBins.size());
  debug() << "list of histograms booked:" << endreq;
  int i=0;
  for(std::vector< std::string >::iterator ivar = m_Variables.begin() ; 
      ivar!= m_Variables.end() ; ++ivar, ++i ){
    boost::optional<IANNSvc::minor_value_type> x = annSvc->value("LumiCounters", *ivar);
    if (!x) {
      warning() << "LumiCounter not found with name: " << *ivar <<  endmsg;
    } else {
      int counter = x->second;
      //Generate Histos using known number and name of variables
      int bins = m_MaxBin;
      if (useMaxBins) {
	bins = m_MaxBins[i];
      }
      Hlt::Histo* countHisto = initializeHisto(*ivar, -0.5, double(bins)-0.5, bins);
      m_Histos.push_back(countHisto);  
      m_names.push_back(*ivar);
      m_keys.push_back(counter);

      // create histograms to hold the counts above threshold
      Hlt::Histo* digitHisto = initializeHisto(*ivar+"_threshold", -0.5, 1.5, 2);
      m_ThresholdHistos.push_back(digitHisto);  

      info() << "Variable: " << *ivar << " Key: " << counter << 
	" Bins: " << bins << " Threshold: " << m_Thresholds[i] << endreq;
    }
  }
  debug() << m_keys.size()<< " variables used " << endmsg ;
  debug() << "Initialised Lumi Histo Maker" << endmsg ;
  return sc;

}

//---------------------------------------------------------

StatusCode LumiHistoMaker::execute() {

  StatusCode sc = StatusCode::SUCCESS;  
  m_counterEntries.increase(1);

  // get data container
  m_HltLumiSummary = get<LHCb::HltLumiSummary>(m_DataName);

  // fill histos
  for ( unsigned int i = 0; i < m_Histos.size(); ++i) {
    std::string cname = m_names[i];
    int counter = m_keys[i];
    Hlt::Histo *histo = m_Histos[i];
    Hlt::Histo *thresholdHisto = m_ThresholdHistos[i];
    int ivalue = m_HltLumiSummary->info(counter,-1);
    fillHisto(*histo, (double) ivalue, 1.);
    double digit=0;
    if ( ivalue > m_Thresholds[i] ) digit=1.;
    fillHisto(*thresholdHisto, digit, 1.);
    debug() << "histo:" << cname << " value " << ivalue << " threshold " << digit<< endmsg;
  }
  
  m_counterHistoInputs.increase(1);
  setDecision(true);
  return sc;

}

void LumiHistoMaker::setDecision(bool ok) {
  setFilterPassed(ok);
}

////  Finalize
StatusCode LumiHistoMaker::finalize() {
  debug() << "LumiHistoMaker::finalize()" << endmsg;
  infoTotalEvents(m_counterEntries);
  infoSubsetEvents(m_counterEntries,m_counterHistoInputs, " events to Histos ");

  return HltBaseAlg::finalize();
}
