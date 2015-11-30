// from Gaudi
#include "GaudiKernel/AlgFactory.h"
// Local
#include "RichDetailedFrontEndResponsePMT.h"

using namespace Rich::MC::Digi;

//-----------------------------------------------------------------------------
// Implementation file for class : RichDetailedFrontEndResponsePMT
// 2015-10-08 : Marcin Kucharczyk, Mariusz Witek
//-----------------------------------------------------------------------------
DECLARE_ALGORITHM_FACTORY(DetailedFrontEndResponsePMT)

DetailedFrontEndResponsePMT::DetailedFrontEndResponsePMT(
                             const std::string& name, ISvcLocator* pSvcLocator)
  : Rich::AlgBase(name, pSvcLocator),
    m_sDeposits(NULL)
{
  declareProperty("MCRichSummedDepositsLocation", m_mcRichSumDepsLocation = 
                  LHCb::MCRichSummedDepositLocation::Default);
  declareProperty("MCRichDigitsLocation", m_mcRichDigitsLocation = 
                  LHCb::MCRichDigitLocation::Default);
  declareProperty("ChargeSharing",m_chargeSharing = false);
  declareProperty("CrossTalk",m_crossTalk = false);
  declareProperty("Spillover",m_spillover = false);
  declareProperty("ElectronicNoise",m_electronicNoise = false);
}


DetailedFrontEndResponsePMT::~DetailedFrontEndResponsePMT(){}


StatusCode DetailedFrontEndResponsePMT::initialize() 
{
  StatusCode sc = Rich::AlgBase::initialize();
  if(sc.isFailure()) {return sc;}
  // Create a collection of all pixels
  const Rich::ISmartIDTool* smartIDs(NULL);
  acquireTool("RichSmartIDTool",smartIDs,0,true);
  const LHCb::RichSmartID::Vector& pixels = smartIDs->readoutChannelList();
  if(msgLevel(MSG::DEBUG)) debug() << "Retrieved " << pixels.size()
                                   << " pixels in active list" << endmsg;
  releaseTool(smartIDs);
  // Load parameters from database
  m_propertyTool = tool<RichChannelPropertiesPMT>("RichChannelPropertiesPMT",
                                                  this);
  m_propertyTool->ReadChannelParameters(pixels);
  // Parameters - temporary solution!
  // In the future they will be loaded for every pixel from database using 
  // RichChannelPropertiesPMT tool
  m_meanGainSmallPix    = 1.8;
  m_sigmaGainSmallPix   = 0.6;
  m_meanGainLargePix    = 1.4;
  m_sigmaGainLargePix   = 0.4;
  m_horizontalChSharing = 0.03;
  m_verticalChSharing   = 0.02;
  m_diagonalChSharing   = 0.01;
  m_meanXTalkHorzSmall  = 0.025;  // max = 0.05
  m_meanXTalkVertSmall  = 0.040;  // max = 0.08
  m_meanXTalkDiagSmall  = 0.010;  // max = 0.02
  m_meanXTalkHorzLarge  = 0.035;  // max = 0.07
  m_meanXTalkVertLarge  = 0.015;  // max = 0.03
  m_meanXTalkDiagLarge  = 0.010;  // max = 0.02
  m_FPGAwindow          = 50.0;
  m_claroShapePar1      = 1.0;
  m_claroShapePar2      = 2.7;
  m_frameLength         = 200;
  m_amplification       = 1.0;
  m_meanTZero           = 4.5;
  m_sigmaTZero          = 1.3;
  m_maxTZero            = 10.0;
  m_thresholdSmall      = 0.90;
  m_thresholdLarge      = 0.86;
  m_meanENoise          = 0.0;
  m_sigmaENoise         = 2.0 / 3.0;
  // Claro response function
  m_claroShape.reserve(m_frameLength);
  getClaroShape();
  // Random number generators
  sc = m_gaussGain.initialize(randSvc(),Rndm::Gauss(0.0,1.0));
  if(sc.isFailure()) return Error("Failed to initialize Rndm",sc);
  sc = m_gaussTZero.initialize(randSvc(),Rndm::Gauss(m_meanTZero,m_sigmaTZero));
  if(sc.isFailure()) return Error("Failed to initialize Rndm",sc);
  sc = m_gaussENoise.initialize(randSvc(),
                                Rndm::Gauss(m_meanENoise,m_sigmaENoise));
  if(sc.isFailure()) return Error("Failed to initialize Rndm",sc);

  return sc;
}


