#include <string>
#include <stdio.h>



#include "ITDet/ITDetectionStation.h"
#include "ITDet/ITSTLayer.h"
#include "ITDet/ITTT1Layer.h"
#include "ITDet/DeITDetector.h"

#include "DetDesc/IGeometryInfo.h"

#include "CLHEP/Units/PhysicalConstants.h"
#include "Kernel/ITChannelID.h"

/** @file DeITDetector.cpp
 *
 *  Implementation of class :  DeITDetector
 *
 *    @author Yuehong Xie Yuehong.Xie@cern.ch
 */


DeITDetector::DeITDetector( const std::string& name ) :
  DetectorElement( name )
{ }


DeITDetector::~DeITDetector() {
  // Delete stations
  std::vector<ITDetectionStation*>::iterator first = m_Stations.begin();
  while (first != m_Stations.end()) {
    ITDetectionStation* aStation = *first;
    delete aStation;
    first =m_Stations.erase(first);
  } // loop stations
}

const CLID& DeITDetector::clID () const
{
  return DeITDetector::classID() ;  
}


StatusCode DeITDetector::initialize() {

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
      double SensorThickness=(*iterStation)->userParameterAsDouble("SensorThickness");
      double SensorOverlap=(*iterStation)->userParameterAsDouble("SensorOverlap");
      double holeX =(*iterStation)->userParameterAsDouble("holeX");
      double holeY =(*iterStation)->userParameterAsDouble("holeY");
      double ladderDist=(*iterStation)->userParameterAsDouble("DzLadder");
      double numStrips =(*iterStation)->userParameterAsInt("numStrips");
      double pitch =(*iterStation)->userParameterAsDouble("pitch");


      ITDetectionStation* station =
          new ITDetectionStation(itZ, itThickness, numLayers, iStation);  
      this->setNextStation(station);

  if(numLayers>2) {
 
    log << MSG::DEBUG << "*****now loop over boxes"<<endreq;
 
    //  loop over boxes 
      IDetectorElement::IDEContainer::const_iterator iterBox;
      for (iterBox = (*iterStation)->childBegin(); 
           (*iterStation)->childEnd() != iterBox;++iterBox ) {



    //  gets the box's name
    std::string boxName = (*iterBox)->name();       
    log << MSG::DEBUG << "*****get parameters from"<<boxName <<endreq;

        double NSensorX,NSensorY;

           NSensorX       =(*iterBox)->userParameterAsInt("NSensorX");
           NSensorY       =(*iterBox)->userParameterAsInt("NSensorY");

    log << MSG::DEBUG << "*****now loop over layers"<<endreq;

    //  loop over layers ( or rows for old all-Si-TT1)
      IDetectorElement::IDEContainer::const_iterator iterLayer;
      for (iterLayer = (*iterBox)->childBegin();
           (*iterBox)->childEnd() != iterLayer;++iterLayer ) {


    //  gets the layer's name
        std::string layerName = (*iterLayer)->name();

   log << MSG::DEBUG << "*****get parameters from"<<layerName <<endreq;   


    // get the z position of a layer
      double LayerZ;
      HepPoint3D pointLocal( 0, 0, 0);
      HepPoint3D pointGlobal( 0, 0, 0);
      IGeometryInfo* geoData = (*iterLayer)->geometry();
      if ( 0 != geoData ) {
        pointGlobal = geoData->toGlobal( pointLocal );
        LayerZ =  pointGlobal.z();
      } else {
    log << MSG::ERROR << "Failure to get layer position " <<endreq;
    return StatusCode::FAILURE ;
      }

    // make sure this layer was not created
        if(station->layerByZ(LayerZ)) continue;  

    // make sure the Layer ID is in scope 
        unsigned int iLayer =(*iterLayer)->userParameterAsInt("LayerID");        
        if(iLayer<=0||iLayer>numLayers) {
         log << MSG::ERROR << "Layer ID out of scope!" <<endreq;
          return StatusCode::FAILURE;
        }

        double stereoAngle=(*iterLayer)->userParameterAsDouble("stereoAngle");
        unsigned int position=(*iterLayer)->userParameterAsInt("position");
        //double angle          =pi/180.*stereoAngle;

        ITSTLayer* layer = new ITSTLayer(iStation, iLayer, LayerZ,
                   stereoAngle, pitch, SensorWidth, SensorHeight, SensorThickness,
                   SensorOverlap, NSensorX, NSensorY,
                   holeX, holeY, ladderDist, numStrips, position);
        station->setLayer(layer, iLayer);    
       } //loop layers 
      } // loop boxes
   } else { //new TT1
        double NSensorX1,NSensorX2;
        std::vector<int> ladderSize1, ladderSize2;
           NSensorX1   =(*iterStation)->userParameterAsInt("NSensorX1");
           NSensorX2   =(*iterStation)->userParameterAsInt("NSensorX2");
           ladderSize1 =(*iterStation)->userParameterVectorAsInt("VSensor1");
           ladderSize2 =(*iterStation)->userParameterVectorAsInt("VSensor2");


      IDetectorElement::IDEContainer::const_iterator iterLayer;

      for (iterLayer = (*iterStation)->childBegin();
           (*iterStation)->childEnd() != iterLayer;++iterLayer ) {
      double LayerZ;
      HepPoint3D pointLocal( 0, 50, 0);
      HepPoint3D pointGlobal( 0, 0, 0);
      IGeometryInfo* geoData = (*iterLayer)->geometry();
      if ( 0 != geoData ) {
        pointGlobal = geoData->toGlobal( pointLocal );
        LayerZ =  pointGlobal.z();
      } else {
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
	//        unsigned int position=(*iterLayer)->userParameterAsInt("position");

          ITTT1Layer* layer = new ITTT1Layer(iStation, iLayer, LayerZ,
                   stereoAngle, pitch, SensorWidth, SensorHeight,
                   SensorThickness, SensorOverlap,
                   NSensorX1, NSensorX2, ladderSize1, ladderSize2,
                   holeX, holeY, ladderDist, numStrips);
          station->setLayer(layer, iLayer);
     }// end loop layer
   }// end new TT1
  } // loop stations
  
  log << MSG::DEBUG << "initializing DeITDetector succeeded!" << endreq;

  return StatusCode::SUCCESS;
}


