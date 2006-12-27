// $Id: VeloClusterPosition.cpp,v 1.5 2006-12-27 17:47:24 szumlat Exp $
// Include files

// stl
#include <vector>

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// from LHCbDefinitions
#include "Kernel/LHCbMath.h"
#include "Kernel/Point3DTypes.h"

// Velo
#include "VeloDet/DeVelo.h"
#include "Event/VeloCluster.h"

// local
#include "VeloClusterPosition.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VeloClusterPosition
//
// 2005-09-30 : Tomasz Szumlak 
// 
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( VeloClusterPosition );

typedef std::pair<double, double> Pair;
typedef LHCb::SiPositionInfo<LHCb::VeloChannelID> toolInfo;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VeloClusterPosition::VeloClusterPosition(const std::string& type,
                               const std::string& name,
                               const IInterface* parent)
  : GaudiTool(type, name, parent)
{
  m_veloDet = getDet<DeVelo>( DeVeloLocation::Default );
  m_defaultRSensorResolution.push_back(-3.23);
  m_defaultRSensorResolution.push_back(0.23);
  m_defaultPhiSensorResolution.push_back(-3.);
  m_defaultPhiSensorResolution.push_back(0.28);
  m_defaultEtaPar.push_back(0.5);
  m_defaultEtaPar.push_back(0.25);
  m_defaultEtaPar.push_back(-8.13);
  //
  declareInterface<IVeloClusterPosition>(this);
  declareProperty("PrintInfo", m_printInfo = false );
  declareProperty("UseWeightedMean", m_useWeightedMean = true );
  declareProperty("UseEtaFit", m_useEtaFit = false );
  declareProperty("ParaTypes", m_paraTypes);
  declareProperty("DefaultRSensorResolution", m_defaultRSensorResolution);
  declareProperty("DefaultPhiSensorResolution", m_defaultPhiSensorResolution);
  declareProperty("ResParameters", m_resParameters);
  declareProperty("DefaultEtaPar", m_defaultEtaPar);
}
//=============================================================================
// Destructor
//=============================================================================
VeloClusterPosition::~VeloClusterPosition() {};
//=============================================================================
// position calculation, depending on sensor type the clusterPos
// is radial or angular variable
//=============================================================================
toolInfo VeloClusterPosition::weightedMeanPos(
                         const LHCb::VeloCluster* cluster,
                         Pair& userInfo) const
{
  debug()<< " ==> weightedMeanPos() " <<endmsg;
  //
  Pair tempPair;
  Pair fPair;
  toolInfo myInfo;
  int intDistance=0;
  double fractionalPos=0.;
  // calculate fractional position in units of 'strip'
  fPair=fractionalPosMean(cluster);
  fractionalPos=fPair.first;
  intDistance=static_cast<int>(fPair.second);
  // return floored nearest strip to the cluster centre - exactly as TELL1 
  LHCb::VeloChannelID centreStripID=cluster->channelID();
  //
    if(centreStripID!=0){
      posAndError(cluster, centreStripID, fractionalPos, userInfo, tempPair);
      // fill the new wrapper object
      myInfo.strip=centreStripID;
      myInfo.fractionalPosition=fractionalPos;
      myInfo.fractionalError=tempPair.second;
      //
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
toolInfo VeloClusterPosition::etaFitPos(
                         const LHCb::VeloCluster* cluster,
                         Pair& userInfo) const
{
  debug()<< " ==> VeloClusterPosition::etaFitPos" <<endmsg;
  //
  Pair fPair;
  toolInfo myInfo;
  Pair tempPair;
  int intDistance=0;
  double fractionalPos=0.;
  //
  double alphaOfTrack=userInfo.first;
  fPair=fractionalPosEta(cluster, alphaOfTrack);
  fractionalPos=fPair.first;
  intDistance=int(fPair.second);
  LHCb::VeloChannelID intDistanceID;
  const DeVeloSensor* sens=m_veloDet->sensor(cluster->firstChannel().sensor());
  StatusCode sc=sens->neighbour(cluster->firstChannel(), intDistance, intDistanceID);
  if(sc){
    posAndError(cluster, intDistanceID, fractionalPos, userInfo, tempPair);
    // fill the wrapper object
    myInfo.strip=intDistanceID;
    myInfo.fractionalPosition=fractionalPos;
    myInfo.fractionalError=tempPair.second;
    //
    return ( myInfo );
  }else{
    info()<< "Given cluster is invalid!" <<endmsg;
    // fill dummy object for invalid cluster
    myInfo.strip=LHCb::VeloChannelID(0);
    myInfo.fractionalPosition=0.;
    myInfo.fractionalError=0.;
    //
    return ( myInfo );
  }
}
//=========================================================================
double VeloClusterPosition::resolution(const double& pitch,
                                       const Pair& resInfo) const
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
  int sensType=static_cast<int>(resInfo.second);
  //
  if(!(resInfo.first)){
    // use default resolution parametrisation for mean LHCb angle
    switch(sensType){
    case 0: resolution=p_Phi_0+p_Phi_1*pitch; break;
    case 1: resolution=p_R_0+p_R_1*pitch; break;
    default: resolution=p_R_0+p_R_1*pitch;
    }
    //
    return (resolution);
  }else{
    // still no good parametrisations available!
    return (0.);
  }  
}
//=========================================================================
toolInfo VeloClusterPosition::position(
                         const LHCb::VeloCluster* cluster,
                         Pair& userInfo) const
{
  debug()<< " ==> VeloClusterPosition::position" <<endmsg;
  
  if(m_useWeightedMean){
    // if user do not provide required info fill the container
    if(0==userInfo.first){
      userInfo=std::make_pair(0., 0.);
    }
    return (weightedMeanPos(cluster, userInfo));
  }else if(m_useEtaFit){
    // if user do not provide required info fill the container 
    if(0==userInfo.first){
      userInfo=std::make_pair(0., 0.);
    }
    if((cluster->size()==1)||(cluster->size()>2)){
      return (weightedMeanPos(cluster, userInfo));
    }else{
      return (etaFitPos(cluster, userInfo));
    }
  }else{
    info()<< " ==> The Tool has been unproperly configured!" <<endmsg;
    // create and fill dummy object if something wrong with ini.
    toolInfo myInfo;
    myInfo.strip=LHCb::VeloChannelID(0);
    myInfo.fractionalPosition=0.;
    myInfo.fractionalError=0.;
    //
    return ( myInfo );
  } 
}
//=========================================================================
Pair VeloClusterPosition::fractionalPosMean(
                         const LHCb::VeloCluster* cluster) const
{
  debug()<< " ==> VeloClusterPosition::fractionalPosMean" <<endmsg;
  //
  Pair temp;
  double centre=0., sum=0., fractionalPos;
  int intDistance=0;
  int stripNumber=cluster->size();
  std::vector<LHCb::VeloChannelID> chanCont=cluster->channels();
  StatusCode sc;
  //
  const DeVeloSensor* sens=m_veloDet->sensor(chanCont[0].sensor());
  for(int i=0; i<stripNumber; i++){
    sc=sens->channelDistance(chanCont[0],chanCont[i],intDistance);
    centre+=static_cast<double>(intDistance)*cluster->adcValue(i);
    sum+=cluster->adcValue(i);
  }
  //
  if(stripNumber==1){
    fractionalPos=0.0;
  }else{
    centre/=sum;
    intDistance=static_cast<int>(LHCbMath::round(centre));
    fractionalPos=centre-intDistance;
  }
  if(fractionalPos<0) fractionalPos+=1;
  temp=std::make_pair(fractionalPos, intDistance);
  //  
  return  (temp);
}
//
Pair VeloClusterPosition::fractionalPosEta(
                         const LHCb::VeloCluster* cluster,
                         double alphaOfTrack) const
{
  debug()<< " ==> VeloClusterPosition::fractionalPosMean" <<endmsg;
  //
  Pair temp;
  Pair localPair;
  //
  localPair=etaFrac(cluster);
  double etaVariable=localPair.first;
  double biasedEta=etaVariable-.5;
  double biasedEta3=biasedEta*biasedEta*biasedEta;
  //
  double p0=m_defaultEtaPar[0];
  double p1=m_defaultEtaPar[1];
  double p2=m_defaultEtaPar[2];
  double etaCentre=0.;
  if(!alphaOfTrack){
    etaCentre=p0+p1*biasedEta-p2*(biasedEta3);
  }else{
    // new tuning not availabe yet
  }
  //
  int intDistance=int(LHCbMath::round(etaCentre));
  double fractionalPos=etaCentre-intDistance;
  //  
  temp=std::make_pair(fractionalPos, intDistance);
  // 
  return (temp);
}
//==============================================================================
void VeloClusterPosition::posAndError(
                     const LHCb::VeloCluster* cluster,
                     const LHCb::VeloChannelID& intDistanceID,
                     const double& fractionalPos,
                     Pair& userInfo, Pair& toolInfo) const
{ 
  debug()<< "==> VeloClusterPosition::posAndError" <<endmsg;
  //  
  unsigned int sensor=cluster->channelID().sensor();
  const DeVeloSensor* sens=m_veloDet->sensor(sensor);
  double pitch=0., clusterPos=0., errorPos=0., rOfPhiCluster=0.;
  double alphaOfTrack=userInfo.first;
  Pair resInfo=std::make_pair(alphaOfTrack, 0.);
  //
  if(sens->isR()||sens->isPileUp()){
    const DeVeloRType* rSens=dynamic_cast<const DeVeloRType*>(sens);
    clusterPos=rSens->rOfStrip(intDistanceID.strip(), fractionalPos);
    if(m_printInfo)
      info()<< " ==> The given cluster is on RType sensor" <<endmsg;
    clusterPos=clusterPos/Gaudi::Units::cm;
    pitch=rSens->rPitch(intDistanceID.strip(), fractionalPos);
    userInfo.second=1.; // RType
    errorPos=resolution(pitch/Gaudi::Units::micrometer, resInfo);
    errorPos=errorPos/(pitch/Gaudi::Units::micrometer);
    // return values for cluster position and position errorPos
    toolInfo=std::make_pair(clusterPos, errorPos);
    //
      if(m_printInfo){
        info()<< "cluster pos: " << clusterPos <<endmsg;
      }
  }
  else if(sens->isPhi()){
    const DeVeloPhiType* phiSens=dynamic_cast<const DeVeloPhiType*>(sens);
    if(!(userInfo.first+userInfo.second)){
      double minRadius=0., maxRadius=0.;
      unsigned int zoneOfCluster=0;
      zoneOfCluster=sens->zoneOfStrip(intDistanceID.strip());
      minRadius=sens->rMin(zoneOfCluster);
      maxRadius=sens->rMax(zoneOfCluster);
      double meanRadius=0.5*(maxRadius+minRadius);
      double frac=fractionalPos;
      clusterPos=phiSens->phiOfStrip(intDistanceID.strip(), frac, meanRadius);
      double phiOf=phiSens->phiOfStrip(intDistanceID.strip(), 0.0, meanRadius);
      debug()<< "phi: " << phiOf/Gaudi::Units::degree << ", " << frac <<endmsg;
      //
      if(m_printInfo)
        info()<< " ==> The given cluster is on PhiType sensor" <<endmsg;
      clusterPos=clusterPos/Gaudi::Units::degree;
      //
      if(m_printInfo){
        info()<< "cluster pos: " << clusterPos <<endmsg;
        info()<< "mean radius: " << meanRadius/Gaudi::Units::cm <<endmsg;
      }
      //
      pitch=phiSens->phiPitch(meanRadius);
      resInfo.second=0.; // phi type
      errorPos=resolution(pitch/Gaudi::Units::micrometer, resInfo);
      double errorRad=errorPos/(meanRadius/Gaudi::Units::micrometer);
      double errorDeg=errorRad/Gaudi::Units::degree;
      if(m_printInfo){
        info()<< "Error [rad] " << errorRad
              << ", error [deg] " << errorDeg <<endmsg;
      }
      // error as a fraction of the strip distance
      errorPos/=(pitch/Gaudi::Units::micrometer);
      //
      toolInfo=std::make_pair(clusterPos, errorPos);
    }else{
      rOfPhiCluster=userInfo.second;
      clusterPos=phiSens->phiOfStrip(intDistanceID.strip(), fractionalPos, rOfPhiCluster);
      if(m_printInfo)
        info()<< " ==> The given cluster is on PhiType sensor" <<endmsg;
      //
      clusterPos=clusterPos/Gaudi::Units::degree;
      pitch=phiSens->phiPitch(rOfPhiCluster);
      resInfo.second=0.; //phi type
      errorPos=resolution(pitch/Gaudi::Units::micrometer, resInfo);
      double errorRad=errorPos/(rOfPhiCluster/Gaudi::Units::micrometer);
      double errorDeg=errorRad/Gaudi::Units::degree;
      if(m_printInfo){
        info()<< " Error [rad] " << errorRad
              << ", errorDeg [deg] " << errorDeg <<endmsg;
      }
      errorPos/=(pitch/Gaudi::Units::micrometer);
      //
      toolInfo=std::make_pair(clusterPos, errorPos);
    }
  }
}
//=========================================================================
Pair VeloClusterPosition::etaFrac(const LHCb::VeloCluster* cluster,
                                 double fracPosTrue) const
{
  //
  debug()<< " ==> etaFrac() " <<endmsg;
  //
  double chargeL=0., chargeR=0., frac=0.;
  bool adcGreater=true;
  Pair temp;
  //
  double first=cluster->adcValue(0);
  double second=cluster->adcValue(1);
  if(first<second) adcGreater=false;
  if(first>second){
    if(adcGreater){
      frac=fracPosTrue;
      chargeL=first;
      chargeR=second;
    }else{
      frac=fracPosTrue;
      chargeL=second;
      chargeR=first;
    }
  }else{
    if(adcGreater){
      frac=1+fracPosTrue;
      chargeL=second;
      chargeR=first;
    }else{
      frac=1+fracPosTrue;
      chargeL=first;
      chargeR=second;
    }
  }
  //
  double etaVariable=chargeL/(chargeR+chargeL);
  temp=std::make_pair(etaVariable, frac);
  //
  return (temp);
}
//=========================================================================
void VeloClusterPosition::createResParTable()
{
  //
  debug()<< " ==> createParametrisationTable() " <<endmsg;
  //
  if(!m_paraTypes[0]){
    info()<< " ==> No resolution parametrisation types have been provided!"
          << " Tool will use default one! (mean LHCb angle)" <<endmsg;
  }else{
    if(!(m_resParameters[0]+m_resParameters[1])){
      info()<< " ==> You need to provide all the params or use default!"
            <<endmsg;
    }else{
      ResPair temp;
      std::vector<double>::const_iterator typeIt;
      for(typeIt=m_paraTypes.begin(); typeIt!=m_paraTypes.end(); typeIt++){
        temp.first=(*typeIt);
        m_resTable.push_back(temp);
      }
      //
      int j=0;
      Pair tempP;
      for(unsigned int i=0; i<m_resParameters.size(); i++){
        tempP.first=m_resParameters[i];
        tempP.second=m_resParameters[i+1];
        m_resTable[j].second=tempP;
        j++;
        i++;
      }
    }
  }
}
//=========================================================================
void VeloClusterPosition::createEtaParTable()
{
  //
  debug()<< " ==> createEtaFitTable() " <<endmsg;
  //
  if(!m_paraTypes[0]){
    info()<< " ==> No resolution parametrisation types have been provided!"
          << " Tool will use default one! (mean LHCb angle)" <<endmsg;
  }else{
    if(!(m_etaParameters[0]+m_etaParameters[1])){
      info()<< " ==> You need to provide all the params or use default!"
            <<endmsg;
    }else{
      EtaPair temp;
      std::vector<double>::const_iterator typeIt;
      for(typeIt=m_paraTypes.begin(); typeIt!=m_paraTypes.end(); typeIt++){
        temp.first=(*typeIt);
        m_etaTable.push_back(temp);
      }
      //
      int j=0;
      EtaPar tempP;
      for(unsigned int i=0; i<m_etaParameters.size(); i++){
        tempP.assign(m_etaParameters.begin()+i, m_etaParameters.begin()+i+2);
        m_etaTable[j].second=tempP;
        j++;
        i+=2;
      }
    }
  }
}
//==============================================================================
toolInfo VeloClusterPosition::position(const LHCb::VeloCluster* cluster) const
{
  //return position using default error parametrizations and radii
  Pair temp=std::make_pair(0., 0.);
  return ( position(cluster, temp) );
}
//==============================================================================
toolInfo VeloClusterPosition::position(const LHCb::VeloCluster* cluster,
                                       double radiusOfCluster) const
{
  Pair temp=std::make_pair(0., radiusOfCluster);
  return ( position(cluster, temp) );
}
//

