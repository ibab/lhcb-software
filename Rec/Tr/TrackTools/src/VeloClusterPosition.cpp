// Include files

// stl
#include <vector>
#include <array>

// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// Math
#include "LHCbMath/LHCbMath.h"
#include "GaudiKernel/Point3DTypes.h"

// Velo
#include "Kernel/VeloChannelID.h"
#include "VeloDet/DeVelo.h"
#include "Event/VeloCluster.h"

// local
#include "VeloClusterPosition.h"

// boost
#include <boost/assign/std/vector.hpp>

#include "vdt/sin.h"
#include "vdt/cos.h"
#include "vdt/asin.h"
#include "vdt/atan.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VeloClusterPosition
//
// 2005-09-30 : Tomasz Szumlak 
// 
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( VeloClusterPosition )

using namespace boost::assign;

typedef IVeloClusterPosition::toolInfo toolInfo;
typedef IVeloClusterPosition::Direction Direction;
typedef std::pair<double, double> Pair;

static const Gaudi::XYZVector ZVersor(0., 0., 1.);

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VeloClusterPosition::VeloClusterPosition(const std::string& type,
                               const std::string& name,
                               const IInterface* parent)
  : GaudiTool(type, name, parent),
    m_veloDet ( 0 ),
    m_defaultResolution ( ),
    m_p0Values ( ),
    m_p1Values ( ),
    m_projAngles ( 24, 0. ),
    m_p0( 0 ),
    m_p1( 0 ),
    m_minAngle ( 0. ),
    m_maxAngle ( 0. ),
    m_trackDir ( ),
    m_gloPoint ( ),
    m_fracPos ( 0. ),
    m_corrFactor ( 1.0 ),
    m_condPath ( "Conditions/Calibration/Velo" )
{
  declareInterface<IVeloClusterPosition>(this);
  //
  declareProperty("CalculateExactProjAngle", m_calculateExactProjAngle=false);
  declareProperty("ErrorParaType", m_errorParaType=PITCH_PARA);
  declareProperty("SplineType", m_splineType="Cspline");
}
//=============================================================================
// Destructor
//=============================================================================
VeloClusterPosition::~VeloClusterPosition(){}

