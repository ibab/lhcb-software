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
#include "VPDet/DeVP.h"
#include "Event/VPCluster.h"

// local
#include "VPClusterPosition.h"

// boost
#include <boost/assign/std/vector.hpp>

//-----------------------------------------------------------------------------
// Implementation file for class : VeloClusterPosition
//
// 2010-02-02 : Victor Coco
// 
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( VPClusterPosition )

using namespace boost::assign;

typedef IVPClusterPosition::toolInfo toolInfo;
typedef IVPClusterPosition::Direction Direction;
typedef std::pair<double, double> Pair;

static const Gaudi::XYZVector ZVersor(0., 0., 1.);

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VPClusterPosition::VPClusterPosition(const std::string& type,
                               const std::string& name,
                               const IInterface* parent)
  : GaudiTool(type, name, parent),
    m_vPDet ( 0 ),
    m_trackDir ( ),
    m_gloPoint ( ),
    m_fracPos ( std::make_pair(0.,0.) )
{
  declareInterface<IVPClusterPosition>(this);
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
VPClusterPosition::~VPClusterPosition(){}
//=============================================================================
StatusCode VPClusterPosition::initialize()
{
  if( msgLevel(MSG::DEBUG) ) debug()<< " ==> initialize() " <<endmsg;
  //
  StatusCode sc=GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;
  // get VP detector
  m_errAnglePara.reserve(7);
  m_vPDet=getDet<DeVP>( DeVPLocation::Default );
  //
  return ( StatusCode::SUCCESS );
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode VPClusterPosition::finalize() {

  if( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return ( GaudiTool::finalize() );  // must be called after all other actions
}


////////////////////////////////////////////////////////////////////////////////
// MAIN FUNCTIONS
////////////////////////////////////////////////////////////////////////////////


//=========================================================================
// Call with a cluster only

toolInfo VPClusterPosition::position(const LHCb::VPLiteCluster* cluster) const
{
  if( msgLevel(MSG::DEBUG) ) debug()<< " ==> position(liteCluster)  " <<endmsg;
  
  return (position(cluster->channelID(),cluster->interPixelFraction()));
}

//=========================================================================
// Call with the channelID + fractional position
toolInfo VPClusterPosition::position(const LHCb::VPChannelID &centreChannel,
				       const Pair &fractionalPos) const{
  toolInfo myInfo;

  const DeVPSensor* sens=m_vPDet->sensor(centreChannel.sensor());
  const DeVPSquareType* sqSens=dynamic_cast<const DeVPSquareType*>(sens);
  Gaudi::XYZPoint aPoint = sqSens->globalXYZ(centreChannel.pixel()) ;
  Direction aDirection ;
  aDirection.first=aPoint.x()/aPoint.z();
  aDirection.second=aPoint.y()/aPoint.z();

  m_errAnglePara[0]=std::make_pair(6.5,6.5);
//   m_errAnglePara[1]=std::make_pair(1.27544e-02,1.26542e-02);
//   m_errAnglePara[2]=std::make_pair(-3.12746e+00,-1.76532e+00);
//   m_errAnglePara[3]=std::make_pair(1.61792e+01,1.33267e+01);
//   m_errAnglePara[4]=std::make_pair(1.21796e-02,0.0116024) ;
//   m_errAnglePara[5]=std::make_pair(-3.46670e-04,-0.000285165) ;
//   m_errAnglePara[6]=std::make_pair(1.72014e-05,1.47024e-05) ;

  m_errAnglePara[1]=std::make_pair(0.0130112,0.0130665);
  m_errAnglePara[2]=std::make_pair(-0.000547622,-0.000563597);
  m_errAnglePara[3]=std::make_pair(3.27229e-05,3.09884e-05);
  m_errAnglePara[4]=std::make_pair(-3.34741e-07,-2.68732e-07) ;
  m_errAnglePara[5]=std::make_pair(0.,0.) ;
  m_errAnglePara[6]=std::make_pair(0.,0.) ;

  return ( position(centreChannel,fractionalPos, aPoint, aDirection) );
}

//===========================================================================
// From cluster and track state

toolInfo VPClusterPosition::position(const LHCb::VPLiteCluster* cluster,
                                       const LHCb::StateVector& aState) const
{
  if( msgLevel(MSG::DEBUG) ) 
    debug()<< " ==> position (LiteCluster,VectorState) " <<endmsg;

  unsigned int sensorNumber=cluster->channelID().sensor();
  const DeVPSensor* sensor=m_vPDet->sensor(sensorNumber);
  double z=sensor->z();
  // build space point in global ref. frame
  Gaudi::XYZPoint aPoint(aState.x(), aState.y(), z);
  // build state pair
  Direction aDirection;
  aDirection.first=aState.tx();
  aDirection.second=aState.ty();
  
  verbose()<<"With Direction"<<endreq;
  m_errAnglePara[0]=std::make_pair(10.5,10.5);                 //limit gaussian / pol2
//   m_errAnglePara[1]=std::make_pair(1.27213e-02,1.27219e-02);   //Cste
//   m_errAnglePara[2]=std::make_pair(-2.56244e+00,-2.55358e+00 );//mean
//   m_errAnglePara[3]=std::make_pair(1.36982e+01,1.37099e+01);   //sigma
//   m_errAnglePara[4]=std::make_pair(1.70390e-02,0.0178506) ;    //p0
//   m_errAnglePara[5]=std::make_pair(-1.18557e-03 ,-0.00120728) ;//p1
//   m_errAnglePara[6]=std::make_pair(2.70955e-05 ,2.63303e-05) ; //p2
 
  m_errAnglePara[1]=std::make_pair(0.0126,0.0125928);
  m_errAnglePara[2]=std::make_pair(-0.000138665,-0.000133091);
  m_errAnglePara[3]=std::make_pair(-5.29756e-05,-5.36509e-05);
  m_errAnglePara[4]=std::make_pair(2.58067e-06,2.61561e-06 ) ;
  m_errAnglePara[5]=std::make_pair(-3.07851e-08,-3.14333e-08) ;
  m_errAnglePara[6]=std::make_pair(0.,0.) ;

  return ( position(cluster->channelID(),cluster->interPixelFraction(), aPoint, aDirection) );
}


//============================================================================
// Call from a cluster and a point+direction
toolInfo VPClusterPosition::position(const LHCb::VPLiteCluster* cluster,
                                       const Gaudi::XYZPoint& aGlobalPoint,
                                       const Direction& aDirection) const
{
  verbose()<<"With Direction"<<endreq; 
  m_errAnglePara[0]=std::make_pair(10.5,10.5);                 //limit gaussian / pol2
//   m_errAnglePara[1]=std::make_pair(1.27213e-02,1.27219e-02);   //Cste
//   m_errAnglePara[2]=std::make_pair(-2.56244e+00,-2.55358e+00 );//mean
//   m_errAnglePara[3]=std::make_pair(1.36982e+01,1.37099e+01);   //sigma
//   m_errAnglePara[4]=std::make_pair(1.70390e-02,0.0178506) ;    //p0
//   m_errAnglePara[5]=std::make_pair(-1.18557e-03 ,-0.00120728) ;//p1
//   m_errAnglePara[6]=std::make_pair(2.70955e-05 ,2.63303e-05) ; //p2
 
  m_errAnglePara[1]=std::make_pair(0.0126,0.0125928);
  m_errAnglePara[2]=std::make_pair(-0.000138665,-0.000133091);
  m_errAnglePara[3]=std::make_pair(-5.29756e-05,-5.36509e-05);
  m_errAnglePara[4]=std::make_pair(2.58067e-06,2.61561e-06 ) ;
  m_errAnglePara[5]=std::make_pair(-3.07851e-08,-3.14333e-08) ;
  m_errAnglePara[6]=std::make_pair(0.,0.) ;


  if( msgLevel(MSG::DEBUG) ) 
    debug() << " ==> position(litecluster, point, direction) " <<endmsg;
  return position(cluster->channelID(),cluster->interPixelFraction(),
                  aGlobalPoint,aDirection);
}


//============================================================================
// The real position function

toolInfo VPClusterPosition::position(const LHCb::VPChannelID &centreChan,
                                          const Pair & fracPos,
                                          const Gaudi::XYZPoint& aGlobalPoint,
                                          const Direction& aDirection) const
{
  // this struct will be returned as output
  toolInfo anInfo;
  // get information about sensor form passed cluster
  const DeVPSensor* sensor=m_vPDet->sensor(centreChan.sensor());
  if(sensor==0){
    Error("No valid pointer to sensor");
    anInfo.pixel=LHCb::VPChannelID(0);
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
Pair VPClusterPosition::errorEstimate(const Pair projAngle,
                                          const Pair pixelSize) const
{
  if( msgLevel(MSG::DEBUG) ) debug() << " ==> errorEstimate() " <<endmsg;
  // Set the angles to degrees
  //verbose()<<"Angles (should be in rad ) "<<projAngle.first<<" "<<projAngle.second<<endreq;
  
  Pair angle=std::make_pair(fabs(projAngle.first*180/Gaudi::Units::pi),fabs(projAngle.second*180/Gaudi::Units::pi));
  Pair error(0.,0.);
  // Parametrisation is taken from digitization simulation
  // Gaussian for angle below 10.5 degree
//   if (angle.first < m_errAnglePara[0].first){
//     error.first = m_errAnglePara[1].first*exp(-pow((angle.first-m_errAnglePara[2].first)/m_errAnglePara[3].first,2));
//   }
//   // 2nd order Polynome otherwise
//   else{
//     error.first = m_errAnglePara[6].first*angle.first*angle.first+m_errAnglePara[5].first*angle.first+m_errAnglePara[4].first;
//   }
//   if (angle.second < m_errAnglePara[0].second){
//     error.second = m_errAnglePara[1].second*exp(-pow((angle.second-m_errAnglePara[2].second)/m_errAnglePara[3].second,2));
//   }
//   // 2nd order Polynome otherwise
//   else{
//     error.second = m_errAnglePara[6].second*angle.second*angle.second+m_errAnglePara[5].second*angle.second+m_errAnglePara[4].second;
//   }

  error.first = m_errAnglePara[1].first+angle.first*m_errAnglePara[2].first+pow(angle.first,2)*m_errAnglePara[3].first
    +pow(angle.first,3)*m_errAnglePara[4].first+pow(angle.first,4)*m_errAnglePara[5].first
    +pow(angle.first,5)*m_errAnglePara[6].first;
  
  error.second = m_errAnglePara[1].second+angle.second*m_errAnglePara[2].second+pow(angle.second,2)*m_errAnglePara[3].second
    +pow(angle.second,3)*m_errAnglePara[4].second+pow(angle.second,4)*m_errAnglePara[5].second
    +pow(angle.second,5)*m_errAnglePara[6].second;
  
  if (pixelSize.first > 0.7)//case of long pixel
  {
    error.first = 0.1 ; //Look not too bad...
  }
  
  error.first /= pixelSize.first;
  error.second /= pixelSize.second;

  verbose()<<angle.first<<" "<<angle.second<<" "<<error.first<<" "<<error.second<<" Limit: "<< m_errAnglePara[0].second<<endreq;
  //
  return ( error );
}

//============================================================================
// Get the angles with sensor (in rad)
Pair VPClusterPosition::projectedAngle(const DeVPSensor* sensor) const
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

Pair VPClusterPosition::projectedAngle() const
{
  if( msgLevel(MSG::DEBUG) ) debug() << " ==> projectedAngle() " << endmsg;
  //
  return ( m_projectedAngle );
}

