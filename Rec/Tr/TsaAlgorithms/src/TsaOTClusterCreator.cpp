// $Id: TsaOTClusterCreator.cpp,v 1.2 2006-07-25 07:00:27 cattanem Exp $

//GaudiKernel
#include "GaudiKernel/AlgFactory.h"
#include <algorithm>


// Det
#include "OTDet/DeOTDetector.h"
#include "OTDet/DeOTModule.h"

// Kernel
#include "Kernel/OTChannelID.h"
#include "Kernel/Trajectory.h"

// Tsa includes
#include "TsaKernel/TsaConstants.h"
#include "TsaOTClusterCreator.h"
#include "TsaKernel/OTCluster.h"

//STL
#include <algorithm>
#include <map>

#include "GaudiKernel/PhysicalConstants.h"

// Needed for the creation of TrSimpleLayersCreator objects.
static const AlgFactory<TsaOTClusterCreator> s_factory;
const IAlgFactory& TsaOTClusterCreatorFactory = s_factory;
//_________________________________________________
// TsaOTClusterCreator
//
// An TrClusterCreator is a Gaudi sub algorithm that creates Inner tracker Cluster 
// from the digitizations and stores them in the correct layer
// 
TsaOTClusterCreator::TsaOTClusterCreator(const std::string& name,
                                             ISvcLocator* pSvcLocator):
  TsaBaseAlg(name, pSvcLocator)
{
  // TsaOTClusterCreator constructor
  declareProperty("maxOcc", m_maxOcc = 0.30); // 0.3
  declareProperty("clusterSize", m_clusterSize = 6); //6
  declareProperty("distFudgeFactor",m_distFudgeFactor=72.0*Gaudi::Units::cm);
}

TsaOTClusterCreator::~TsaOTClusterCreator()
{
  // TsaOTClusterCreator destructor
}

StatusCode TsaOTClusterCreator::initialize()
{
  // Initializes TsaOTClusterCreator at the begin of program execution.
  StatusCode sc = TsaBaseAlg::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }
  
  // get geometry
  m_tracker = getDet<DeOTDetector>(DeOTDetectorLocation::Default);
  
  return StatusCode::SUCCESS;
}

StatusCode TsaOTClusterCreator::execute(){
  // Executes TsaOTClusterCreator for one event.
  startTimer();
 
  // init
  m_hotModule = 0u;  
 
  // retrieve clusters
  LHCb::OTTimes* clustCont = get<LHCb::OTTimes>(LHCb::OTTimeLocation::Default);
  //plot(clustCont->size(),"nC",0.,20000.,500); 

  // clusterize
  Tsa::OTClusters* pattClusCont = new Tsa::OTClusters();
  pattClusCont->reserve(clustCont->size());
  this->convert(clustCont, pattClusCont);

  put(pattClusCont,Tsa::OTClusterLocation::Default);

  stopTimer();

  if (m_hotModule != 0){
    return Warning(" hot module", StatusCode::SUCCESS, 1);
  }

  return StatusCode::SUCCESS;

}

StatusCode TsaOTClusterCreator::finalize(){
   
  return TsaBaseAlg::finalize();
 
}


StatusCode TsaOTClusterCreator::convert(LHCb::OTTimes* clusCont, 
           Tsa::OTClusters* pattClusCont) {

 // first hit in first seeding station
 LHCb::OTTimes::iterator startModule = clusCont->begin();
 
 // convert to patt rec clusters
 while (startModule != clusCont->end()){
   LHCb::OTTimes::iterator endModule = startModule;
   bool isOK = processModule(startModule,clusCont->end(),endModule);

   if (isOK == true) {
     // make strings of hits....and copy to a tmp container
     std::vector<Tsa::OTCluster*> tmpCont;
     LHCb::OTTimes::iterator startString = startModule;
     LHCb::OTTimes::iterator endString; 

     while (startString != endModule){
       makeString(startString,endString,endModule);
       std::size_t slength = endString - startString;
       // plot(slength,"slength",0.,50.,50);
       if (slength < m_clusterSize){
         createHits(startString, endString, tmpCont);
       }
       startString = endString;     
     } // startString
 
     addNeighbours(tmpCont,pattClusCont);
   }
   else {
    // flag as hot
    ++m_hotModule;
   }

   startModule = endModule;
 } // clusIter

 return StatusCode::SUCCESS;

}

bool TsaOTClusterCreator::processModule(LHCb::OTTimes::iterator start, 
                                        LHCb::OTTimes::iterator stop,
                                        LHCb::OTTimes::iterator& iterCluster){
 iterCluster = start;
 unsigned int iCount = 0u;
 unsigned int startMod = (*start)->channel().uniqueModule(); 
 while ((iterCluster != stop) &&
       ((*iterCluster)->channel().uniqueModule() == startMod)){
   ++iterCluster;
   ++iCount;
 }

 DeOTModule* theModule = m_tracker->findModule((*start)->channel());  
 double occ = (double)iCount/(double)theModule->nChannels();
 // plot(occ,"count", -0.005, 1.005, 101);

 return (occ > m_maxOcc ? false : true);

}

void TsaOTClusterCreator::makeString(LHCb::OTTimes::iterator start, 
                                     LHCb::OTTimes::iterator& stop,
                                     LHCb::OTTimes::iterator end){

  // string together conseq hits
  LHCb::OTTimes::iterator prev = start;
  stop = start; ++stop;
  while ((stop != end)
    &&((*stop)->channel().straw() == (*prev)->channel().straw()+1)){
    ++stop;
    ++prev;
  } // start
}


