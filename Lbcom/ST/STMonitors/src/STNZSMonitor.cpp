
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
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "GaudiUtils/Aida2ROOT.h"
#include "TProfile.h"
#include "TH2D.h"

// local
#include "STNZSMonitor.h"
#include "Kernel/ISTNoiseCalculationTool.h"

using namespace LHCb;
using namespace AIDA;
using namespace STDAQ;
using namespace STBoardMapping;

DECLARE_ALGORITHM_FACTORY( STNZSMonitor)

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
  declareProperty("UpdateSummaryRate", m_summaryUpdateRate = -1);

  // Debugging
  declareProperty("CheckNoiseCalculation", m_checkCalculation=false);
  // Limit calculation to vector of tell1s given in terms of TELLID (eg TTTELL1 = 1)
  declareProperty("LimitToTell",      m_limitToTell     );

  // Use ODIN time in histograms
  declareProperty("UseODINTime", m_useODINTime = false);

  /// Noise calculation tool
  declareProperty("NoiseToolType",m_noiseToolType="ST::STNoiseCalculationTool");
  declareSTConfigProperty("NoiseToolName",m_noiseToolName,"TTNoiseCalculationTool");

  /// Plot RAW noise - default is CMS noise
  declareProperty("UseRawNoise", m_rawNoise=false);

  /// Plot noise after pedestal subtraction - default is CMS noise
  declareProperty("UsePedSubNoise", m_pedSubNoise=false);

}

StatusCode STNZSMonitor::initialize() {
  // Initialize ST::HistoAlgBase
  StatusCode sc = ST::HistoAlgBase::initialize();
  if (sc.isFailure()) return sc;

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

  if(m_rawNoise && m_pedSubNoise) {
    Warning("Trying to plot raw noise and ped sub noise, setting m_pedSubNoise to false",1,StatusCode::SUCCESS).ignore();
    m_pedSubNoise = false;
  }
  bookHistograms();

  return StatusCode::SUCCESS;
}

void STNZSMonitor::bookHistograms() {
  if(m_debug) debug() << "bookHistograms" << endmsg;
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
    std::string strTellID  = std::to_string(tellID);
    HistoID noiseHistoID        = "noise_$tell" + strTellID;
    std::string noiseHistoTitle = "Noise for " + detType() + "TELL" + strTellID;
    m_noiseHistos[sourceID] =
      Gaudi::Utils::Aida2ROOT::aida2root( bookProfile1D(noiseHistoID, noiseHistoTitle,
                                                        -0.5, nStripsPerBoard-0.5, nStripsPerBoard) );

    HistoID pedHistoID        = "pedestal_$tell" + strTellID;
    std::string pedHistoTitle = "Pedestal for " + detType() + "TELL" + strTellID;
    m_pedestalHistos[sourceID] =
      Gaudi::Utils::Aida2ROOT::aida2root( bookProfile1D(pedHistoID, pedHistoTitle, -0.5, nStripsPerBoard-0.5, nStripsPerBoard) );
  }
  m_2d_noisePerLinkVsTell1 =
    Gaudi::Utils::Aida2ROOT::aida2root( book2D("Noise per link vs TELL1", 0.5, m_nTELL1s+0.5, m_nTELL1s, 0., 96., 96) );
  m_2d_pedestalPerLinkVsTell1 =
    Gaudi::Utils::Aida2ROOT::aida2root( book2D("Pedestal per link vs TELL1", 0.5, m_nTELL1s+0.5, m_nTELL1s, 0., 96., 96) );
  m_2d_normalisationPerLinkVsTell1 =
    Gaudi::Utils::Aida2ROOT::aida2root( book2D("Normalisation", 0.5, m_nTELL1s+0.5, m_nTELL1s, 0., 96., 96) );

  m_1d_noise = book1D("Noise (all strips)", 0., 10., 500);
  if(m_rawNoise) {
    m_1d_pedestal = book1D("Pedestal (all strips)", 100., 160., 500);
  } else {
    m_1d_pedestal = book1D("Pedestal (all strips)", -5., 5., 501);
  }

}

StatusCode STNZSMonitor::execute() {
  m_evtNumber++;

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
  if(m_summaryUpdateRate > 0 && m_evtNumber%m_summaryUpdateRate == 0) {
    updateSummaryPlots();
  }

  return StatusCode::SUCCESS;
}

