// $Id: OTExpectedHits.cpp,v 1.1.1.1 2006-07-24 14:56:45 mneedham Exp $
// GaudiKernel
#include "GaudiKernel/ToolFactory.h"

// Tsa
#include "OTExpectedHits.h"
#include "TsaKernel/Parabola.h"
#include "TsaKernel/Line.h"
#include "TsaKernel/Line3D.h"

// CLHEP
#include "Kernel/PhysicalConstants.h"

#include "Kernel/Plane3DTypes.h"


// Geometry
#include "OTDet/DeOTDetector.h"
#include "OTDet/DeOTLayer.h"
#include "OTDet/DeOTQuarter.h"
#include "OTDet/DeOTModule.h"
#include "Kernel/OTChannelID.h"

static const ToolFactory<OTExpectedHits>  s_factory;
const IToolFactory& OTExpectedHitsFactory = s_factory;


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

StatusCode OTExpectedHits::collect(const Tsa::Parabola& parab, 
                                   const Tsa::Line& line,
				   const LHCb::OTChannelID& aChan,
                                   std::vector<IOTExpectedHits::OTPair>& hits) const{
 
  std::vector<LHCb::OTChannelID> channels;
  std::vector<double> distances;

  // pick up layer...
  DeOTLayer* theLayer = m_tracker->findLayer(aChan);
  if (theLayer != 0){

    DeOTModule* firstModule = theLayer->quarters().front()->modules().front();;
    const double halfThickness = 0.5*10.74*mm; // add an epsilon
    Gaudi::XYZPoint entry(0.0,0.0,-halfThickness);
    Gaudi::XYZPoint exit(0.0,0.0,halfThickness);

    const double z = firstModule->z();
    Tsa::Line3D aLine3D = Tsa::Line3D(parab.tangent(z),line,z);
    Gaudi::XYZPoint aPoint;     
    if (aLine3D.intersection(theLayer->plane(),aPoint) == true){

      if (theLayer->geometry()->isInside(aPoint)){

        typedef std::vector<DeOTModule*> Modules;
        typedef std::vector<DeOTQuarter*> Quarters;

        const Quarters& qVector = theLayer->quarters();
        for (Quarters::const_iterator iterQ = qVector.begin(); iterQ != qVector.end(); ++iterQ){
          if ((*iterQ)->geometry()->isInside(aPoint) == true){
             const Modules& modVector = (*iterQ)->modules();
             for (Modules::const_iterator iterM = modVector.begin(); iterM != modVector.end(); ++iterM){
               DeOTModule* aModule = *iterM;
               if (insideModule(aModule,aLine3D) == true){

		 Gaudi::XYZPoint globalEntry = intersection(aLine3D,aModule,entry);
                 Gaudi::XYZPoint globalExit = intersection(aLine3D,aModule,exit);
                 aModule->calculateHits(globalEntry,globalExit,channels,distances);

	       }  // in module
	     }  // iterM
	  } // inside quarter
	} //iterQ
      } // inside layer
    } // intersection
  } // layer
		                   
 
  // copy to output vector
  for (unsigned int i = 0; i < channels.size(); ++i){
    hits.push_back(std::make_pair(channels[i],distances[i]));
  }

  return StatusCode::SUCCESS;
}


bool OTExpectedHits::insideModule(const DeOTModule* module,
                                  const Tsa::Line3D& line) const{

  bool isIn = false;
  Gaudi::XYZPoint point;
  if (line.intersection(module->plane(),point) == true){
    isIn = module->isInside(point);
  }
  return isIn;
}

Gaudi::XYZPoint OTExpectedHits::intersection(const Tsa::Line3D& line, const DeOTModule* module,
                                             const Gaudi::XYZPoint& aPoint) const{

  // make a plane
  Gaudi::Plane3D aPlane(module->plane().Normal(),module->toGlobal(aPoint));
  Gaudi::XYZPoint inter;
  line.intersection(aPlane,inter);
  return inter;
}