void  TsaOTClusterCreator::createHits(LHCb::OTTimes::iterator& start, 
                                      LHCb::OTTimes::iterator& stop,
                                          std::vector<Tsa::OTCluster*>& tmpCont){

  // make the working hits

  // geometry info
  DeOTModule* modInfo = m_tracker->findModule((*start)->channel());
  double error = modInfo->cellRadius()/sqrt(12.0);

  // make in a tmpContainer
  tmpCont.reserve(400);
  for (LHCb::OTTimes::iterator iterC = start; iterC != stop; ++iterC){

    std::auto_ptr<LHCb::Trajectory> traj = modInfo->trajectory((*iterC)->channel());
    
 
    Tsa::OTCluster* aCluster = new Tsa::OTCluster(traj,error, driftRadius(*iterC),
						  m_tracker, *iterC);

    tmpCont.push_back(aCluster);

  } // iterC

}

void TsaOTClusterCreator::addNeighbours(std::vector<Tsa::OTCluster*>& tmpCont,
                                        Tsa::OTClusters* pattClusCont){

  // copy across adding neighbour info....
  for (std::vector<Tsa::OTCluster*>::iterator tmpIter = tmpCont.begin(); tmpIter != tmpCont.end(); ++tmpIter){
    std::vector<Tsa::OTCluster*> tneighbours;
    findNeighbours(tmpCont, (*tmpIter)->channel(), tneighbours);
    (*tmpIter)->setNeighbours(tneighbours);
    pattClusCont->add(*tmpIter);
    // plot(tneighbours.size(),"neighbours", -0.5, 10.5, 11);
  } // tmpIter

}

void TsaOTClusterCreator::findNeighbours(const std::vector<Tsa::OTCluster*>& clusCont,
                                         const LHCb::OTChannelID aChannel,
                                         std::vector<Tsa::OTCluster*>& neighbours ) const{
 
  std::vector<LHCb::OTChannelID> testChans; 
  neighbourChan(aChannel,testChans);
  // check for neighbours
  for (std::vector<LHCb::OTChannelID>::iterator iterChan = testChans.begin(); iterChan != testChans.end() ;++iterChan){ 
     Tsa::OTClusters::const_iterator iterClus = clusCont.begin();
     bool found = false;
     while ((iterClus != clusCont.end())&&(found == false)){
       if ((*iterClus)->channel() == *iterChan){
         found = true;
         neighbours.push_back(*iterClus);
       }
       else {
         ++iterClus;
       }
     } // iterClus 
  } // iterChan
  
}

void TsaOTClusterCreator::neighbourChan(const LHCb::OTChannelID& aChannel, 
                                         std::vector<LHCb::OTChannelID>& nChannels) const{

  // left neighbour
 
  LHCb::OTChannelID nextLeft = m_tracker->nextChannelLeft(aChannel);
  if (nextLeft != LHCb::OTChannelID(0) ){
    nChannels.push_back(nextLeft);
  }

  // rightNeighbour
  LHCb::OTChannelID nextRight = m_tracker->nextChannelRight(aChannel);
  if (nextRight != LHCb::OTChannelID(0) ){
    nChannels.push_back(nextRight);
  }
 
  // doublet neighbours....
  doubletNeighbours(aChannel, nChannels);
}

void TsaOTClusterCreator::doubletNeighbours(const LHCb::OTChannelID& aChannel, 
                                           std::vector<LHCb::OTChannelID>& nChannels) const{

  // neighbours in double layer...
  DeOTModule* theModule = m_tracker->findModule(aChannel);  
  unsigned int nStraw = theModule->nChannels();
  unsigned int theStraw = aChannel.straw();
  if (theModule->monoLayerA(theStraw)){

    // monolayer A 
    unsigned int firstStraw = (nStraw/2u)+theStraw;

    unsigned int secondStraw  = firstStraw-1u;
    LHCb::OTChannelID firstChan  = LHCb::OTChannelID(aChannel.station(),aChannel.layer(),aChannel.quarter(),
                                         aChannel.module(),firstStraw);
    nChannels.push_back(firstChan);
    if (secondStraw > nStraw/2u){
      LHCb::OTChannelID secondChan  = LHCb::OTChannelID(aChannel.station(),aChannel.layer(),aChannel.quarter(),
                                            aChannel.module(),secondStraw);
      nChannels.push_back(secondChan);
    }
  }   
  else {
    // monolayer B
    unsigned int firstStraw  = theStraw - (nStraw/2u);
    unsigned int secondStraw  = firstStraw+1u;
     LHCb::OTChannelID firstChan  = LHCb::OTChannelID(aChannel.station(),aChannel.layer(),aChannel.quarter(),
                                         aChannel.module(),firstStraw);
    nChannels.push_back(firstChan);
    if (secondStraw < nStraw/2u){
      LHCb::OTChannelID secondChan  = LHCb::OTChannelID(aChannel.station(),aChannel.layer(),aChannel.quarter(),
                                           aChannel.module(),secondStraw);
      nChannels.push_back(secondChan);
    }
  }

}

double TsaOTClusterCreator::driftRadius(const LHCb::OTTime* aCluster){

  /// "average drift radius"
  LHCb::OTChannelID aChan = aCluster->channel();

  //Geometry Information
  Gaudi::XYZPoint center =m_tracker->findModule(aChan)->centerOfStraw(aChan.straw());
 
  double distToCenter =
          m_tracker->distanceAlongWire(aChan,center.x(),center.y());
  double timeAlongWire =
          (distToCenter+m_distFudgeFactor)*m_tracker->propagationDelay();
   
  //Convert the corrected time to a distance.
  double driftDist =
         m_tracker->driftDistance(aCluster->calibratedTime()-timeAlongWire);
  return fabs(driftDist);
}
