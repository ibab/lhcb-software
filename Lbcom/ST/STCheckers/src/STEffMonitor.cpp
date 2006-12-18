//
// This File contains the definition of the OTSmearer -class
//
// C++ code for 'LHCb Tracking package(s)'
//
//   Author: M. Needham
//   Created: 19-09-2000

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

#include "STEffMonitor.h"
#include "HistFun.h"

// xml geometry
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTLayer.h"

// Event
#include "Event/STCluster.h"
#include "Event/MCHit.h"
#include "Event/MCParticle.h"

#include "Kernel/STDetSwitch.h"
#include "Kernel/IMCParticleSelector.h"

// BOOST
#include "boost/lexical_cast.hpp"

#include "Kernel/STDetSwitch.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( STEffMonitor );

//--------------------------------------------------------------------
//
//  STClusterMonitor : Check Clusterization procedure for the outer tracker
//
//--------------------------------------------------------------------

STEffMonitor::STEffMonitor(const std::string& name, 
                              ISvcLocator* pSvcLocator) :
  GaudiHistoAlg(name, pSvcLocator),
  m_tracker(0)
{
 
  this->declareProperty( "hitTableLocation", m_hitTableLocation =
                         LHCb::MCParticleLocation::Default + "2MCTTHits" );

  this->declareProperty("selectorName", m_selectorName = "MCParticleSelector" );
  this->declareProperty("detType", m_detType = "TT");
  this->declareProperty("includeGuardRings", m_includeGuardRings = true);
  this->declareProperty("printEfficiency", m_pEff = true);
}

STEffMonitor::~STEffMonitor(){
  // destructer
}

StatusCode STEffMonitor::initialize(){

  if( "" == histoTopDir() ) setHistoTopDir(m_detType+"/");

  StatusCode sc = GaudiHistoAlg::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

  // detector element
  m_tracker =  getDet<DeSTDetector>(DeSTDetLocation::location(m_detType));

  m_clusterLocation = STClusterLocation::TTClusters;
  STDetSwitch::flip(m_detType,m_clusterLocation);
  STDetSwitch::flip(m_detType,m_hitTableLocation);

  // Retrieve MCParticle 2 ST MCHit associator

  // selector
  m_selector = tool<IMCParticleSelector>(m_selectorName,m_selectorName,this);

  m_asctLocation = m_clusterLocation+"2MCHits";

  // init histos
  initHistograms();

  return StatusCode::SUCCESS;

}

StatusCode STEffMonitor::execute(){

  // histos per particle
  MCParticles* particles = get<MCParticles>(MCParticleLocation::Default);

  AsctTool associator(evtSvc(), m_asctLocation);
  m_table = associator.inverse();
  if (!m_table) return Error("Failed to find table", StatusCode::FAILURE);

  HitTable hitAsct( evtSvc(), m_hitTableLocation );
  m_hitTable = hitAsct.inverse();  
  if (!m_hitTable) return Error("Failed to find hit table at "+m_hitTableLocation, StatusCode::FAILURE);

  MCParticles::const_iterator iterPart = particles->begin(); 
  for ( ; iterPart != particles->end(); ++iterPart){
    if (m_selector->accept(*iterPart)){
      this->layerEff(*iterPart);
    }
  } // loop iterPart

  return StatusCode::SUCCESS;
}

StatusCode STEffMonitor::finalize(){

  // Finalize

  // init the message service
  if (m_pEff == true){
    info() << " -------Efficiency %--------- " << endmsg;
  }
  else {
    info() << " -------InEfficiency %--------- " << endmsg;
  }

  // print out efficiencys

  // find 
  std::vector<DeSTLayer*>::const_iterator iterLayer = m_tracker->layers().begin();
  for ( ; iterLayer != m_tracker->layers().end(); ++iterLayer){
 
    STChannelID aChan = (*iterLayer)->elementID();
    int iHistoId = findHistoId(uniqueHistoID(aChan));
     
    // eff for this layer
    int nAcc =  m_xLayerHistos[iHistoId]->allEntries();
    int nFound = m_effXLayerHistos[iHistoId]->allEntries();
    double eff = 9999.;
    double err = 9999.;
    if (nAcc > 0){
      eff = 100.0*(double)nFound/(double)nAcc;
      err = sqrt((eff*(100.0-eff))/(double)nAcc);
      if (m_pEff = false) eff = 1-eff;
    }

    if (m_detType == "TT"){
      info() << "Station "  << aChan.station()
             << " Layer" <<  aChan.layer()  <<
                " eff " <<eff<<" +/- "<<err<<endmsg;
    }
    else {
      info() << "Station "  << aChan.station()
             << " Box " << aChan.detRegion() 
             << " Layer" <<  aChan.layer()  <<
                " eff " <<eff<<" +/- "<<err<<endmsg;

    }


  } // iterLayer

  info() << " -----------------------" << endreq;

  // unbook if not full detail mode
  if (fullDetail() == false){
    unBookHistos();
    eraseHistos();
  }

  return  GaudiHistoAlg::finalize();
}

