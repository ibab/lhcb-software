// $Id: STNZSMonitor.cpp,v 1.9 2009-10-30 13:21:48 mtobin Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"

// LHCbKernel
#include "Kernel/STDAQDefinitions.h"
#include "Kernel/STBoardMapping.h"
#include "Kernel/ISTReadoutTool.h"
#include "Kernel/LHCbConstants.h"

// STTELL1Event
#include "Event/STTELL1Data.h"

// AIDA
#include "AIDA/IHistogram1D.h"

// standard
#include "gsl/gsl_math.h"
#include "boost/lexical_cast.hpp"

// local
#include "STNZSMonitor.h"
#include "Kernel/ISTNoiseCalculationTool.h"

using namespace LHCb;
using namespace AIDA;
using namespace STDAQ;
using namespace STBoardMapping;

DECLARE_ALGORITHM_FACTORY( STNZSMonitor);

//--------------------------------------------------------------------
//
//  STNZSMonitor
//
//--------------------------------------------------------------------

STNZSMonitor::STNZSMonitor( const std::string& name, 
                            ISvcLocator* pSvcLocator ) :
  ST::HistoAlgBase(name, pSvcLocator)
{
  // constructer
  declareProperty("UseSourceID", m_useSourceID = true );
  declareProperty("UpdateRate", m_updateRate = -1);  

  // Debugging
  declareProperty("CheckNoiseCalculation", m_checkCalculation=false);

  /// Noise calculation tool
  declareProperty("NoiseToolType",m_noiseToolType="ST::STNoiseCalculationTool");
  declareSTConfigProperty("NoiseToolName",m_noiseToolName,"TTNoiseCalculationTool");

}

StatusCode STNZSMonitor::initialize()
{
  // Initialize ST::HistoAlgBase
  StatusCode sc = ST::HistoAlgBase::initialize();
  if (sc.isFailure()) return sc;

  m_evtNumber = 0;

  bookHistograms();
  m_noiseTool = tool<ST::ISTNoiseCalculationTool>(m_noiseToolType, m_noiseToolName);

  // Read following period, reset rate and skip events from configuration of tool
  m_followingPeriod = m_noiseTool->followPeriod();
  m_resetRate = m_noiseTool->resetRate();
  m_skipEvents = m_noiseTool->skipEvents();



  return StatusCode::SUCCESS;
}

void STNZSMonitor::bookHistograms() {

  // Get the tell1 mapping from source ID to tell1 number
  std::map<unsigned int, unsigned int>::const_iterator itT = (this->readoutTool())->SourceIDToTELLNumberMap().begin();
  for(; itT != (this->readoutTool())->SourceIDToTELLNumberMap().end(); ++itT) {
    unsigned int sourceID = (*itT).first;
    unsigned int tellID = m_useSourceID ? sourceID : (*itT).second;
    // Create a title for the histogram
    std::string strTellID  = boost::lexical_cast<std::string>(tellID);
    HistoID histoID        = "noise_$tell" + strTellID;
    std::string histoTitle = "Noise for Tell" + strTellID;
    m_noiseHistos[sourceID] = book1D(histoID, histoTitle, -0.5, nStripsPerBoard-0.5, nStripsPerBoard);
  }
}

StatusCode STNZSMonitor::execute()
{ 
  m_evtNumber++;
  // Skip first m_skipEvents. Useful when running over CMS data.
  if( m_evtNumber < m_skipEvents ) {
    return StatusCode::SUCCESS;
  }
 
  // Get the tell1 mapping from source ID to tell1 number
  std::map<unsigned int, unsigned int>::const_iterator itT = (this->readoutTool())->SourceIDToTELLNumberMap().begin();

  // loop over the boards
  for(; itT != (this->readoutTool())->SourceIDToTELLNumberMap().end(); ++itT) {
    // Flag to check if histogram needs to be updated
    bool needToUpdate = false;
    
    unsigned int sourceID = (*itT).first;
    //    int tellID = m_useSourceID ? sourceID : (*itT).second;

    // Loop over number of events for FPGA-PP and see if the histograms need to be reset
    std::vector<unsigned int>::const_iterator itEvts = m_noiseTool->cmsNEventsBegin(sourceID);
    for(; itEvts != m_noiseTool->cmsNEventsEnd(sourceID); ++itEvts) {
      
      // Cumulative average up to m_followingPeriod; after that
      // exponential moving average
      int nEvt = (*itEvts);
      if( m_followingPeriod > 0 && nEvt > m_followingPeriod ) 
        nEvt = m_followingPeriod;
    
      // Check if at least one of the PPs requires to update the histogram
      if( m_updateRate > 0 && nEvt%m_updateRate == 0 && nEvt != 0 ) {
        needToUpdate = true;
      }
    } // FPGA-PP
    
    // Update the noise histogram
    if( needToUpdate ) updateNoiseHistogram( sourceID );
    
  } // boards
  return StatusCode::SUCCESS;
}

StatusCode STNZSMonitor::finalize()
{
  //printHistos();
  // Update all histograms at the end
  std::map<int, AIDA::IHistogram1D*>::const_iterator itH = m_noiseHistos.begin();
  for( ; itH != m_noiseHistos.end(); ++itH ) { 
    updateNoiseHistogram( (*itH).first );
    if(m_checkCalculation) dumpNoiseCalculation( (*itH).first );
  } 
  

  return ST::HistoAlgBase::finalize();// must be called after all other actions
}