//=============================================================================
StatusCode VeloClusterPosition::initialize()
{
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug()<< " ==> initialize() " <<endmsg;
  //
  StatusCode sc=GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;
  // get Velo detector
  m_veloDet=getDet<DeVelo>( DeVeloLocation::Default );

  IUpdateManagerSvc* mgrSvc=svc<IUpdateManagerSvc>("UpdateManagerSvc", true);
  // use SmartDataPtr to test existence, did not exist for 2009, mc09, mc10
  SmartDataPtr<Condition> cond(detSvc(), (m_condPath+"/VeloErrorParam"));
  
  if(cond)
  {
    
    mgrSvc->registerCondition(this, m_condPath,
                              &VeloClusterPosition::i_cacheConditions);
    StatusCode mgrSvcStatus=mgrSvc->update(this);
    mgrSvcStatus.ignore();

  }else{    
    
    info()<< " --> Error para condition not found! "<<endmsg;
    info()<< " --> The tool Will use the default values! " <<endmsg;

    
    m_p0Values+=-2.5, -3.591, -3.430, -4.112, -4.964, -5.705, -6.301, -7.065,
      -7.353, -7.055, -6.168, -4.814, -3.075, -1.067, 0.798, 2.753,
      4.446, 5.116, 6.411, 7.124, 7.683, 7.573, 9.946, 10.97;
    
    m_p1Values+=0.29, 0.29, 0.28, 0.28, 0.28, 0.27, 0.27, 0.269,
      0.239, 0.23, 0.215, 0.18, 0.15, 0.11, 0.09, 0.06,
      0.04, 0.03, 0.0226, 0.0154, 0.0154, 0.0235, 0.0008, -0.0079;

    m_errAnglePara+=0.038, 0.00033, -25.52, 0.167, 1.447, 0.0069, 0.121, 0.00032;

    m_defaultResolution+=-6.301, 0.27;

  }
  
  // assign angles in rad from 0 to 400 mrad
  auto it=m_projAngles.begin();
  int value=0;
  while(it!=m_projAngles.end()){
    (*it)=static_cast<double>(value*Gaudi::Units::pi/RAD_TO_DEG);
    ++it;
    ++value;
  }
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) {
    debug()<< " Parametrisation proj angles bins: " << m_projAngles.size() <<endmsg;
    debug()<< (m_projAngles.size()) << " vs. " << (m_p0Values.size()) <<endmsg;
  }
  assert(m_projAngles.size()==m_p0Values.size());
  assert(m_projAngles.size()==m_p1Values.size());
  

  m_minAngle=m_projAngles.front();
  m_maxAngle=m_projAngles.back();
  m_p0=new GaudiMath::SimpleSpline(m_projAngles, m_p0Values, splineType());
  m_p1=new GaudiMath::SimpleSpline(m_projAngles, m_p1Values, splineType());

  //
  return ( StatusCode::SUCCESS );
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode VeloClusterPosition::finalize() {

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Finalize" << endmsg;
  delete m_p0; m_p0=0;
  delete m_p1; m_p0=0;

  return ( GaudiTool::finalize() );  // must be called after all other actions
}
//=============================================================================
// position calculation, depending on sensor type the clusterPos
// is radial or angular variable
//=============================================================================
double VeloClusterPosition::meanResolution(const double& pitch) const
{ 
  if( msgLevel(MSG::DEBUG) ) debug()<< " ==> resolution() " <<endmsg;
  //
  double p_0=m_defaultResolution[0];
  double p_1=m_defaultResolution[1];
  //
  double resolution=p_0+(p_1*pitch);
  //
  return resolution;
}
//=========================================================================
double VeloClusterPosition::fracPosLA(const LHCb::VeloCluster* cluster) const
{
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug()<< " ==> fracPosLA()" <<endmsg;
  //
  double centre=0., sum=0., fractionalPos;
  int intDistance=0;
  std::array<LHCb::VeloChannelID, 16> chanCont; // assume cluster size <= 16 (always true!)
  unsigned stripNumber=std::min(cluster->size(), unsigned(chanCont.size()));
  {
    // avoid creating a std::vector on the fly - this is stolen from
    // LHCb::VeloCluster::channels() and adapted to fit here
    auto it = std::begin(chanCont);
    LHCb::VeloChannelID first = cluster->firstChannel();
    for (unsigned i = 0; i < stripNumber; ++i, ++it) {
      *it = LHCb::VeloChannelID(first.sensor(),first.strip()+i,first.type());
    }
  }
  //
  const DeVeloSensor* sens=m_veloDet->sensor(chanCont[0].sensor());
  for(unsigned i=0; i<stripNumber; ++i){
    StatusCode sc=sens->channelDistance(chanCont[0],chanCont[i],intDistance);
    sc.ignore(); // channels in a cluster are always in same sensor!
    centre+=static_cast<float>(intDistance)*cluster->adcValue(i);
    sum+=cluster->adcValue(i);
  }
  //
  if(stripNumber==1){
    fractionalPos=0.0;
  }else{
    centre=centre/sum;
    intDistance=int(LHCb::Math::round(centre));
    fractionalPos=centre-intDistance;
  }
  // redefine fractional position accordingly - tell1 raw bank format
  if(fractionalPos<0.) fractionalPos+=1;
  // Tell1 feature related with the resolution
  if(fractionalPos>0.9376) fractionalPos=0;
  //
  if(fabs(fractionalPos-cluster->interStripFraction())>0.5){
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug()<< " clu size: " << stripNumber
          << " strip adcs: " <<endmsg;
    for(unsigned str=0; str<stripNumber; str++){
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug()<< " adc[ " << str << "] = " << (cluster->adcValue(str)) <<endmsg;
    }
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << " frac pos tool: " << fractionalPos
           << " frac pos clu: " << cluster->interStripFraction() <<endmsg;
  }
  return  ( fractionalPos );
}

