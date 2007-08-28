// $Id: OTExpectedHits.cpp,v 1.2 2007-08-28 10:46:35 jonrob Exp $

// GaudiKernel
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/Plane3DTypes.h"

// Tsa
#include "OTExpectedHits.h"

using namespace Tf::Tsa;

DECLARE_TOOL_FACTORY( OTExpectedHits );

OTExpectedHits::OTExpectedHits(const std::string& type,
                               const std::string& name,
                               const IInterface* parent):
  GaudiTool(type, name, parent){

  // constructer
  declareInterface<IOTExpectedHits>(this);
};

OTExpectedHits::~OTExpectedHits(){
  // destructer
}

StatusCode OTExpectedHits::initialize(){

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize",sc);
  }

  // get geometry
  m_tracker = getDet<DeOTDetector>( DeOTDetectorLocation::Default);

  return StatusCode::SUCCESS;
}

StatusCode OTExpectedHits::collect(const Parabola& parab,
                                   const Line& line,
                                   const LHCb::OTChannelID& aChan,
                                   std::vector<IOTExpectedHits::OTPair>& hits,
                                   const unsigned int iSector) const{

  std::vector<LHCb::OTChannelID> channels; channels.reserve(4);
  std::vector<double> distances; distances.reserve(4);


  DeOTModule* aModule = 0;
  if (aChan.module()  == 0){
    aModule = findModule(parab,line,aChan, iSector);
  }
  else {
    aModule = m_tracker->findModule(aChan);
  }

  if (aModule != 0){
    Line tanLine = parab.tangent(aModule->z());
    Line3D aLine3D = createLine3D(tanLine,line,aModule->z());
    Gaudi::XYZPoint globalEntry = intersection(aLine3D,aModule->entryPlane());
    Gaudi::XYZPoint globalExit = intersection(aLine3D,aModule->exitPlane());
    aModule->calculateHits(globalEntry,globalExit,hits);
  }

  return StatusCode::SUCCESS;
}


bool OTExpectedHits::insideModule(const DeOTModule* module,
                                  const Line3D& line) const{

  bool isIn = false;
  Gaudi::XYZPoint point;
  double mu;
  if (Gaudi::Math::intersection(line, module->plane() ,point, mu) == true){
    isIn = module->isInside(point);
  }
  return isIn;
}

Gaudi::XYZPoint OTExpectedHits::intersection(const Line3D& line, const DeOTModule* module,
                                             const Gaudi::XYZPoint& aPoint) const{

  // make a plane
  Gaudi::Plane3D aPlane(module->plane().Normal(),module->toGlobal(aPoint));
  Gaudi::XYZPoint inter;
  double mu = 0;
  Gaudi::Math::intersection(line,aPlane,inter,mu);
  return inter;
}


Gaudi::XYZPoint OTExpectedHits::intersection(const Line3D& line,
                                             const Gaudi::Plane3D& aPlane) const{

  // make a plane
  Gaudi::XYZPoint inter;
  double mu = 0;
  Gaudi::Math::intersection(line,aPlane,inter,mu);
  return inter;
}

DeOTModule* OTExpectedHits::findModule(const Parabola& parab, const Line& line,
                                       const LHCb::OTChannelID& aChan, const unsigned int iSector) const{

  // pick up layer...
  DeOTModule* aModule = 0;
  DeOTLayer* theLayer = m_tracker->findLayer(aChan);
  if (theLayer != 0){

    DeOTModule* firstModule = theLayer->quarters().front()->modules().front();;

    Line tanLine = parab.tangent(firstModule->z());
    Line3D aLine3D = createLine3D(tanLine,line,firstModule->z());
    Gaudi::XYZPoint aPoint;
    double mu = 0;
    if (Gaudi::Math::intersection(aLine3D,theLayer->plane(),aPoint, mu) == true){

      typedef std::vector<DeOTModule*> Modules;
      typedef std::vector<DeOTQuarter*> Quarters;

      bool found = false;
      const Quarters& qVector = theLayer->quarters();
      for (Quarters::const_iterator iterQ = qVector.begin(); iterQ != qVector.end() && !found; ++iterQ){
        if ((correctSector((*iterQ)->elementID().quarter(), iSector) == true)){
          const Modules& modVector = (*iterQ)->modules();
          for (Modules::const_iterator iterM = modVector.begin(); iterM != modVector.end() && !found; ++iterM){
            found = insideModule(*iterM,aLine3D);
            if (found == true){
              aModule = *iterM;
            }  // in module
          }  // iterM
        } // inside quarter
      } //iterQ
      //      } // inside layer
    } // intersection
  } // layer

  return aModule;
}