StatusCode STNZSMonitor::finalize() {
  //printHistos();
  // Update all histograms at the end
  std::map<int, TProfile*>::const_iterator itH = m_noiseHistos.begin();

  for( ; itH != m_noiseHistos.end(); ++itH ) {
    // Limit to selected tell1s
    if ( m_selectedTells &&
         !binary_search(m_limitToTell.begin(), m_limitToTell.end(), (this->readoutTool())->SourceIDToTELLNumber((*itH).first))) {
      continue;
    }
    updateNoiseHistogram( (*itH).first, m_useODINTime );
    updateSummaryPlots();
    if(m_checkCalculation) dumpNoiseCalculation( (*itH).first );
  }

  return ST::HistoAlgBase::finalize();// must be called after all other actions
}

void STNZSMonitor::updateNoiseHistogram(unsigned int sourceID, bool updateTitle) {
  if(m_debug) debug() << "updateNoiseHistogram: " << m_evtNumber << endmsg;

  // Get the histogram and reset it in case it is already booked.
  if( m_noiseHistos.find(sourceID) != m_noiseHistos.end() && m_pedestalHistos.find(sourceID) != m_pedestalHistos.end()) {

    TProfile* noiseHist = m_noiseHistos.find(sourceID)->second;//->second.end();
    noiseHist->Reset();

    TProfile* pedestalHist = m_pedestalHistos.find(sourceID)->second;//->second.end();
    pedestalHist->Reset();
 
    // Loop over strips in tell1
    unsigned int strip=0;

    std::vector<unsigned int>::const_iterator itNEvt = m_noiseTool->nEventsBegin(sourceID);
    std::vector<unsigned int>::const_iterator itNEvtEnd = m_noiseTool->nEventsEnd(sourceID);

    std::vector<double>::const_iterator itNoiseBegin;
    std::vector<double>::const_iterator itPedBegin;
    if(m_rawNoise) {
      itPedBegin = m_noiseTool->pedestalBegin(sourceID);
      itNoiseBegin = m_noiseTool->rawNoiseBegin(sourceID);
    } else if(m_pedSubNoise) {
      itPedBegin = m_noiseTool->pedSubMeanBegin(sourceID);
      itNoiseBegin = m_noiseTool->pedSubNoiseBegin(sourceID);
    } else {
      itPedBegin = m_noiseTool->cmsMeanBegin(sourceID);
      itNoiseBegin = m_noiseTool->cmsNoiseBegin(sourceID);
    }
    std::vector<double>::const_iterator itNoise = itNoiseBegin;
    std::vector<double>::const_iterator itPed = itPedBegin;
    for(; itNEvt != itNEvtEnd; ++itNoise, ++itPed, ++strip, ++itNEvt) {
      double noise = (*itNoise);
      double nevts = (*itNEvt);
      double error = (nevts > 0) ? noise/sqrt(nevts) : 0;
      // trick to set bin error for profile histograms
      noiseHist->SetBinContent( strip+1, noise );
      noiseHist->SetBinEntries( strip+1, 1 );
      noiseHist->SetBinError( strip+1 , sqrt(noise*noise + error*error) );
      pedestalHist->Fill( strip, (*itPed) );
    }
    if(updateTitle) {
      std::string title=noiseHist->GetTitle();
      title += " "+m_odinEvent;
      noiseHist->SetTitle(title.c_str());
    }
  } else {
    unsigned int tellID = m_useSourceID ? sourceID : (this->readoutTool())->SourceIDToTELLNumber(sourceID);
    Warning("No histogram booked for "+std::to_string(tellID),0,StatusCode::SUCCESS).ignore();
  }
}

