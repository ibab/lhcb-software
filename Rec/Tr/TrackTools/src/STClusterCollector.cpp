// $Id: STClusterCollector.cpp,v 1.1 2009-04-06 07:55:38 mneedham Exp $
 
// Kernel
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IIncidentSvc.h" 
#include "GaudiKernel/PhysicalConstants.h"

#include "TrackInterfaces/ITrackExtrapolator.h"
#include "Kernel/ITrajPoca.h"


// Event
#include "Event/STCluster.h"
#include "Event/Track.h"
#include "Event/State.h"
#include "Kernel/LHCbID.h"
#include "Kernel/LineTraj.h"
#include "Event/StateVector.h"
#include "Event/STCluster.h"
#include "Kernel/Trajectory.h"
#include "TsaKernel/Line.h"

// Det
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"

#include "STClusterCollector.h"

#include "GaudiKernel/GenericVectorTypes.h"
#include "GaudiKernel/GenericMatrixTypes.h"


DECLARE_TOOL_FACTORY( STClusterCollector);

// trivial helpers to make code clearer...
namespace
{
  typedef Gaudi::Matrix1x3 DualVector;

  DualVector dual( const Gaudi::XYZVector& v )
  {
    DualVector d;
    v.GetCoordinates( d.Array() );
    return d;
  }

  double dot( const DualVector& a, const Gaudi::XYZVector& b )
  {
    return a(0,0)*b.X() + a(0,1)*b.Y() + a(0,2)*b.Z();
  }
}
 
STClusterCollector::STClusterCollector(const std::string& type, 
                                       const std::string& name,
                                       const IInterface* parent ) :
  ST::ToolBase(type, name, parent),
  m_configured(false)
{
  declareSTConfigProperty("dataLocation" , m_dataLocation , LHCb::STClusterLocation::TTClusters);
  declareProperty("xTol", m_xTol = 20.0 * Gaudi::Units::mm);
  declareProperty("yTol", m_yTol = 10.0 * Gaudi::Units::mm);
  declareProperty("window", m_windowSize = 20.0 *Gaudi::Units::mm);

  declareProperty("extrapolatorName", m_extrapolatorName = "TrackMasterExtrapolator") ;

  declareInterface<ISTClusterCollector>(this);
}


StatusCode STClusterCollector::initialize() {
  
  StatusCode sc = ST::ToolBase::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  incSvc()->addListener(this, IncidentType::BeginEvent);

  m_extrapolator = tool<ITrackExtrapolator>(m_extrapolatorName, "Extrapolator", this);
  m_trajPoca = tool<ITrajPoca>("TrajPoca");

  detType() == "IT" ? m_refZ = 750.0 *Gaudi::Units::cm : m_refZ = 250.0 *Gaudi::Units::cm ; 

  return StatusCode::SUCCESS;
}

STClusterCollector::~STClusterCollector()
{
  //destructer
}

void STClusterCollector::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) {
    m_configured = false; 
  }
}

StatusCode STClusterCollector::execute(const LHCb::Track& track, 
                                       ISTClusterCollector::Hits& outputCont) const{

  if (m_configured == false){
    m_configured = true;
    m_dataCont = get<LHCb::STClusters>(m_dataLocation);
  }  
  
  LHCb::State aState;
  StatusCode sc = m_extrapolator->propagate(track, m_refZ,aState );
  if (sc.isFailure()){
    return Warning("Failed to extrapolate state", StatusCode::SUCCESS, 1);
  }

  // make a line traj from the state
  LHCb::StateVector stateVec = LHCb::StateVector(aState.stateVector(), aState.z());
  LHCb::LineTraj trackTraj = LHCb::LineTraj(aState.position(), aState.slopes(), 
                                            std::make_pair(-100000., 100000.));

  // helper lines in x and y
  Tf::Tsa::Line yLine = Tf::Tsa::Line(stateVec.ty(), stateVec.y(), stateVec.z());
  Tf::Tsa::Line xLine = Tf::Tsa::Line(stateVec.tx(), stateVec.x(), stateVec.z());

  for (LHCb::STClusters::const_iterator iter = m_dataCont->begin();
      iter != m_dataCont->end() ; ++iter){

      // check its not on the track
      if (track.isOnTrack(LHCb::LHCbID((*iter)->channelID())) == true) continue;

      const DeSTSector* aSector = findSector((*iter)->channelID());
      std::auto_ptr<LHCb::Trajectory> tmpTraj = 
      aSector->trajectory((*iter)->channelID() , (*iter)->interStripFraction() );
      
      // check that y is consistant....
      double yMin = tmpTraj->beginPoint().y();
      double yMax = tmpTraj->endPoint().y();
      if (yMin > yMax) std::swap(yMin,yMax);
      double zTest = tmpTraj->beginPoint().z();     
      double yTest = yLine.value(zTest);
      if (yTest > yMax + m_yTol || yTest < yMin - m_yTol ) continue;

      // and and also the x range
      double xMin = tmpTraj->beginPoint().x();
      double xMax = tmpTraj->endPoint().x();
      if (xMin > xMax) std::swap(xMin,xMax);
      double xTest = xLine.value(zTest);
      if (xTest > xMax+m_xTol || xTest < xMin-m_xTol) continue;
     
      // poca....
      Gaudi::XYZVector distance;
      double s1 = 0;
      double s2 = tmpTraj->muEstimate(trackTraj.position(s1));
      StatusCode sc = m_trajPoca->minimize(trackTraj,s1, *tmpTraj, s2, distance, 20.0*Gaudi::Units::mm);

      // Set up the vector onto which we project everything
      DualVector unit = dual( (tmpTraj->direction(s2).Cross( trackTraj.direction(s1) ) ).Unit() ) ;
  
      // Calculate the residual by projecting the distance onto unit
      const double residual = - dot( unit, distance ) ;

  
     if (fabs(residual) < m_windowSize) {
       ISTClusterCollector::Hit hitPair; hitPair.cluster = *iter; hitPair.residual = residual;
       outputCont.push_back(hitPair);
     }
  } // iter

  return StatusCode::SUCCESS;
}
  
