// $Id: VeloClusterPosition.cpp,v 1.8 2007-05-16 07:40:39 szumlat Exp $
// Include files

// stl
#include <vector>

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// from LHCbDefinitions
#include "LHCbMath/LHCbMath.h"
#include "GaudiKernel/Point3DTypes.h"

// Velo
#include "VeloDet/DeVelo.h"
#include "Event/VeloCluster.h"

// local
#include "VeloClusterPosition.h"

// boost
#include <boost/assign/std/vector.hpp>

//-----------------------------------------------------------------------------
// Implementation file for class : VeloClusterPosition
//
// 2005-09-30 : Tomasz Szumlak 
// 
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( VeloClusterPosition );

using namespace boost::assign;

typedef IVeloClusterPosition::toolInfo toolInfo;
typedef IVeloClusterPosition::Pair Pair;

static const Gaudi::XYZVector ZVersor(0., 0., 1.);

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VeloClusterPosition::VeloClusterPosition(const std::string& type,
                               const std::string& name,
                               const IInterface* parent)
  : GaudiTool(type, name, parent)
{
  m_veloDet = getDet<DeVelo>( DeVeloLocation::Default );
  // default paramertrizations are of form error=slope*pitch+const
  // the first value represents const and is given in um
  // the second value is slope and is dimensionless
  m_defaultRSensorResolution.push_back(-3.23);
  m_defaultRSensorResolution.push_back(0.23);
  m_defaultPhiSensorResolution.push_back(-3.);
  m_defaultPhiSensorResolution.push_back(0.28);
  //
  declareInterface<IVeloClusterPosition>(this);
  // Limits for projected angle for R sensor
  m_rAngleLimits+=3., 5., 7., 9., 11., 15., 20., 25., 30., 90.;
  // Limits for projected angle for Phi sensor
  m_phiAngleLimits+=2., 4., 6., 8., 10., 15., 90.;
  // the resolution parametrizations are of form: slope*pitch+const
  // const vector for R sensor - each number is in micrometers
  m_rConstPara+=-1.67, -3.20, -5.04, -6.89, -5.26, -4.76, 6.31, 9.12, 9.37, 9.37;
  // slopes vector for R sensor - each number is dimensionless
  m_rSlopePara+=.231, .238, .244, .246, .200, .114, .023, .018, .0196, .0196;
  // const vector for Phi sensor
  m_phiConstPara+=-1.86, -2.65, -4.63, -7.01, -4.67, 7.31, 7.31;
  // slope vector for Phi sensor
  m_phiSlopePara+=.240, .237, .246, .256, .191, .092, .092;
  declareProperty("DefaultRSensorResolution", m_defaultRSensorResolution);
  declareProperty("DefaultPhiSensorResolution", m_defaultPhiSensorResolution);
  declareProperty("RAngleLimits", m_rAngleLimits);
  declareProperty("PhiAngleLimits", m_phiAngleLimits);
  declareProperty("RConstPara", m_rConstPara);
  declareProperty("RSlopePara", m_rSlopePara);
  declareProperty("PhiConstPara", m_phiConstPara);
  declareProperty("PhiSlopePara", m_phiSlopePara);
}
//=============================================================================
// Destructor
//=============================================================================
VeloClusterPosition::~VeloClusterPosition() {};
//=============================================================================
StatusCode VeloClusterPosition::initialize()
{
  debug()<< " ==> initialize() " <<endmsg;
  //
  StatusCode sc=GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;
  // build table with resolution paramettrizations
  StatusCode paraStatus=createResParaTable();
  if(paraStatus.isFailure()) return ( paraStatus );
  //
  return ( StatusCode::SUCCESS );
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode VeloClusterPosition::finalize() {

  debug() << "==> Finalize" << endmsg;

  return ( GaudiTool::finalize() );  // must be called after all other actions
}
//=============================================================================
// position calculation, depending on sensor type the clusterPos
// is radial or angular variable
//=============================================================================
toolInfo VeloClusterPosition::weightedLAPos(
                              const LHCb::VeloCluster* cluster) const
{
  debug()<< " ==> weightedLAPos() " <<endmsg;
  //
  Pair posInfo;
  toolInfo myInfo;
  int intDistance=0;
  double fractionalPos=0.;
  // calculate fractional position in units of 'strip'
  posInfo=fracPosLA(cluster);
  fractionalPos=posInfo.first;
  intDistance=static_cast<int>(posInfo.second);
  //
  LHCb::VeloChannelID intDistanceID;
  // determine the closest VeloChannelID (strip) to the cluster
  const DeVeloSensor* sens=m_veloDet->sensor(cluster->firstChannel().sensor());
  StatusCode neighbourStrip=sens->neighbour(cluster->firstChannel(),
                                            intDistance, intDistanceID);
  //
  if(neighbourStrip.isSuccess()){
    double error=errorEstimate(cluster, intDistanceID, fractionalPos);
    // fill the new wrapper object
    myInfo.strip=intDistanceID;
    myInfo.fractionalPosition=fractionalPos;
    myInfo.fractionalError=error;
    return ( myInfo );
  }else{
    info()<< "Given cluster is invalid!" <<endmsg;
    // return dummy object for nvalid cluster
    myInfo.strip=LHCb::VeloChannelID(0);
    myInfo.fractionalPosition=0.;
    myInfo.fractionalError=0.;
    //
    return ( myInfo );
  }
}
//=========================================================================
double VeloClusterPosition::resolution(const double& pitch,
                                       const int& sensType) const
{ 
  debug()<< " ==> resolution() " <<endmsg;
  //
  double p_R_0=m_defaultRSensorResolution[0];
  double p_R_1=m_defaultRSensorResolution[1];
  //
  double p_Phi_0=m_defaultPhiSensorResolution[0];
  double p_Phi_1=m_defaultPhiSensorResolution[1];
  //
  double resolution=0.;
  if(sensType==0){
    // R sensor
    resolution=p_Phi_0+(p_Phi_1*pitch);
  }else if(sensType==1){
    resolution=p_R_0+(p_R_1*pitch);
  }
  //
  return ( resolution );
}
//=========================================================================
Pair VeloClusterPosition::fracPosLA(const LHCb::VeloCluster* cluster) const
{
  debug()<< " ==> fracPosLA()" <<endmsg;
  //
  Pair posInfo;
  double centre=0., sum=0., fractionalPos;
  int intDistance=0;
  int stripNumber=cluster->size();
  std::vector<LHCb::VeloChannelID> chanCont=cluster->channels();
  //
  const DeVeloSensor* sens=m_veloDet->sensor(chanCont[0].sensor());
  for(int i=0; i<stripNumber; i++){
    StatusCode sc=sens->channelDistance(chanCont[0],chanCont[i],intDistance);
    sc.ignore(); // channels in a cluster are always in same sensor!
    centre+=float(intDistance)*cluster->adcValue(i);
    sum+=cluster->adcValue(i);
  }
  //
  if(stripNumber==1){
    fractionalPos=0.0;
  }else{
    centre=centre/sum;
    intDistance=int(LHCbMath::round(centre));
    fractionalPos=centre-intDistance;
  }
  //
  if(fractionalPos<0.) fractionalPos+=1;
  posInfo=std::make_pair(fractionalPos, intDistance);
  //  
  return  ( posInfo );
}
//=========================================================================
double VeloClusterPosition::errorEstimate(
                     const LHCb::VeloCluster* cluster,
                     const LHCb::VeloChannelID& intDistanceID,
                     const double& fractionalPos) const
{ 
  debug()<< "==> posAndError() " <<endmsg;
  //  
  unsigned int sensor=cluster->channelID().sensor();
  const DeVeloSensor* sens=m_veloDet->sensor(sensor);
  double pitch=0., errorPos=0.;
  //
  if(sens->isR()||sens->isPileUp()){
    const DeVeloRType* rSens=dynamic_cast<const DeVeloRType*>(sens);
    pitch=rSens->rPitch(intDistanceID.strip(), fractionalPos);
    errorPos=resolution(pitch/Gaudi::Units::micrometer, 1);
    errorPos/=(pitch/Gaudi::Units::micrometer);
    // return values for cluster position and position errorPos
  }
  else if(sens->isPhi()){
    const DeVeloPhiType* phiSens=dynamic_cast<const DeVeloPhiType*>(sens);
    double minRadius=0., maxRadius=0.;
    unsigned int zoneOfCluster=0;
    zoneOfCluster=sens->zoneOfStrip(intDistanceID.strip());
    minRadius=sens->rMin(zoneOfCluster);
    maxRadius=sens->rMax(zoneOfCluster);
    double meanRadius=0.5*(maxRadius+minRadius);
    pitch=phiSens->phiPitch(meanRadius);
    errorPos=resolution(pitch/Gaudi::Units::micrometer, 0);
    // error as a fraction of the strip distance
    errorPos/=(pitch/Gaudi::Units::micrometer);
    //
  }
  return ( errorPos );
}
//============================================================================
toolInfo VeloClusterPosition::position(const LHCb::VeloCluster* cluster) const
{
  debug()<< " ==> position(cluster) " <<endmsg;
  //
  return ( weightedLAPos(cluster) );
}
//============================================================================
toolInfo VeloClusterPosition::position(const LHCb::VeloCluster* cluster,
                                       const Gaudi::XYZPoint& aGlobalPoint,
                                       const Pair& aDirection) const
{
  debug()<< " ==> position(cluster, point, direction) " <<endmsg;
  // this struct will be returned as output
  toolInfo anInfo;
  // get information about sensor form passed cluster
  unsigned int sensorNumber=cluster->channelID().sensor();
  const DeVeloSensor* sensor=m_veloDet->sensor(sensorNumber);
  if(sensor==0){
    Error("No valid pointer to sensor", StatusCode::FAILURE );
  }
  // transform global point to the local reference frame
  Gaudi::XYZPoint aLocalPoint=sensor->globalToLocal(aGlobalPoint);
  StatusCode pointStatus=sensor->isInActiveArea(aLocalPoint);
  // transform global slopes to local
  double localSlopeXZ=0., localSlopeYZ=0.;
  if(pointStatus.isSuccess()){
    if(sensor->isLeft()&&(!(sensor->isDownstream()))){
      debug()<< " left R sensor on Phi module (upstream) " <<endmsg;
      // the local ref. frame is the same as global, do nothing
      localSlopeXZ=aDirection.first;
      localSlopeYZ=aDirection.second;
    }
    else if(sensor->isLeft()&&sensor->isDownstream()){    
      debug()<< " left R sensor on R module (downstream) " <<endmsg;
      localSlopeXZ=(aDirection.first)*(-1);
      localSlopeYZ=aDirection.second;
    }
    else if(sensor->isRight()&&sensor->isDownstream()){
      debug()<< " right R sensor on R module (downstream) " <<endmsg;
      localSlopeXZ=aDirection.first;
      localSlopeYZ=(aDirection.second)*(-1);
    }
    else if(sensor->isRight()&&(!(sensor->isDownstream()))){
      debug()<< " right R sensor on Phi module (downstream) " <<endmsg;
      localSlopeXZ=(aDirection.first)*(-1);
      localSlopeYZ=(aDirection.second)*(-1);
    }
  }else{
    info()<< " ==> Local point is outside active area! " <<endmsg;
    info()<< " --------------------------------------- " <<endmsg;
    anInfo.strip=LHCb::VeloChannelID(0);
    anInfo.fractionalPosition=0.;
    anInfo.fractionalError=0.;
    //
    return ( anInfo );
  }
  // a track angle and vector parallel to the track
  double alphaOfTrack=0.;
  Gaudi::XYZVector parallel2Track(0., 0., 0.);
  double modParallel2Track=0.;
  Pair localSlopes=std::make_pair(localSlopeXZ, localSlopeYZ);
  alphaOfTrack=angleOfTrack(localSlopes, parallel2Track);
  modParallel2Track=sqrt(parallel2Track.Mag2());
  // calculate fractional position
  Pair fractionalPair=fracPosLA(cluster);
  // inter-strip position translate into (0., 1.) range
  double isp=fractionalPair.first;
  // fill partially the toolInfo structure
  LHCb::VeloChannelID centreChannel=cluster->channelID();
  anInfo.strip=centreChannel;
  anInfo.fractionalPosition=isp;
  //--------------------------------------------------------------------------
  // now comes part that is specific to the sensor type  
  //--------------------------------------------------------------------------
  // calculate projected angle - sensor specific!
  //--------------------------------------------------------------------------
  // cosine of angle between vector parallel to the track and vector
  // normal to the strip - depends on local strip geometry
  double cosTrackOnNormal=0.;
  // projection of the parallel to track on normal to local strip
  double trackOnNormal=0.;
  // projection of the parallel to track on Z axis
  double trackOnZ=0.;
  // local pitch
  double localPitch=0.;
  // projected angle of the track wrt local strip
  double projectedAngle=0.;
  // error estimate
  double error=0.;
  unsigned int centreStrip=centreChannel.strip();
  //--------------------------------------------------------------------------
  // R sensor
  //--------------------------------------------------------------------------
  if(sensor->isR()||(sensor->isPileUp())){ 
    const DeVeloRType* rSensor=dynamic_cast<const DeVeloRType*>(sensor);
    double rOfCluCentre=rSensor->rOfStrip(centreStrip, isp);
    double localPitch=rSensor->rPitch(rOfCluCentre);
    // vector normal to tangent to the entry strip
    Gaudi::XYZVector perp2RStrip(aLocalPoint.x(), aLocalPoint.y(), 0.);
    double modPerp2RStrip=sqrt(perp2RStrip.Mag2());
    // the vector normalized
    Gaudi::XYZVector perp2RStripVersor(perp2RStrip.x()/modPerp2RStrip,
                                       perp2RStrip.y()/modPerp2RStrip,
                                       0.
    );
    cosTrackOnNormal=parallel2Track.Dot(perp2RStripVersor);
    trackOnNormal=fabs(modParallel2Track*cosTrackOnNormal);
    trackOnZ=modParallel2Track*cos(alphaOfTrack);
    m_projectedAngle=atan(trackOnNormal/trackOnZ);
    error=errorOnR(projectedAngle/Gaudi::Units::degree, 
                   localPitch/Gaudi::Units::micrometer);
  }
  //--------------------------------------------------------------------------
  // Phi sensor
  //--------------------------------------------------------------------------
  if(sensor->isPhi()){
    const DeVeloPhiType* phiSensor=dynamic_cast<const DeVeloPhiType*>(sensor);
    // calculate local pitch
    double x2=aLocalPoint.x()*aLocalPoint.x();
    double y2=aLocalPoint.y()*aLocalPoint.y();
    double radiusOnPhi=sqrt(x2+y2);
    localPitch=phiSensor->phiPitch(radiusOnPhi);
    // get the centre strip geometry for phi sensor
    std::pair<Gaudi::XYZPoint, Gaudi::XYZPoint> stripLimits;
    stripLimits=phiSensor->localStripLimits(centreStrip);
    Gaudi::XYZPoint stripAPoint=stripLimits.first;
    Gaudi::XYZPoint stripBPoint=stripLimits.second;
    // vector normal entry strip - Phi
    Gaudi::XYZVector perp2PhiStrip((stripAPoint.y()-stripBPoint.y()),   
                                   (stripBPoint.x()-stripAPoint.x()),
                                   0.
    );
    double modPerp2PhiStrip=sqrt(perp2PhiStrip.Mag2());
    // the vector normalized
    Gaudi::XYZVector perp2PhiStripVersor(perp2PhiStrip.x()/modPerp2PhiStrip,
                                         perp2PhiStrip.y()/modPerp2PhiStrip,
                                         0.
    );
    cosTrackOnNormal=parallel2Track.Dot(perp2PhiStripVersor);
    trackOnNormal=fabs(modParallel2Track*cosTrackOnNormal);
    trackOnZ=modParallel2Track*cos(alphaOfTrack);
    m_projectedAngle=atan(trackOnNormal/trackOnZ);
    error=errorOnPhi(projectedAngle/Gaudi::Units::degree,
                     localPitch/Gaudi::Units::micrometer);
  }
  anInfo.fractionalError=error;
  //
  return ( anInfo );
}
//============================================================================
double VeloClusterPosition::angleOfTrack(const Pair localSlopes,
                                  Gaudi::XYZVector& parallel2Track) const
{
  debug()<< " ==> trackAngle() " <<endmsg;
  // vectors
  Gaudi::XYZVector perpPi1(1., 0., -(localSlopes.first));
  Gaudi::XYZVector perpPi2(0., 1., -(localSlopes.second));
  // vector parallel to the track
  Gaudi::XYZVector parallel=perpPi1.Cross(perpPi2);
  double modParallel=sqrt(parallel.Mag2());
  // and normalized parallel to track
  Gaudi::XYZVector normParallel(parallel.x()/modParallel,
                                parallel.y()/modParallel,
                                parallel.z()/modParallel
  );
  parallel2Track=normParallel;
  double cosOfInsertion=parallel2Track.Dot(ZVersor);
  double alphaOfInsertion=acos(cosOfInsertion);
  //
  return ( alphaOfInsertion );
}
//============================================================================
StatusCode VeloClusterPosition::createResParaTable()
{
  debug()<< " ==> createResParaTable() " <<endmsg;
  // build table with parametrizations and corresponding 
  // projected angle limits - 
  // table is of type => pair<double, std::pair<double, double> > 
  //--------------------------------------------------------------------------
  // build table for R sensor
  //--------------------------------------------------------------------------
  assert(m_rAngleLimits.size()==m_rConstPara.size());
  assert(m_rAngleLimits.size()==m_rSlopePara.size());
  for(unsigned int para=0; para<m_rAngleLimits.size(); ++para){
    Pair paraPair=std::make_pair(m_rConstPara[para], m_rSlopePara[para]);
    ResPair resPair=std::make_pair(m_rAngleLimits[para], paraPair);
    m_rParaTable.push_back(resPair);
  }
  //--------------------------------------------------------------------------
  // build table for Phi sensor
  //--------------------------------------------------------------------------
  assert(m_phiAngleLimits.size()==m_phiConstPara.size());
  assert(m_phiAngleLimits.size()==m_phiSlopePara.size());
  for(unsigned int para=0; para<m_phiAngleLimits.size(); ++para){
    Pair paraPair=std::make_pair(m_phiConstPara[para], m_phiSlopePara[para]);
    ResPair resPair=std::make_pair(m_phiAngleLimits[para], paraPair);
    m_phiParaTable.push_back(resPair);
  }
  return ( StatusCode::SUCCESS );
}
//============================================================================
double VeloClusterPosition::errorOnR(const double projAngle,
                                     const double pitch) const
{
  debug()<< " ==> errorOnR() " <<endmsg;
  // pick up appropriate parametrization - iterate over rTable
  ResVector::const_iterator resIT=m_rParaTable.begin();
  for( ; resIT!=m_rParaTable.end(); ++resIT){
    if(projAngle<(*resIT).first) break;
  }
  // 
  double paraConst=(*resIT).second.first;
  double paraSlope=(*resIT).second.second;
  double error=paraSlope*pitch+paraConst;
  //
  return ( error/pitch );
}
//============================================================================
double VeloClusterPosition::errorOnPhi(const double projAngle,
                                       const double pitch) const
{
  debug()<< " ==> errorOnPhi() " <<endmsg;
  // pick up the right parametrization
  ResVector::const_iterator resIT=m_phiParaTable.begin();
  for( ; resIT!=m_phiParaTable.end(); ++resIT){
    if(projAngle<(*resIT).first) break;
  }
  double paraConst=(*resIT).second.first;
  double paraSlope=(*resIT).second.second;
  double error=paraSlope*pitch+paraConst;
  //
  return ( error/pitch );
}
//============================================================================
double VeloClusterPosition::projectedAngle() const
{
  debug()<< " ==> projectedAngle() " <<endmsg;
  //
  return ( m_projectedAngle/Gaudi::Units::degree );
}
//
