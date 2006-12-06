// $Id: ITExpectedHits.cpp,v 1.7 2006-12-06 14:35:00 mneedham Exp $

// GaudiKernel
#include "GaudiKernel/ToolFactory.h"

// Tsa
#include "ITExpectedHits.h"
#include "TsaKernel/Parabola.h"
#include "TsaKernel/Line.h"
#include "TsaKernel/Line3D.h"
#include "TsaKernel/TsaConstants.h"

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

  hits.reserve(8);
  
  unsigned int ibox = 0;
  if (iSector == 0){
    const double xTest = parab.value(TsaConstants::beginTracker);
    xTest < 0 ? ibox = 1 : ibox = 2; 
  }
  else if (iSector == 1) {
    ibox = 3;
  }
  else {
    ibox = 4;
  }


  unsigned int iCount = 0;

  LHCb::STChannelID layerID = LHCb::STChannelID(aChan.type(),aChan.station(),aChan.layer(),ibox, 0u, 0u);

  const DeSTLayer* layer = m_tracker->findLayer(layerID);

  if (layer != 0){

    Tsa::Line tanLine = parab.tangent(layer->globalCentre().z());

    Tsa::Line3D aLine3D = Tsa::createLine3D(tanLine,
                                            line,layer->globalCentre().z());

    const DeSTLayer::Sectors& tSectors = layer->sectors();

    for ( DeSTLayer::Sectors::const_iterator iterS = tSectors.begin(); iterS != tSectors.end() ; ++iterS){

      DeSTSector* aSector = *iterS;

      if (insideSector(aSector,aLine3D) == true){

        Gaudi::XYZPoint globalEntry = intersection(aLine3D,aSector->entryPlane());
	Gaudi::XYZPoint globalExit = intersection(aLine3D,aSector->exitPlane());

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
  } 
  else {
    warning() << "failed to find layer " << endmsg; 
  }

  return StatusCode::SUCCESS;

};
   
bool ITExpectedHits::insideLayer(const DeSTLayer* layer,
                                 const Tsa::Line3D& line) const{

  bool isIn = false;
  Gaudi::XYZPoint point;
  double mu;
  if (Gaudi::Math::intersection(line, layer->plane() ,point, mu) == true){
    isIn = layer->isInside(point);
  }
  return isIn;
}

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

Gaudi::XYZPoint ITExpectedHits::intersection(const Tsa::Line3D& line,
                                             const Gaudi::Plane3D& aPlane) const{

  // make a plane
  Gaudi::XYZPoint inter;
  double mu = 0;
  Gaudi::Math::intersection(line,aPlane,inter,mu);
  return inter;
}
