// LHCbKernel
#include "Kernel/STDAQDefinitions.h"
#include "Kernel/STBoardMapping.h"
#include "Kernel/ISTReadoutTool.h"
#include "Kernel/LHCbConstants.h"

// ODIN..
#include "Event/ODIN.h"

// STTELL1Event
#include "Event/STTELL1Data.h"
#include "Kernel/STTell1ID.h"
#include "Kernel/STTell1Board.h"

#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"

// AIDA/root histograms
#include "AIDA/IProfile1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IHistogram1D.h"
#include "GaudiUtils/Aida2ROOT.h"
#include "TProfile.h"
#include "TH2D.h"

// standard
#include "gsl/gsl_math.h"

// local
#include "STNoiseMonitor.h"
#include "Kernel/ISTNoiseCalculationTool.h"

using namespace LHCb;
using namespace AIDA;
using namespace STDAQ;
using namespace STBoardMapping;
using namespace Gaudi::Utils;//::Aida2ROOT::aida2root

namespace ST{
  DECLARE_ALGORITHM_FACTORY( STNoiseMonitor)
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

  // Make summary plots
  declareProperty("SummaryPlots", m_summaryPlots=true );

  // plot by sensor type (long/short IT; 1-4, TT)
  declareProperty("PlotBySensorType", m_plotByType=false );

}

StatusCode ST::STNoiseMonitor::initialize() {
  // Initialize ST::HistoAlgBase
  StatusCode sc = ST::HistoAlgBase::initialize();
  if (sc.isFailure()) return sc;

  m_nStripsInSector = (detType() == "TT") ? STDAQ::nStripsInTTSector : STDAQ::nStripsInITSector;

  m_evtNumber = 0;

  m_debug = msgLevel( MSG::DEBUG );
 
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
  // Store DeSTSector for each TELL1 sector: 
  // -----------------------------> BIT OF A NASTY HACK
  for(const auto& itT : (this->readoutTool())->SourceIDToTELLNumberMap() ) {
    unsigned int TELL1SourceID = itT.first;
    STTell1Board* board = readoutTool()->findByBoardID(STTell1ID(TELL1SourceID));
    std::vector<DeSTSector*>* sectors = &m_sectorMap[TELL1SourceID];
    unsigned int strip = 0;
    while(strip < 3072) {
      LHCb::STChannelID channelID = (board->DAQToOffline(0, STDAQ::v4, STDAQ::StripRepresentation(strip)).first);
      DeSTSector* sector = tracker()->findSector(channelID);
        sectors->push_back(sector);
      if(sector){
        m_types[sector->type()] = sector->type();
      }
      strip += m_nStripsInSector;      
    }
  }
  bookHistograms();

  return StatusCode::SUCCESS;
}

