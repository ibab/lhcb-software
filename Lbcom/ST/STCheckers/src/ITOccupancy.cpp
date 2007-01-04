// $Id: ITOccupancy.cpp,v 1.8 2007-01-04 10:37:36 jvantilb Exp $

// BOOST
#include "boost/lexical_cast.hpp"

// Gaudi
#include "GaudiKernel/AlgFactory.h"

// Histogramming
#include "AIDA/IHistogram1D.h"

// xml geometry
#include "STDet/DeSTDetector.h"
#include "STDet/DeITLadder.h"

// GaudiKernel
#include "Kernel/ISTSignalToNoiseTool.h"

// Event
#include "Event/STDigit.h"

// local
#include "ITOccupancy.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( ITOccupancy );

//--------------------------------------------------------------------
//
//  ITOccupancy : Check occupancies in inner tracker
//
//--------------------------------------------------------------------

ITOccupancy::ITOccupancy( const std::string& name, 
                          ISvcLocator* pSvcLocator ) :
  GaudiHistoAlg(name, pSvcLocator),
  m_sigNoiseTool(0),
  m_tracker(0)
{
  // constructer
  declareProperty("Threshold",m_threshold);
  declareProperty("SigNoiseTool",m_sigNoiseToolName = "STSignalToNoiseTool");
  declareProperty("BinSize", m_binSize = 32);
  declareProperty("DataLocation",m_dataLocation = STDigitLocation::ITDigits);

  m_threshold.reserve(3);
  for (int iThres=0;iThres<3;++iThres){
    m_threshold.push_back(3.0);
  } //i
}

ITOccupancy::~ITOccupancy()
{
  // destructer
}

StatusCode ITOccupancy::initialize()
{
  // Set the top directory to IT or TT.
  if( "" == histoTopDir() ) setHistoTopDir("IT/");
 
  // Initialize GaudiHistoAlg
  StatusCode sc = GaudiHistoAlg::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  // detector element
  m_tracker = getDet<DeSTDetector>(DeSTDetLocation::IT);

  // signal to noise tool
  m_sigNoiseTool = tool<ISTSignalToNoiseTool>(m_sigNoiseToolName, 
                                              m_sigNoiseToolName + "IT");

  // intialize histos
  this->initHistograms();

  return StatusCode::SUCCESS;
}

StatusCode ITOccupancy::initHistograms()
{
  int numInVector = 0;
  unsigned int nstrip = m_tracker->sectors().front()->nStrip();
  
  std::vector<DeSTLayer*>::const_iterator iterLayer=m_tracker->layers().begin();
  for ( ; iterLayer != m_tracker->layers().end(); ++iterLayer){

    // Get teh number of strips in this layer
    const DeITLayer* tLayer = dynamic_cast<const DeITLayer*>(*iterLayer);
    unsigned int nStripInLayer = nstrip*tLayer->ladders().size();
    unsigned int bins = nStripInLayer/m_binSize;

    // unique id using station, layer and detRegion
    STChannelID aChan = (*iterLayer)->elementID();
    int id = this->uniqueInt( aChan );

    // construct the histogram title
    std::string histo = " station "
      +boost::lexical_cast<std::string>(aChan.station()) +
      ", layer "+boost::lexical_cast<std::string>(aChan.layer()) +
      ", region "+boost::lexical_cast<std::string>(aChan.detRegion());

    IHistogram1D* stripOccHisto =
      book(id, "Number of digits"+histo,0., bins*m_binSize, bins);
    m_stripOccVector.push_back(stripOccHisto);

    m_layerOccVector.push_back(0.);
    m_nStripsVector.push_back((double)nStripInLayer);

    IHistogram1D* layerOccHisto = book(id+2000,"Occupancy"+histo,0.0,0.2,100);
    m_layerOccHistos.push_back(layerOccHisto);
    
    // add to map
    m_mapping[id] = numInVector;
    ++numInVector;
  } // iterLayer

  return StatusCode::SUCCESS;
}

StatusCode ITOccupancy::execute()
{
  std::vector<double>::iterator iter = m_layerOccVector.begin();
  for( ; iter!=m_layerOccVector.end();++iter) *iter=0.;

  // retrieve Digitizations
  STDigits* digitsCont = get<STDigits>(m_dataLocation);
  
  // histos per digit
  STDigits::const_iterator iterDigit = digitsCont->begin();
  for( ; iterDigit != digitsCont->end(); ++iterDigit){
    this->fillHistograms(*iterDigit);
  } // loop iterDigit

  // av occupancy in each layer
  int iCount = 0;
  for (iter=m_layerOccVector.begin();iter!=m_layerOccVector.end();++iter){
    if ( m_nStripsVector[iCount] != 0 ) { 
      double occ = (double)(*iter)/m_nStripsVector[iCount];
      m_layerOccHistos[iCount]->fill(occ);
    }
    ++iCount;
  }
 
  return StatusCode::SUCCESS;
}

StatusCode ITOccupancy::fillHistograms(const STDigit* aDigit)
{
  // retrieve geom info
  if (m_sigNoiseTool->signalToNoise(aDigit) >
      m_threshold[aDigit->channelID().station()-m_tracker->firstStation()]){
  
    const STChannelID aChan = aDigit->channelID();  
    DeSTSector* aSector = m_tracker->findSector(aChan);

    int id = uniqueInt(aChan);
    int numInVector = m_mapping[id];

    const unsigned int nstrips = aSector->nStrip();

    m_stripOccVector[numInVector]->fill( (double) aChan.strip() - 1. +
                                        (nstrips*(aChan.sector()-1)));
    ++m_layerOccVector[numInVector];

  } // if above threshold

  return StatusCode::SUCCESS;
}

int ITOccupancy::uniqueInt(const STChannelID aChan) const 
{
  return ((aChan.station()*100)+(10*aChan.detRegion())+aChan.layer());
}