StatusCode DetailedFrontEndResponsePMT::finalize()
{
  const StatusCode sc = (m_gaussGain.finalize() &&
                         m_gaussENoise.finalize() && 
                         m_gaussTZero.finalize());
  if(sc.isFailure()) { 
    Warning("Failed to finaliZe random number generators").ignore();
  }
  return Rich::AlgBase::finalize();
}


StatusCode DetailedFrontEndResponsePMT::execute()
{
  if(msgLevel(MSG::DEBUG)) debug() << "Execute" << endmsg;
  m_sDeposits = get<LHCb::MCRichSummedDeposits>(m_mcRichSumDepsLocation);
  if(msgLevel(MSG::DEBUG)) {
    debug() << "Successfully located " << m_sDeposits->size()
            << " MCRichSummedDeposits at " 
            << m_mcRichSumDepsLocation << endmsg;
  }
  m_channels.clear();
  m_channelsToCheck.clear();
  // Analog simulation
  StatusCode sc = Analog();
  if(sc.isFailure()) {return sc;}
  // Digital simulation
  sc = Digital();
  return sc;
}


StatusCode DetailedFrontEndResponsePMT::Analog()
{
  if(msgLevel(MSG::DEBUG)) debug() << "Analog Simulation" << endmsg;

  // ANALOG RESPONSE
  std::vector<PMTChannel> channels;
  int iLoop = 0;
  for(LHCb::MCRichSummedDeposits::const_iterator iSD = m_sDeposits->begin(); 
      iSD != m_sDeposits->end(); ++iSD) {
    if(!((*iSD)->history().signalEvent())) continue;
    iLoop++;
    // Check pixel size (temporary solution)
    bool grandPixel = (*iSD)->key().pixelID().isLargePMT();
    if(msgLevel(MSG::DEBUG)) {
      debug() << "Pixel size (0:small, 1:large) = " << grandPixel << endmsg;
    }
    // Equivalence of given summed deposit in nr of photons (E = 0.02)
    const int nrPhotons = (int)((*iSD)->summedEnergy() / 0.02);
    // Nr of electrons from single photon - gain in pixel (up to 3-4)
    double gain = -1.0;
    if(!grandPixel) {
      while(gain < 0.0) gain = getPixelGain(m_meanGainSmallPix,
                                            m_sigmaGainSmallPix);
    } else {
      while(gain < 0.0) gain = getPixelGain(m_meanGainLargePix,
                                            m_sigmaGainLargePix);
    }
    // Number of electrons per channel (in mln)
    double NE = nrPhotons * gain;
    // Load information for every channel
    PMTChannel channel;
    channel.summedDeposit = *iSD;
    channel.richPMTSmartID = (*iSD)->key().pixelID();
    channel.grandPixel = grandPixel;
    channel.NE = NE;
    channel.isCSCT = 0;
    channels.push_back(channel);
  }

  // CHARGE SHARING + CROSS TALK
  if(m_chargeSharing || m_crossTalk) {
    int iLoop = 0;
    for(std::vector<PMTChannel>::iterator ch = channels.begin(); 
        ch != channels.end(); ++ch) {
      iLoop++;
      PMTChannel channel = *ch;
      LHCb::RichSmartID smartID = channel.richPMTSmartID;
      unsigned int pixelRow = smartID.pixelRow();
      unsigned int pixelCol = smartID.pixelCol();
      double meanXTalkHorz = m_meanXTalkHorzSmall;
      double meanXTalkVert = m_meanXTalkVertSmall;
      double meanXTalkDiag = m_meanXTalkDiagSmall;
      if(channel.grandPixel) {
        meanXTalkHorz = m_meanXTalkHorzLarge;
        meanXTalkVert = m_meanXTalkVertLarge;
        meanXTalkDiag = m_meanXTalkDiagLarge;
      }
      // Left horizontal neighbour
      if(pixelRow != 0) {
        StatusCode sc = addChannel(smartID,channel.summedDeposit,channel.NE,
                                   pixelRow-1,pixelCol,m_horizontalChSharing,
                                   meanXTalkHorz);
        if(sc.isFailure()) Warning("Cannot add channel").ignore();
      }
      // Right horizontal neighbour
      if(pixelRow != 7) {
        StatusCode sc = addChannel(smartID,channel.summedDeposit,channel.NE,
                                   pixelRow+1,pixelCol,m_horizontalChSharing,
                                   meanXTalkHorz);
        if(sc.isFailure()) Warning("Cannot add channel").ignore();
      }
      // Lower vertical neighbour
      if(pixelCol != 0) {
        StatusCode sc = addChannel(smartID,channel.summedDeposit,channel.NE,
                                   pixelRow,pixelCol-1,m_verticalChSharing,
                                   meanXTalkVert);
        if(sc.isFailure()) Warning("Cannot add channel").ignore();
      }
      // Upper vertical neighbour
      if(pixelCol != 7) {
        StatusCode sc = addChannel(smartID,channel.summedDeposit,channel.NE,
                                   pixelRow,pixelCol+1,m_verticalChSharing,
                                   meanXTalkVert);
        if(sc.isFailure()) Warning("Cannot add channel").ignore();
      }
      // Lower left diagonal neighbour
      if((pixelRow != 0) && (pixelCol != 0)) {
        StatusCode sc = addChannel(smartID,channel.summedDeposit,channel.NE,
                                   pixelRow-1,pixelCol-1,m_diagonalChSharing,
                                   meanXTalkDiag);
        if(sc.isFailure()) Warning("Cannot add channel").ignore();
      }
      // Lower right diagonal neighbour
      if((pixelRow != 7) && (pixelCol != 0)) {
        StatusCode sc = addChannel(smartID,channel.summedDeposit,channel.NE,
                                   pixelRow+1,pixelCol-1,m_diagonalChSharing,
                                   meanXTalkDiag);
        if(sc.isFailure()) Warning("Cannot add channel").ignore();
      }
      // Upper left diagonal neighbour
      if((pixelRow != 0) && (pixelCol != 7)) {
        StatusCode sc = addChannel(smartID,channel.summedDeposit,channel.NE,
                                   pixelRow-1,pixelCol+1,m_diagonalChSharing,
                                   meanXTalkDiag);
        if(sc.isFailure()) Warning("Cannot add channel").ignore();
      }
      // Upper right diagonal neighbour
      if((pixelRow != 7) && (pixelCol != 7)) {
        StatusCode sc = addChannel(smartID,channel.summedDeposit,channel.NE,
                                   pixelRow+1,pixelCol+1,m_diagonalChSharing,
                                   meanXTalkDiag);
        if(sc.isFailure()) Warning("Cannot add channel").ignore();
      }
    }
    // Add new channels
    for(std::vector<PMTChannel>::iterator chNew = m_channelsToCheck.begin(); 
        chNew != m_channelsToCheck.end(); ++chNew) {
      bool isUnique = true;
      int ich = 0;
      for(std::vector<PMTChannel>::iterator ch = channels.begin(); 
          ch != channels.end(); ++ch) {
        if((*chNew).richPMTSmartID == (*ch).richPMTSmartID) {
          if((*ch).isCSCT == 0) channels.at(ich).NE += (*chNew).NE;
          isUnique = false;
        }
      }
      if(isUnique) channels.push_back(*chNew);
    }
  }

  // TIME SAMPLING
  for(unsigned int i = 0; i <  channels.size() ; ++i) {
    std::vector<double> timeShape;
    timeShape.reserve(m_frameLength);
    timeShape = timeSampling(channels.at(i).NE);
    channels.at(i).timeShape = timeShape;
    m_channels.push_back(channels.at(i));
  }

  return StatusCode::SUCCESS;
}


