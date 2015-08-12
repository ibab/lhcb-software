// Kernel
#include "GaudiKernel/IIncidentSvc.h" 
#include "GaudiKernel/PhysicalConstants.h"

#include "TrackInterfaces/ITrackExtrapolator.h"
#include "Kernel/ITrajPoca.h"

#include "GaudiKernel/IMagneticFieldSvc.h"

// Event
#include "Event/STCluster.h"
#include "Event/Track.h"
#include "Event/State.h"
#include "Kernel/LHCbID.h"
#include "Kernel/LineTraj.h"
#include "TrackKernel/TrackTraj.h"
#include "Event/StateVector.h"
#include "Event/STCluster.h"
#include "Kernel/Trajectory.h"
#include "TsaKernel/Line.h"

// Det
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"

#include "Kernel/ISTChannelIDSelector.h"
#include "STClusterCollector.h"

#include "GaudiKernel/GenericVectorTypes.h"
#include "GaudiKernel/GenericMatrixTypes.h"

DECLARE_TOOL_FACTORY( STClusterCollector )

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
  m_configured(false),
  m_selector(0)
{
  declareSTConfigProperty("dataLocation" , m_dataLocation , LHCb::STClusterLocation::TTClusters);
  declareProperty("xTol", m_xTol = 20.0 * Gaudi::Units::mm);
  declareProperty("yTol", m_yTol = 10.0 * Gaudi::Units::mm);
  declareProperty("window", m_windowSize = 20.0 *Gaudi::Units::mm);
  declareProperty("ignoreHitsOnTrack", m_ignoreHitsOnTrack = true);
  declareProperty("extrapolatorName", m_extrapolatorName = "TrackMasterExtrapolator") ;
  declareProperty( "SelectorType", m_selectorType = "STSelectChannelIDByElement" );
  declareProperty( "SelectorName", m_selectorName = "ALL" );
  declareProperty( "MagFieldOn", m_magneticField = true );

  declareInterface<ISTClusterCollector>(this);
}


StatusCode STClusterCollector::initialize() {
  
  StatusCode sc = ST::ToolBase::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  incSvc()->addListener(this, IncidentType::BeginEvent);

  m_magFieldSvc  = svc<IMagneticFieldSvc>( "MagneticFieldSvc");//, this );

  m_extrapolator = tool<ITrackExtrapolator>(m_extrapolatorName, "Extrapolator", this);
  m_trajPoca = tool<ITrajPoca>("TrajPoca");
  if (m_selectorName != "ALL") m_selector  = tool< ISTChannelIDSelector >( m_selectorType,m_selectorName );

  detType() == "IT" ? m_refZ = 750.0 *Gaudi::Units::cm : m_refZ = 250.0 *Gaudi::Units::cm ;

  if ( m_magneticField )
    info() << "Magnetic field is set to ON" << endmsg;
  else
    info() << "Magnetic field is set to OFF" << endmsg;

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

void STClusterCollector::initEvent() const{

  // retrieve clusters
  const LHCb::STClusters* clusterCont = get<LHCb::STClusters>(m_dataLocation);
  m_dataCont.clear();
  m_dataCont.reserve(clusterCont->size());

  // Loop over the clusters and make a trajectory for each cluster
  // TODO: maybe put this functionality in STMeasurement and use projectors
  LHCb::STClusters::const_iterator iClus = clusterCont -> begin();
  for ( ; iClus != clusterCont->end(); ++iClus ) {
    STClusterTrajectory thisClusTraj;    
    thisClusTraj.first  = *iClus ;
    const DeSTSector* aSector = tracker()->findSector((*iClus)->channelID());
    thisClusTraj.second.reset(
			      aSector->trajectory((*iClus)->channelID() , (*iClus)->interStripFraction()).release());     
    m_dataCont.push_back( thisClusTraj ) ;
  }

}

StatusCode STClusterCollector::execute(const LHCb::Track& track, 
                                       ISTClusterCollector::Hits& outputCont) const{

  if (m_configured == false)
    {
      m_configured = true;
      initEvent();
    }  
  
  LHCb::Trajectory *trackTraj;
  
  if ( m_magneticField )
    // If there is a mag field, use tracktraj
    {
      trackTraj = new LHCb::TrackTraj( track, m_magFieldSvc );
    }
  else
    // else, a LineTraj is used
    {
      LHCb::State aState;
      StatusCode sc = m_extrapolator->propagate( track, m_refZ, aState );
      if (sc.isFailure())
	{
	  return Warning("Failed to extrapolate state", StatusCode::SUCCESS, 1);
	}
      LHCb::StateVector stateVec = LHCb::StateVector(aState.stateVector(), aState.z());
      Gaudi::XYZVector slope( aState.slopes() );
      slope /= slope.z();
      trackTraj = new LHCb::LineTraj(aState.position(), slope, 
				     std::make_pair(-100000., 100000.));
    }

  StatusCode sc;
  LHCb::Trajectory* tmpTraj;
  double xMin, xMax, yMin, yMax, xTest, yTest, zTest, s1, s2;
  Gaudi::XYZVector distance;

  for (STClusterTrajectories::const_iterator iter = m_dataCont.begin();
       iter != m_dataCont.end() ; ++iter)
    {
      // check its not on the track
      if (m_ignoreHitsOnTrack == true && track.isOnTrack(LHCb::LHCbID((iter->first)->channelID())))
	{
	  continue;
	}
      
      const DeSTSector* aSector = findSector((iter->first)->channelID());
      
      // check we want this sector
      if (select(aSector->elementID()) == false)
	{
	  continue;
	}
      
      // get the traj
      tmpTraj = ((*iter).second).get();

      //       info() << "From : " << tmpTraj -> beginPoint()
      // 	     << " to " << tmpTraj -> endPoint() << endmsg;
      
      // check that y is consistant....
      yMin = tmpTraj->beginPoint().y();
      yMax = tmpTraj->endPoint().y();
      if (yMin > yMax) std::swap(yMin,yMax);
      zTest = tmpTraj->beginPoint().z();     
      yTest = (trackTraj -> position( zTest )).y();
      if (yTest > yMax + m_yTol || yTest < yMin - m_yTol )
	{
	  continue;
	}

      // and and also the x range
      xMin = tmpTraj->beginPoint().x();
      xMax = tmpTraj->endPoint().x();
      if (xMin > xMax) std::swap(xMin,xMax);
      xTest = (trackTraj -> position( zTest )).x();
      //double xTest = xLine.value(zTest);
      if (xTest > xMax+m_xTol || xTest < xMin-m_xTol)
	{
	  continue;
	}
     
      // poca....
      s1 = 0;
      s2 = tmpTraj -> muEstimate( trackTraj -> position(s1) );
      sc = m_trajPoca->minimize( *trackTraj, s1, true, *tmpTraj, s2, true, distance, 0.5*Gaudi::Units::mm);

      // Set up the vector onto which we project everything
      DualVector unit = dual( (tmpTraj->direction(s2).Cross( trackTraj->direction(s1) ) ).Unit() ) ;
      
      // Calculate the residual by projecting the distance onto unit
      const double residual = - dot( unit, distance ) ;
      
      if (fabs(residual) < m_windowSize) {
	ISTClusterCollector::Hit hitPair; hitPair.cluster = iter->first; hitPair.residual = residual;
	outputCont.push_back(hitPair);
      }      
    } // iter
  delete trackTraj;
  return StatusCode::SUCCESS;
}
  
bool STClusterCollector::select(const LHCb::STChannelID& chan) const{
  return m_selector == 0 ? true : m_selector->select(chan); 
}
