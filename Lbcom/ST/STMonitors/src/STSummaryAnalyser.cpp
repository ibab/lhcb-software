// $Id: STSummaryAnalyser.cpp,v 1.2 2009-03-10 13:55:07 cattanem Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"

// LHCbKernel
#include "Kernel/STDetSwitch.h"
#include "Kernel/STDAQDefinitions.h"
#include "Kernel/LHCbConstants.h"

// STTELL1Event
#include "Event/STTELL1Data.h"
#include "Event/STSummary.h"

// AIDA
#include "AIDA/IHistogram1D.h"

// standard
#include "gsl/gsl_math.h"
#include "boost/lexical_cast.hpp"

// local
#include "STSummaryAnalyser.h"

using namespace LHCb;
using namespace AIDA;
using namespace STDAQ;
using namespace STBoardMapping;

DECLARE_ALGORITHM_FACTORY( STSummaryAnalyser);

//--------------------------------------------------------------------
//
//  STSummaryAnalyser
//
//--------------------------------------------------------------------

STSummaryAnalyser::STSummaryAnalyser( const std::string& name, ISvcLocator* pSvcLocator ) :
  GaudiHistoAlg(name, pSvcLocator),
  m_pcnHisto("pcnEvent"),
  c_binIDaSynch(0.5),
  c_binIDmissing(1.5), 
  c_binIDcorrupted(2.5)
{
  // constructer
  declareProperty("DetType", m_detType = "TT" );
  declareProperty("InputData", m_dataLocation = STTELL1DataLocation::TTFull );
  declareProperty("Summary", m_summaryLocation   = STSummaryLocation::TTSummary );
  declareProperty("UseSourceID", m_useSourceID = true );
  //declareProperty("BaseNameHisto", m_basenameHisto = "pcn");
  declareProperty("SkipEvents", m_skipEvents = -1 );
}

STSummaryAnalyser::~STSummaryAnalyser()
{
  // destructer
}

StatusCode STSummaryAnalyser::initialize()
{
  // Initialize GaudiHistoAlg
  StatusCode sc = GaudiHistoAlg::initialize();
  if (sc.isFailure()) {
    return Error("Failed to initialize", sc);
  }
  
  STDetSwitch::flip(m_detType,m_dataLocation);
  
  m_errorHisto[c_id]    = "errors";
  m_errorHisto[c_title] = "Error Info in Summary";
  //const Map *NumberToSourceIDMap;
//  if (m_detType == "TT") {
//    m_TELL1Mapping      = &TTSourceIDToNumberMap();						  ;
//    NumberToSourceIDMap = &TTNumberToSourceIDMap();
//  } else {
//    m_TELL1Mapping      = &ITSourceIDToNumberMap();
//    NumberToSourceIDMap = &ITNumberToSourceIDMap();
//  }
//  const std::map<unsigned int, unsigned int> *NumberToSourceIDMap;
//  if (m_detType == "TT") {
//    m_TELL1Mapping      = &TTSourceIDToNumberMap;						  ;
//    NumberToSourceIDMap = &TTNumberToSourceIDMap;
//  } else {
//    m_TELL1Mapping      = &ITSourceIDToNumberMap;
//    NumberToSourceIDMap = &ITNumberToSourceIDMap;
//  }
  
//  m_basenameHisto += "_$tell";
  m_evtNumber      = 0;
  
//  book(m_pcnHisto, 0, pipelineSize, pipelineSize);
    
//        std::map<unsigned int, unsigned int>::const_iterator iterMap = NumberToSourceIDMap->begin();
//  const std::map<unsigned int, unsigned int>::const_iterator endMap  = NumberToSourceIDMap->end();
//  for ( ; iterMap != endMap ; iterMap++ ) {
//    int tellID = iterMap->first;
//    if (m_useSourceID) {
//      tellID = iterMap->second;
//    }
//    std::string title = m_basenameHisto + boost::lexical_cast<std::string>(tellID);
//    book(title, 0, pipelineSize, pipelineSize);
//  }

  return StatusCode::SUCCESS;
}





StatusCode STSummaryAnalyser::execute()
{ 
  int pipelineSize = 187;
  m_evtNumber++;
  
  // Skip first m_skipEvents. Useful when running over CMS data.
  if( m_evtNumber < m_skipEvents ) {
    debug() << "skipping Event" << endmsg;
    return StatusCode::SUCCESS;
  }
 
  // Skip if there is no Tell1 data
  //if (!exist<STTELL1Datas>(m_dataLocation)) {
  //  debug() << "No data at given location" << endmsg;
  //  return StatusCode::SUCCESS;
  //}
  
  // Skip if there is no Tell1 data
  if (!exist<STSummary>(m_summaryLocation)) {
    debug() << "No data at given location" << endmsg;
    return StatusCode::SUCCESS;
  }

  // Get the data
  //const STTELL1Datas *data    = get<STTELL1Datas>(m_dataLocation);
  const STSummary    *summary = get<STSummary>(m_summaryLocation);
  // debug() << "Found " << data->size() << " boards." << endmsg;

  //if (!(summary->pcnSynch())) {
  //  error() << "PCN in event " << m_evtNumber << "do not agree!" << endmsg; 
  //  return StatusCode::SUCCESS;
  //}
  
  //unsigned int pcn = summary->pcn();
  
  //IHistogram1D* hist = histo1D( m_pcnHisto ) ;
  //if ( hist == 0 ) {
  //  error() << "Histogram " << m_pcnHisto << " not found" << endmsg;
  //}
  //hist->fill(pcn);
  plot1D(summary->pcn(), m_pcnHisto, "PCN", 0, pipelineSize, pipelineSize);
  
  if (!(summary->pcnSynch())) {
    plot1D(c_binIDaSynch, m_errorHisto[c_id], m_errorHisto[c_title], 
		    0, c_nErrorBins, c_nErrorBins );
  }
  plot1D(c_binIDcorrupted, m_errorHisto[c_id], m_errorHisto[c_title], 
                    0, c_nErrorBins, c_nErrorBins, 
		    (summary->corruptedBanks()).size());
  plot1D(c_binIDmissing, m_errorHisto[c_id], m_errorHisto[c_title], 
                    0, c_nErrorBins, c_nErrorBins, 
		    (summary->missingBanks()).size());
  

  return StatusCode::SUCCESS;
}

StatusCode STSummaryAnalyser::finalize()
{
  //printHistos();
  // Update al histograms at the end
  //DataMap::const_iterator iTell = m_meanMap.begin();
  //for( ; iTell != m_meanMap.end(); ++iTell ) { 
  //  updateNoiseHistogram( (*iTell).first );
  //} 

  return StatusCode::SUCCESS;
}

//void STSummaryAnalyser::fill(int tellID)
//{
  // Create a title for the histogram
//  std::string title = m_basenameHisto + boost::lexical_cast<std::string>(tellID);

//  IHistogram1D* hist = histo1D( title ) ;
//  if ( hist == 0 ) {
//    error() << "Histogram " << title << " not found" << endmsg;
//    return;
//  }
//  hist->reset();

  // Loop over strips in tell1
//  for (unsigned int strip = 0u; strip < nStripsPerBoard; ++strip) {
//    double rms = sqrt( m_meanSqMap[tellID][strip] 
//                       - gsl_pow_2(m_meanMap[tellID][strip]));
//    hist->fill(strip, rms);
//  }
//}
