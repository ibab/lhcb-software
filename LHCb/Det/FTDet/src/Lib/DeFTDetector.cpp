// $Id: $

// FTDet
#include "FTDet/DeFTDetector.h"

// Gaudi Kernel
#include "GaudiKernel/SystemOfUnits.h"


/** @file DeFTDetector.cpp
 *
 *  Implementation of class : DeFTDetector
 *
 *  @author Plamen Hopchev
 *  @date   2012-04-25
 */

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DeFTDetector::DeFTDetector( const std::string& name ) :
  DetectorElement( name ),
  m_FTversion(0),
  m_stations(),
  m_bilayers(),
  m_layers(),
  m_modules(),
  m_fibremodules(),
  m_fibremats(),
  m_msg(NULL)
{

}

//=============================================================================
// Destructor
//=============================================================================
DeFTDetector::~DeFTDetector(){
  if ( m_msg ) {
    delete m_msg;
    m_msg = 0;
  }
} 

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

  //load ftversion
  if( exists("FTversion") ) m_FTversion = this->params()->param<int>("FTversion")  ;

  /// Fill in the vectors holding pointers to the Daugther DetElements
  // loop over stations
  typedef IDetectorElement::IDEContainer::const_iterator DEIter;
  DEIter iS, iBL, iL, iM, iFM, iFMat;

  if( m_FTversion == 20 ){
    for (iS = this->childBegin(); iS != this->childEnd(); ++iS) {
      DeFTStation* station = dynamic_cast<DeFTStation*>(*iS);
      if ( station != 0 ) {
        /// fill the vector of stations
        m_stations.push_back(station);
        ///loop over layers and fill the vector of layers
        for (iL = (*iS)->childBegin(); iL!= (*iS)->childEnd(); ++iL) {
          DeFTLayer* layer = dynamic_cast<DeFTLayer*>(*iL);
          if ( layer != 0 ) {
            m_layers.push_back(layer);
            ///loop over modules and fill the vector of modules
            for (iM = (*iL)->childBegin(); iM!= (*iL)->childEnd(); ++iM) {
              DeFTModule* module = dynamic_cast<DeFTModule*>(*iM);
              if ( module != 0 ) {
                m_modules.push_back(module);
                ///loop over fibremodules and fill the vector of fibremodules
                for (iFM = (*iM)->childBegin(); iFM!= (*iM)->childEnd(); ++iFM) {
                  DeFTFibreModule* fibremodule = dynamic_cast<DeFTFibreModule*>(*iFM);
                  if ( fibremodule != 0 ) {
                    m_fibremodules.push_back(fibremodule);
                    ///loop over fibremats and fill the vector of fibremats
                    for (iFMat = (*iFM)->childBegin(); iFMat!= (*iFM)->childEnd(); ++iFMat) {
                      DeFTFibreMat* fibremat = dynamic_cast<DeFTFibreMat*>(*iFMat);
                      if ( fibremat != 0 ) m_fibremats.push_back(fibremat);
                    } // loop fibremat
                  }
                }// loop fibremodules
              }
            } //loop modules
          }
        } // loop layers
      }
    } // loop stations
  }else{
    for (iS = this->childBegin(); iS != this->childEnd(); ++iS) {
      DeFTStation* station = dynamic_cast<DeFTStation*>(*iS);
      if ( station != 0 ) {
        /// fill the vector of stations
        m_stations.push_back(station);
        ///loop over layers and fill the vector of bilayers
        for (iBL = (*iS)->childBegin(); iBL!= (*iS)->childEnd(); ++iBL) {
          DeFTBiLayer* bilayer = dynamic_cast<DeFTBiLayer*>(*iBL);
          if ( bilayer != 0 ) {
            m_bilayers.push_back(bilayer);
            ///loop over modules and fill the vector of modules
            for (iL = (*iBL)->childBegin(); iL!= (*iBL)->childEnd(); ++iL) {
              DeFTLayer* layer = dynamic_cast<DeFTLayer*>(*iL);
              if ( layer != 0 ) {
                m_layers.push_back(layer);
              }
            } // loop layers
          }
        }//loop bilayers
      }
    } // loop stations 
  }
  
  ///>>> print the layer properties <<<///
  if ( m_msg->level() <= MSG::DEBUG ) {
    
    if( m_FTversion == 20 ){
      
      ///loop over fibremats
      FibreMats::const_iterator iFMat;
      for ( iFMat = fibremats().begin(); iFMat != fibremats().end(); ++iFMat) {
        DeFTFibreMat* fibremat = dynamic_cast<DeFTFibreMat*>(*iFMat);
        if ( fibremat != 0 ) {
          
          *m_msg << MSG::DEBUG << "Properties of FT fibreMat with ID " << fibremat->FibreMatID() << ":" << endmsg;
          *m_msg << MSG::DEBUG << "\tIn layer  " << fibremat->layer()  <<  endmsg;
          *m_msg << MSG::DEBUG << "\tIn module " << fibremat->module() <<  endmsg;
          *m_msg << MSG::DEBUG << "\tIs bottom " << fibremat->isBottom() <<  endmsg;
          *m_msg << MSG::DEBUG<< "\tGeometrical borders of fibremat: " << endmsg;
          *m_msg << MSG::DEBUG<< format("\txMin/xMax = %.1f / %.1f      yMin/yMax = %.1f / %.1f      zMin/zMax = %.1f / %.1f mm",
                                        fibremat->fibreMatMinX(), fibremat->fibreMatMaxX(),
                                        fibremat->fibreMatMinY(), fibremat->fibreMatMaxY(),
                                        fibremat->fibreMatMinZ(), fibremat->fibreMatMaxZ())<< endmsg;
          *m_msg << MSG::DEBUG<< "\tGeometrical borders of corresponding layer: " << endmsg;
          *m_msg << MSG::DEBUG<< format("\txMin/xMax = %.1f / %.1f      yMin/yMax = %.1f / %.1f      zMin/zMax = %.1f / %.1f mm",
                                        fibremat->layerMinX(), fibremat->layerMaxX(),
                                        fibremat->layerMinY(), fibremat->layerMaxY(),
                                        fibremat->layerMinZ(), fibremat->layerMaxZ())<< endmsg;
          *m_msg << MSG::DEBUG<< "\tBeam pipe radius: " << format("%7.1f mm", fibremat->layerInnerHoleRadius())<< endmsg;
          *m_msg << MSG::DEBUG<< "\tStereo angle: " << format("%.1f degrees", fibremat->angle()/Gaudi::Units::degree)<< endmsg;
          *m_msg << MSG::DEBUG<< "\tSlope dzDy: " << format("%.5f rad", fibremat->slopeDzDy())<< endmsg;
          *m_msg << MSG::DEBUG<< endmsg;
          
        }
      } 
    } 
  }

  /// sort the fibremats according to their fibreMatID
  std::sort(m_fibremats.begin(), m_fibremats.end(), 
            [](const DeFTFibreMat* lhs, const DeFTFibreMat* rhs){ return lhs->FibreMatID() < rhs->FibreMatID(); });
  
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
const DeFTStation* DeFTDetector::findStation(const Gaudi::XYZPoint& point) const {
  if ( !isInside(point) ) { return 0; }
  else {
    Stations::const_iterator iS;
    for (iS = m_stations.begin(); iS != m_stations.end(); ++iS) {
      if ( (*iS)->isInside(point) ) break;
    }
    return (iS != m_stations.end() ? (*iS) : 0);
  }
}


