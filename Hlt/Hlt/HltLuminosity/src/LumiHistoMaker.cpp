// $Id: LumiHistoMaker.cpp,v 1.13 2009-09-30 07:24:56 graven Exp $
// Include files 
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IAlgManager.h"

#include "Event/LumiCounters.h"

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
  debug() << "list of histograms booked:" << endmsg;
  int i=0;
  for(std::vector< std::string >::iterator ivar = m_Variables.begin() ; 
      ivar!= m_Variables.end() ; ++ivar, ++i ){
    int counter = LHCb::LumiCounters::counterKeyToType(*ivar);
    if ( counter == LHCb::LumiCounters::Unknown ) {
      warning() << "LumiCounter not found with name: " << *ivar <<  endmsg;
    } else {
      //Generate Histos using known number and name of variables
      int bins = m_MaxBin;
      if (useMaxBins) {
	bins = m_MaxBins[i];
      }
      AIDA::IHistogram1D* countHisto = initializeHisto(*ivar, -0.5, double(bins)-0.5, bins);
      m_Histos.push_back(countHisto);  
      m_names.push_back(*ivar);
      m_keys.push_back(counter);

      // create histograms to hold the counts above threshold
      AIDA::IHistogram1D* digitHisto = initializeHisto(*ivar+"_threshold", -0.5, 1.5, 2);
      m_ThresholdHistos.push_back(digitHisto);  

      debug() << "Variable: " << *ivar << " Key: " << counter << 
	" Bins: " << bins << " Threshold: " << m_Thresholds[i] << endmsg;
    }
  }
  debug() << m_keys.size()<< " variables used " << endmsg ;
  debug() << "Initialised Lumi Histo Maker" << endmsg ;
  return sc;

}

//---------------------------------------------------------

StatusCode LumiHistoMaker::execute() {

  StatusCode sc = StatusCode::SUCCESS;  
  counter("#entries") += 1;

  // get data container
  m_HltLumiSummary = get<LHCb::HltLumiSummary>(m_DataName);

  // fill histos
  for ( unsigned int i = 0; i < m_Histos.size(); ++i) {
    std::string cname = m_names[i];
    int counter = m_keys[i];
    AIDA::IHistogram1D *histo = m_Histos[i];
    AIDA::IHistogram1D *thresholdHisto = m_ThresholdHistos[i];
    int ivalue = m_HltLumiSummary->info(counter,-1);
    fill(histo, (double) ivalue, 1.);
    double digit=0;
    if ( ivalue > m_Thresholds[i] ) digit=1.;
    fill(thresholdHisto, digit, 1.);
    if ( msgLevel(MSG::DEBUG) ) debug() << "histo:" << cname << " value " << ivalue << " threshold " << digit<< endmsg;
  }
  
  counter("#HistoInputs") += 1;
  setDecision(true);
  return sc;

}

void LumiHistoMaker::setDecision(bool ok) {
  setFilterPassed(ok);
}
