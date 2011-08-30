// $Id: ST::STNoiseMonitor.cpp,v 1.16 2010-04-12 13:13:24 mtobin Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"

// LHCbKernel
#include "Kernel/STDAQDefinitions.h"
#include "Kernel/STBoardMapping.h"
#include "Kernel/ISTReadoutTool.h"
#include "Kernel/LHCbConstants.h"

// ODIN..
#include "Event/ODIN.h"

// STTELL1Event
#include "Event/STTELL1Data.h"

// AIDA/root histograms
#include "AIDA/IProfile1D.h"
#include "AIDA/IHistogram2D.h"
#include "GaudiUtils/Aida2ROOT.h"
#include "TProfile.h"
#include "TH2D.h"

// standard
#include "gsl/gsl_math.h"
#include "boost/lexical_cast.hpp"

// local
#include "STNoiseMonitor.h"
#include "Kernel/ISTNoiseCalculationTool.h"

using namespace LHCb;
using namespace AIDA;
using namespace STDAQ;
using namespace STBoardMapping;

namespace ST{
  DECLARE_ALGORITHM_FACTORY( STNoiseMonitor);
}
//--------------------------------------------------------------------
//
//  STNoiseMonitor
//
//--------------------------------------------------------------------

ST::STNoiseMonitor::STNoiseMonitor( const std::string& name, 
                            ISvcLocator* pSvcLocator ) :
  ST::HistoAlgBase(name, pSvcLocator)
{
  // constructer
  declareProperty("UseSourceID", m_useSourceID = false );
  declareProperty("UpdateRate", m_updateRate = 1);  

  // Debugging
  declareProperty("CheckNoiseCalculation", m_checkCalculation=false);
  // Limit calculation to vector of tell1s given in terms of TELLID (eg TTTELL1 = 1) 
  declareProperty("LimitToTell",      m_limitToTell     ); 

  // Use ODIN time in histograms
  declareProperty("UseODINTime", m_useODINTime = false);

  /// Noise calculation tool
  declareProperty("NoiseToolType",m_noiseToolType="ST::STOnlineNoiseCalculationTool");
  declareSTConfigProperty("NoiseToolName",m_noiseToolName,"TTNoiseCalculationTool");

}

StatusCode ST::STNoiseMonitor::initialize() {
  // Initialize ST::HistoAlgBase
  StatusCode sc = ST::HistoAlgBase::initialize();
  if (sc.isFailure()) return sc;

  m_evtNumber = 0;

  m_noiseTool = tool<ST::ISTNoiseCalculationTool>(m_noiseToolType, m_noiseToolName);

  // Read following period, reset rate and skip events from configuration of tool
  m_followingPeriod = m_noiseTool->followPeriod();
  m_resetRate = m_noiseTool->resetRate();
  m_skipEvents = m_noiseTool->skipEvents();

  // Select small number of TELL1s (useful for debugging) then book histograms
  m_nTELL1s = readoutTool()->nBoard();
  m_selectedTells = false;
  if ( m_limitToTell.size() > 0 ) {
    m_selectedTells = true;
    sort(m_limitToTell.begin(), m_limitToTell.end());
  }
 
  bookHistograms();

  return StatusCode::SUCCESS;
}