void DeITDetector::setNextStation(ITDetectionStation* station) { 
  m_Stations.push_back(station); 
}

ITDetectionStation* DeITDetector::stationByZ(const double z) const{

  // get pointer to station object using z
  unsigned int iStation = 1;
  ITDetectionStation* aStation = 0;
  bool found = false;

  while ((found == false)&&(iStation<=numStations())){
    ITDetectionStation* testStation = this->station(iStation);
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

ITDetectionStation* DeITDetector::station(const unsigned int iStation) const{

  // get pointer to station object

  ITDetectionStation* aStation = 0;
 
  if (iStation >= 1 && iStation <= numStations()) {
     aStation = m_Stations[iStation-1];
  }

  return aStation;
}

ITDetectionLayer* DeITDetector::layerByZ(const double z) const {

  // get pointer to a layer

  ITDetectionLayer* aLayer = 0;
  ITDetectionStation* aStation = this->stationByZ(z);
 
  if (0 != aStation){
    aLayer = aStation->layerByZ(z);
  }

  return aLayer;
}


ITDetectionLayer* DeITDetector::layer(const ITChannelID aChannel) const {

  // get pointer to a layer by channel id

  ITDetectionLayer* aLayer = 0;
  ITDetectionStation* aStation = this->station(aChannel.station());
 
  if (0 != aStation){
    aLayer = aStation->layer(aChannel.layer());
  }

  return aLayer;
}

ITChannelID DeITDetector::nextChannelRight(const ITChannelID aChannel) const{

  ITDetectionLayer* aLayer = this->layer(aChannel);
  return aLayer->nextRight(aChannel); 
}

ITChannelID DeITDetector::nextChannelLeft(const ITChannelID aChannel) const{

  ITDetectionLayer* aLayer = this->layer(aChannel);
  return aLayer->nextLeft(aChannel); 
}