void STNZSMonitor::updateNoiseHistogram(unsigned int sourceID)
{
  // Get the histogram and reset it in case it is already booked. 
  if( m_noiseHistos.find(sourceID) != m_noiseHistos.end() ) { 

    IHistogram1D* hist = m_noiseHistos.find(sourceID)->second;//->second.end();
    hist->reset();
  
    // Loop over strips in tell1
    unsigned int strip=0;
    std::vector<double>::const_iterator itNoise = m_noiseTool->cmsNoiseBegin(sourceID);
    
    for(; itNoise != m_noiseTool->cmsNoiseEnd(sourceID); ++itNoise, ++strip) {
      hist->fill( strip, (*itNoise) );
    }
  } else {
    unsigned int tellID = m_useSourceID ? sourceID : (this->readoutTool())->SourceIDToTELLNumber(sourceID);
    Warning("No histogram booked for "+boost::lexical_cast<std::string>(tellID),50,StatusCode::SUCCESS).ignore();
  }
}

void STNZSMonitor::dumpNoiseCalculation(unsigned int sourceID) {
  const unsigned int TELL=(this->readoutTool())->SourceIDToTELLNumber(sourceID);
  unsigned int strip=0;
  std::vector<double> rawMean = m_noiseTool->rawMean(TELL);
  std::vector<double>::iterator rawMeanIt = rawMean.begin();
  std::vector<double> rawMeanSq = m_noiseTool->rawMeanSq(TELL);
  std::vector<double>::iterator rawMeanSqIt = rawMeanSq.begin();
  std::vector<double> rawNoise = m_noiseTool->rawNoise(TELL);
  std::vector<double>::iterator rawNoiseIt = rawNoise.begin();
  std::vector<unsigned int> rawN = m_noiseTool->rawN(TELL);
  std::vector<unsigned int>::iterator rawNIt = rawN.begin();

  std::vector<double> cmsMean = m_noiseTool->cmsMean(TELL);
  std::vector<double>::iterator cmsMeanIt = cmsMean.begin();
  std::vector<double> cmsMeanSq = m_noiseTool->cmsMeanSq(TELL);
  std::vector<double>::iterator cmsMeanSqIt = cmsMeanSq.begin();
  std::vector<double> cmsNoise = m_noiseTool->cmsNoise(TELL);
  std::vector<double>::iterator cmsNoiseIt = cmsNoise.begin();
  std::vector<unsigned int> cmsN = m_noiseTool->cmsN(TELL);
  std::vector<unsigned int>::iterator cmsNIt = cmsN.begin();
  
  std::string idTELL = boost::lexical_cast<std::string>(TELL);
  std::string idRawMean = "Raw mean, TELL "+idTELL;
  std::string idRawMeanSq = "Raw mean squared, TELL "+idTELL;
  std::string idRawNoiseS = "Raw noise stored, TELL "+idTELL;
  std::string idRawNoiseC = "Raw noise calculated, TELL "+idTELL;
  std::string idRawN = "Raw number events, TELL "+idTELL;

  std::string idCMSMean = "CMS mean, TELL "+idTELL;
  std::string idCMSMeanSq = "CMS mean squared, TELL "+idTELL;
  std::string idCMSNoiseS = "CMS noise stored, TELL "+idTELL;
  std::string idCMSNoiseC = "CMS noise calculated, TELL "+idTELL;
  std::string idCMSN = "CMS number events, TELL "+idTELL;
  int pp=0;
  for(; rawNIt != rawN.end(); ++rawNIt, ++cmsNIt, ++pp) {
    plot1D(pp,idRawN,idRawN, -0.5, 3.5, 4, (*rawNIt));  // sort this out (4 bins)
    plot1D(pp,idCMSN,idCMSN, -0.5, 3.5, 4, (*cmsNIt)); // sort this out (4 bins)


  }
  for(; rawMeanIt != rawMean.end(); ++rawMeanIt, ++rawMeanSqIt, ++rawNoiseIt, ++cmsMeanIt, ++cmsMeanSqIt, ++cmsNoiseIt, ++strip) {
    plot1D(strip,idRawMean,idRawMean, -0.5, 3071.5, 3072, (*rawMeanIt)); 
    plot1D(strip,idRawMeanSq,idRawMeanSq, -0.5, 3071.5, 3072, (*rawMeanSqIt)); 
    plot1D(strip,idRawNoiseS,idRawNoiseS, -0.5, 3071.5, 3072, (*rawNoiseIt)); 
    plot1D(strip,idRawNoiseC,idRawNoiseC, -0.5, 3071, 3072, sqrt( *rawMeanSqIt - gsl_pow_2( *rawMeanIt ) ));

    plot1D(strip,idCMSMean,idCMSMean, -0.5, 3071.5, 3072, (*cmsMeanIt)); 
    plot1D(strip,idCMSMeanSq,idCMSMeanSq, -0.5, 3071.5, 3072, (*cmsMeanSqIt)); 
    plot1D(strip,idCMSNoiseS,idCMSNoiseS, -0.5, 3071.5, 3072, (*cmsNoiseIt)); 
    plot1D(strip,idCMSNoiseC,idCMSNoiseC, -0.5, 3071.5, 3072, sqrt( *cmsMeanSqIt - gsl_pow_2( *cmsMeanIt ) ));

  }
}