void ST::STNoiseMonitor::bookHistograms() {
  // Get the tell1 mapping from source ID to tell1 number
  std::map<unsigned int, unsigned int>::const_iterator itT = (this->readoutTool())->SourceIDToTELLNumberMap().begin();
  for(; itT != (this->readoutTool())->SourceIDToTELLNumberMap().end(); ++itT) {
    unsigned int sourceID = (*itT).first;
    // Limit to selected tell1s
    if ( m_selectedTells ) {
      if (!binary_search(m_limitToTell.begin(), m_limitToTell.end(), (this->readoutTool())->SourceIDToTELLNumber(sourceID))) {
        continue;
      }
    }
    unsigned int tellID = m_useSourceID ? sourceID : (*itT).second;
    // Create a title for the histogram
    std::string strTellID  = boost::lexical_cast<std::string>(tellID);
    // RAW Noise
    HistoID raw_noiseHistoID        = "raw_noise_$tell" + strTellID;
    std::string raw_noiseHistoTitle = " Raw Noise for " + detType() + "TELL" + strTellID;
    m_noiseHistosRAW[sourceID] = bookProfile1D(raw_noiseHistoID, raw_noiseHistoTitle, -0.5, nStripsPerBoard-0.5, nStripsPerBoard);
    HistoID raw_pedHistoID        = "raw_pedestal_$tell" + strTellID;
    std::string raw_pedHistoTitle = "Raw Pedestal for " + detType() + "TELL" + strTellID;
    m_pedestalHistosRAW[sourceID] = bookProfile1D(raw_pedHistoID, raw_pedHistoTitle, -0.5, nStripsPerBoard-0.5, nStripsPerBoard);
    // Common Mode Subtracted Noise
    HistoID cms_noiseHistoID        = "cms_noise_$tell" + strTellID;
    std::string cms_noiseHistoTitle = "Common Mode Subtracted Noise for " + detType() + "TELL" + strTellID;
    m_noiseHistosCMS[sourceID] = bookProfile1D(cms_noiseHistoID, cms_noiseHistoTitle, -0.5, nStripsPerBoard-0.5, nStripsPerBoard);
    HistoID cms_pedHistoID        = "cms_pedestal_$tell" + strTellID;
    std::string cms_pedHistoTitle = "Pedestal after CMS for " + detType() + "TELL" + strTellID;
    m_pedestalHistosCMS[sourceID] = bookProfile1D(cms_pedHistoID, cms_pedHistoTitle, -0.5, nStripsPerBoard-0.5, nStripsPerBoard);
    // Common Mode Noise
    HistoID cm_noiseHistoID        = "cm_noise_$tell" + strTellID;
    std::string cm_noiseHistoTitle = "Common Mode Noise for " + detType() + "TELL" + strTellID;
    m_noiseHistosCM[sourceID] = bookProfile1D(cm_noiseHistoID, cm_noiseHistoTitle, -0.5, nStripsPerBoard-0.5, nStripsPerBoard);

  }
//   m_2d_NoisePerLinkVsTell1 = 
//     Gaudi::Utils::Aida2ROOT::aida2root( book2D("Noise per link vs TELL1", 0.5, m_nTELL1s+0.5, m_nTELL1s, 0., 96., 96) );
  m_1dp_AvRAWNoisePerTell1 = bookProfile1D("Average RAW Noise vs TELL1", 0.5, m_nTELL1s+0.5, m_nTELL1s);
  m_1dp_AvCMSNoisePerTell1 = bookProfile1D("Average CMS Noise vs TELL1", 0.5, m_nTELL1s+0.5, m_nTELL1s);
  m_1dp_AvCMNoisePerTell1  = bookProfile1D("Average CM Noise vs TELL1",  0.5, m_nTELL1s+0.5, m_nTELL1s);
//   m_2d_PedestalPerLinkVsTell1 = 
//     Gaudi::Utils::Aida2ROOT::aida2root( book2D("Pedestal per link vs TELL1", 0.5, m_nTELL1s+0.5, m_nTELL1s, 0., 96., 96) );
   m_1dp_AvRAWPedestalPerTell1 = bookProfile1D("Average Pedestal (RAW) vs TELL1", 0.5, m_nTELL1s+0.5, m_nTELL1s);
   m_1dp_AvCMSPedestalPerTell1 = bookProfile1D("Average Pedestal (CMS) vs TELL1", 0.5, m_nTELL1s+0.5, m_nTELL1s);
//   m_2d_NormalisationPerLinkVsTell1 = 
//     Gaudi::Utils::Aida2ROOT::aida2root( book2D("Normalisation", 0.5, m_nTELL1s+0.5, m_nTELL1s, 0., 96., 96) );

}

