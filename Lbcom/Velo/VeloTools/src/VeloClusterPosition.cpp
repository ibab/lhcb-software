// $Id: VeloClusterPosition.cpp,v 1.13 2007-07-19 15:53:21 truf Exp $
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
  : GaudiTool(type, name, parent),
    m_isOutsideSensor ( false )
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
    LHCb::VeloChannelID centreChannel=cluster->channelID();
    myInfo.strip=centreChannel;
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
//TRuf July 19, 2007 temporary fix =====================================================
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
  // calculate fractional position
  Pair fractionalPair=fracPosLA(cluster);
  // inter-strip position translate into (0., 1.) range
  double isp=fractionalPair.first;
  LHCb::VeloChannelID centreChannel=cluster->channelID();
// fill partially the toolInfo structure
  anInfo.strip=centreChannel;
  anInfo.fractionalPosition=isp;
  unsigned int centreStrip=centreChannel.strip();

  // error estimate
  double error=0.;
  Gaudi::XYZVector trackdir = Gaudi::XYZVector(aDirection.first,aDirection.second,1.);
  //--------------------------------------------------------------------------
  // R sensor is easy, projected angle = theta
  //--------------------------------------------------------------------------
  if(sensor->isR()||(sensor->isPileUp())){ 
    const DeVeloRType* rSensor=dynamic_cast<const DeVeloRType*>(sensor);
    double rOfCluCentre=rSensor->rOfStrip(centreStrip, isp);
    double localPitch=rSensor->rPitch(rOfCluCentre);
    // for r sensors, this is a good approximation, sqrt(tx**2+ty**2)
    m_projectedAngle=trackdir.rho();
    error=errorOnR(m_projectedAngle/Gaudi::Units::rad, 
                   localPitch/Gaudi::Units::micrometer);
    // make an adhoc correction for the position, Velo Simulation bug in DC06 !
    // 10mu / rad 
    // anInfo.fractionalPosition = isp + m_projectedAngle*0.01/localPitch;                
  }
  //--------------------------------------------------------------------------
  // Phi sensor is more complicated 
  //--------------------------------------------------------------------------
  if(sensor->isPhi()){
    const DeVeloPhiType* phiSensor=dynamic_cast<const DeVeloPhiType*>(sensor);
   // transform global point to the local reference frame
    Gaudi::XYZPoint aLocalPoint=sensor->globalToLocal(aGlobalPoint);
    double radiusOnPhi=aLocalPoint.rho();
    // make velo trajectory
    std::auto_ptr<LHCb::Trajectory>  traj  = m_veloDet->trajectory(LHCb::LHCbID(centreChannel),isp);
    double r_e = traj->endPoint().rho();
    double r_b = traj->beginPoint().rho();
    double max_r = r_e;
    if (r_e<r_b){max_r = r_b;}
    double min_r = r_e;
    if (r_e>r_b){min_r = r_b;}
    if (radiusOnPhi > max_r){radiusOnPhi = max_r;}
    if (radiusOnPhi < min_r){radiusOnPhi = min_r;}
       
    double localPitch = phiSensor->phiPitch(radiusOnPhi);
    // this is a hack, but should be ok since sensors are never tilted so much that it matters
    double stereoAngle = acos(traj->direction(0.5).Dot(aGlobalPoint)/aGlobalPoint.rho());
    // for phi sensors, projection angle is diluted by stereo angle
    m_projectedAngle = trackdir.rho()*stereoAngle;
    error=errorOnR(m_projectedAngle/Gaudi::Units::rad,
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
//TRuf July 19, 2007 temporary fix =====================================================
double VeloClusterPosition::errorOnR(const double projAngle,
                                     const double pitch) const
{
  std::vector<double> m_params;
  m_params+=0.038, 0.00033, -25.52, 0.167, 1.447, 0.0069, 0.121, 0.00032;
  debug()<< " ==> errorOnR() " <<endmsg;
  double x = projAngle;
  double error;
  if (x < 0){x = -x;}
  // make cutoff at unphysical angles
  if (x > 0.4) {
    error = 0.28;
  } else {
   double p0 = m_params[0]+m_params[1]*pitch;
   double p1 = m_params[2]+m_params[3]*pitch;
   double p2 = m_params[4]+m_params[5]*pitch;
   double p3 = m_params[6]+m_params[7]*pitch;
   error=p0*sin(p1*x+p2)+p3; 
  }
  // make some fine tuning
  return ( 1.08 * error );
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
//============================================================================
void VeloClusterPosition::setOutsideFlag() const
{
  m_isOutsideSensor=true;
}
//============================================================================
void VeloClusterPosition::unsetOutsideFlag() const
{
  m_isOutsideSensor=false;
}
//============================================================================
bool VeloClusterPosition::outsideFlag() const
{
  return ( m_isOutsideSensor );
}
//============================================================================
toolInfo VeloClusterPosition::position(const LHCb::VeloCluster* cluster,
                                       const LHCb::StateVector& aState) const
{
  debug()<< " ==> position (VectorState) " <<endmsg;
  unsigned int sensorNumber=cluster->channelID().sensor();
  const DeVeloSensor* sensor=m_veloDet->sensor(sensorNumber);
  double z=sensor->z();
  // build space point in global ref. frame
  Gaudi::XYZPoint aPoint(aState.x(), aState.y(), z);
  // build state pair
  Pair aDirection;
  aDirection.first=aState.tx();
  aDirection.second=aState.ty();
  return ( position(cluster, aPoint, aDirection) );
}
//
