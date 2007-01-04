// $Id: TTOccupancy.cpp,v 1.10 2007-01-04 10:37:37 jvantilb Exp $

// BOOST
#include "boost/lexical_cast.hpp"

// Gaudi
#include "GaudiKernel/AlgFactory.h"

// xml geometry
#include "STDet/DeSTDetector.h"
#include "STDet/DeTTSector.h"

// Event
#include "Event/STDigit.h"

// LHCbKernel
#include "Kernel/ISTSignalToNoiseTool.h"

// local
#include "TTOccupancy.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( TTOccupancy );

//--------------------------------------------------------------------
//
//  TTOccupancy : Monitor occupancies in Trigger Tracker
//
//--------------------------------------------------------------------

TTOccupancy::TTOccupancy( const std::string& name, 
                          ISvcLocator* pSvcLocator) :
  GaudiHistoAlg(name, pSvcLocator),
  m_sigNoiseTool(0),
  m_tracker(0)
{
  // constructer
  declareProperty("Threshold", m_threshold);
  declareProperty("SigNoiseTool",m_sigNoiseToolName = "STSignalToNoiseTool");
  declareProperty("BinSize", m_binSize = 32);
  declareProperty("DataLocation",m_dataLocation = STDigitLocation::TTDigits);

  m_threshold.reserve(2);
  for (int iThres=0; iThres<2; ++iThres ) {
    m_threshold.push_back(3.0);
  }
}

TTOccupancy::~TTOccupancy()
{
  // destructer
}

StatusCode TTOccupancy::initialize()
{
  // Set the top directory to IT or TT.
  if( "" == histoTopDir() ) setHistoTopDir("TT/");

  // Initialize GaudiHistoAlg
  StatusCode sc = GaudiHistoAlg::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  // detector element
  m_tracker = getDet<DeSTDetector>(DeSTDetLocation::TT);

  // sig to noise tool
  m_sigNoiseTool = tool<ISTSignalToNoiseTool>(m_sigNoiseToolName, 
                                              m_sigNoiseToolName + "TT");

  m_hMax = 512*8;
  m_nBins = m_hMax/m_binSize;

  return StatusCode::SUCCESS;
}

StatusCode TTOccupancy::execute()
{
  // retrieve Digitizations
  STDigits* digitsCont = get<STDigits>(m_dataLocation);

  // histos per digit
  STDigits::const_iterator iterDigit =  digitsCont->begin();
  for( ; iterDigit != digitsCont->end(); ++iterDigit){
    this->fillHistograms(*iterDigit);
  } // loop iterDigit

 
  return StatusCode::SUCCESS;
}

StatusCode TTOccupancy::fillHistograms(const STDigit* aDigit)
{
  // retrieve geom info
  if ( m_sigNoiseTool->signalToNoise(aDigit) >
       m_threshold[aDigit->channelID().station()-1u]){
  
    const STChannelID aChan = aDigit->channelID();  

    DeSTSector* aSector = m_tracker->findSector(aChan);
    DeTTSector* ttSector = dynamic_cast<DeTTSector*>(aSector);

    const unsigned int nstrips = ttSector->nStrip();

    // construct the histogram title
    std::string histo = "Number of digits station "
      +boost::lexical_cast<std::string>(aChan.station()) +
      ", layer "+boost::lexical_cast<std::string>(aChan.layer()) +
      ", region "+boost::lexical_cast<std::string>(aChan.detRegion()) +
      ", row "+boost::lexical_cast<std::string>(ttSector->row());
    
    int offset;
    if (aChan.detRegion() == 1) {
      offset = ttSector->column() - 1;
    }
    else if (aChan.detRegion() == 2) {
      aChan.station() == 1 ? 
        offset = ttSector->column() - 7: offset = ttSector->column() - 8;
    }
    else if (aChan.detRegion() == 3){
      aChan.station() == 1 ? 
        offset = ttSector->column() - 10: offset = ttSector->column() - 11;
    }
    else {
      return Warning( "unknown row " );
    } 
  
    plot((double)aChan.strip()-1.+(nstrips*offset),histo, 0.,m_hMax,m_nBins);

  } // if above threshold

  return StatusCode::SUCCESS;
}