StatusCode DetailedFrontEndResponsePMT::Digital()
{
  if(msgLevel(MSG::DEBUG)) debug() << "Digital simulation" << endmsg;
  LHCb::MCRichDigits* mcRichDigits = new LHCb::MCRichDigits();
  put(mcRichDigits,m_mcRichDigitsLocation);
  int iLoop = 0;
  int nrAboveThreshold = 0;
  // Loop over all channels
  for(unsigned int i = 0; i < m_channels.size() ; ++i) {
    iLoop++;
    // Threshold (cut off 5% of NE distribution (from left))
    double threshold = m_thresholdSmall * m_amplification;
    if(m_channels.at(i).grandPixel) threshold = m_thresholdLarge * 
                                                m_amplification;
    // Smearing from electronic noise 
    if(m_electronicNoise) threshold += m_gaussENoise();
    int iTimeShift = 0;
    double timeShift = 999999.0;
    int aboveThreshold = 0;
    for(std::vector<double>::iterator iTS =
        m_channels.at(i).timeShape.begin(); 
        iTS != m_channels.at(i).timeShape.end(); ++iTS) {
      if((*iTS) > threshold) {
        timeShift = (double)iTimeShift * (m_FPGAwindow / m_frameLength);
        aboveThreshold = 1;
        break;
      }
      iTimeShift++;
    }
    if((!aboveThreshold) || (timeShift > m_FPGAwindow)) continue;
    // SPILLOVER
    // Do not save digit if there was a signal in corresponding
    // channel from 'Prev' signal photon
    if(m_spillover) {
      if(m_channels.at(i).summedDeposit->history().prevEvent()) continue;
    }
    // Create MCRichHit links
    LHCb::MCRichDigitHit::Vector hitVect;
    const SmartRefVector<LHCb::MCRichDeposit>& deps =
      m_channels.at(i).summedDeposit->deposits();
    for(SmartRefVector<LHCb::MCRichDeposit>::const_iterator iDep =
          deps.begin(); iDep != deps.end(); ++iDep) {
      hitVect.push_back(LHCb::MCRichDigitHit(*((*iDep)->parentHit()),
                                             (*iDep)->history()));
    }
    nrAboveThreshold++;
    LHCb::MCRichDigit* newDigit = new LHCb::MCRichDigit();
    mcRichDigits->insert(newDigit,
                         m_channels.at(i).richPMTSmartID);
    if(msgLevel(MSG::DEBUG)) {
      debug() << newDigit->history() << " newDigit" << endmsg;
    }
    newDigit->setHits(hitVect);
    // Store overall history
    newDigit->setHistory(m_channels.at(i).summedDeposit->history());
    if(msgLevel(MSG::DEBUG)) {
      debug() << newDigit->history() << " MCRichDigit" << endmsg;
    }
  }
 
  return StatusCode::SUCCESS;
}


