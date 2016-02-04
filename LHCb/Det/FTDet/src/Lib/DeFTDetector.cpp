// FTDet
#include "FTDet/DeFTDetector.h"

// Gaudi Kernel
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/IUpdateManagerSvc.h"

#include <boost/lexical_cast.hpp>

/** @file DeFTDetector.cpp
 *
 *  Implementation of class : DeFTDetector
 *
 *  @author Plamen Hopchev
 *  @date   2012-04-25
 *  Mod  SE 2014-10-12
 */

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DeFTDetector::DeFTDetector( const std::string& name ) :
  DetectorElement( name )
{
}

//=============================================================================
// Destructor
//=============================================================================
DeFTDetector::~DeFTDetector() = default;

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
  if ( !m_msg ) m_msg.reset( new MsgStream( msgSvc(), "DeFTDetector" ) );

  *m_msg << MSG::DEBUG << "==> Initialize DeFTDetector" << endmsg;

  //load ftversion
  m_FTversion = ( exists("FTversion") ? this->params()->param<int>("FTversion") : 0 );

  *m_msg << MSG::INFO << "Current FT geometry version =   " <<  m_FTversion<<endmsg;

  /// Fill in the vectors holding pointers to the Daugther DetElements
  // loop over stations

  const IDetectorElement::IDEContainer & detelems = childIDetectorElements();
  updMgrSvc()->registerCondition( this, geometry(), &DeFTDetector::geometryUpdate );

  if( m_FTversion >= 20 ){    //geom 20,4x,5x...
    for (auto iS = detelems.begin(); iS != detelems.end(); ++iS) {
      SmartDataPtr<DeFTStation> station(dataSvc(), (*iS)->name());
      if (station  ) {
        /// fill the vector of stations
        m_stations.push_back(station);
        // register UMS dependency
        updMgrSvc()->registerCondition( this, station->geometry(), &DeFTDetector::geometryUpdate );
        ///loop over layers and fill the vector of layers

        if( ! (station->childIDetectorElements().empty()) ){
          for (auto iL = station->childIDetectorElements().begin() ;
               iL != station->childIDetectorElements().end(); ++iL) {
           SmartDataPtr<DeFTLayer>  layer (dataSvc(),(*iL)->name());

           if ( layer ) {
            m_layers.push_back(layer);
            updMgrSvc()->registerCondition( this, layer->geometry(), &DeFTDetector::geometryUpdate );
            //*m_msg << MSG::INFO <<"registered FT  layer "<< iL - station->childIDetectorElements().begin() <<endmsg;
            ///loop over modules and fill the vector of modules
            if( !(layer ->childIDetectorElements().empty()) ){
            for (auto iM = layer->childIDetectorElements().begin(); iM != layer->childIDetectorElements().end(); ++iM) {
              SmartDataPtr<DeFTModule> module (dataSvc(),(*iM)->name());
              if ( module ) {
                m_modules.push_back(module);
                updMgrSvc()->registerCondition( this,
                                                module->geometry(), &DeFTDetector::geometryUpdate );

                if( ! (module ->childIDetectorElements().empty()) ){
                ///loop over fibremodules and fill the vector of fibremodules
                  for (auto iFM = module ->childIDetectorElements().begin();
                       iFM != module->childIDetectorElements().end() ; ++iFM) {
                    if ( std::string::npos != (*iFM)->name().find(m_FibreModuleNameSpec) ){

                      SmartDataPtr<DeFTFibreModule> fibremodule (dataSvc(),(*iFM)->name());
                      if ( fibremodule) {
                        m_fibremodules.push_back(fibremodule);
                        updMgrSvc()->registerCondition( this,
                                     fibremodule->geometry(), &DeFTDetector::geometryUpdate );
                        //*m_msg << MSG::INFO <<"registered FT fibremodule "
                        //       << iFM - module->childIDetectorElements().begin() <<endmsg;

                        if( ! (fibremodule ->childIDetectorElements().empty()) ){
                           ///loop over fibremats and fill the vector of fibremats
                           for (auto iFMat = fibremodule ->childIDetectorElements().begin() ;
                                iFMat != fibremodule ->childIDetectorElements().end() ; ++iFMat) {
                             if ( std::string::npos != (*iFMat)->name().find(m_FibreMatNameSpec) ){

                               SmartDataPtr<DeFTFibreMat> fibremat (dataSvc(),(*iFMat)->name());
                               if ( fibremat) {
                                 m_fibremats.push_back(fibremat);
                                 updMgrSvc()->registerCondition( this,
                                              fibremat->geometry(), &DeFTDetector::geometryUpdate );

                               }} // tests for  existance of fibremat

                           } // loop fibremat

                           } // test existance of children of fibre module


                      }} //tests for existance of fibre module
                }// loop fibremodules
                }//test existance of chilren of module


              }// test existanace of module
            } //loop modules
            } // test existance of children of layer

          }  //test existance of layer
          } // loop layers
        } // test of existance of children of station

      }// test existance of station
    } // loop stations

  }else if ( m_FTversion < 20 )  {
    // old version of geometry; to become obsolete
    for (auto iS = this->childBegin(); iS != this->childEnd(); ++iS) {
      DeFTStation* station = dynamic_cast<DeFTStation*>(*iS);
      if ( station ) {
        /// fill the vector of stations
        m_stations.push_back(station);
        ///loop over layers and fill the vector of bilayers
        for (auto iBL = (*iS)->childBegin(); iBL!= (*iS)->childEnd(); ++iBL) {
          DeFTBiLayer* bilayer = dynamic_cast<DeFTBiLayer*>(*iBL);
          if ( bilayer ) {
            m_bilayers.push_back(bilayer);
            ///loop over modules and fill the vector of modules
            for (auto iL = (*iBL)->childBegin(); iL!= (*iBL)->childEnd(); ++iL) {
              DeFTLayer* layer = dynamic_cast<DeFTLayer*>(*iL);
              if ( layer ) {
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

    if( m_FTversion >= 20 ){

      ///loop over fibremats
      for (auto iFMat = fibremats().begin(); iFMat != fibremats().end(); ++iFMat) {
        DeFTFibreMat* fibremat = dynamic_cast<DeFTFibreMat*>(*iFMat);
        if ( fibremat ) {

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


    // trigger first UMS update
  const StatusCode update = updMgrSvc()->update(this);

  return ( sc && update) ;
}
//=============================================================================
// geometryUpdate
//=============================================================================

StatusCode DeFTDetector::geometryUpdate ( )
{
  MsgStream msg ( msgSvc(), "DeFTDetector" );
  //get geometry and other user parameters

  return StatusCode::SUCCESS;

}

//=============================================================================
// Finalization
//=============================================================================
StatusCode DeFTDetector::finalize(){
  // destroy the MsgService object
  m_msg.reset();
  return StatusCode::SUCCESS;
}

//=============================================================================
/// Functions to find the Station, BiLayer or Layer
/// for a given XYZ point or FTChannel
//=============================================================================
namespace {
    template <typename C>
    const typename C::value_type find_inside(const C& c, const Gaudi::XYZPoint& p) {
        auto i = std::find_if( std::begin(c), std::end(c),
                               [&](typename C::const_reference s) {
                                   return s->isInside(p);
                               });
        return i != std::end(c) ? *i : nullptr;
    }
}

/// XYZ point -> Station
const DeFTStation* DeFTDetector::findStation(const Gaudi::XYZPoint& point) const {
  return isInside(point) ? find_inside(m_stations, point) : nullptr;
}

/// XYZ point -> BiLayer
const DeFTBiLayer* DeFTDetector::findBiLayer(const Gaudi::XYZPoint& point) const {
  return isInside(point) ? find_inside(m_bilayers, point) : nullptr;
}

/// XYZ point -> Layer
const DeFTLayer* DeFTDetector::findLayer(const Gaudi::XYZPoint& point) const {
  return isInside(point) ? find_inside(m_layers, point) : nullptr;
}

/// XYZ point -> Module
const DeFTModule* DeFTDetector::findModule(const Gaudi::XYZPoint& point) const {
  return isInside(point) ? find_inside(m_modules, point) : nullptr;
}

/// XYZ point -> FibreModule
const DeFTFibreModule* DeFTDetector::findFibreModule(const Gaudi::XYZPoint& point) const {
  return isInside(point) ? find_inside(m_fibremodules, point) : nullptr;
}

/// XYZ point -> FibreMat
const DeFTFibreMat* DeFTDetector::findFibreMat(const Gaudi::XYZPoint& point) const {
  return isInside(point) ? find_inside(m_fibremats, point) : nullptr;
}

//=========================================================================
// Find a FibreMat using an FTChannelID
//=========================================================================
const DeFTFibreMat* DeFTDetector::findFibreMat ( const LHCb::FTChannelID id ) const {
 // convention change: TTLLQQMMM (binary 9 bits, MSB to the left)
 // simplified from hardware format, but same conventions
 // T station (1-3), L layer (0-3), Q quarter (0-3),
 // M module in quarter (1-6, full Left:1->5, 5->1 full right, holes: 6)

  unsigned int fibreMatID;
  if (m_FTversion<=42) {   //geom 2 and 4x
    unsigned int layer    = id.layer();
    unsigned int module = id.module();
    unsigned int mat = id.mat();
    fibreMatID = mat+100*module+10000*layer;
  } else {   //geom 5x: Fibrematid changed, temp hack  //DBL
    unsigned int layerv2  = id.layer();  //numbering v2 style: 0 -> 11
    unsigned int modulev2 = id.module();   //numbering v2 style: 0,1,2,3,4,10,11,5,6,7,8,9 left(x>0) to right
    unsigned int matv2 = id.mat();

    unsigned int station    = layerv2/4 + 1;   //4 layers per station, numbering start at 1...
    unsigned int layer = layerv2%4;

    unsigned int quarter;
    if((modulev2<=4 || modulev2==10) && matv2==0) quarter=3;
    else if((modulev2>=5 || modulev2==11) && matv2==0) quarter=2;
    else if((modulev2<=4 || modulev2==10) && matv2==1) quarter=1;
    else quarter=0;

    unsigned int module;    //numbering v5 style: 1,2,3,4,5,6 (left, L->R) and 6,5,4,3,2,1 (right, L->R)
    if(modulev2==10||modulev2==11) module=6;
    else if(modulev2<=4) module=modulev2+1;
    else module=(9-modulev2)+1;    //modulev2>=5 and != 10 or 11
    std::bitset<2> bT(station);
    std::bitset<2> bL(layer);
    std::bitset<2> bQ(quarter);
    std::bitset<3> bM(module);
    fibreMatID=boost::lexical_cast<unsigned int>(bT.to_string()+bL.to_string()+bQ.to_string()+bM.to_string());
  }

  /// Find the fibreMat using a binary search
  auto itL = std::lower_bound(m_fibremats.begin(),
                              m_fibremats.end(), fibreMatID,
                              [](const DeFTFibreMat* mat, const unsigned int val){
                                return mat->FibreMatID() < val ;
                              });

  /// If the fibreMat was found,return the pointer to it, otherwise return a NULLptr
  return (itL != m_fibremats.end() && (fibreMatID == (*itL)->FibreMatID() )) ? *itL : nullptr;

}
