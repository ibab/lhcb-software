#include <string>
#include <stdio.h>

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

    double itZ= (*iterStation)->userParameterAsDouble("zCenter");    
    double itThickness=(*iterStation)->userParameterAsDouble("Thickness");
    unsigned int numLayers=(*iterStation)->userParameterAsInt("numLayers");   
    unsigned int iStation =(*iterStation)->userParameterAsInt("StationID");
    double SensorWidth=(*iterStation)->userParameterAsDouble("SensorWidth");
    double SensorHeight=(*iterStation)->userParameterAsDouble("SensorHeight");
  
    double holeX =(*iterStation)->userParameterAsDouble("holeX");
    double holeY =(*iterStation)->userParameterAsDouble("holeY");
    double ladderDist=(*iterStation)->userParameterAsDouble("DzLadder");
    unsigned int numStrips =(*iterStation)->userParameterAsInt("numStrips");
    double pitch =(*iterStation)->userParameterAsDouble("pitch");
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

      unsigned int NSensorX = (*iterBox)->userParameterAsInt("NSensorX");
      unsigned int NSensorY = (*iterBox)->userParameterAsInt("NSensorY");
      double SensorOverlap=(*iterStation)->userParameterAsDouble("SensorOverlap");

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
        unsigned int iLayer =(*iterLayer)->userParameterAsInt("LayerID"); 
        if (0 != station->layer(iLayer)) continue;  

        // make sure the Layer ID is in scope 
        //unsigned int iLayer =(*iterLayer)->userParameterAsInt("LayerID");        
        if(iLayer<=0||iLayer>numLayers) {
          log << MSG::ERROR << "Layer ID out of scope!" <<endreq;
          return StatusCode::FAILURE;
        }

        if (iLayer<=4){
          SensorThickness=(*iterStation)->userParameterAsDouble("SensorThicknessTB");
	}
        else {
          SensorThickness=(*iterStation)->userParameterAsDouble("SensorThicknessLR");
	}

        double stereoAngle=(*iterLayer)->userParameterAsDouble("stereoAngle");
        unsigned int position=(*iterLayer)->userParameterAsInt("position");

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
    unsigned int NSensorX1   =(*iterStation)->userParameterAsInt("NSensorX1");
    unsigned int NSensorX2   =(*iterStation)->userParameterAsInt("NSensorX2");

    double SensorOverlap1=(*iterStation)->userParameterAsDouble("SensorOverlap1");
    double SensorOverlap2=(*iterStation)->userParameterAsDouble("SensorOverlap2");
    unsigned int nOverlap1 = (*iterStation)->userParameterAsInt("nOverlap1");
    unsigned int nFine =  (*iterStation)->userParameterAsInt("nFine");
    double  vertGuardRing = (*iterStation)->userParameterAsDouble("verticalGuardRing");
 

    SensorThickness=(*iterStation)->userParameterAsDouble("SensorThickness");
    ladderSize1 =(*iterStation)->userParameterVectorAsInt("VSensor1");
    ladderSize2 =(*iterStation)->userParameterVectorAsInt("VSensor2");

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
       unsigned int iLayer =(*iterLayer)->userParameterAsInt("LayerID");
       if(iLayer<=0||iLayer>numLayers) {
         log << MSG::ERROR << "Layer ID out of scope!" <<endreq;
         return StatusCode::FAILURE;
       }

       double  stereoAngle=(*iterLayer)->userParameterAsDouble("stereoAngle");
      
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










