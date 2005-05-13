#include <algorithm>


//#include <stdio.h>

#include "STDet/STDetectionStation.h"
#include "STDet/ITLayer.h"
#include "STDet/TTLayer.h"
#include "STDet/DeSTDetector.h"

#include "DetDesc/IGeometryInfo.h"

#include "CLHEP/Units/PhysicalConstants.h"
#include "Kernel/ITChannelID.h"

/** @file DeSTDetector.cpp
 *
 *  Implementation of class :  DeSTDetector
 *
 *    @author Yuehong Xie Yuehong.Xie@cern.ch
 */


DeSTDetector::DeSTDetector( const std::string& name ) :
  DetectorElement( name ),
    m_nTT(0)
{ }


DeSTDetector::~DeSTDetector() {
  // Delete stations
  std::vector<STDetectionStation*>::iterator first = m_Stations.begin();
  while (first != m_Stations.end()) {
    STDetectionStation* aStation = *first;
    delete aStation;
    first =m_Stations.erase(first);
  } // loop stations
}

const CLID& DeSTDetector::clID () const
{
  return DeSTDetector::classID() ;  
}


StatusCode DeSTDetector::initialize() {

  MsgStream log( msgSvc(), name() );

  StatusCode sc = DetectorElement::initialize();
  if( sc.isFailure() ) {
    log << MSG::ERROR << "Failure to initialize DetectorElement" << endreq;
    return sc;
  }

  log << MSG::DEBUG << "*****now loop over stations"<<endreq;

  // loop over stations
  IDetectorElement::IDEContainer::const_iterator iterStation;
  for (iterStation = this->childBegin(); this->childEnd() != iterStation;
       ++iterStation ) {

    log << MSG::DEBUG << "*****get parameters from"<<(*iterStation)->name()
        << endreq;

    double itZ= (*iterStation)->params()->param<double>("zCenter");    
    double itThickness=(*iterStation)->params()->param<double>("Thickness");
    unsigned int numLayers=(*iterStation)->params()->param<int>("numLayers");   
    unsigned int iStation =(*iterStation)->params()->param<int>("StationID");
    double SensorWidth=(*iterStation)->params()->param<double>("SensorWidth");
    double SensorHeight=(*iterStation)->params()->param<double>("SensorHeight");
  
    double holeX =(*iterStation)->params()->param<double>("holeX");
    double holeY =(*iterStation)->params()->param<double>("holeY");
    double ladderDist=(*iterStation)->params()->param<double>("DzLadder");
    unsigned int numStrips =(*iterStation)->params()->param<int>("numStrips");
    double pitch =(*iterStation)->params()->param<double>("pitch");
    double SensorThickness;

    STDetectionStation* station = new STDetectionStation(itZ, itThickness, numLayers, iStation);  
    this->setNextStation(station);

    if(numLayers>2){
 
      log << MSG::DEBUG << "*****now loop over boxes"<<endreq;
 
      //  loop over boxes 
      IDetectorElement::IDEContainer::const_iterator iterBox;
      for (iterBox = (*iterStation)->childBegin(); 
          (*iterStation)->childEnd() != iterBox;++iterBox ) {

      //  gets the box's name
      std::string boxName = (*iterBox)->name();       
      log << MSG::DEBUG << "*****get parameters from"<<boxName <<endreq;

      unsigned int NSensorX = (*iterBox)->params()->param<int>("NSensorX");
      unsigned int NSensorY = (*iterBox)->params()->param<int>("NSensorY");
      double SensorOverlap=(*iterStation)->params()->param<double>("SensorOverlap");

      log << MSG::DEBUG << "*****now loop over layers"<<endreq;

      //  loop over layers ( or rows for old all-Si-TT1)
      IDetectorElement::IDEContainer::const_iterator iterLayer;
      for (iterLayer = (*iterBox)->childBegin();
           (*iterBox)->childEnd() != iterLayer;++iterLayer ) {


        //  gets the layer's name
        std::string layerName = (*iterLayer)->name();
        log << MSG::DEBUG << "*****get parameters from"<<layerName <<endreq;   
 
        // get the z position of a layer
        double LayerZ = 0.0;
        HepPoint3D pointLocal(0.0, 0.0, 0.0);
        HepPoint3D pointGlobal(0.0, 0.0, 0.0);
        IGeometryInfo* geoData = (*iterLayer)->geometry();
        if ( 0 != geoData ) {
          pointGlobal = geoData->toGlobal( pointLocal );
          LayerZ =  pointGlobal.z();
        } 
        else {
          log << MSG::ERROR << "Failure to get layer position " <<endreq;
          return StatusCode::FAILURE ;
        }

        // make sure this layer was not created
        unsigned int iLayer =(*iterLayer)->params()->param<int>("LayerID"); 
        if (0 != station->layer(iLayer)) continue;  

        // make sure the Layer ID is in scope 
        //unsigned int iLayer =(*iterLayer)->params()->param<int>("LayerID");        
        if(iLayer<=0||iLayer>numLayers) {
          log << MSG::ERROR << "Layer ID out of scope!" <<endreq;
          return StatusCode::FAILURE;
        }

        if (iLayer<=4){
          SensorThickness=(*iterStation)->params()->param<double>("SensorThicknessTB");
	}
        else {
          SensorThickness=(*iterStation)->params()->param<double>("SensorThicknessLR");
	}

        double stereoAngle=(*iterLayer)->params()->param<double>("stereoAngle");
        unsigned int position=(*iterLayer)->params()->param<int>("position");

        ITLayer* layer = new ITLayer(iStation, iLayer, LayerZ,
                   stereoAngle, pitch, SensorWidth, SensorHeight, SensorThickness,
                   SensorOverlap, NSensorX, NSensorY,
                   holeX, holeY, ladderDist, numStrips, position);
        station->setLayer(layer, iLayer);    
       } //loop layers 
      } // loop boxes
  } 

  else { 

    //new TT1
    ++m_nTT;

    std::vector<int> ladderSize1, ladderSize2;
    unsigned int NSensorX1   =(*iterStation)->params()->param<int>("NSensorX1");
    unsigned int NSensorX2   =(*iterStation)->params()->param<int>("NSensorX2");

    double SensorOverlap1=(*iterStation)->params()->param<double>("SensorOverlap1");
    double SensorOverlap2=(*iterStation)->params()->param<double>("SensorOverlap2");
    unsigned int nOverlap1 = (*iterStation)->params()->param<int>("nOverlap1");
    unsigned int nFine =  (*iterStation)->params()->param<int>("nFine");
    double  vertGuardRing = (*iterStation)->params()->param<double>("verticalGuardRing");
 

    SensorThickness=(*iterStation)->params()->param<double>("SensorThickness");
    ladderSize1 =(*iterStation)->params()->param<std::vector<int> >("VSensor1");
    ladderSize2 =(*iterStation)->params()->param<std::vector<int> >("VSensor2");

    IDetectorElement::IDEContainer::const_iterator iterLayer;

    for (iterLayer = (*iterStation)->childBegin();
           (*iterStation)->childEnd() != iterLayer;++iterLayer ) {

       double LayerZ = 0.0;
       HepPoint3D pointLocal( 0.0, 50.0, 0.0);
       HepPoint3D pointGlobal( 0.0, 0.0, 0.0);
       IGeometryInfo* geoData = (*iterLayer)->geometry();
       if ( 0 != geoData ) {
         pointGlobal = geoData->toGlobal( pointLocal );
         LayerZ =  pointGlobal.z();
       } 
       else {
         log << MSG::ERROR << "Failure to get layer position " <<endreq;
         return StatusCode::FAILURE ;
       }
  
       // make sure the Layer ID is in scope
       unsigned int iLayer =(*iterLayer)->params()->param<int>("LayerID");
       if(iLayer<=0||iLayer>numLayers) {
         log << MSG::ERROR << "Layer ID out of scope!" <<endreq;
         return StatusCode::FAILURE;
       }

       double  stereoAngle=(*iterLayer)->params()->param<double>("stereoAngle");
      
       TTLayer* layer = new TTLayer(iStation, iLayer, LayerZ,
                                    stereoAngle, pitch, SensorWidth, SensorHeight,
				    SensorThickness, SensorOverlap1, SensorOverlap2,
                                    nOverlap1, nFine,
                                    NSensorX1, NSensorX2, ladderSize1, ladderSize2,
                                    holeX, holeY, ladderDist, numStrips,vertGuardRing );
       station->setLayer(layer, iLayer);
     }// end loop layer
   }// end new TT1
  } // loop stations
  
  log << MSG::DEBUG << "initializing DeSTDetector succeeded!" << endreq;

  return StatusCode::SUCCESS;
}