void ST::STNoiseMonitor::bookHistograms() {

  m_1d_nNZS = book1D("Number of NZS banks", "#  NZS banks / TELL1", 0.5, m_nTELL1s+0.5, m_nTELL1s);

  // Get the tell1 mapping from source ID to tell1 number
  auto itT = (this->readoutTool())->SourceIDToTELLNumberMap().begin();
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
    std::string strTellID  = std::to_string(tellID);

    //================================================== noise / strip 

    // Raw Noise
    HistoID raw_noiseHistoID        = "raw_noise_$tell" + strTellID;
    std::string raw_noiseHistoTitle = " Raw Noise for " + detType() + "TELL" + strTellID;
    m_raw_noiseHistos[sourceID] = Aida2ROOT::aida2root( bookProfile1D(raw_noiseHistoID, raw_noiseHistoTitle, -0.5, 
                                                                      nStripsPerBoard-0.5, nStripsPerBoard) );
                                                              
    HistoID raw_pedHistoID        = "raw_pedestal_$tell" + strTellID;
    std::string raw_pedHistoTitle = "Raw Pedestal for " + detType() + "TELL" + strTellID;
    m_raw_pedestalHistos[sourceID] = Aida2ROOT::aida2root( bookProfile1D(raw_pedHistoID, raw_pedHistoTitle, 
                                                                         -0.5, nStripsPerBoard-0.5, nStripsPerBoard) );
                                                                         

    // Pedestal subtracted noise
    HistoID pedsub_noiseHistoID        = "pedsub_noise_$tell" + strTellID;
    std::string pedsub_noiseHistoTitle = "Pedestal Subtracted Noise for " + detType() + "TELL" + strTellID;
    m_pedsub_noiseHistos[sourceID] = 
      Aida2ROOT::aida2root( bookProfile1D(pedsub_noiseHistoID, pedsub_noiseHistoTitle, 
                                          -0.5, nStripsPerBoard-0.5, nStripsPerBoard) );
    HistoID pedsub_pedHistoID        = "pedsub_pedestal_$tell" + strTellID;
    std::string pedsub_pedHistoTitle = "Pedestal after Pedestal Subtraction for " + detType() + "TELL" + strTellID;
    m_pedsub_pedestalHistos[sourceID] = 
      Aida2ROOT::aida2root( bookProfile1D(pedsub_pedHistoID, pedsub_pedHistoTitle, 
                                          -0.5, nStripsPerBoard-0.5, nStripsPerBoard) );

    // Common Mode Subtracted Noise
    HistoID cms_noiseHistoID        = "cms_noise_$tell" + strTellID;
    std::string cms_noiseHistoTitle = "Common Mode Subtracted Noise for " + detType() + "TELL" + strTellID;
    m_cms_noiseHistos[sourceID] = 
      Aida2ROOT::aida2root( bookProfile1D(cms_noiseHistoID, cms_noiseHistoTitle, 
                                          -0.5, nStripsPerBoard-0.5, nStripsPerBoard) );
    HistoID cms_pedHistoID        = "cms_pedestal_$tell" + strTellID;
    std::string cms_pedHistoTitle = "Pedestal after CMS for " + detType() + "TELL" + strTellID;
    m_cms_pedestalHistos[sourceID] = 
      Aida2ROOT::aida2root( bookProfile1D(cms_pedHistoID, cms_pedHistoTitle, 
                                          -0.5, nStripsPerBoard-0.5, nStripsPerBoard) );

    // Common Mode Noise
    HistoID cm_noiseHistoID        = "cm_noise_$tell" + strTellID;
    std::string cm_noiseHistoTitle = "Common Mode Noise for " + detType() + "TELL" + strTellID;
    m_cm_noiseHistos[sourceID] = Aida2ROOT::aida2root( bookProfile1D(cm_noiseHistoID, cm_noiseHistoTitle, 
                                                                     -0.5, nStripsPerBoard-0.5, nStripsPerBoard) );

  }
  if( m_summaryPlots ) {

    m_2d_RawPedestalPerLinkVsTell1 = 
      Aida2ROOT::aida2root( book2D("Mean Raw ADCs per link vs TELL1", 0.5, m_nTELL1s+0.5, m_nTELL1s, 0., 96., 96) );
    m_2d_RawNoisePerLinkVsTell1 = 
      Aida2ROOT::aida2root( book2D("Raw Noise per link vs TELL1", 0.5, m_nTELL1s+0.5, m_nTELL1s, 0., 96., 96) );

    m_2d_PedSubPedestalPerLinkVsTell1 = 
      Aida2ROOT::aida2root( book2D("Mean Ped Subtracted ADCs per link vs TELL1", 0.5, m_nTELL1s+0.5, m_nTELL1s, 0., 96., 96) );
    m_2d_PedSubNoisePerLinkVsTell1 = 
      Aida2ROOT::aida2root( book2D("Pedestal Subtracted Noise per link vs TELL1", 0.5, m_nTELL1s+0.5, m_nTELL1s, 0., 96., 96) );
                                       
    m_2d_CMSPedestalPerLinkVsTell1 = 
      Aida2ROOT::aida2root( book2D("Mean CMS ADCs per link vs TELL1", 0.5, m_nTELL1s+0.5, m_nTELL1s, 0., 96., 96) );
    m_2d_CMSNoisePerLinkVsTell1 = 
      Aida2ROOT::aida2root( book2D("CMS Noise per link vs TELL1", 0.5, m_nTELL1s+0.5, m_nTELL1s, 0., 96., 96) );

    m_2d_NormalisationPerLinkVsTell1 = 
      Aida2ROOT::aida2root( book2D("Normalisation", 0.5, m_nTELL1s+0.5, m_nTELL1s, 0., 96., 96) );

    m_1d_raw_noise = book1D("Raw noise", 0., 10., 500);
    m_1d_pedsub_noise = book1D("Pedestal subtracted noise", 0., 10., 500);
    m_1d_cms_noise = book1D("CMS noise", 0., 10., 500);
    m_1d_cm_noise  = book1D("Common Mode noise", 0., 10., 500);
    m_1d_raw_pedestal = book1D("Raw pedestal", 100., 160., 500);
    m_1d_pedsub_pedestal = book1D("Pedestal subtracted pedestal", -5, 5., 501);
    m_1d_cms_pedestal = book1D("CMS pedestal", -5., 5., 501);

    if(m_plotByType) {
      std::map<std::string, std::string>::iterator iType = m_types.begin();
      for(; iType != m_types.end(); ++iType) {
        std::string type = (*iType).first;
        m_1d_raw_noiseByType[type] = book1D("Raw noise "+type, 0., 10., 500);
        m_1d_cms_noiseByType[type] = book1D("CMS noise "+type, 0., 10., 500);
        m_1d_cm_noiseByType[type]  = book1D("Common Mode noise "+type, 0., 10., 500);
        m_1d_raw_pedestalByType[type] = book1D("Raw pedestal "+type, 100., 160., 500);
        m_1d_cms_pedestalByType[type] = book1D("CMS pedestal "+type, -5., 5., 501);
      }
    }
  }
}

