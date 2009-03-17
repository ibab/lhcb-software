// $Id: STSummaryAnalyser.cpp,v 1.3 2009-03-17 10:47:58 nchiapol Exp $

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
  //m_pcnHisto("pcnEvent"),
  c_binIDaSynch(0.5),
  c_binIDmissing(2.5), 
  c_binIDcorrupted(1.5),
  c_binIDrecovered(3.5)
{
  // constructer
  declareProperty("DetType", m_detType = "TT" );
  //declareProperty("InputData", m_dataLocation = STTELL1DataLocation::TTFull );
  //declareProperty("Summary", m_summaryLocation   = STSummaryLocation::TTSummary );
  declareProperty("InputData", m_summaryLocation   = STSummaryLocation::TTSummary );
  declareProperty("UseSourceID", m_useSourceID = true );
  declareProperty("SkipEvents", m_skipEvents = -1 );
  declareProperty("SuppressMissing", m_suppressMissing = false);
  declareProperty("SuppressRecovered", m_suppressRecovered = false);
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
  
  STDetSwitch::flip(m_detType,m_summaryLocation);

  // defining histogram ids and titles
  m_pcnHisto[c_id]         = "pcn";
  m_pcnHisto[c_title]      = "PCN distribution";
  m_errorHisto[c_id]       = "errors";
  m_errorHisto[c_title]    = "Error Info in Summary";
  m_dataSizeHisto[c_id]    = "dataSize";
  m_dataSizeHisto[c_title] = "Data size per event";
  
  m_evtNumber      = 0;
  
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
  if (!exist<STSummary>(m_summaryLocation)) {
    debug() << "No data at given location" << endmsg;
    return StatusCode::SUCCESS;
  }

  // Get the data
  const STSummary    *summary = get<STSummary>(m_summaryLocation);
  // debug() << "Found " << data->size() << " boards." << endmsg;

  
  /** 
   * Filling the histograms
   */
  
  // PCN
  plot1D(summary->pcn(), m_pcnHisto[c_id], m_pcnHisto[c_title], 0, pipelineSize, pipelineSize);
  
  // Error Summary
  if (!(summary->pcnSynch())) {
    plot1D(c_binIDaSynch, m_errorHisto[c_id], m_errorHisto[c_title], 
		    0, c_nErrorBins, c_nErrorBins );
  }
  plot1D(c_binIDcorrupted, m_errorHisto[c_id], m_errorHisto[c_title], 
                    0, c_nErrorBins, c_nErrorBins, 
		    (summary->corruptedBanks()).size());
  if ( !m_suppressMissing ) {
    plot1D(c_binIDmissing, m_errorHisto[c_id], m_errorHisto[c_title], 
                      0, c_nErrorBins, c_nErrorBins, 
        	      (summary->missingBanks()).size());
  }
  if ( !m_suppressRecovered ) {
    plot1D(c_binIDrecovered, m_errorHisto[c_id], m_errorHisto[c_title], 
                      0, c_nErrorBins, c_nErrorBins, 
                      (summary->recoveredBanks()).size());
  }

  // Data Size
  plot1D(summary->rawBufferSize()/1024, m_dataSizeHisto[c_id], m_dataSizeHisto[c_title], 
                    0, 100 , 200); 
  

  
  return StatusCode::SUCCESS;
}

StatusCode STSummaryAnalyser::finalize()
{
  return StatusCode::SUCCESS;
}
