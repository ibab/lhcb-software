// $Id: ITExpectedHits.cpp,v 1.3 2006-08-17 08:36:07 mneedham Exp $

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


#include <algorithm>

DECLARE_TOOL_FACTORY( ITExpectedHits );

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
                                   std::vector<IITExpectedHits::ITPair>& hits,
                                   const unsigned int iSector) const{

  // convert the sector to a sector of boxes
  std::vector<unsigned int> boxes;
  if (iSector == 2){
    boxes.push_back(1); boxes.push_back(2); 
  }
  else if (iSector == 3) {
    boxes.push_back(3);
  }
  else {
    boxes.push_back(4);
  }

  unsigned int iCount = 0;

  for (std::vector<unsigned int>::iterator iter = boxes.begin(); iter != boxes.end(); ++iter ){
    LHCb::STChannelID layerID = LHCb::STChannelID(aChan.type(),aChan.station(),aChan.layer(),*iter, 0u, 0u);
    
    const DeSTDetector::Layers layers = m_tracker->layers();
    const DeSTLayer* layer = 0;
    for (DeSTDetector::Layers::const_iterator iterLayer = layers.begin();
         iterLayer != layers.end() && layer == 0; ++iterLayer){

      LHCb::STChannelID elemChan = (*iterLayer)->elementID();

      if (elemChan.station() == layerID.station()
          && elemChan.layer() == layerID.layer()
          && elemChan.detRegion() == layerID.detRegion() ) {
        layer = *iterLayer;

      }
    } // layers

    if (layer == 0){

       const DeSTLayer::Sectors tSectors = layer->sectors();
       double halfThickness = 0.205;
       if (iSector != 2) halfThickness = 0.16;
  
       Gaudi::XYZPoint entry(0.0,0.0,-halfThickness);
       Gaudi::XYZPoint exit(0.0,0.0,halfThickness);

       for ( DeSTLayer::Sectors::const_iterator iterS = tSectors.begin(); iterS != tSectors.end() ; ++iterS){

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

            // might have to swap...
            if (firstStrip > lastStrip) std::swap(firstStrip, lastStrip);
         
	    // allow for capacitive coupling....
            if (aSector->isStrip(firstStrip-1) == true) --firstStrip;
            if (aSector->isStrip(lastStrip+1) == true) ++lastStrip;
                         
	    LHCb::STChannelID elemChan = (*iterS)->elementID();

            for (unsigned int iStrip = firstStrip; iStrip <= lastStrip; ++iStrip){

               LHCb::STChannelID aChan = LHCb::STChannelID(elemChan.type(),
                                                           elemChan.station(), 
                                                           elemChan.layer(),
		 	                                   elemChan.detRegion(), 
                                                           elemChan.sector(), iStrip);

	       hits.push_back(std::make_pair(aChan,double(iCount)));           

	    }  // iStrip
            ++iCount;
	 } // if
      } // sectors
    } // if layer
  } // boxes
 
     
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