//=========================================================================
double VeloClusterPosition::fracPosLA(const LHCb::VeloLiteCluster* cluster) const
{
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug()<< " ==> fracPosLA() Lite Cluster" <<endmsg;
  return  ( cluster->interStripFraction() );
}

//=========================================================================
toolInfo VeloClusterPosition::position(const LHCb::VeloCluster* cluster) const
{
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug()<< " ==> position(cluster) " <<endmsg;
  toolInfo myInfo = position(cluster->channelID(),fracPosLA(cluster));
  myInfo.clusterSize = cluster->size();
  return myInfo;
}

//=========================================================================
toolInfo VeloClusterPosition::position(const LHCb::VeloLiteCluster* cluster) const
{
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug()<< " ==> position(liteCluster)  " <<endmsg;
  toolInfo myInfo = position(cluster->channelID(),fracPosLA(cluster));
  myInfo.clusterSize = cluster->pseudoSize();
  return myInfo;
}

toolInfo VeloClusterPosition::position(const LHCb::VeloChannelID &centreChannel,
				       const double &fractionalPos) const{
  toolInfo myInfo;
  myInfo.clusterSize=0.; // set by wrapper functions calling this function
  // calculate fractional position in units of 'strip'
  double errorPos=0., pitch=0.;
  const DeVeloSensor* sens=m_veloDet->sensor(centreChannel.sensor());
  if(sens==0){
    Error("No valid pointer to sensor").ignore();
    myInfo.strip=LHCb::VeloChannelID(0);
    myInfo.fractionalPosition=0.;
    myInfo.fractionalError=0.;
    return ( myInfo );
  }
  if(sens->isR()||sens->isPileUp()){
    const DeVeloRType* rSens=static_cast<const DeVeloRType*>(sens);
    pitch=rSens->rPitch(centreChannel.strip(), fractionalPos);
    errorPos=meanResolution(pitch/Gaudi::Units::micrometer);
    errorPos/=(pitch/Gaudi::Units::micrometer);
  }else if(sens->isPhi()){
    const DeVeloPhiType* phiSens=static_cast<const DeVeloPhiType*>(sens);
    double minRadius=0., maxRadius=0.;
    unsigned int zoneOfCluster=0;
    zoneOfCluster=sens->zoneOfStrip(centreChannel.strip());
    minRadius=sens->rMin(zoneOfCluster);
    maxRadius=sens->rMax(zoneOfCluster);
    double meanRadius=0.5*(maxRadius+minRadius);
    pitch=phiSens->phiPitch(meanRadius);
    errorPos=meanResolution(pitch/Gaudi::Units::micrometer);
    errorPos/=(pitch/Gaudi::Units::micrometer);
  }
  //
  myInfo.strip=centreChannel;
  myInfo.fractionalPosition=fractionalPos;
  myInfo.fractionalError=errorPos;
  //
  return ( myInfo );
}  

//============================================================================
toolInfo VeloClusterPosition::position(const LHCb::VeloCluster* cluster,
                                       const Gaudi::XYZPoint& aGlobalPoint,
                                       const Direction& aDirection) const
{
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << " ==> position(cluster, point, direction) " <<endmsg;
  toolInfo myInfo = position(cluster->channelID(),fracPosLA(cluster), aGlobalPoint, aDirection);
  myInfo.clusterSize = cluster->size();
  return myInfo;
}

//============================================================================
toolInfo VeloClusterPosition::position(const LHCb::VeloLiteCluster* cluster,
                                       const Gaudi::XYZPoint& aGlobalPoint,
                                       const Direction& aDirection) const
{
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << " ==> position(litecluster, point, direction) " <<endmsg;
  toolInfo myInfo = position(cluster->channelID(),fracPosLA(cluster), aGlobalPoint, aDirection);
  myInfo.clusterSize = cluster->pseudoSize();
  return myInfo;
}

