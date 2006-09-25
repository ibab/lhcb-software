// $Id: TsaOTClusterCreator.cpp,v 1.8 2006-09-25 14:54:58 mneedham Exp $

//GaudiKernel
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// Det
#include "OTDet/DeOTDetector.h"
#include "OTDet/DeOTModule.h"

// Kernel
#include "Kernel/OTChannelID.h"
#include "Kernel/Trajectory.h"
#include "Kernel/IUsedLHCbID.h"

// Tsa includes
#include "TsaKernel/TsaConstants.h"
#include "TsaOTClusterCreator.h"
#include "TsaKernel/OTCluster.h"


//STL
#include <algorithm>
#include <map>

DECLARE_ALGORITHM_FACTORY( TsaOTClusterCreator );



//_________________________________________________
// TsaOTClusterCreator
//
// An TrClusterCreator is a Gaudi sub algorithm that creates Inner tracker Cluster 
// from the digitizations and stores them in the correct layer
// 
TsaOTClusterCreator::TsaOTClusterCreator(const std::string& name,
                                             ISvcLocator* pSvcLocator):
  TsaBaseAlg(name, pSvcLocator),
  m_usedClusterTool(0)
{
  // TsaOTClusterCreator constructor
  declareProperty("maxOcc", m_maxOcc = 0.4); // 0.3
  declareProperty("clusterSize", m_clusterSize = 6); //6
  declareProperty("distFudgeFactor",m_distFudgeFactor=75.387*Gaudi::Units::cm);
  declareProperty("filterClusters", m_filterClusters = false );
  declareProperty("clusterFilterName", m_clusterFilterName = "TrackUsedLHCbID");
  declareProperty("outputLocation", m_outputLocation = Tsa::OTClusterLocation::Default);

  m_sqrt12 = sqrt(12.0);

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
  
  // get the modules
  const DeOTDetector::Modules& modVector = m_tracker->modules();
  m_modMap.reserve(modVector.size());
  for (DeOTDetector::Modules::const_iterator iterM = modVector.begin(); iterM != modVector.end(); ++iterM){
     m_modMap.insert((*iterM)->elementID().uniqueModule(),*iterM);
  }  // iterM

  m_usedClusterTool = tool<IUsedLHCbID>(m_clusterFilterName,
                                       "Filter",this );

  return StatusCode::SUCCESS;
}

StatusCode TsaOTClusterCreator::execute(){
  // Executes TsaOTClusterCreator for one event.
  //  startTimer();
 
  // init
  m_hotModule = 0u;  
 
  // retrieve clusters
  LHCb::OTTimes* clustCont = get<LHCb::OTTimes>(LHCb::OTTimeLocation::Default);
  //plot(clustCont->size(),"nC",0.,20000.,500); 

  // clusterize
  Tsa::OTClusters* pattClusCont = new Tsa::OTClusters();
  pattClusCont->reserve(clustCont->size());
  //pattClusCont->reserve(10000);
  this->convert(clustCont, pattClusCont);

  put(pattClusCont,m_outputLocation);

  //  stopTimer();

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
   if (isOK == false) ++m_hotModule;
   // make strings of hits....and copy to a tmp container
   LHCb::OTTimes::iterator startString = startModule;
   LHCb::OTTimes::iterator endString; 

   while (startString != endModule){
     makeString(startString,endString,endModule);
     std::size_t slength = endString - startString;
     //     plot(slength,"slength",0.,50.,50);
     if (slength < m_clusterSize) createHits(startString, endString, pattClusCont, isOK);
     startString = endString;     
   } // startString
 
 
   startModule = endModule;
 } // clusIter

 return StatusCode::SUCCESS;

}

bool TsaOTClusterCreator::processModule(LHCb::OTTimes::iterator start, 
                                        LHCb::OTTimes::iterator stop,
                                        LHCb::OTTimes::iterator& iterCluster ){
 iterCluster = start;
 unsigned int iCount = 0u;
 unsigned int startMod = (*start)->channel().uniqueModule(); 
 while ((iterCluster != stop) &&
       ((*iterCluster)->channel().uniqueModule() == startMod)){
   ++iterCluster;
   ++iCount;
 }

 ModuleMap::iterator iter = m_modMap.find(startMod);
 iter != m_modMap.end() ? m_cachedModule = iter->second : 0;

 // m_cachedModule = m_tracker->findModule((*start)->channel());  
 double occ = (double)iCount/(double)m_cachedModule->nChannels();
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


void  TsaOTClusterCreator::createHits(LHCb::OTTimes::iterator start, 
                                      LHCb::OTTimes::iterator stop,
                                      Tsa::OTClusters* patClusCont, 
                                      const bool isOK){

  // make the working hits

  // geometry info
  double error = m_cachedModule->cellRadius()/m_sqrt12;
  bool isHot = !isOK;

  for (LHCb::OTTimes::iterator iterC = start; iterC != stop; ++iterC){

    LHCb::LHCbID id = LHCb::LHCbID((*iterC)->channel());

    bool take = true;
    if (m_filterClusters == true) take = !m_usedClusterTool->used(id);

    if (take == true) {

      //std::auto_ptr<LHCb::Trajectory> traj = m_cachedModule->trajectory((*iterC)->channel());
      //double wirelength = traj->length();
	   //
	   //            Tsa::OTCluster* aCluster = new Tsa::OTCluster(traj , error, driftRadius(*iterC, wirelength),
	   //						    m_tracker, *iterC, isHot);
  
	   Tsa::OTCluster* aCluster = new Tsa::OTCluster(m_cachedModule , error,
	    					    m_tracker, *iterC, m_distFudgeFactor, isHot);

      patClusCont->add(aCluster);
    }

  } // iterC

}

double TsaOTClusterCreator::driftRadius(const LHCb::OTTime* aCluster, const double wireLength) const{

  const double timeAlongWire =
          ((0.5*wireLength)+m_distFudgeFactor)*m_tracker->propagationDelay();
  const double driftDist =  
         m_tracker->driftDistance(aCluster->calibratedTime()-timeAlongWire);
  return fabs(driftDist);

}