/// XYZ point -> BiLayer
const DeFTBiLayer* DeFTDetector::findBiLayer(const Gaudi::XYZPoint& point) const {
  if ( !isInside(point) ) { return 0; }
  else {
    BiLayers::const_iterator iL;
    for (iL = m_bilayers.begin(); iL != m_bilayers.end(); ++iL) {
      if ( (*iL)->isInside(point) ) break;
    }
    return (iL != m_bilayers.end() ? (*iL) : 0);
  }
}

/// XYZ point -> Layer
const DeFTLayer* DeFTDetector::findLayer(const Gaudi::XYZPoint& point) const {
  if ( !isInside(point) ) { return 0; }
  else {
    Layers::const_iterator iL;
    for (iL = m_layers.begin(); iL != m_layers.end(); ++iL) {
      if ( (*iL)->isInside(point) ) break;
    }
    return (iL != m_layers.end() ? (*iL) : 0);
  }
}

/// XYZ point -> Module
const DeFTModule* DeFTDetector::findModule(const Gaudi::XYZPoint& point) const {
  if ( !isInside(point) ) { return 0; }
  else {
    Modules::const_iterator iM;
    for (iM = m_modules.begin(); iM != m_modules.end(); ++iM) {
      if ( (*iM)->isInside(point) ) break;
    }
    return (iM != m_modules.end() ? (*iM) : 0);
  }
}

/// XYZ point -> FibreModule
const DeFTFibreModule* DeFTDetector::findFibreModule(const Gaudi::XYZPoint& point) const {
  if ( !isInside(point) ) { return 0; }
  else {
    FibreModules::const_iterator iFM;
    for (iFM = m_fibremodules.begin(); iFM != m_fibremodules.end(); ++iFM) {
      if ( (*iFM)->isInside(point) ) break;
    }
    return (iFM != m_fibremodules.end() ? (*iFM) : 0);
  }
}

/// XYZ point -> FibreMat
const DeFTFibreMat* DeFTDetector::findFibreMat(const Gaudi::XYZPoint& point) const {
  if ( !isInside(point) ) { return 0; }
  else {
    FibreMats::const_iterator iFMat;
    for (iFMat = m_fibremats.begin(); iFMat != m_fibremats.end(); ++iFMat) {
      if ( (*iFMat)->isInside(point) ) break;
    }
    return (iFMat != m_fibremats.end() ? (*iFMat) : 0);
  }
}




//=========================================================================
// Find a FibreMat using an FTChannelID
//=========================================================================
const DeFTFibreMat* DeFTDetector::findFibreMat ( const LHCb::FTChannelID id ) const {

  unsigned int layer    = id.layer();
  unsigned int module = id.module();
  unsigned int mat = id.mat();
  
  unsigned int fibreMatID = mat+100*module+10000*layer;

  /// Find the fibreMat using a binary search
  FibreMats::const_iterator itL = std::lower_bound(m_fibremats.begin(),
                                                   m_fibremats.end(), fibreMatID,
                                                   [](const DeFTFibreMat* mat, const unsigned int val){
                                                     return mat->FibreMatID() < val ;
                                                   });
  
  /// If the fibreMat was found,return the pointer to it, otherwise return a NULLptr
  if (itL != m_fibremats.end() && (fibreMatID == (*itL)->FibreMatID() )){
    return *itL;
  }else{
    return nullptr;
  }
  
  //for ( FibreMats:: const_iterator iL = m_fibremats.begin(); iL != m_fibremats.end(); ++iL) {
  //  if ( fibreMatID == (*iL)->FibreMatID() ) return *iL;
  //}
  //return NULL;

}