//============================================================================
toolInfo VeloClusterPosition::position(const LHCb::VeloChannelID &centreChan,
				       const double & fracPos,
                                       const Gaudi::XYZPoint& aGlobalPoint,
                                       const Direction& aDirection) const
{
  // this struct will be returned as output
  toolInfo anInfo;
  anInfo.clusterSize=0.; // set in wrapper calling functions
  // get information about sensor form passed cluster
  const DeVeloSensor* sensor=m_veloDet->sensor(centreChan.sensor());
  if(sensor==0){
    Error("No valid pointer to sensor").ignore();
    anInfo.strip=LHCb::VeloChannelID(0);
    anInfo.fractionalPosition=0.;
    anInfo.fractionalError=0.;
    return ( anInfo );
  }
  // fill partially the toolInfo structure
  anInfo.strip=centreChan;
  anInfo.fractionalPosition=fracPos;
  // error estimate
  double error=0.;
  m_trackDir=Gaudi::XYZVector(aDirection.first, aDirection.second, 1.);
  m_gloPoint=aGlobalPoint;
  //-- return projected angle and local pitch
  Pair aPair=projectedAngle(sensor, centreChan);
  m_projectedAngle=aPair.first;
  double pitch=aPair.second;
  //-- error estimate
  error=errorEstimate(m_projectedAngle/Gaudi::Units::rad,
                      pitch/Gaudi::Units::micrometer);
  anInfo.fractionalError=error;
  //
  return ( anInfo );
}
//============================================================================
double VeloClusterPosition::angleOfTrack(const Direction& localSlopes,
                                  Gaudi::XYZVector& parallel2Track) const
{
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << " ==> trackAngle() " <<endmsg;
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
  double alphaOfInsertion=vdt::fast_acos(cosOfInsertion);
  //
  return ( alphaOfInsertion );
}
//============================================================================
double VeloClusterPosition::errorEstimate(const double projAngle,
                                          const double pitch) const
{
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << " ==> errorEstimate() " <<endmsg;
  //
  double angle=projAngle;
  double error=0.;
  //-- make cutoff at unphysical angles
  if(angle<m_minAngle){ angle=-angle; }
  //-- pitch projection of the residuals
  if(m_errorParaType==PITCH_PARA){
    if(angle>m_maxAngle){ angle=m_maxAngle; }
    double p0=m_p0->eval(angle);
    double p1=m_p1->eval(angle);
    error=m_corrFactor*(p0+p1*pitch);
    error/=pitch;
  }
  //-- this set of parametrizations use angle projection
  if(m_errorParaType==ANGLE_PARA){
    if(angle>m_maxAngle){
      error=0.28;
    }else{
     double p0=m_errAnglePara[0]+m_errAnglePara[1]*pitch;
     double p1=m_errAnglePara[2]+m_errAnglePara[3]*pitch;
     double p2=m_errAnglePara[4]+m_errAnglePara[5]*pitch;
     double p3=m_errAnglePara[6]+m_errAnglePara[7]*pitch;
    //-- make some fine tuning - difference between sigma snd RMS
     error=1.08*(p0*vdt::fast_sin(p1*angle+p2)+p3); 
    }
  }
  //
  return ( error );
}
//============================================================================
double VeloClusterPosition::projectedAngle() const
{
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << " ==> projectedAngle() " << endmsg;
  //
  return ( m_projectedAngle/Gaudi::Units::degree );
}
//============================================================================
toolInfo VeloClusterPosition::position(const LHCb::VeloCluster* cluster,
                                       const LHCb::StateVector& aState) const
{
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug()<< " ==> position (cluster,VectorState) " <<endmsg;
  unsigned int sensorNumber=cluster->channelID().sensor();
  const DeVeloSensor* sensor=m_veloDet->sensor(sensorNumber);
  double z=sensor->z();
  // build space point in global ref. frame
  Gaudi::XYZPoint aPoint(aState.x(), aState.y(), z);
  // build state pair
  Direction aDirection;
  aDirection.first=aState.tx();
  aDirection.second=aState.ty();
  toolInfo myInfo = position(cluster->channelID(),fracPosLA(cluster), aPoint, aDirection);
  myInfo.clusterSize = cluster->size();
  return myInfo;
}

