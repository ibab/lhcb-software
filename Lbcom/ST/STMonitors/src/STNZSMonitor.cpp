// $Id: STNZSMonitor.cpp,v 1.7 2009-07-06 17:30:58 mtobin Exp $

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
  declareSTConfigProperty("InputData" , m_dataLocation,
                          STTELL1DataLocation::TTFull);
  declareProperty("UseSourceID", m_useSourceID = true );
  declareProperty("FollowPeriod", m_followingPeriod = 2000);
  declareProperty("UpdateRate", m_updateRate = -1);  
  declareProperty("ResetRate", m_resetRate = -1);  
  declareProperty("SkipEvents", m_skipEvents = -1 );
}

StatusCode STNZSMonitor::initialize()
{
  // Initialize ST::HistoAlgBase
  StatusCode sc = ST::HistoAlgBase::initialize();
  if (sc.isFailure()) return sc;

  m_evtNumber = 0;

  return StatusCode::SUCCESS;
}

StatusCode STNZSMonitor::execute()
{ 
  m_evtNumber++;
  
  // Skip first m_skipEvents. Useful when running over CMS data.
  if( m_evtNumber < m_skipEvents ) {
    return StatusCode::SUCCESS;
  }
 
  // Skip if there is no Tell1 data
  if (!exist<STTELL1Datas>(m_dataLocation)) {
    return StatusCode::SUCCESS;
  }

  // Get the data
  const STTELL1Datas* data = get<STTELL1Datas>(m_dataLocation);
  //debug() << "Found " << data->size() << " boards." << endmsg;

  // loop over the data
  STTELL1Datas::const_iterator iterBoard = data->begin(); 
  for (; iterBoard != data->end() ; ++iterBoard){

    // get the tell board and the data headers
    int sourceID = (*iterBoard)->TELL1ID();
    const STTELL1Data::Data& dataValues = (*iterBoard)->data();

    // Create a name for the tell1
    int tellID = m_useSourceID ? sourceID :
      (this->readoutTool())->SourceIDToTELLNumber(sourceID);

    // Reset the maps for each tell1
    if ( m_meanMap.find(tellID) == m_meanMap.end() ) {
      m_meanMap[tellID].resize(3072, 0.0);
      m_meanSqMap[tellID].resize(3072, 0.0);
      m_nEvents[tellID].resize(4,0);
    }

    // Flag to check if histogram needs to be updated
    bool needToUpdate = false;
    
    // Loop over the PPs that have sent data
    std::vector<unsigned int> sentPPs = (*iterBoard)->sentPPs();
    std::vector<unsigned int>::const_iterator iPP = sentPPs.begin();
    for( ; iPP != sentPPs.end(); ++iPP ) {
      unsigned int pp = *iPP;
      
      // Count the number of events per PP
      m_nEvents[tellID][pp]++;

      // Cumulative average up to m_followingPeriod; after that
      // exponential moving average
      int nEvt = m_nEvents[tellID][pp];
      if( m_followingPeriod > 0 && nEvt > m_followingPeriod ) 
        nEvt = m_followingPeriod;
    
      // Loop over the links (i.e. Beetles)
      unsigned int iBeetle = 0;
      for ( ; iBeetle < nBeetlesPerPPx; ++iBeetle ){
        unsigned int beetle = iBeetle + pp*nBeetlesPerPPx;
        
        // Loop over the strips in this link
        unsigned int iStrip = 0;
        for ( ; iStrip < LHCbConstants::nStripsInBeetle ; ++iStrip){
          
          // Get the ADC value
          const int value = dataValues[beetle][iStrip];
          
          // Calculate the pedestal and the pedestal squared
          int strip = iStrip + beetle * LHCbConstants::nStripsInBeetle;
          m_meanMap[tellID][strip] = (m_meanMap[tellID][strip]*(nEvt-1)
                                      + value ) / nEvt;
          m_meanSqMap[tellID][strip] = (m_meanSqMap[tellID][strip]*(nEvt-1) 
                                        + gsl_pow_2(value) ) / nEvt;
        } // strip
      }  // beetle

      // Resets the event counter
      if( m_resetRate > 0  && nEvt%m_resetRate == 0 ) {
        m_nEvents[tellID][pp] = 0;
      }

      // Check if at least one of the PPs requires to update the histogram
      if( m_updateRate > 0 && nEvt%m_updateRate == 0 && nEvt != 0 ) {
        needToUpdate = true;
      }
    } // FPGA-PP
    
    // Update the noise histogram
    if( needToUpdate ) updateNoiseHistogram( tellID );

  } // boards
  return StatusCode::SUCCESS;
}

StatusCode STNZSMonitor::finalize()
{
  //printHistos();
  // Update al histograms at the end
  DataMap::const_iterator iTell = m_meanMap.begin();
  for( ; iTell != m_meanMap.end(); ++iTell ) { 
    updateNoiseHistogram( (*iTell).first );
  } 

  return ST::HistoAlgBase::finalize();// must be called after all other actions
}

void STNZSMonitor::updateNoiseHistogram(int tellID)
{
  // Create a title for the histogram
  std::string strTellID  = boost::lexical_cast<std::string>(tellID);
  HistoID histoID        = "noise_$tell" + strTellID;
  std::string histoTitle = "Noise for Tell" + strTellID;

  // Get the histogram and reset it in case it is already booked. 
  IHistogram1D* hist = histo1D( histoID ) ;
  if ( hist ) {
    hist->reset();
  }
  
  // Loop over strips in tell1
  for (unsigned int strip = 0u; strip < nStripsPerBoard; ++strip) {
    double rms = sqrt( m_meanSqMap[tellID][strip] 
                       - gsl_pow_2(m_meanMap[tellID][strip]));
    if ( hist ) {
      hist->fill( strip, rms );
    } else {
      plot1D( strip, histoID, histoTitle, 0, nStripsPerBoard, nStripsPerBoard, 
              rms );
    }
  }
}