StatusCode ST::STNoiseMonitor::execute() { 
  m_evtNumber++;

  // Get the time of the first event and convert to a string for the histogram title.
  if(m_evtNumber == 1) {
    if(m_useODINTime) {
      m_ODIN = get<ODIN>(LHCb::ODINLocation::Default); 
      const Gaudi::Time odinTime = m_ODIN->eventTime();
      m_odinEvent  = "(#"+boost::lexical_cast<std::string>(m_ODIN->runNumber());
      m_odinEvent += " on "+boost::lexical_cast<std::string>(odinTime.day(0));
      m_odinEvent += "/"+boost::lexical_cast<std::string>(odinTime.month(0)+1);
      m_odinEvent += "/"+boost::lexical_cast<std::string>(odinTime.year(0));
      m_odinEvent += " @ "+boost::lexical_cast<std::string>(odinTime.hour(0)); 
      m_odinEvent += ":"+boost::lexical_cast<std::string>(odinTime.minute(0)); 
      m_odinEvent += ":"+boost::lexical_cast<std::string>(odinTime.second(0))+")"; 
    }
  }

  // Skip first m_skipEvents. Useful when running over CMS data.
  if( m_evtNumber < m_skipEvents ) {
    return StatusCode::SUCCESS;
  }

  // loop over the boards which contained an NZS bank
  std::vector<unsigned int>::const_iterator itT = m_noiseTool->tell1WithNZSBegin();
  for(; itT != m_noiseTool->tell1WithNZSEnd(); ++itT) {
    // Flag to check if histogram needs to be updated
    bool needToUpdate = false;
    
    unsigned int sourceID = (*itT);

    // Loop over number of events for FPGA-PP and see if the histograms need to be reset
    std::vector<unsigned int>::const_iterator itEvts = m_noiseTool->cmsNEventsPPBegin(sourceID);
    for(; itEvts != m_noiseTool->cmsNEventsPPEnd(sourceID); ++itEvts) {
      
      int nEvt = (*itEvts);

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

StatusCode ST::STNoiseMonitor::finalize() {
  //printHistos();
  // Update all histograms at the end
  std::map<int, AIDA::IProfile1D*>::const_iterator itH = m_noiseHistosRAW.begin();

  for( ; itH != m_noiseHistosRAW.end(); ++itH ) { 
    // Limit to selected tell1s
    if ( m_selectedTells && 
         !binary_search(m_limitToTell.begin(), m_limitToTell.end(), (this->readoutTool())->SourceIDToTELLNumber((*itH).first))) {
      continue;
    }
    updateNoiseHistogram( (*itH).first, m_useODINTime );
    if(m_checkCalculation) dumpNoiseCalculation( (*itH).first );
  } 

  return ST::HistoAlgBase::finalize();// must be called after all other actions
}

void ST::STNoiseMonitor::updateNoiseHistogram(unsigned int sourceID, bool updateTitle) {
  // Get the histogram and reset it in case it is already booked. 
  if( m_noiseHistosRAW.find(sourceID) != m_noiseHistosRAW.end() 
      && m_pedestalHistosRAW.find(sourceID) != m_pedestalHistosRAW.end()
      && m_noiseHistosCMS.find(sourceID) != m_noiseHistosCMS.end() 
      && m_pedestalHistosCMS.find(sourceID) != m_pedestalHistosCMS.end()
      && m_noiseHistosCM.find(sourceID) != m_noiseHistosCM.end()  ) { 

    IProfile1D* noiseHistRAW = m_noiseHistosRAW.find(sourceID)->second;
    noiseHistRAW->reset();
    IProfile1D* pedestalHistRAW = m_pedestalHistosRAW.find(sourceID)->second;
    pedestalHistRAW->reset();
  
    IProfile1D* noiseHistCMS = m_noiseHistosCMS.find(sourceID)->second;
    noiseHistCMS->reset();
    IProfile1D* pedestalHistCMS = m_pedestalHistosCMS.find(sourceID)->second;
    pedestalHistCMS->reset();
  
    IProfile1D* noiseHistCM = m_noiseHistosCM.find(sourceID)->second;
    noiseHistCM->reset();

    // Loop over strips in tell1
    unsigned int strip=0;
    std::vector<double>::const_iterator itRAWPedBegin = m_noiseTool->pedestalBegin(sourceID);
    std::vector<double>::const_iterator itRAWNoiseBegin = m_noiseTool->rawNoiseBegin(sourceID);
    std::vector<double>::const_iterator itRAWNoiseEnd = m_noiseTool->rawNoiseEnd(sourceID);

    std::vector<double>::const_iterator itCMSPedBegin = m_noiseTool->cmsMeanBegin(sourceID);
    std::vector<double>::const_iterator itCMSNoiseBegin = m_noiseTool->cmsNoiseBegin(sourceID);

    unsigned int tell1 = (this->readoutTool())->SourceIDToTELLNumber(sourceID);
    std::vector<double>::const_iterator itNoiseRAW = itRAWNoiseBegin;
    std::vector<double>::const_iterator itPedRAW = itRAWPedBegin;
    std::vector<double>::const_iterator itNoiseCMS = itCMSNoiseBegin;
    std::vector<double>::const_iterator itPedCMS = itCMSPedBegin;
    for(; itNoiseRAW != itRAWNoiseEnd; ++itNoiseRAW, ++itPedRAW, ++itNoiseCMS, ++itPedCMS, ++strip) {
      noiseHistRAW->fill( strip, (*itNoiseRAW) );
      pedestalHistRAW->fill( strip, (*itPedRAW) );
      noiseHistCMS->fill( strip, (*itNoiseCMS) );
      pedestalHistCMS->fill( strip, (*itPedCMS) );
      if((*itNoiseRAW) > 1e-4) { // check if channel is valid at some point....
        m_1dp_AvRAWNoisePerTell1->fill(tell1,(*itNoiseRAW));
        m_1dp_AvRAWPedestalPerTell1->fill(tell1,(*itPedRAW));
      }
      if((*itNoiseCMS) > 1e-4) { // add accessors to check if channel is valid in tool
        m_1dp_AvCMSNoisePerTell1->fill(tell1,(*itNoiseCMS));
        m_1dp_AvCMSPedestalPerTell1->fill(tell1,(*itPedCMS));
      }
      
      double commonMode = gsl_pow_2(*itNoiseRAW)-gsl_pow_2(*itNoiseCMS);
      if(commonMode > 0) commonMode = sqrt(commonMode);
      else commonMode = 0;
//         std::cout << tell1 << "\t" << (*itNoiseRAW) << "\t" << (*itNoiseCMS) << "\t" 
//                   <<  gsl_pow_2(*itNoiseRAW)-gsl_pow_2(*itNoiseCMS)  << "\t" 
//                   << fabs( gsl_pow_2(*itNoiseRAW)-gsl_pow_2(*itNoiseCMS) )
//                   << "\t" << commonMode << std::endl;
      noiseHistCM->fill( strip, commonMode );
      m_1dp_AvCMNoisePerTell1->fill(tell1, commonMode);
    }
    if(updateTitle) {
      TProfile* profNoise = Gaudi::Utils::Aida2ROOT::aida2root ( noiseHistRAW );
      std::string title=profNoise->GetTitle();
      title += " "+m_odinEvent;
      profNoise->SetTitle(title.c_str());
    }
//     strip = 0;
//     for(unsigned int link=0; link < STDAQ::noptlinks*STDAQ::nports; link++) {
//       itNoise = itNoiseBegin+link*LHCbConstants::nStripsInPort;
//       itNoiseEnd = itNoise + LHCbConstants::nStripsInPort;
//       itPed = itPedBegin+link*LHCbConstants::nStripsInPort;
//       double sumNoise = 0; double sumPed = 0; int nChan=0;
//       for(; itNoise != itNoiseEnd; ++itNoise, ++itPed, ++strip) {
//         if((*itNoise) > 1e-4) {
//           nChan += 1;
//           sumNoise += (*itNoise);
//           sumPed += (*itPed);
//         }
//       }
//       if(nChan > 0) {
//         int bin = m_2d_NoisePerLinkVsTell1->FindBin(tell1, link);
//         m_2d_NoisePerLinkVsTell1->SetBinContent(bin , sumNoise/nChan);
//         m_2d_PedestalPerLinkVsTell1->SetBinContent( bin, sumPed/nChan);
//         m_2d_NormalisationPerLinkVsTell1->SetBinContent( bin, 1.);
//       }
//  }
  } else {
    unsigned int tellID = m_useSourceID ? sourceID : (this->readoutTool())->SourceIDToTELLNumber(sourceID);
    Warning("No histogram booked for "+boost::lexical_cast<std::string>(tellID),0,StatusCode::SUCCESS).ignore();
  }
}

void ST::STNoiseMonitor::dumpNoiseCalculation(unsigned int sourceID) {
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
    profile1D(pp, (*rawNIt), idRawN, idRawN, -0.5, 3.5, 4);
    profile1D(pp, (*cmsNIt), idCMSN, idCMSN, -0.5, 3.5, 4);


  }
  for(; rawMeanIt != rawMean.end(); ++rawMeanIt, ++rawMeanSqIt, ++rawNoiseIt, ++cmsMeanIt, ++cmsMeanSqIt, ++cmsNoiseIt, ++strip) {
    profile1D(strip, (*rawMeanIt), idRawMean, idRawMean, -0.5, 3071.5, 3072); 
    profile1D(strip, (*rawMeanSqIt), idRawMeanSq, idRawMeanSq, -0.5, 3071.5, 3072); 
    profile1D(strip, (*rawNoiseIt), idRawNoiseS, idRawNoiseS, -0.5, 3071.5, 3072); 
    profile1D(strip, sqrt(*rawMeanSqIt - gsl_pow_2(*rawMeanIt)), idRawNoiseC, idRawNoiseC, -0.5, 3071, 3072);

    profile1D(strip, (*cmsMeanIt), idCMSMean, idCMSMean, -0.5, 3071.5, 3072); 
    profile1D(strip, (*cmsMeanSqIt), idCMSMeanSq, idCMSMeanSq, -0.5, 3071.5, 3072); 
    profile1D(strip, (*cmsNoiseIt), idCMSNoiseS, idCMSNoiseS, -0.5, 3071.5, 3072); 
    profile1D(strip, sqrt(*cmsMeanSqIt - gsl_pow_2(*cmsMeanIt)), idCMSNoiseC, idCMSNoiseC, -0.5, 3071.5, 3072);

  }
}