unsigned int DeSTDetector::nTT() const{
  return m_nTT;
}

void DeSTDetector::setNextStation(STDetectionStation* station) { 
  m_Stations.push_back(station); 
}

STDetectionStation* DeSTDetector::stationByZ(const double z) const{

  // get pointer to station object using z
  unsigned int iStation = 1;
  STDetectionStation* aStation = 0;
  bool found = false;

  while ((found == false)&&(iStation<=numStations())){
    STDetectionStation* testStation = this->station(iStation);
    if (testStation->isInside(z)){
      aStation = testStation;
      found = true;
    }
    else {
      iStation++;
    }
   
  } // while

  return aStation;
}

STDetectionStation* DeSTDetector::station(const unsigned int iStation) const{

  // get pointer to station object

  STDetectionStation* aStation = 0;
 
  if (iStation >= 1 && iStation <= numStations()) {
     aStation = m_Stations[iStation-1];
  }

  return aStation;
}

STDetectionLayer* DeSTDetector::layerByZ(const double z) const {

  // get pointer to a layer

  STDetectionLayer* aLayer = 0;
  STDetectionStation* aStation = this->stationByZ(z);
 
  if (0 != aStation){
    aLayer = aStation->layerByZ(z);
  }

  return aLayer;
}


STDetectionLayer* DeSTDetector::layer(const ITChannelID aChannel) const {

  // get pointer to a layer by channel id

  STDetectionLayer* aLayer = 0;
  STDetectionStation* aStation = this->station(aChannel.station());
 
  if (0 != aStation){
    aLayer = aStation->layer(aChannel.layer());
  }

  return aLayer;
}

