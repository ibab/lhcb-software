// Include files

// stl
#include <vector>

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// Math
#include "LHCbMath/LHCbMath.h"
#include "GaudiKernel/Point3DTypes.h"

// Velo
#include "VeloPixDet/DeVeloPix.h"
#include "Event/VeloPixCluster.h"

// local
#include "VeloPixClusterPosition.h"

// boost
#include <boost/assign/std/vector.hpp>

//-----------------------------------------------------------------------------
// Implementation file for class : VeloClusterPosition
//
// 2010-02-02 : Victor Coco
// 
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( VeloPixClusterPosition );

using namespace boost::assign;

typedef IVeloPixClusterPosition::toolInfo toolInfo;
typedef IVeloPixClusterPosition::Direction Direction;
typedef std::pair<double, double> Pair;

static const Gaudi::XYZVector ZVersor(0., 0., 1.);

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VeloPixClusterPosition::VeloPixClusterPosition(const std::string& type,
                               const std::string& name,
                               const IInterface* parent)
  : GaudiTool(type, name, parent),
    m_veloPixDet ( 0 ),
    m_trackDir ( ),
    m_gloPoint ( ),
    m_fracPos ( std::make_pair(0.,0.) )
{
  declareInterface<IVeloPixClusterPosition>(this);
  // default paramertrizations are of form error=gaussian if angle is < 10.5 deg,
  // pol2 otherwise
  // By default the parameters are for tracks comming from PV (0,0,0)
  // If the track state is known, the parametrers correspond to resolution as function
  // of the angle between track and sensor
  
  //
  declareProperty("DefaultResolution", m_defaultResolution);
}
//=============================================================================
// Destructor
//=============================================================================
VeloPixClusterPosition::~VeloPixClusterPosition(){
};
//=============================================================================
StatusCode VeloPixClusterPosition::initialize()
{
  debug()<< " ==> initialize() " <<endmsg;
  //
  StatusCode sc=GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;
  // get VeloPix detector
  m_errAnglePara.reserve(6);
  m_veloPixDet=getDet<DeVeloPix>( DeVeloPixLocation::Default );
  //
  return ( StatusCode::SUCCESS );
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode VeloPixClusterPosition::finalize() {

  debug() << "==> Finalize" << endmsg;

  return ( GaudiTool::finalize() );  // must be called after all other actions
}


////////////////////////////////////////////////////////////////////////////////
// MAIN FUNCTIONS
////////////////////////////////////////////////////////////////////////////////


//=========================================================================
// Call with a cluster only

toolInfo VeloPixClusterPosition::position(const LHCb::VeloPixLiteCluster* cluster) const
{
  if( msgLevel(MSG::DEBUG) ) debug()<< " ==> position(liteCluster)  " <<endmsg;
  
  return (position(cluster->channelID(),cluster->interPixelFraction()));
}

//=========================================================================
// Call with the channelID + fractional position
toolInfo VeloPixClusterPosition::position(const LHCb::VeloPixChannelID &centreChannel,
				       const Pair &fractionalPos) const{
  toolInfo myInfo;
  const DeVeloPixSensor* sens=m_veloPixDet->sensor(centreChannel.sensor());
  const DeVeloPixSquareType* sqSens=dynamic_cast<const DeVeloPixSquareType*>(sens);
  Gaudi::XYZPoint aPoint = sqSens->globalXYZ(centreChannel.pixel()) ;
  Direction aDirection ;
  aDirection.first=aPoint.x()/aPoint.z();
  aDirection.second=aPoint.y()/aPoint.z();

  m_errAnglePara[0]=0.0134901;
  m_errAnglePara[1]=-6.46661;
  m_errAnglePara[2]=15.6350;
  m_errAnglePara[3]=0.0113722;
  m_errAnglePara[4]= -0.000419185;
  m_errAnglePara[5]= 0.00000866839;

  return ( position(centreChannel,fractionalPos, aPoint, aDirection) );
}

//===========================================================================
// From cluster and track state

toolInfo VeloPixClusterPosition::position(const LHCb::VeloPixLiteCluster* cluster,
                                       const LHCb::StateVector& aState) const
{
  if( msgLevel(MSG::DEBUG) ) 
    debug()<< " ==> position (LiteCluster,VectorState) " <<endmsg;
  unsigned int sensorNumber=cluster->channelID().sensor();
  const DeVeloPixSensor* sensor=m_veloPixDet->sensor(sensorNumber);
  double z=sensor->z();
  // build space point in global ref. frame
  Gaudi::XYZPoint aPoint(aState.x(), aState.y(), z);
  // build state pair
  Direction aDirection;
  aDirection.first=aState.tx();
  aDirection.second=aState.ty();
  m_errAnglePara[0]=0.0127046;
  m_errAnglePara[1]=-2.65707;
  m_errAnglePara[2]=12.9359;
  m_errAnglePara[3]=0.0192615;
  m_errAnglePara[4]=-0.00146512;
  m_errAnglePara[5]=0.0000355052;
  return ( position(cluster->channelID(),cluster->interPixelFraction(), aPoint, aDirection) );
}


//============================================================================
// Call from a cluster and a point+direction
toolInfo VeloPixClusterPosition::position(const LHCb::VeloPixLiteCluster* cluster,
                                       const Gaudi::XYZPoint& aGlobalPoint,
                                       const Direction& aDirection) const
{
  if( msgLevel(MSG::DEBUG) ) 
    debug() << " ==> position(litecluster, point, direction) " <<endmsg;
  return position(cluster->channelID(),cluster->interPixelFraction(),
                  aGlobalPoint,aDirection);
}


//============================================================================
// The real position function

toolInfo VeloPixClusterPosition::position(const LHCb::VeloPixChannelID &centreChan,
                                          const Pair & fracPos,
                                          const Gaudi::XYZPoint& aGlobalPoint,
                                          const Direction& aDirection) const
{
  // this struct will be returned as output
  toolInfo anInfo;
  // get information about sensor form passed cluster
  const DeVeloPixSensor* sensor=m_veloPixDet->sensor(centreChan.sensor());
  if(sensor==0){
    Error("No valid pointer to sensor");
    anInfo.pixel=LHCb::VeloPixChannelID(0);
    anInfo.fractionalPosition=std::make_pair(0.,0.);
    anInfo.fractionalError=std::make_pair(0.,0.);
    return ( anInfo );
  }

  
  // fill partially the toolInfo structure
  anInfo.pixel=centreChan;
  anInfo.fractionalPosition=fracPos;
  m_trackDir=Gaudi::XYZVector(aDirection.first, aDirection.second, 1.);
  m_gloPoint=aGlobalPoint;
  Pair pixelSize = sensor->PixelSize(centreChan);
  // get the angles between aDirection and the sensor
  Pair aProjAngle =projectedAngle(sensor);
  //-- error estimate
  Pair error=errorEstimate(aProjAngle, pixelSize);

  anInfo.fractionalError=error;
  //
  return ( anInfo );
}


////////////////////////////////////////////////////////////////////////////////
// UTILS
////////////////////////////////////////////////////////////////////////////////


//============================================================================
// distance in mm, angles in degrees.
// projAngle can be the anlge between TrState and sensor, as well as from 0
//
Pair VeloPixClusterPosition::errorEstimate(const Pair projAngle,
                                          const Pair pixelSize) const
{
  if( msgLevel(MSG::DEBUG) ) debug() << " ==> errorEstimate() " <<endmsg;
  // Set the angles to degrees
  Pair angle=std::make_pair(fabs(projAngle.first*180/Gaudi::Units::pi),fabs(projAngle.second*180/Gaudi::Units::pi));
  Pair error(0.,0.);
  // Parametrisation is taken from digitization simulation
  // Gaussian for angle below 10.5 degree
  if (angle.first < 10.5){
    error.first = m_errAnglePara[0]*exp(pow((angle.first-m_errAnglePara[1])/m_errAnglePara[2],2));
  }
  // 2nd order Polynome otherwise
  else{
    error.first = m_errAnglePara[3]*angle.first*angle.first+m_errAnglePara[4]*angle.first+m_errAnglePara[5];
  }
  if (angle.second < 10.5){
    error.second = m_errAnglePara[0]*exp(pow((angle.second-m_errAnglePara[1])/m_errAnglePara[2],2));
  }
  // 2nd order Polynome otherwise
  else{
    error.second = m_errAnglePara[3]*angle.second*angle.second+m_errAnglePara[4]*angle.second+m_errAnglePara[5];
  }
  error.first /= pixelSize.first;
  error.second /= pixelSize.second;
  
  //
  return ( error );
}

//============================================================================
// Get the angles with sensor (in rad)
Pair VeloPixClusterPosition::projectedAngle(const DeVeloPixSensor* sensor) const
{
  if( msgLevel(MSG::DEBUG) ) debug()<< " ==> projectedAngle(sensor) " <<endmsg;
  //-- returned pair consists of projected angles over x and y axis
  //-- both values are needed to estimate error
  // transform global point to the local reference frame
  Gaudi::XYZPoint aLocPoint=sensor->globalToLocal(m_gloPoint);
  // get some point making the direction from m_gloPoint and transform it to local frame
  Gaudi::XYZPoint aGlobPointDir(m_gloPoint.x()-m_trackDir.x(),
                                m_gloPoint.y()-m_trackDir.y(),
                                m_gloPoint.z()-m_trackDir.z());
  Gaudi::XYZPoint aLocPointDir=sensor->globalToLocal(aGlobPointDir);
  // Now get the direction in the local frame
  Gaudi::XYZVector LocalDir = Gaudi::XYZVector(aLocPoint.x()-aLocPointDir.x(),
                                               aLocPoint.y()-aLocPointDir.y(),
                                               aLocPoint.z()-aLocPointDir.z()
                                               );
  
  // This gives the angle in x and y
  m_projectedAngle=std::make_pair( atan(LocalDir.x()/LocalDir.z()),atan(LocalDir.y()/LocalDir.z()));
  return ( m_projectedAngle );
}

Pair VeloPixClusterPosition::projectedAngle() const
{
  if( msgLevel(MSG::DEBUG) ) debug() << " ==> projectedAngle() " << endmsg;
  //
  return ( m_projectedAngle );
}

