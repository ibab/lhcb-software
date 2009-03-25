// $Id: STSummaryMonitor.cpp,v 1.2 2009-03-25 09:39:15 jvantilb Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"

// LHCbKernel
#include "Kernel/STDetSwitch.h"
#include "Kernel/STDAQDefinitions.h"

// STTELL1Event
#include "Event/STSummary.h"

// local
#include "STSummaryMonitor.h"

using namespace LHCb;
//using namespace STDAQ;

DECLARE_ALGORITHM_FACTORY( STSummaryMonitor);

//--------------------------------------------------------------------
//
//  STSummaryMonitor
//
//--------------------------------------------------------------------

STSummaryMonitor::STSummaryMonitor( const std::string& name, 
                                    ISvcLocator* pSvcLocator ) :
  ST::HistoAlgBase(name, pSvcLocator),
  c_binIDaSynch(0.5),
  c_binIDmissing(2.5), 
  c_binIDcorrupted(1.5),
  c_binIDrecovered(3.5)
{
  // constructer
  declareSTConfigProperty("InputData" , m_summaryLocation, 
                          STSummaryLocation::TTSummary);
  declareProperty("SuppressMissing",   m_suppressMissing   = false);
  declareProperty("SuppressRecovered", m_suppressRecovered = false);
  declareProperty("PipeLineSize",      m_pipeLineSize      = 187  );
}

StatusCode STSummaryMonitor::initialize()
{
  // Initialize ST::HistoAlgBase
  StatusCode sc = ST::HistoAlgBase::initialize();
  if (sc.isFailure()) return sc;  
  
  return StatusCode::SUCCESS;
}


StatusCode STSummaryMonitor::execute()
{ 
  // Skip if there is no Tell1 data
  if( !exist<STSummary>(m_summaryLocation) ) {
    return Warning("No data at given location", StatusCode::SUCCESS, 0);
  }

  // Get the data
  const STSummary* summary = get<STSummary>(m_summaryLocation);
  // debug() << "Found " << data->size() << " boards." << endmsg;
  
  // Filling the histograms
  
  // Fill PCN histogram
  plot1D( summary->pcn(), "pcn", "PCN distribution", 0, m_pipeLineSize, 
          m_pipeLineSize );
  
  // Fill error summary histogram
  std::string errorHistoID    = "errors";
  std::string errorHistoTitle = "Error Info in Summary";
  if( !(summary->pcnSynch()) ) {
    plot1D( c_binIDaSynch, errorHistoID, errorHistoTitle, 
            0, c_nErrorBins, c_nErrorBins );
  }
  if( (summary->corruptedBanks()).size() > 0 ) {
    plot1D( c_binIDcorrupted, errorHistoID, errorHistoTitle, 
            0, c_nErrorBins, c_nErrorBins);
  }
  if ( !m_suppressMissing && (summary->missingBanks()).size() > 0 ) {
    plot1D( c_binIDmissing, errorHistoID, errorHistoTitle, 
            0, c_nErrorBins, c_nErrorBins );
  }
  if ( !m_suppressRecovered && (summary->recoveredBanks()).size() > 0 ) {
    plot1D( c_binIDrecovered, errorHistoID, errorHistoTitle, 
            0, c_nErrorBins, c_nErrorBins );
  }

  // Fill data size histogram
  plot1D( summary->rawBufferSize()/1024, "dataSize", "Data size (kB) per event",
          0, 500 , 250);
  
  return StatusCode::SUCCESS;
}

StatusCode STSummaryMonitor::finalize()
{
  return ST::HistoAlgBase::finalize();// must be called after all other actions
}