StatusCode STEffMonitor::initHistograms(){
 
 // Intialize histograms

 std::string tDirPath = this->histoPath()+"/";
  
 int numInVector = 0;
 int histID;  

 // Intialize histograms 
 std::vector<DeSTLayer*>::const_iterator iterLayer = m_tracker->layers().begin();
 for ( ; iterLayer != m_tracker->layers().end(); ++iterLayer) {
   
     // uniquely id using station and layer
     // add to map
     STChannelID aChan = (*iterLayer)->elementID();
     unsigned int uniqueID = uniqueHistoID(aChan);
 
     m_Mapping[uniqueID] = numInVector;
     histID = (int)uniqueID+1000;

     // ..
     histID = (int)uniqueID+5000;
     IHistogram1D* xLayerHisto = histoSvc()->book(
                       tDirPath + boost::lexical_cast<std::string>(histID),
		       "Number of hits on strips."+boost::lexical_cast<std::string>(histID),
        		400,-100.,100.);

     m_xLayerHistos.push_back(xLayerHisto);

     //..
     histID = (int)uniqueID+6000;
     IHistogram1D* yLayerHisto = histoSvc()->book(
                       tDirPath + boost::lexical_cast<std::string>(histID),
		       "Number of hits on strips."+boost::lexical_cast<std::string>(histID),
			400,-100.,100.);

     m_yLayerHistos.push_back(yLayerHisto);

     //..
     histID = (int)uniqueID+7000;
     IHistogram1D* effXLayerHisto = histoSvc()->book(
                       tDirPath + boost::lexical_cast<std::string>(histID),
		       "Number of hits on strips."+boost::lexical_cast<std::string>(histID),
			400,-100.,100.);

     m_effXLayerHistos.push_back(effXLayerHisto);

     //..
     histID = (int)uniqueID+8000;
     IHistogram1D* effYLayerHisto = histoSvc()->book(
                       tDirPath + boost::lexical_cast<std::string>(histID),
		       "Number of hits on strips. vs y "+boost::lexical_cast<std::string>(histID),
			400,-100.,100.);

     m_effYLayerHistos.push_back(effYLayerHisto);

     histID = (int)uniqueID+9000;
     IHistogram2D* xyLayerHisto = histoSvc()->book(
                       tDirPath + boost::lexical_cast<std::string>(histID),
		       "Number of hits on strips. vs y "+boost::lexical_cast<std::string>(histID),
			200,-100.,100., 100, -100., 200.);

     m_xyLayerHistos.push_back(xyLayerHisto);

     
     histID = (int)uniqueID+10000;
     IHistogram2D* effXYLayerHisto = histoSvc()->book(
                       tDirPath + boost::lexical_cast<std::string>(histID),
		       "xy eff "+boost::lexical_cast<std::string>(histID),
			200,-100.,100., 200, -100., 100.);

     m_effXYLayerHistos.push_back(effXYLayerHisto);


     ++numInVector;
 } // iLayer

 return StatusCode::SUCCESS;
}