toolInfo VeloClusterPosition::position(const LHCb::VeloLiteCluster* cluster,
                                       const LHCb::StateVector& aState) const
{
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug()<< " ==> position (LiteCluster,VectorState) " <<endmsg;
  unsigned int sensorNumber=cluster->channelID().sensor();
  const DeVeloSensor* sensor=m_veloDet->sensor(sensorNumber);
  double z=sensor->z();
  // build space point in global ref. frame
  Gaudi::XYZPoint aPoint(aState.x(), aState.y(), z);
  // build state pair
  Direction aDirection;
  aDirection.first=aState.tx();
  aDirection.second=aState.ty();
  toolInfo myInfo = position(cluster->channelID(),fracPosLA(cluster), aPoint, aDirection);
  myInfo.clusterSize = cluster->pseudoSize();
  return myInfo;
}

//============================================================================
GaudiMath::Interpolation::Type VeloClusterPosition::splineType() const
{
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug()<< " splineType() " <<endmsg;
  //
  GaudiMath::Interpolation::Type aType;
  using namespace GaudiMath::Interpolation;
  if(m_splineType=="Cspline"){ 
    aType=Cspline;
  }else if(m_splineType=="Linear"){
    aType=Linear;
  }else if(m_splineType=="Polynomial"){
    aType=Polynomial;
  }else{
    aType=Linear;
  }
  //
  return aType;
}
//============================================================================
Pair VeloClusterPosition::projectedAngle(const DeVeloSensor* sensor,
                          const LHCb::VeloChannelID centreChan) const
{
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug()<< " ==> projectedAngle(sensor) " <<endmsg;
  //-- returned pair consists of projected angle and local pitch
  //-- both values are needed to estimate error
  double projectedAngle=0., localPitch=0.;
  Pair locPair;
  // transform global point to the local reference frame
  Gaudi::XYZPoint aLocPoint=sensor->globalToLocal(m_gloPoint);
  unsigned int centreStrip=centreChan.strip();
  //----------------
  //   R sensor
  //----------------
  if(sensor->isR()||(sensor->isPileUp())){ 
    const DeVeloRType* rSensor=static_cast<const DeVeloRType*>(sensor);
    double rOfCluCentre=rSensor->rOfStrip(centreStrip, m_fracPos);
    localPitch=rSensor->rPitch(rOfCluCentre);
    if(m_calculateExactProjAngle){  //-- exact calculations for R
      Gaudi::XYZVector parallel2Track;
      Direction locDir=localTrackDirection(m_trackDir, sensor);
      //-- track angle
      double alphaOfTrack=angleOfTrack(locDir, parallel2Track);
      //-- vector normal to the strip - sensor type specific
      Gaudi::XYZVector perp2RStrip(aLocPoint.x(), aLocPoint.y(), 0.);
      double cosTrackOnNormal=parallel2Track.Dot(perp2RStrip.Unit());
      //-- projection of track on normal to local strip
      double trackOnNormal=fabs(cosTrackOnNormal);
      //-- projection of track on Z axis
      double trackOnZ=vdt::fast_cos(alphaOfTrack);
      projectedAngle=vdt::fast_atan(trackOnNormal/trackOnZ);
      locPair=std::make_pair(projectedAngle, localPitch);
    }else{                          //-- approximated calculations for R
      // for R sensor, this is a good approximation, sqrt(tx**2+ty**2)
      projectedAngle=m_trackDir.rho();
      locPair=std::make_pair(projectedAngle, localPitch);
    }
  }
  //----------------
  //   Phi sensor
  //----------------
  if(sensor->isPhi()){
    const DeVeloPhiType* phiSensor=static_cast<const DeVeloPhiType*>(sensor);
    double radiusOnPhi=aLocPoint.rho();
    // make velo trajectory
    auto traj= m_veloDet->trajectory(LHCb::LHCbID(centreChan), m_fracPos);
    double trajEnd=traj->endPoint().rho();
    double trajBeg=traj->beginPoint().rho();
    double rMax=trajEnd;
    if(trajEnd<trajBeg) {rMax=trajBeg;}
    double rMin=trajEnd;
    if(trajEnd>trajBeg) {rMin=trajBeg;}
    if(radiusOnPhi>rMax) {radiusOnPhi=rMax;}
    if(radiusOnPhi<rMin) {radiusOnPhi=rMin;}       
    localPitch=phiSensor->phiPitch(radiusOnPhi);
    if(m_calculateExactProjAngle){      //-- exact calculations for Phi
      Gaudi::XYZVector parallel2Track;
      std::pair<Gaudi::XYZPoint, Gaudi::XYZPoint> strip;
      strip=phiSensor->localStripLimits(centreStrip);
      Gaudi::XYZVector perp2PhiStrip((strip.first.y()-strip.second.y()),
                                     (strip.second.x()-strip.first.x()), 0.);
      Direction locDir=localTrackDirection(m_trackDir, sensor);
      double alphaOfTrack=angleOfTrack(locDir, parallel2Track);
      double cosTrackOnNormal=parallel2Track.Dot(perp2PhiStrip.Unit());
      double trackOnNormal=fabs(cosTrackOnNormal);
      double trackOnZ=vdt::fast_cos(alphaOfTrack);
      projectedAngle=vdt::fast_atan(trackOnNormal/trackOnZ);
      locPair=std::make_pair(projectedAngle, localPitch);
    }else{                              //-- approximated calculations for Phi
      // this is an approximation, but should be ok since sensors are never 
      // tilted so much that it matters. Make sure we just use the xy-plane:
      Gaudi::XYZVector stripdir = traj->direction(0.5).Unit() ;
      double cosangle = (stripdir.x()*m_gloPoint.x() + stripdir.y()*m_gloPoint.y())/m_gloPoint.rho() ;
      double stereoAngle = std::abs(cosangle)<1 ? vdt::fast_acos( cosangle ) : 0 ;

      // for phi sensors, projection angle is diluted by stereo angle
      projectedAngle=m_trackDir.rho()*stereoAngle;
      locPair=std::make_pair(projectedAngle, localPitch);
    }
  }
  return ( locPair );
}
//============================================================================
Direction VeloClusterPosition::localTrackDirection(
          const Gaudi::XYZVector& gloTrackDir,
          const DeVeloSensor* sensor) const
{
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug()<< " ==> localTrackDirection " <<endmsg;
  //-- translate global slopes into local
  using namespace std;
  Direction locTrackDir;
  if(sensor->isLeft()&&(!sensor->isDownstream())){
    locTrackDir=make_pair(gloTrackDir.x(), gloTrackDir.y());
  }else if(sensor->isLeft()&&sensor->isDownstream()){
    locTrackDir=make_pair((-1)*gloTrackDir.x(), gloTrackDir.y());
  }else if(sensor->isRight()&&sensor->isDownstream()){
    locTrackDir=make_pair(gloTrackDir.x(), (-1)*gloTrackDir.y());
  }else if(sensor->isRight()&&(!sensor->isDownstream())){
    locTrackDir=make_pair((-1)*gloTrackDir.x(), (-1)*gloTrackDir.y());
  }
  return ( locTrackDir );
}
//============================================================================
StatusCode VeloClusterPosition::i_cacheConditions()
{
  if(UNLIKELY( msgLevel(MSG::DEBUG) )) debug()<< " --> i_cacheCoditions() " <<endmsg;
  
  // connect to the LHCBCOND/SIMCOND and fetch the error param conditions
  std::string condName=m_condPath+"/VeloErrorParam";
  Condition* cond=getDet<Condition>(condName);
  if(NULL!=cond)
  {
    
    m_p0Values=cond->param<std::vector<double> >("ConstParams");
    m_p1Values=cond->param<std::vector<double> >("DirectionParams");
    //    std::vector<double>::iterator it=m_p1Values.begin();
    m_errAnglePara=cond->param<std::vector<double> >("AngleErrorParams");
    m_defaultResolution=cond->param<std::vector<double> >("AverageVeloError");

    info()<< " --> Tool configured using the Cond DB " <<endmsg;

  }

  return ( StatusCode::SUCCESS );
}
//
