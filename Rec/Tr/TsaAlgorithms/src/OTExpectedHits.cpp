// $Id: OTExpectedHits.cpp,v 1.5 2006-09-05 15:48:51 mneedham Exp $
// GaudiKernel
#include "GaudiKernel/ToolFactory.h"

// Tsa
#include "OTExpectedHits.h"
#include "TsaKernel/Parabola.h"
#include "TsaKernel/Line.h"
#include "TsaKernel/Line3D.h"

// Kernel
#include "GaudiKernel/SystemOfUnits.h"
#include "Kernel/Plane3DTypes.h"
#include "Kernel/OTChannelID.h"
#include "LHCbMath/GeomFun.h"


// Geometry
#include "OTDet/DeOTDetector.h"
#include "OTDet/DeOTLayer.h"
#include "OTDet/DeOTQuarter.h"
#include "OTDet/DeOTModule.h"

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

  // OT module
  //  DeOTModule* firstModule = m_tracker->modules().front();;
  //  const double halfThickness = 0.5*firstModule->sensThickness(); // add an epsilon
  //  m_entry = Gaudi::XYZPoint(0.0,0.0,-halfThickness);
  // m_exit = Gaudi::XYZPoint(0.0,0.0,halfThickness);
 

  return StatusCode::SUCCESS;
}

StatusCode OTExpectedHits::collect(const Tsa::Parabola& parab, 
                                   const Tsa::Line& line,
				   const LHCb::OTChannelID& aChan,
                                   std::vector<IOTExpectedHits::OTPair>& hits,
                                   const unsigned int iSector) const{
 
  std::vector<LHCb::OTChannelID> channels;
  std::vector<double> distances;

  // pick up layer...
  DeOTLayer* theLayer = m_tracker->findLayer(aChan);
  if (theLayer != 0){

    DeOTModule* firstModule = theLayer->quarters().front()->modules().front();;

    Tsa::Line tanLine = parab.tangent(firstModule->z());
    Tsa::Line3D aLine3D = Tsa::createLine3D(tanLine,line,firstModule->z());
    Gaudi::XYZPoint aPoint; 
    double mu = 0;
    if (Gaudi::Math::intersection(aLine3D,theLayer->plane(),aPoint, mu) == true){

      if (theLayer->geometry()->isInside(aPoint)){

        typedef std::vector<DeOTModule*> Modules;
        typedef std::vector<DeOTQuarter*> Quarters;
        
        bool found = false;
        const Quarters& qVector = theLayer->quarters();
        for (Quarters::const_iterator iterQ = qVector.begin(); iterQ != qVector.end() && !found; ++iterQ){

	    if ((correctSector((*iterQ)->elementID().quarter(), iSector) == true)
               && ((*iterQ)->geometry()->isInside(aPoint) == true)){
             const Modules& modVector = (*iterQ)->modules();
             for (Modules::const_iterator iterM = modVector.begin(); iterM != modVector.end() && !found; ++iterM){
               const DeOTModule* aModule = *iterM;
               found = insideModule(aModule,aLine3D);
               if (found == true){
		 // Gaudi::XYZPoint globalEntry = intersection(aLine3D,aModule,m_entry);
		 // Gaudi::XYZPoint globalExit = intersection(aLine3D,aModule,m_exit);
                 Gaudi::XYZPoint globalEntry = intersection(aLine3D,aModule->entryPlane());
		 Gaudi::XYZPoint globalExit = intersection(aLine3D,aModule->exitPlane());
                 aModule->calculateHits(globalEntry,globalExit,channels,distances);

	       }  // in module
	     }  // iterM
	   } // inside quarter
	} //iterQ
      } // inside layer
    } // intersection
  } // layer
		                   
 
  // copy to output vector
  hits.reserve(channels.size());
  for (unsigned int i = 0; i < channels.size(); ++i){
    hits.push_back(std::make_pair(channels[i],distances[i]));
  }

  return StatusCode::SUCCESS;
}


bool OTExpectedHits::insideModule(const DeOTModule* module,
                                  const Tsa::Line3D& line) const{

  bool isIn = false;
  Gaudi::XYZPoint point;
  double mu;
  if (Gaudi::Math::intersection(line, module->plane() ,point, mu) == true){
    isIn = module->isInside(point);
  }
  return isIn;
}

Gaudi::XYZPoint OTExpectedHits::intersection(const Tsa::Line3D& line, const DeOTModule* module,
                                             const Gaudi::XYZPoint& aPoint) const{

  // make a plane
  Gaudi::Plane3D aPlane(module->plane().Normal(),module->toGlobal(aPoint));
  Gaudi::XYZPoint inter;
  double mu = 0;
  Gaudi::Math::intersection(line,aPlane,inter,mu);
  return inter;
}


Gaudi::XYZPoint OTExpectedHits::intersection(const Tsa::Line3D& line,
                                             const Gaudi::Plane3D& aPlane) const{

  // make a plane
  Gaudi::XYZPoint inter;
  double mu = 0;
  Gaudi::Math::intersection(line,aPlane,inter,mu);
  return inter;
}