//=============================================================================
// Get Claro shape
//=============================================================================
void DetailedFrontEndResponsePMT::getClaroShape() 
{
  for(unsigned int bin = 0; bin < m_frameLength; ++bin) {
    double timeBin = bin * (m_FPGAwindow / m_frameLength);
    m_claroShape.push_back(pow((double(timeBin) / m_claroShapePar1),
                               m_claroShapePar2) * 
                           exp(-((double(timeBin) - m_claroShapePar1) / 
                                 (m_claroShapePar1 * m_claroShapePar2))));
  }
  std::transform(m_claroShape.begin(),m_claroShape.end(),m_claroShape.begin(),
                 std::bind2nd(std::divides<double>(),
                              *(std::max_element(m_claroShape.begin(),
                                                 m_claroShape.end()))));
}


//=============================================================================
// Get pixel gain
//=============================================================================
double DetailedFrontEndResponsePMT::getPixelGain(double mean, double sigma)
{
  double pixelGain = m_gaussGain();
  pixelGain = pixelGain * sigma + mean;
  return pixelGain;
}


//=============================================================================
// Time sampling (shape from CLARO)
//=============================================================================
std::vector<double> DetailedFrontEndResponsePMT::timeSampling(double NE)
{
  std::vector<double> timeShape;
  timeShape.reserve(m_frameLength);
  // Time zero: from detection to aquisition (up to 10 ns (~2 sigma))
  double tZero = -1.0;
  while((tZero < 0.0) || (tZero > m_maxTZero)) tZero = m_gaussTZero();
  // Bin number corresponding to timeZero
  const unsigned int binZero = (int)(tZero / (m_FPGAwindow / m_frameLength));
  for(unsigned int bin = 0; bin < m_frameLength; ++bin) {
    if(bin < binZero) {  
      timeShape.push_back(0.0);
    } else {
      timeShape.push_back(m_claroShape.at(bin-binZero) * NE * m_amplification);
    }
  }
  return timeShape;
}