StatusCode ST::STNoiseMonitor::execute() { 
  m_evtNumber++;

  if(m_debug) debug() << "execute:\t" << m_evtNumber << endmsg;
  // Get the time of the first event and convert to a string for the histogram title.
  if(m_evtNumber == 1) {
    if(m_useODINTime) {
      m_ODIN = get<ODIN>(LHCb::ODINLocation::Default); 
      const Gaudi::Time odinTime = m_ODIN->eventTime();
      m_odinEvent  = "(#"+std::to_string(m_ODIN->runNumber());
      m_odinEvent += " on "+std::to_string(odinTime.day(0));
      m_odinEvent += "/"+std::to_string(odinTime.month(0)+1);
      m_odinEvent += "/"+std::to_string(odinTime.year(0));
      m_odinEvent += " @ "+std::to_string(odinTime.hour(0));
      m_odinEvent += ":"+std::to_string(odinTime.minute(0));
      m_odinEvent += ":"+std::to_string(odinTime.second(0))+")";
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
    unsigned int tell1 = (this->readoutTool())->SourceIDToTELLNumber(sourceID);
    m_1d_nNZS->fill(tell1);

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
  if( m_summaryPlots && m_updateRate > 0 && m_evtNumber%m_updateRate == 0) updateSummaryHistograms();

  return StatusCode::SUCCESS;
}

StatusCode ST::STNoiseMonitor::finalize() {
  //printHistos();
  // Update all histograms at the end
  std::map<int, TProfile*>::const_iterator itH = m_raw_noiseHistos.begin();

  for( ; itH != m_raw_noiseHistos.end(); ++itH ) { 
    // Limit to selected tell1s
    if ( m_selectedTells && 
         !binary_search(m_limitToTell.begin(), m_limitToTell.end(), (this->readoutTool())->SourceIDToTELLNumber((*itH).first))) {
      continue;
    }
    updateNoiseHistogram( (*itH).first, m_useODINTime );
    if(m_checkCalculation) dumpNoiseCalculation( (*itH).first );
  } 
  if(m_summaryPlots) updateSummaryHistograms();

  return ST::HistoAlgBase::finalize();// must be called after all other actions
}

void ST::STNoiseMonitor::updateNoiseHistogram(unsigned int sourceID, bool updateTitle) {
  if(m_debug) debug() << "updateNoiseHistograms:\t" << m_evtNumber << endmsg;
  // Get the histogram and reset it in case it is already booked. 
  if( m_raw_noiseHistos.find(sourceID) != m_raw_noiseHistos.end() 
      && m_raw_pedestalHistos.find(sourceID) != m_raw_pedestalHistos.end()
      && m_pedsub_noiseHistos.find(sourceID) != m_pedsub_noiseHistos.end() 
      && m_pedsub_pedestalHistos.find(sourceID) != m_pedsub_pedestalHistos.end()
      && m_cms_noiseHistos.find(sourceID) != m_cms_noiseHistos.end() 
      && m_cms_pedestalHistos.find(sourceID) != m_cms_pedestalHistos.end()
      && m_cm_noiseHistos.find(sourceID) != m_cm_noiseHistos.end() ){

    // Plots / strip
    TProfile* raw_noiseHist = m_raw_noiseHistos.find(sourceID)->second;
    raw_noiseHist->Reset();
    TProfile* raw_pedestalHist = m_raw_pedestalHistos.find(sourceID)->second;
    raw_pedestalHist->Reset();
  
    TProfile* pedsub_noiseHist = m_pedsub_noiseHistos.find(sourceID)->second;
    pedsub_noiseHist->Reset();
    TProfile* pedsub_pedestalHist = m_pedsub_pedestalHistos.find(sourceID)->second;
    pedsub_pedestalHist->Reset();
  
    TProfile* cms_noiseHist = m_cms_noiseHistos.find(sourceID)->second;
    cms_noiseHist->Reset();
    TProfile* cms_pedestalHist = m_cms_pedestalHistos.find(sourceID)->second;
    cms_pedestalHist->Reset();
  
    TProfile* cm_noiseHist = m_cm_noiseHistos.find(sourceID)->second;
    cm_noiseHist->Reset();

    // Loop over strips in tell1
    unsigned int strip=0;
    std::vector<double>::const_iterator itRawPed = m_noiseTool->pedestalBegin(sourceID);
    std::vector<double>::const_iterator itRawNoise = m_noiseTool->rawNoiseBegin(sourceID);
    std::vector<double>::const_iterator itRawNoiseEnd = m_noiseTool->rawNoiseEnd(sourceID);

    std::vector<double>::const_iterator itPedSubPed = m_noiseTool->pedSubMeanBegin(sourceID);
    std::vector<double>::const_iterator itPedSubNoise = m_noiseTool->pedSubNoiseBegin(sourceID);

    std::vector<double>::const_iterator itCMSPed = m_noiseTool->cmsMeanBegin(sourceID);
    std::vector<double>::const_iterator itCMSNoise = m_noiseTool->cmsNoiseBegin(sourceID);

    std::vector<bool>::const_iterator itStatus = m_noiseTool->stripStatusBegin(sourceID);
    for(; itRawNoise != itRawNoiseEnd; ++itRawNoise, ++itRawPed, ++itPedSubNoise, ++itPedSubPed, 
          ++itCMSNoise, ++itCMSPed, ++itStatus, ++strip) {
      if(*itStatus) {
        raw_noiseHist->Fill( strip, (*itRawNoise) );
        raw_pedestalHist->Fill( strip, (*itRawPed) );
        pedsub_noiseHist->Fill( strip, (*itPedSubNoise) );
        pedsub_pedestalHist->Fill( strip, (*itPedSubPed) );
        cms_noiseHist->Fill( strip, (*itCMSNoise) );
        cms_pedestalHist->Fill( strip, (*itCMSPed) );
        double commonMode = gsl_pow_2(*itRawNoise)-gsl_pow_2(*itCMSNoise);
        if(commonMode > 0) commonMode = sqrt(commonMode);
        else commonMode = 0;
        cm_noiseHist->Fill( strip, commonMode );
      }
    }
    if(updateTitle) {
      std::string title=raw_noiseHist->GetTitle();
      title += " "+m_odinEvent;
      raw_noiseHist->SetTitle(title.c_str());
    }
  } else {
    unsigned int tellID = m_useSourceID ? sourceID : (this->readoutTool())->SourceIDToTELLNumber(sourceID);
    Warning("No histogram booked for "+std::to_string(tellID),0,StatusCode::SUCCESS).ignore();
  }
}

void ST::STNoiseMonitor::updateSummaryHistograms(){

  if(m_debug) debug() << "updateSummaryHistograms:\t" << m_evtNumber << endmsg;
  m_1d_raw_noise->reset();
  m_1d_raw_pedestal->reset();
  m_1d_pedsub_noise->reset();
  m_1d_pedsub_pedestal->reset();
  m_1d_cms_noise->reset();
  m_1d_cms_pedestal->reset();
  m_1d_cm_noise->reset();

  m_2d_RawPedestalPerLinkVsTell1->Reset();
  m_2d_RawNoisePerLinkVsTell1->Reset();
  m_2d_PedSubPedestalPerLinkVsTell1->Reset();
  m_2d_PedSubNoisePerLinkVsTell1->Reset();
  m_2d_CMSPedestalPerLinkVsTell1->Reset();
  m_2d_CMSNoisePerLinkVsTell1->Reset();
  m_2d_NormalisationPerLinkVsTell1->Reset();

  if(m_plotByType) {
    std::map<std::string, std::string>::iterator iType = m_types.begin();
    for(; iType != m_types.end(); ++iType) {
      std::string type = (*iType).first;
      m_1d_raw_noiseByType[type]->reset();
      m_1d_cms_noiseByType[type]->reset();
      m_1d_cm_noiseByType[type]->reset();
      m_1d_raw_pedestalByType[type]->reset();
      m_1d_cms_pedestalByType[type]->reset();
    }
  }
  std::map<unsigned int, unsigned int>::const_iterator itT = (this->readoutTool())->SourceIDToTELLNumberMap().begin();
  for(; itT != (this->readoutTool())->SourceIDToTELLNumberMap().end(); ++itT) {
    unsigned int sourceID = (*itT).first;
    unsigned int tell1ID =  (*itT).second;
    std::vector<double>::const_iterator itRawPed = m_noiseTool->pedestalBegin(sourceID);
    std::vector<double>::const_iterator itRawNoise = m_noiseTool->rawNoiseBegin(sourceID);
    std::vector<double>::const_iterator itRawNoiseEnd = m_noiseTool->rawNoiseEnd(sourceID);
    
    std::vector<double>::const_iterator itPedSubPed = m_noiseTool->pedSubMeanBegin(sourceID);
    std::vector<double>::const_iterator itPedSubNoise = m_noiseTool->pedSubNoiseBegin(sourceID);
    
    std::vector<double>::const_iterator itCMSPed = m_noiseTool->cmsMeanBegin(sourceID);
    std::vector<double>::const_iterator itCMSNoise = m_noiseTool->cmsNoiseBegin(sourceID);

    std::vector<bool>::const_iterator itStatus = m_noiseTool->stripStatusBegin(sourceID);
    std::vector<DeSTSector*>* sectors = &m_sectorMap[sourceID];
    int strip=0;
    int sector=0;
    for(; itRawNoise != itRawNoiseEnd; ++itRawNoise, ++itRawPed, ++itPedSubNoise, ++itPedSubPed, ++itCMSNoise, ++itCMSPed,
          ++itStatus, ++strip) {
      if(*itStatus) {
        m_1d_raw_noise->fill( (*itRawNoise) );
        m_1d_raw_pedestal->fill( (*itRawPed) );
        m_1d_pedsub_noise->fill( (*itPedSubNoise) );
        m_1d_pedsub_pedestal->fill( (*itPedSubPed) );
        m_1d_cms_noise->fill( (*itCMSNoise) );
        m_1d_cms_pedestal->fill( (*itCMSPed) );
        double commonMode = gsl_pow_2(*itRawNoise)-gsl_pow_2(*itCMSNoise);
        if(commonMode > 0) commonMode = sqrt(commonMode);
        else commonMode = 0;
        m_1d_cm_noise->fill(commonMode);
        int port = strip / LHCbConstants::nStripsInPort;
        m_2d_RawPedestalPerLinkVsTell1->Fill(tell1ID, port, (*itRawPed));
        m_2d_RawNoisePerLinkVsTell1->Fill(tell1ID, port, (*itRawNoise));
        m_2d_PedSubPedestalPerLinkVsTell1->Fill(tell1ID, port, (*itPedSubPed));
        m_2d_PedSubNoisePerLinkVsTell1->Fill(tell1ID, port, (*itPedSubNoise));
        m_2d_CMSPedestalPerLinkVsTell1->Fill(tell1ID, port, (*itCMSPed));
        m_2d_CMSNoisePerLinkVsTell1->Fill(tell1ID, port, (*itCMSNoise));
        m_2d_NormalisationPerLinkVsTell1->Fill(tell1ID, port, 1.);
        if(m_plotByType) {
          sector = strip / m_nStripsInSector;
          if((*sectors)[sector] != 0) {
            std::string type=(*sectors)[sector]->type();
            m_1d_raw_noiseByType[type]->fill( (*itRawNoise) );
            m_1d_raw_pedestalByType[type]->fill( (*itRawPed) );
            m_1d_cms_noiseByType[type]->fill( (*itCMSNoise) );
            m_1d_cms_pedestalByType[type]->fill( (*itCMSPed) );
            m_1d_cm_noiseByType[type]->fill( commonMode );
          }          
        }
      } // strip status
    }
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
  
  std::string idTELL = std::to_string(TELL);
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
