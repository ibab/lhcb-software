// $Id: TsaFollow.cpp,v 1.2 2006-08-01 09:10:38 cattanem Exp $
// GaudiKernel
#include "GaudiKernel/ToolFactory.h"

// Tsa
#include "TsaKernel/IITDataSvc.h"
#include "TsaFollow.h"

#include "TsaKernel/Parabola.h"
#include "TsaKernel/Line.h"
#include "TsaKernel/Line3D.h"
#include "Kernel/Trajectory.h"

// CLHEP
#include "GaudiKernel/SystemOfUnits.h"
#include "LHCbMath/GeomFun.h"


// Geometry
#include "STDet/DeITDetector.h"
#include "STDet/DeITStation.h"
#include "STDet/DeITBox.h"
#include "STDet/DeITLayer.h"

DECLARE_TOOL_FACTORY( TsaFollow );


TsaFollow::TsaFollow(const std::string& type,
                     const std::string& name,
                     const IInterface* parent):
  GaudiTool(type, name, parent){

  // constructer
  declareProperty("itDataSvc", m_dataSvcName = "ITDataSvc");
  declareProperty("vTol",   m_vTol   = 0.5 *Gaudi::Units::mm );
  declareProperty("deltaU", m_deltaU = 0.90*Gaudi::Units::mm );
 
  declareInterface<ITsaFollow>(this);
};

TsaFollow::~TsaFollow(){
  // destructer
}

StatusCode TsaFollow::initialize(){

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
     return Error("Failed to initialize",sc);
  }

  // data service tool !
  m_dataSvc = tool<IITDataSvc>(m_dataSvcName);

  // get geometry
  DeSTDetector* tracker = getDet<DeITDetector>(DeSTDetLocation::location("IT"));

  const DeSTDetector::Stations& tStations = tracker->stations();
  for ( DeSTDetector::Stations::const_iterator iterS = tStations.begin();
       iterS != tStations.end(); ++iterS){
    const DeITStation* itStation = dynamic_cast<const DeITStation*>(*iterS); 
    if (itStation != 0) m_stations[itStation->elementID().station()] = itStation;
  }

  return StatusCode::SUCCESS;
}

StatusCode TsaFollow::execute(const unsigned int stationNum, const Tsa::Parabola& parab, 
                              const Tsa::Line& line, 
                              std::vector<Tsa::Cluster*>& clusVector){


  // find station
  const  DeITStation* aStation = m_stations[stationNum];
  const DeITStation::Children& boxVector = aStation->boxes();
  DeITStation::Children::const_iterator iterBox = boxVector.begin();
  for (; iterBox != boxVector.end(); ++iterBox){
    const DeITBox::Children& layerVector = (*iterBox)->layers();
    DeITBox::Children::const_iterator iterLayer = layerVector.begin();
    for (; iterLayer != layerVector.end(); ++iterLayer){
      searchLayer(*iterLayer,parab,line,clusVector);
    }   // iterLayer
  }  // iterBox

  return StatusCode::SUCCESS;
}

void TsaFollow::searchLayer(const DeSTLayer* layer,
                            const Tsa::Parabola& parab, 
                            const Tsa::Line& line, 
                            std::vector<Tsa::Cluster*>& clusVector){

  const double z = layer->globalCentre().z();
  Tsa::Line tanLine = parab.tangent(z); 
  Tsa::Line3D firstLine = Tsa::createLine3D(tanLine,line,z);
  if (insideLayer(layer,firstLine) == true){

    // find the data for this layer...
    LHCb::STChannelID elemID = layer->elementID();
    Tsa::STRange hits = m_dataSvc->partition(elemID.station(), elemID.layer(), elemID.detRegion()); 
    double minDist = 9999.0*Gaudi::Units::mm;
    Tsa::Cluster* bestClus = 0;

    for (Tsa::STRange::iterator iterHit = hits.begin(); iterHit != hits.end(); ++iterHit){

      Tsa::Line3D secondLine = Tsa::Line3D((*iterHit)->trajectory().position(0.),
	                                  (*iterHit)->trajectory().direction(0.));
      double poca = Gaudi::Math::distance(firstLine,secondLine);
      if (poca < minDist){
        minDist = poca;
        bestClus = *iterHit;
      }  
    } // iterHit

    if ((bestClus !=0) &&(minDist < m_deltaU)) clusVector.push_back(bestClus);
  }
}

bool TsaFollow::insideLayer(const DeSTLayer* layer,  
                            const Tsa::Line3D& line) const{
  
  bool isIn = false;
  Gaudi::XYZPoint point;
  double mu;
  if (Gaudi::Math::intersection(line,layer->plane(),point,mu) == true){
    isIn = layer->isInside(point);
  } 
  return isIn;
}