//=============================================================================
// Set Rich PMT smartID
//=============================================================================
LHCb::RichSmartID DetailedFrontEndResponsePMT::setRichPmtSmartID(
                                               Rich::DetectorType rich,
                                               Rich::Side panel,
                                               unsigned int pdCol,
                                               unsigned int pdNumInCol,
                                               unsigned int pixelRow,
                                               unsigned int pixelCol)
{
  LHCb::RichSmartID smartID;
  smartID.setIDType(LHCb::RichSmartID::MaPMTID);
  smartID.setRich(rich);
  smartID.setPanel(panel);
  smartID.setPD(pdCol,pdNumInCol);
  smartID.setPixelRow(pixelRow);
  smartID.setPixelCol(pixelCol);
  return(smartID);
}


//=============================================================================
// Add charge sharing + cross talk channels
//=============================================================================
StatusCode DetailedFrontEndResponsePMT::addChannel(LHCb::RichSmartID smartID,
                                                   LHCb::MCRichSummedDeposit* 
                                                   summedDeposit,
                                                   double NE,
                                                   unsigned int pixelRow,
                                                   unsigned int pixelCol,
                                                   double chSharing,
                                                   double crossTalk)
{
  LHCb::RichSmartID newSmartID = setRichPmtSmartID(smartID.rich(),
                                                   smartID.panel(),
                                                   smartID.pdCol(),
                                                   smartID.pdNumInCol(),
                                                   pixelRow,
                                                   pixelCol);
  // Check if the channel has been already used
  bool isUsed = false;
  int pntr = -1;
  if(m_channelsToCheck.size()) {
    for(std::vector<PMTChannel>::iterator c = m_channelsToCheck.begin(); 
        c != m_channelsToCheck.end(); ++c) {
      int numCh = c - m_channelsToCheck.begin();
      if((*c).richPMTSmartID == smartID) {
        isUsed = true;
        pntr = numCh;
      }
    }
  }
  if(isUsed) {
    if(m_chargeSharing) m_channelsToCheck.at(pntr).NE += NE * chSharing;
    if(m_crossTalk) m_channelsToCheck.at(pntr).NE += NE * crossTalk;
  } else {
    PMTChannel newChannel;
    newChannel.summedDeposit = summedDeposit;
    newChannel.richPMTSmartID = newSmartID;
    double chSharingNE = 0.0;
    if(m_chargeSharing) chSharingNE = NE * chSharing;
    double crossTalkNE = 0.0;
    if(m_crossTalk) crossTalkNE = NE * crossTalk;
    newChannel.NE = chSharingNE + crossTalkNE;
    newChannel.isCSCT = 1;
    m_channelsToCheck.push_back(newChannel);
  }
  return StatusCode::SUCCESS;
}