StatusCode STEffMonitor::layerEff(const MCParticle* aParticle){

  // find all MC hitsts

  HitTable::InverseType::Range hits = m_hitTable->relations( aParticle) ;
  if (hits.empty()){
     return StatusCode::FAILURE;
  }
 
  std::vector<DeSTLayer*>::const_iterator iterLayer = m_tracker->layers().begin();
  for ( ; iterLayer != m_tracker->layers().end(); ++iterLayer){
 
     // look for MCHit in this layer.....
     HitTable::InverseType::Range::iterator iterHit = hits.begin();
     std::vector<MCHit*> layerHits;  
     while (iterHit != hits.end()){
       MCHit* aHit = const_cast<MCHit*>( iterHit->to() ); 
       if (isInside(*iterLayer,aHit) == true) layerHits.push_back(aHit);
       ++iterHit;
     } // iterHit

     if (layerHits.size() != 0){
       bool found = false;
       std::vector<MCHit*>::iterator tIter = layerHits.begin();  
       while ((tIter != layerHits.end())&&(found == false)){
         Range range = m_table->relations(*tIter);
         if (range.empty() == true) {
            ++tIter;  
	 }
         else {
	    found = true; 
	 }
       } // tIter
       
       int iHistoId = findHistoId(uniqueHistoID((*iterLayer)->elementID()));
       MCHit* aHit = layerHits.front(); 
       const Gaudi::XYZPoint midPoint = aHit->midPoint();

       // histo vs x
       m_xLayerHistos[iHistoId]->fill(midPoint.x()/Gaudi::Units::cm);    

       // histo vs y
       m_yLayerHistos[iHistoId]->fill(midPoint.y()/Gaudi::Units::cm); 

       //  xy 
       m_xyLayerHistos[iHistoId]->fill(midPoint.x()/Gaudi::Units::cm,
                                       midPoint.y()/Gaudi::Units::cm);

       if (found == true){
         if (m_pEff == true){
           m_effXYLayerHistos[iHistoId]->fill(midPoint.x()/Gaudi::Units::cm,
                                              midPoint.y()/Gaudi::Units::cm);
           m_effXLayerHistos[iHistoId]->fill(midPoint.x()/Gaudi::Units::cm);  
           m_effYLayerHistos[iHistoId]->fill(midPoint.y()/Gaudi::Units::cm);  
         }
       } 
       else {
	 if (m_pEff == false){
           m_effXYLayerHistos[iHistoId]->fill(midPoint.x()/Gaudi::Units::cm,
                                              midPoint.y()/Gaudi::Units::cm);
           m_effXLayerHistos[iHistoId]->fill(midPoint.x()/Gaudi::Units::cm);
           m_effYLayerHistos[iHistoId]->fill(midPoint.y()/Gaudi::Units::cm);
         }
       }       
     } //if
  } // iterLayer

  return StatusCode::SUCCESS;
}


int STEffMonitor::findHistoId(unsigned int aLayerId) {
 return m_Mapping[aLayerId];
} 

int STEffMonitor::uniqueHistoID(const STChannelID aChan) const{
  return m_detType == "TT" ? aChan.station()*100 + aChan.layer()  :
                             aChan.station()*100 + aChan.detRegion()*10 + aChan.layer();
} 

bool STEffMonitor::isInside(const DeSTLayer* aLayer, const MCHit* aHit) const{

  // check if expect hit to make cluster
  bool isFound = false;
  if (aLayer->isInside(aHit->midPoint()) == true){
    if (m_includeGuardRings == false){
      DeSTSector* aSector = m_tracker->findSector(aHit->midPoint());
      isFound = aSector->globalInActive(aHit->midPoint());
    }
    else {
      isFound = true;
    }
  }
  return isFound;
}

void STEffMonitor::unBookHistos(){

  // give ownership back to vector - histos no longer in store
  HistFun::unBookVector(m_xLayerHistos,     histoSvc());
  HistFun::unBookVector(m_yLayerHistos,     histoSvc());
  HistFun::unBookVector(m_xyLayerHistos,    histoSvc());
  HistFun::unBookVector(m_effXLayerHistos,  histoSvc());
  HistFun::unBookVector(m_effYLayerHistos,  histoSvc());
  HistFun::unBookVector(m_effXYLayerHistos, histoSvc());  
}


void STEffMonitor::eraseHistos(){

  // clear everything
  HistFun::eraseVector(m_xLayerHistos);
  HistFun::eraseVector(m_yLayerHistos);
  HistFun::eraseVector(m_xyLayerHistos);
  HistFun::eraseVector(m_effXLayerHistos);
  HistFun::eraseVector(m_effYLayerHistos);
  HistFun::eraseVector(m_effXYLayerHistos);
}




