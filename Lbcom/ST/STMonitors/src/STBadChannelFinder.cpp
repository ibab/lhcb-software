// Include files 

// STKernel
#include "Kernel/STDetSwitch.h"
#include "Kernel/ISTReadoutTool.h"
#include "Kernel/STBoardMapping.h"
#include "Kernel/STTell1Board.h"

// STDAQ
#include "Kernel/STDAQDefinitions.h"

// STDet
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"

// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

// LHCbKernel
#include "Kernel/LHCbConstants.h"
#include "Kernel/BeetleRepresentation.h"

// ODIN..
#include "Event/ODIN.h"

// local
#include "STBadChannelFinder.h"
#include "Kernel/ISTNoiseCalculationTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : STBadChannelFinder
//
// 2009-10-26 : Mark Tobin
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
namespace ST {
  DECLARE_ALGORITHM_FACTORY( STBadChannelFinder )
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ST::STBadChannelFinder::STBadChannelFinder( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : ST::HistoAlgBase ( name , pSvcLocator )
{
  declareProperty("MaxEvents",m_maxEvents=4000);

  /// Noise calculation tool
  declareProperty("NoiseToolType",m_noiseToolType="ST::STNoiseCalculationTool");
  declareSTConfigProperty("NoiseToolName",m_noiseToolName,"TTNoiseCalculationTool");

  /// Cut value for low noise in ADC counts (Hard cut for open channels)
  declareProperty("RawNoiseOpen", m_rawNoiseOpen=1.75);

  /// Cut value for low noise in ADC counts (Soft cut to fitted line)
  declareProperty("RawNoiseLow", m_rawNoiseLow=0.5);

  /// Make 2d plots showing bad channels per sector
  declareProperty("PlotBySector", m_plotBySector=false);

  /// Compare channel status with DB
  declareProperty("CompareDB", m_compareWithDB=false);

}
//=============================================================================
// Destructor
//=============================================================================
ST::STBadChannelFinder::~STBadChannelFinder() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode ST::STBadChannelFinder::initialize() {

  StatusCode sc = ST::HistoAlgBase::initialize(); 
  if ( sc.isFailure() ) {
    error() << "Failed to initialize base class" << endmsg;
    return sc;
  }
  m_debug = msgLevel( MSG::DEBUG );
  m_verbose = msgLevel( MSG::VERBOSE);
  if(m_verbose) m_debug = true;
  if (m_debug) debug() << "==> Initialize" << endmsg;
  if(fullDetail()) {
    m_plotBySector=true;
    m_compareWithDB=true;
  }
  if( "" == histoTopDir() ) setHistoTopDir(this->detType()+"/");
  
  m_noiseTool = tool<ST::ISTNoiseCalculationTool>(m_noiseToolType, m_noiseToolName);

  m_nTELL1s = readoutTool()->nBoard();

  // Book histograms...
  bookHistograms();

  return StatusCode::SUCCESS;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode ST::STBadChannelFinder::execute() {

  counter("Number of events") += 1; 
  /*
    const LHCb::ODIN* odin = get<LHCb::ODIN>(LHCb::ODINLocation::Default); 
    const Gaudi::Time time = odin->eventTime();
    info() << "GaudiTime: " << time.day(0) << "/" << time.month(0)+1 << "/" << time.year(0) 
    << " @ " << time.hour(0) << ":" << time.minute(0) << ":" << time.second(0) << endmsg;
  */
  if(m_debug) debug() << "==> Execute: " << counter("Number of events").nEntries() << endmsg;

  if(counter("Number of events").nEntries()%m_maxEvents == 0) {
    // code goes here  
    if(m_debug) debug() << "Looking for new bad channels" << endmsg;
    m_1d_nBad->reset();
    m_1d_nOpen->reset();
    m_1d_nLowNoise->reset();
    if(m_plotBySector) { 
      m_2d_nBad->reset();
      m_2d_nOpen->reset();
      m_2d_nLowNoise->reset();
    }
    findBadChannels();
  } // counter
  return StatusCode::SUCCESS;
}

//==============================================================================
// Loop over list of channels and compare status with that in the conditions DB
//==============================================================================
void ST::STBadChannelFinder::compareWithDB() {
  if(m_debug) debug() << "compareWithDB" << endmsg;
  m_1d_nDifferent->reset();
  /// Loop over TELL1s with bad channels
  std::map<unsigned int,std::vector<ChannelStatus> >::iterator itBad;
  for(itBad = m_badChannelsPerTELL1.begin(); itBad != m_badChannelsPerTELL1.end(); ++itBad) {

    unsigned int TELL1 = (*itBad).first;
    unsigned int sourceID = this->readoutTool()->TELLNumberToSourceID(TELL1);

    STTell1Board* board = readoutTool()->findByBoardID(STTell1ID(sourceID));

    std::vector<ChannelStatus>::iterator itChan = (*itBad).second.begin();
    for(; itChan !=  (*itBad).second.end(); ++itChan) {
      //      unsigned int link = static_cast<unsigned int>((*itChan).first/128);
      LHCb::STChannelID channelID = (board->DAQToOffline(0, STDAQ::v4, STDAQ::StripRepresentation((*itChan).first)).first);
      //      STDAQ::BeetleRepresentation beetle(link);
      DeSTSector *sector = tracker()->findSector(channelID);
      DeSTSector::Status StripStatus = sector->stripStatus(channelID);
      if((int)(*itChan).second != (int)StripStatus) {
        m_1d_nDifferent->fill(TELL1);
        if(m_debug) {
          debug() << "Algorithm+DB different: TELL=" << TELL1 
                  << ",channel=" << (*itChan).first << ",strip=" << channelID.strip() 
                  << ",statusAlg=" << (*itChan).second << ",statusDB=" << StripStatus << endmsg;
        }
      }
    }
  }
}

//==============================================================================
// Loop over all channels and find open/low noise channels
//==============================================================================
void ST::STBadChannelFinder::findBadChannels() {
  // Get the tell1 mapping from source ID to tell1 number
  std::map<unsigned int, unsigned int>::const_iterator itT = (this->readoutTool())->SourceIDToTELLNumberMap().begin();
    
  // loop over the boards
  for(; itT != (this->readoutTool())->SourceIDToTELLNumberMap().end(); ++itT) {
    unsigned int sourceID = (*itT).first;
    unsigned int TELLID = (*itT).second;
    // Temporary hack as possibility to subtract online histograms does not exist in the histogramdb yet.
    m_1d_nBad->fill(TELLID);
    m_1d_nOpen->fill(TELLID);
    m_1d_nLowNoise->fill(TELLID);
    std::vector<ChannelStatus>* badChannels = &m_badChannelsPerTELL1[TELLID];
    badChannels->clear();
    // Loop over raw noise and calculate bad channels
    std::vector<double>::const_iterator itNoiseBegin = m_noiseTool->rawNoiseBegin(sourceID);
    for(unsigned int optLink=0; optLink < STDAQ::noptlinks; ++optLink) {
      for(unsigned int port=0; port < STDAQ::nports; ++port) {
        unsigned int TELL1Channel = optLink*LHCbConstants::nStripsInBeetle+port*LHCbConstants::nStripsInPort;
        std::vector<double>::const_iterator itNoise = itNoiseBegin+TELL1Channel;
        std::vector<double>::const_iterator itNoiseEnd = itNoiseBegin+TELL1Channel+STDAQ::nstrips;
        calcSlope(itNoise, itNoiseEnd); 
        // loop over strips and make cuts
        unsigned int channel = 0;
        for(; itNoise != itNoiseEnd; ++itNoise, ++TELL1Channel, ++channel) {
          double noise = (*itNoise);
          unsigned int sector = static_cast<unsigned int>(TELL1Channel/512);
          // hard cut: opens...
          if(m_rawNoiseOpen > noise && 0.05 < noise) {
            m_1d_nBad->fill(TELLID);
            m_1d_nOpen->fill(TELLID);
            if(m_plotBySector) {
              m_2d_nBad->fill(TELLID,sector);
              m_2d_nOpen->fill(TELLID,sector);
            }
            if(m_debug) debug() << "Open channel: " << TELL1Channel 
                                << ", TELL=" << sourceID << ", "+detType()+"TELL" << TELLID << endmsg;
            // Add some checks for status....
            badChannels->push_back(ChannelStatus(TELL1Channel, ST::STNoiseFlag::Open));
            continue;
          }
          // Soft cut
          double noiseFit = m_slope*channel + m_intercept;
          if(noise > 0.05 && (noiseFit-noise) > m_rawNoiseLow) {
            if(m_debug) debug() << "Low noise channel: " << TELL1Channel << ", TELL=" << sourceID 
                                << ", "+detType()+"TTTELL" << TELLID << endmsg;
            m_1d_nBad->fill(TELLID);
            m_1d_nLowNoise->fill(TELLID);
            if(m_plotBySector) {
              m_2d_nBad->fill(TELLID,sector);
              m_2d_nLowNoise->fill(TELLID,sector);
            }
            badChannels->push_back(ChannelStatus(TELL1Channel, ST::STNoiseFlag::LowNoise));
            continue;
          }
          if(noise > 0.05) {
            badChannels->push_back(ChannelStatus(TELL1Channel,ST::STNoiseFlag::OK));
          }
        }
      } // ports
    } // links
  } // TELL1s
  if(m_compareWithDB) compareWithDB();
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ST::STBadChannelFinder::finalize() {

  debug() << "==> Finalize" << endmsg;

  findBadChannels();

  return ST::HistoAlgBase::finalize(); 
}

//======================================================================================================================
// Calculate slope and intercept of line for channels in a beetle port
//======================================================================================================================
void ST::STBadChannelFinder::calcSlope(std::vector<double>::const_iterator noiseBegin, 
                                       std::vector<double>::const_iterator noiseEnd) {

  std::vector<double>::const_iterator itNoise = noiseBegin;
  double channel=0.;
  double meanX, meanY, meanXY, meanX2;
  meanX=meanY=meanXY=meanX2=0.;
  unsigned int n=0;
  for(; itNoise != noiseEnd; ++itNoise, ++channel, ++n) {
    meanX += channel;
    meanX2 += channel*channel;
    meanY += (*itNoise);
    meanXY += channel*(*itNoise);
  }
  if( n > 0 ) {
    meanX /= n;
    meanX2 /= n;
    meanY /= n;
    meanXY /=n;
    
    m_slope = (meanXY - meanX*meanY) / (meanX2 - meanX*meanX);
    m_intercept = meanY - m_slope*meanX;
  }
  else {
    m_slope = 0.;
    m_intercept = 0.;
    Warning("calcSlope failed due to zero noisy channels").ignore();
  }
  
} 

//======================================================================================================================
// Book histograms
//======================================================================================================================
void ST::STBadChannelFinder::bookHistograms() {
  m_1d_nBad = book1D("Number of bad channels vs TELL1", 0.5, m_nTELL1s+0.5, m_nTELL1s);
  m_1d_nOpen = book1D("Number of open channels vs TELL1", 0.5, m_nTELL1s+0.5, m_nTELL1s);
  m_1d_nLowNoise = book1D("Number of low noise channels vs TELL1", 0.5, m_nTELL1s+0.5, m_nTELL1s);
  if(m_plotBySector) {
    int nSectors=6;
    if(detType() == "IT") nSectors=8;
    double yMax=nSectors-0.5;
    m_2d_nBad = book2D("Number of bad channels per sector vs TELL1", 0.5, m_nTELL1s+0.5, m_nTELL1s, -0.5, yMax, nSectors);
    m_2d_nOpen = book2D("Number of open channels per sector vs TELL1", 0.5, m_nTELL1s+0.5, m_nTELL1s, -0.5, yMax, nSectors);
    m_2d_nLowNoise = book2D("Number of low noise channels per sector vs TELL1", 
                            0.5, m_nTELL1s+0.5, m_nTELL1s, -0.5, yMax, nSectors);
  }
  if(m_compareWithDB) m_1d_nDifferent = book1D("Number of status different channels vs TELL1", 0.5, m_nTELL1s+0.5, m_nTELL1s);
}
