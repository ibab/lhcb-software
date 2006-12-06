
#include <algorithm>

// Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/IIncidentSvc.h"

#include "LHCbMath/GeomFun.h"

// Tsa
#include "TsaKernel/Cluster.h"
#include "TsaKernel/STCluster.h"
#include "TsaKernel/OTCluster.h"
#include "TsaKernel/Line3D.h"

#include "TsaCollector.h" 


// Boost
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>

using namespace LHCb;
using namespace boost::lambda;

DECLARE_TOOL_FACTORY( TsaCollector );
  
TsaCollector::TsaCollector(const std::string& type,
                     const std::string& name,
                     const IInterface* parent):
GaudiTool(type, name, parent),
m_initEvent(false){

 declareProperty("curvatureFactor", m_curvFactor = 42.0);
 declareProperty("searchOT", m_searchOT = true);
 declareProperty("searchIT", m_searchIT = true);
 declareProperty("itClusterLocation", m_itClusterLocation = Tsa::STClusterLocation::IT );
 declareProperty("otClusterLocation", m_otClusterLocation = Tsa::OTClusterLocation::Default);
 declareProperty("maxDriftDistance", m_maxDriftRadius = 2.8); 
 declareProperty("minDriftDistance", m_minDriftRadius = -0.3);

 declareInterface<ITsaCollector>(this);

}

TsaCollector::~TsaCollector(){
  // destructer
}

StatusCode TsaCollector::initialize(){

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
     return Error("Failed to initialize",sc);
  }

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );

  return StatusCode::SUCCESS;
}

StatusCode TsaCollector::collect(const LHCb::State& aState, 
                                 Tsa::Clusters* clusters, const unsigned int nsigma) const{

  // check init
  if (m_initEvent == false) getClusters(); 
 
  // make paramterization of the track
  Tsa::Line line = yParam(aState);
  Tsa::Parabola parab = xParam(aState);
  
  WindowSize winSize(nsigma*sqrt(aState.errX2()),nsigma*sqrt(aState.errY2()));

  if (m_searchOT) searchOT(parab, line, winSize,clusters);  
  if (m_searchIT) searchIT(parab, line, winSize,clusters);

  return StatusCode::SUCCESS;
}

StatusCode TsaCollector::collect(const LHCb::State& aState, 
                                 Tsa::STClusters* clusters, const unsigned int nsigma) const{

  // check init
  if (m_initEvent == false) getClusters(); 
 
  // make paramterization of the track
  Tsa::Line line = yParam(aState);
  Tsa::Parabola parab = xParam(aState);
  
  WindowSize winSize(nsigma*sqrt(aState.errX2()),nsigma*sqrt(aState.errY2()));
  
  searchIT(parab, line, winSize,clusters);

  return StatusCode::SUCCESS;
}


StatusCode TsaCollector::collect(const LHCb::State& aState, 
                                 Tsa::OTClusters* clusters, const unsigned int nsigma) const{

  // check init
  if (m_initEvent == false) getClusters(); 
 
  // make paramterization of the track
  Tsa::Line line = yParam(aState);
  Tsa::Parabola parab = xParam(aState);

  WindowSize winSize(nsigma*sqrt(aState.errX2()),nsigma*sqrt(aState.errY2()));
  
  searchOT(parab, line, winSize,clusters);
  return StatusCode::SUCCESS;
}



void TsaCollector::getClusters() const {

  m_itClusters = get<Tsa::STClusters>(m_itClusterLocation);
  m_otClusters = get<Tsa::OTClusters>(m_otClusterLocation);
  m_initEvent = true;
}

void TsaCollector::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) { m_initEvent = false; }
}

bool TsaCollector::inWindow(const Tsa::Parabola& parab,
                            const Tsa::Line& line, 
                            const Tsa::Cluster* cluster,
                            const TsaCollector::WindowSize& win) const{
 
  // get z of cluster
  const double zMid = cluster->zMid(); 
 
  // check y  
  if (cluster->isInsideY(line.value(zMid), win.second) == false) return false;

  // check x - saves some time ? but not exact - hence big window
  if (cluster->isInsideX(parab.value(zMid), 20.0*Gaudi::Units::cm) == false) return false;

  // now look at 3-d intersection --> need two lines
  Tsa::Line tanLine = parab.tangent(zMid);
  Tsa::Line3D aLine3D = Tsa::createLine3D(tanLine,line,zMid);
  Tsa::Line3D traj(cluster->trajectory().position(0.),
                   cluster->trajectory().direction(0.));

  double doca = Gaudi::Math::distance(traj, aLine3D);
  return (doca < win.first);     
}


bool TsaCollector::inContainer(LHCb::LHCbID testChan ,
                               std::vector<LHCb::LHCbID>& ids ) const{

 std::vector<LHCb::LHCbID>::iterator iter = std::find(ids.begin(),ids.end(), testChan); 
 return (iter != ids.end() ? true : false);
}
 