ITChannelID DeSTDetector::nextChannelRight(const ITChannelID aChannel) const{

  STDetectionLayer* aLayer = this->layer(aChannel);
  return aLayer->nextRight(aChannel); 
}

ITChannelID DeSTDetector::nextChannelLeft(const ITChannelID aChannel) const{

  STDetectionLayer* aLayer = this->layer(aChannel);
  return aLayer->nextLeft(aChannel); 
}

std::vector<double> DeSTDetector::zPositions(std::string aType) const {

  std::vector<double> zCont;  
  for (unsigned int iStation =1; iStation <= numStations(); ++iStation){
    STDetectionStation* aStation = station(iStation);
    for (unsigned int iLayer = 1u; iLayer <= aStation->numLayers();++iLayer){
      STDetectionLayer* aLayer = aStation->layer(iLayer);
      if (aLayer->type() == aType){
        zCont.push_back(aLayer->z()+(0.5*aLayer->stagger()));
        zCont.push_back(aLayer->z()-(0.5*aLayer->stagger()));
      }      
    } // iLayer
  } // iStation

  std::sort(zCont.begin(),zCont.end());

  return zCont;
} 
  
std::vector<double> DeSTDetector::zPositions(const std::vector<std::string >& typesCont) const {

  std::vector<double> zCont;
  std::vector<std::string >::const_iterator iterT = typesCont.begin();
  for ( ; iterT != typesCont.end() ; ++iterT){
    std::vector<double> tmpCont = zPositions(*iterT);
    zCont.insert(zCont.begin(),tmpCont.begin(),tmpCont.end());
  } // iterT
 
  std::sort(zCont.begin(),zCont.end());
  return zCont; 
}






