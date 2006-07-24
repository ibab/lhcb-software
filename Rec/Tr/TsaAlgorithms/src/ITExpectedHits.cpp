// $Id: ITExpectedHits.cpp,v 1.1.1.1 2006-07-24 14:56:45 mneedham Exp $
// GaudiKernel
#include "GaudiKernel/ToolFactory.h"

// Tsa
#include "ITExpectedHits.h"
#include "TsaKernel/Parabola.h"
#include "TsaKernel/Line.h"
#include "TsaKernel/Line3D.h"

// Kernel
#include "Kernel/PhysicalConstants.h"
#include "Kernel/Plane3DTypes.h"
#include "Kernel/STChannelID.h"
#include "LHCbMath/GeomFun.h"


// Geometry
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTLayer.h"
#include "STDet/DeSTSector.h"


static const ToolFactory<ITExpectedHits>  s_factory;
const IToolFactory& ITExpectedHitsFactory = s_factory;


ITExpectedHits::ITExpectedHits(const std::string& type,
                     const std::string& name,
                     const IInterface* parent):
  GaudiTool(type, name, parent){

  // constructer 
  declareInterface<IITExpectedHits>(this);
};

ITExpectedHits::~ITExpectedHits(){
  // destructer
}

StatusCode ITExpectedHits::initialize(){

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
     return Error("Failed to initialize",sc);
  }

  // get geometry 
  m_tracker = getDet<DeSTDetector>(DeSTDetLocation::location("IT"));

  return StatusCode::SUCCESS;
}

StatusCode ITExpectedHits::collect(const Tsa::Parabola& parab, 
                                   const Tsa::Line& line,
				   const LHCb::STChannelID& aChan,
                                   std::vector<IITExpectedHits::ITPair>& hits) const{
 
  double halfThickness = 0.2;

  const DeSTDetector::Layers& tLayers = m_tracker->layers();
  for ( DeSTDetector::Layers::const_iterator iterLayer = tLayers.begin(); 
        iterLayer != tLayers.end() ; ++iterLayer){

    if ((*iterLayer)->elementID().station() == aChan.station()&&
       ((*iterLayer)->elementID().layer() == aChan.layer())){

       Gaudi::XYZPoint entry(0.0,0.0,-halfThickness);
       Gaudi::XYZPoint exit(0.0,0.0,halfThickness);

       const DeSTLayer::Sectors tSectors = (*iterLayer)->sectors();
       for ( DeSTLayer::Sectors::const_iterator iterS = tSectors.begin(); 
             iterS != tSectors.end() ; ++iterS){

          DeSTSector* aSector = *iterS;
	  Tsa::Line tanLine = parab.tangent(aSector->globalCentre().z());
          Tsa::Line3D aLine3D = Tsa::createLine3D(tanLine,
                                                  line,aSector->globalCentre().z());
          if (insideSector(aSector,aLine3D) == true){

	    Gaudi::XYZPoint globalEntry = intersection(aLine3D,aSector,entry);
            Gaudi::XYZPoint globalExit = intersection(aLine3D,aSector,exit);
            Gaudi::XYZPoint localEntry = aSector->toLocal(globalEntry);
            Gaudi::XYZPoint localExit = aSector->toLocal(globalExit);
            
            unsigned int firstStrip = aSector->localUToStrip(localEntry.x());
            unsigned int lastStrip =  aSector->localUToStrip(localExit.x());
         
                         
	    LHCb::STChannelID elemChan = (*iterS)->elementID();

            for (unsigned int iStrip = firstStrip; iStrip < lastStrip; ++iStrip){

               LHCb::STChannelID aChan = LHCb::STChannelID(elemChan.type(),
                                                           elemChan.station(), 
                                                           elemChan.layer(),
		 	                                   elemChan.detRegion(), 
                                                           elemChan.sector(), iStrip);

	       hits.push_back(std::make_pair(aChan,0.0));           

	    }  // iStrip
 
	  } // if
       } // sectors
    } // if
  } // layers

  return StatusCode::SUCCESS;

};
   


bool ITExpectedHits::insideSector(const DeSTSector* sector,
                                  const Tsa::Line3D& line) const{

  bool isIn = false;
  Gaudi::XYZPoint point;
  double mu;
  if (Gaudi::Math::intersection(line, sector->plane() ,point, mu) == true){
    isIn = sector->globalInActive(point);
  }
  return isIn;
}

Gaudi::XYZPoint ITExpectedHits::intersection(const Tsa::Line3D& line, const DeSTSector* sector,
                                             const Gaudi::XYZPoint& aPoint) const{

  // make a plane
  Gaudi::Plane3D aPlane(sector->plane().Normal(),sector->toGlobal(aPoint));
  Gaudi::XYZPoint inter;
  double mu = 0;
  Gaudi::Math::intersection(line,aPlane,inter,mu);
  return inter;
}
