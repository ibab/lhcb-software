// $Id: ITFaultsCalculator.cpp,v 1.2 2006-08-01 09:10:37 cattanem Exp $
// GaudiKernel
#include "GaudiKernel/ToolFactory.h"

#include "GaudiKernel/SystemOfUnits.h"
#include "LHCbMath/GeomFun.h"

// Tsa
#include "ITFaultsCalculator.h"
#include "TsaKernel/Parabola.h"
#include "TsaKernel/Line.h"
#include "TsaKernel/Line3D.h"
#include "TsaKernel/Cluster.h"

// STDet
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTLayer.h"

DECLARE_TOOL_FACTORY( ITFaultsCalculator );


ITFaultsCalculator::ITFaultsCalculator(const std::string& type,
                     const std::string& name,
                     const IInterface* parent):
  GaudiTool(type, name, parent){

  // constructer
  declareProperty("efficiency",m_efficiency = 0.99);

  declareProperty("tolX",m_tolX = 0.5*Gaudi::Units::mm);
  declareProperty("tolY",m_tolY = 0.5*Gaudi::Units::mm);
  declareProperty("tolZ",m_tolZ = 0.0*Gaudi::Units::mm);

  declareInterface<ITsaFaultsCalculator>(this);
};

ITFaultsCalculator::~ITFaultsCalculator(){
  // destructer
}

StatusCode ITFaultsCalculator::initialize(){

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
     return Error("Failed to initialize",sc);
  }

  m_tol = Gaudi::XYZPoint(m_tolX, m_tolY, m_tolZ);

  // get geometry
  m_tracker = getDet<DeSTDetector>(DeSTDetLocation::location("IT"));
  
  return StatusCode::SUCCESS;
}

double ITFaultsCalculator::nFaults(const Tsa::Parabola& parab, 
                                   const Tsa::Line& line,
                                   const std::vector<Tsa::Cluster*>& clusters) const{

  double n = 0.0;
  const DeSTDetector::Layers& tLayers = m_tracker->layers();
  for ( DeSTDetector::Layers::const_iterator iterLayer = tLayers.begin(); 
        iterLayer != tLayers.end() ; ++iterLayer){

    if (haveCluster(*iterLayer,clusters) == false){
      // no cluster ==> could be fault

      double z = (*iterLayer)->globalCentre().z();
      Tsa::Line3D line3D = Tsa::createLine3D(parab.tangent(z),line, z);

      if ((insideLayer(*iterLayer, line3D) == true)
      &&( insideActive(*iterLayer, line3D) == true)){
        n += m_efficiency; 
      }
    }
  } // iterL 
  
  return n;
}

bool ITFaultsCalculator::insideLayer(const DeSTLayer* layer,  
                                   const Tsa::Line3D& line) const{
  
  bool isIn = false;
  Gaudi::XYZPoint point;
  double mu = 0;
  if (Gaudi::Math::intersection(line,layer->plane(),point, mu) == true){
    isIn = layer->isInside(point);
  } 
  return isIn;
}

bool ITFaultsCalculator::insideActive(const DeSTLayer* layer, 
                                      const Tsa::Line3D& line) const{
  
  const DeSTLayer::Sectors& tSectors = layer->sectors();
  DeSTLayer::Sectors::const_iterator iterS = tSectors.begin();
  bool found = false;
  Gaudi::XYZPoint gPoint;
  double mu;

  while ((iterS != tSectors.end())&&(found == false)){
       
    Gaudi::Math::intersection(line,(*iterS)->plane(),gPoint, mu);
    if ((*iterS)->globalInActive(gPoint,m_tol)){
      found = true; 
    }
    else {
      ++iterS;
    }

  } // iterS
  
  return found;
}

bool ITFaultsCalculator::haveCluster(const DeSTLayer* layer, 
                                     const std::vector<Tsa::Cluster*>& clusters) const{

  LHCb::STChannelID chan = layer->elementID();
  std::vector<Tsa::Cluster*>::const_iterator iterC = clusters.begin();
  bool found = false;
  while ((iterC != clusters.end())&&(found == false)){
    if ((*iterC)->id().isIT() == true){
      if ((*iterC)->id().stID().uniqueLayer() == chan.uniqueLayer()) found = true;    
    } 
    ++iterC;
  } // iterC
  return found;
}  