// Make plots of the noise and pedestal for all strips
// Make 2-d plots of noise and pedestal per link vs tell1.
void STNZSMonitor::updateSummaryPlots() {

  if(m_debug) debug() << "updateSummaryPlots: " << m_evtNumber << endmsg;
  m_1d_noise->reset();
  m_1d_pedestal->reset();
  m_2d_pedestalPerLinkVsTell1->Reset();
  m_2d_noisePerLinkVsTell1->Reset();
  m_2d_normalisationPerLinkVsTell1->Reset();

  std::map<unsigned int, unsigned int>::const_iterator itT = (this->readoutTool())->SourceIDToTELLNumberMap().begin();
  for(; itT != (this->readoutTool())->SourceIDToTELLNumberMap().end(); ++itT) {
    unsigned int sourceID = (*itT).first;
    unsigned int tell1ID =  (*itT).second;

    std::vector<double>::const_iterator itNoiseBegin;
    std::vector<double>::const_iterator itPedBegin;
    if(m_rawNoise) {
      itPedBegin = m_noiseTool->pedestalBegin(sourceID);
      itNoiseBegin = m_noiseTool->rawNoiseBegin(sourceID);
    } else if(m_pedSubNoise) {
      itPedBegin = m_noiseTool->pedSubMeanBegin(sourceID);
      itNoiseBegin = m_noiseTool->pedSubNoiseBegin(sourceID);
    } else {
      itPedBegin = m_noiseTool->cmsMeanBegin(sourceID);
      itNoiseBegin = m_noiseTool->cmsNoiseBegin(sourceID);
    }
    auto itNoise = itNoiseBegin;
    auto itPed = itPedBegin;
    auto itStatus = m_noiseTool->stripStatusBegin(sourceID);
    auto itStatusEnd = m_noiseTool->stripStatusEnd(sourceID);
    int strip=0;
    for(; itStatus != itStatusEnd; ++itNoise, ++itPed, ++itStatus, ++strip) {
      if(*itStatus) {
        m_1d_noise->fill( (*itNoise) );
        m_1d_pedestal->fill( (*itPed) );
        int port = strip / LHCbConstants::nStripsInPort;
        m_2d_pedestalPerLinkVsTell1->Fill(tell1ID, port, (*itPed));
        m_2d_noisePerLinkVsTell1->Fill(tell1ID, port, (*itNoise));
        m_2d_normalisationPerLinkVsTell1->Fill(tell1ID, port, 1.);
      }
    }
  }
}

void STNZSMonitor::dumpNoiseCalculation(unsigned int sourceID) {
  const unsigned int TELL=(this->readoutTool())->SourceIDToTELLNumber(sourceID);
  unsigned int strip=0;
  std::vector<double> rawMean = m_noiseTool->rawMean(TELL);
  auto rawMeanIt = rawMean.begin();
  std::vector<double> rawMeanSq = m_noiseTool->rawMeanSq(TELL);
  auto rawMeanSqIt = rawMeanSq.begin();
  std::vector<double> rawNoise = m_noiseTool->rawNoise(TELL);
  auto rawNoiseIt = rawNoise.begin();
  std::vector<unsigned int> rawN = m_noiseTool->rawN(TELL);
  auto rawNIt = rawN.begin();

  std::vector<double> cmsMean = m_noiseTool->cmsMean(TELL);
  auto cmsMeanIt = cmsMean.begin();
  std::vector<double> cmsMeanSq = m_noiseTool->cmsMeanSq(TELL);
  auto cmsMeanSqIt = cmsMeanSq.begin();
  std::vector<double> cmsNoise = m_noiseTool->cmsNoise(TELL);
  auto cmsNoiseIt = cmsNoise.begin();
  std::vector<unsigned int> cmsN = m_noiseTool->cmsN(TELL);
  auto cmsNIt = cmsN.begin();
  
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
    profile1D(strip, sqrt(*rawMeanSqIt - std::pow(*rawMeanIt,2)), idRawNoiseC, idRawNoiseC, -0.5, 3071, 3072);

    profile1D(strip, (*cmsMeanIt), idCMSMean, idCMSMean, -0.5, 3071.5, 3072); 
    profile1D(strip, (*cmsMeanSqIt), idCMSMeanSq, idCMSMeanSq, -0.5, 3071.5, 3072); 
    profile1D(strip, (*cmsNoiseIt), idCMSNoiseS, idCMSNoiseS, -0.5, 3071.5, 3072); 
    profile1D(strip, sqrt(*cmsMeanSqIt - std::pow(*cmsMeanIt,2)), idCMSNoiseC, idCMSNoiseC, -0.5, 3071.5, 3072);

  }
}
