// $Id: $

// FTDet
#include "FTDet/DeFTDetector.h"

// Gaudi Kernel
#include "GaudiKernel/SystemOfUnits.h"

// Boost
#include <boost/lambda/bind.hpp>

/** @file DeFTDetector.cpp
 *
 *  Implementation of class : DeFTDetector
 *
 *  @author Plamen Hopchev
 *  @date   2012-04-25
 */

using namespace boost::lambda;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DeFTDetector::DeFTDetector( const std::string& name ) :
  DetectorElement( name ),
  m_stations(),
  m_bilayers(),
  m_layers(),
  m_msg(NULL)
{

}

//=============================================================================
// Destructor
//=============================================================================
DeFTDetector::~DeFTDetector(){} 

//=============================================================================
// classID function
//=============================================================================
const CLID& DeFTDetector::clID () const {
  return DeFTDetector::classID();
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode DeFTDetector::initialize(){

  StatusCode sc = DetectorElement::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;

  /// Create a MsgStream object with an add-hoc name (the second argument),
  /// instead of the ususual name(), which gives a too long string
  if ( 0 == m_msg ) m_msg = new MsgStream( msgSvc(), "DeFTDetector" );

  *m_msg << MSG::INFO << "==> Initialize DeFTDetector" << endmsg;

  /// Fill in the vectors holding pointers to the Daugther DetElements

  // loop over stations
  typedef IDetectorElement::IDEContainer::const_iterator DEIter;
  DEIter iS, iBL, iL;
  for (iS = this->childBegin(); iS != this->childEnd(); ++iS) {
    DeFTStation* station = dynamic_cast<DeFTStation*>(*iS);
    if ( station != 0 ) {
      /// fill the vector of stations
      m_stations.push_back(station);
      ///loop over bilayers
      for (iBL = (*iS)->childBegin(); iBL!= (*iS)->childEnd(); ++iBL) {
        DeFTBiLayer* bilayer = dynamic_cast<DeFTBiLayer*>(*iBL);
        if ( bilayer != 0 ) {
          /// fill the vector of bilayers
          m_bilayers.push_back(bilayer);
          ///loop over layers and fill the vector of layers
          for (iL = (*iBL)->childBegin(); iL!= (*iBL)->childEnd(); ++iL) {
            DeFTLayer* layer = dynamic_cast<DeFTLayer*>(*iL);
            if ( layer != 0 ) m_layers.push_back(layer);
	  } // loop layers
	}
      } // loop bilayers
    }
  } // loop stations


  ///>>> print the layer properties <<<///
  typedef std::vector<DeFTLayer*>::const_iterator LIter;
  if ( m_msg->level() <= MSG::DEBUG ) {

    ///loop over layers
    LIter iL;
    for (iL = layers().begin(); iL != layers().end(); ++iL) {
      DeFTLayer* layer = dynamic_cast<DeFTLayer*>(*iL);
      if ( layer != 0 ) {

        *m_msg << MSG::DEBUG << "Properties of FT layer with ID " << layer->layerID() << ":"
	       << "\tGeometrical borders: "
	       << format("xMin/xMax = %7.1f /%7.1f  yMin/yMax = %7.1f /%7.1f  zMin/zMax = %7.1f /%7.1f mm",
			 layer->layerMinX(), layer->layerMaxX(),
			 layer->layerMinY(), layer->layerMaxY(),
			 layer->layerMinZ(), layer->layerMaxZ())
	       << "\tBeam pipe radius: " << format("%7.1f mm", layer->layerInnerHoleRadius())
	       << "\tStereo angle: " << format("%.1f degrees", layer->angle()/Gaudi::Units::degree)
	       << "\tSlope dzDy: " << format("%.5f rad", layer->slopeDzDy())
	       << endmsg;

      } //end if( layer != 0 )
    } //end loop layers
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Finalization
//=============================================================================
StatusCode DeFTDetector::finalize(){
  // destroy the MsgService object
  if ( m_msg ) {
    delete m_msg;
    m_msg = 0;
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
/// Functions to find the Station, BiLayer or Layer
/// for a given XYZ point or FTChannel
//=============================================================================

/// XYZ point -> Station
const DeFTStation* DeFTDetector::findStation(const Gaudi::XYZPoint& aPoint) const {
  if ( !isInside(aPoint) ) { return 0; }
  else {
    Stations::const_iterator iS = std::find_if( m_stations.begin(), m_stations.end(),
                                                bind(&DetectorElement::isInside, _1, aPoint) );
    return (iS != m_stations.end() ? (*iS) : 0);
  }
}

/// XYZ point -> BiLayer
const DeFTBiLayer* DeFTDetector::findBiLayer(const Gaudi::XYZPoint& aPoint) const {
  if ( !isInside(aPoint) ) { return 0; }
  else {
    BiLayers::const_iterator iBL = std::find_if( m_bilayers.begin(), m_bilayers.end(),
                                                 bind(&DetectorElement::isInside, _1, aPoint) );
    return (iBL != m_bilayers.end() ? (*iBL) : 0);
  }
}

/// XYZ point -> Layer
const DeFTLayer* DeFTDetector::findLayer(const Gaudi::XYZPoint& aPoint) const {
  if ( !isInside(aPoint) ) { return 0; }
  else {
    Layers::const_iterator iL = std::find_if( m_layers.begin(), m_layers.end(),
                                              bind(&DetectorElement::isInside, _1, aPoint) );
    return (iL != m_layers.end() ? (*iL) : 0);
  }
}

