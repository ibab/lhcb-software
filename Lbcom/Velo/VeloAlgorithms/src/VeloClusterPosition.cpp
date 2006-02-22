// $Id: VeloClusterPosition.cpp,v 1.2 2006-02-22 15:36:27 szumlat Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "Kernel/LHCbMath.h"
#include "Kernel/SystemOfUnits.h"
#include "Kernel/Point3DTypes.h"

// local
#include "VeloClusterPosition.h"
#include "Event/VeloCluster.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VeloClusterPosition
//
// 2005-09-30 : Tomasz Szumlak 
// 
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<VeloClusterPosition>          s_factory ;
const        IToolFactory& VeloClusterPositionFactory = s_factory ; 

typedef std::pair<double, double> Pair;
typedef std::pair<LHCb::VeloChannelID, double> stripPair;
typedef std::pair<stripPair, double> toolPair;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
typedef std::pair<double, double> Pair;

VeloClusterPosition::VeloClusterPosition(const std::string& type,
                               const std::string& name,
                               const IInterface* parent)
  : GaudiTool(type, name, parent),
    m_veloDet ( getDet<DeVelo>("/dd/Structure/LHCb/BeforeMagnetRegion/Velo") ),
    m_printInfo ( false ),
    m_useWeightedMean ( false ),
    m_useEtaFit ( false ),
    m_paraTypes ( 6, 0. ),
    m_defaultResOneStrip ( 2, 0. ),
    m_defaultResTwoStrip ( 2, 0. ),
    m_defaultEtaPar ( 3, 0. ),
    m_resParameters ( 12, 0. ),
    m_sensType ( "" )
{
  declareInterface<IVeloClusterPosition>(this);
  declareProperty("PrintInfo", m_printInfo);
  declareProperty("UseWeightedMean", m_useWeightedMean);
  declareProperty("UseEtaFit", m_useEtaFit);
  declareProperty("ParaTypes", m_paraTypes);
  declareProperty("DefaultResolutionOneStrip", m_defaultResOneStrip);
  declareProperty("DefaultResolutionTwoStrip", m_defaultResTwoStrip);
  declareProperty("ResParameters", m_resParameters);
  declareProperty("DefaultEtaPar", m_defaultEtaPar);
  //
  //  createResParTable();
  //  createEtaParTable();
  //
}
//=============================================================================
// Destructor
//=============================================================================
VeloClusterPosition::~VeloClusterPosition() {};
//=============================================================================
// position calculation, depending on sensor type the clusterPos
// is radial or angular variable
//=============================================================================
toolPair VeloClusterPosition::weightedMeanPos(
                         const LHCb::VeloCluster* cluster,
                         Pair& userInfo)
{
  debug()<< " ==> weightedMeanPos() " <<endmsg;
  //
  Pair tempPair;
  Pair fPair;
  stripPair sPair;
  toolPair localPair;
  int intDistance=0;
  double fractionalPos=0.;
  // calculate fractional position in units of 'strip'
  fPair=fractionalPosMean(cluster);
  fractionalPos=fPair.first;
  intDistance=int(fPair.second);
  //
  LHCb::VeloChannelID intDistanceID;
  // determine the closest VeloChannelID (strip) to the cluster
  StatusCode sc;
  sc=m_veloDet->neighbour(cluster->firstChannel(), intDistance, intDistanceID);
  //
    if(sc){
      posAndError(cluster, intDistanceID, fractionalPos, userInfo, tempPair);
      sPair=std::make_pair(intDistanceID, fractionalPos);
      localPair=std::make_pair(sPair, tempPair.second);
      //
      return (localPair);
    }else{
      info()<< "Given cluster is invalid!" <<endmsg;
      sPair=std::make_pair(LHCb::VeloChannelID(0), 0.);
      localPair=std::make_pair(sPair, 0.);
      //
      return (localPair);
    }
}
//=========================================================================
toolPair VeloClusterPosition::etaFitPos(
                         const LHCb::VeloCluster* cluster,
                         Pair& userInfo)
{
  debug()<< " ==> VeloClusterPosition::etaFitPos" <<endmsg;
  //
  Pair fPair;
  stripPair sPair;
  toolPair localPair;
  Pair tempPair;
  int intDistance=0;
  double fractionalPos=0.;
  //
  double alphaOfTrack=userInfo.first;
  fPair=fractionalPosEta(cluster, alphaOfTrack);
  fractionalPos=fPair.first;
  intDistance=int(fPair.second);
  LHCb::VeloChannelID intDistanceID;
  StatusCode sc;
  sc=m_veloDet->neighbour(cluster->firstChannel(), intDistance, intDistanceID);
  if(sc){
    posAndError(cluster, intDistanceID, fractionalPos, userInfo, tempPair);
    sPair=std::make_pair(intDistanceID, fractionalPos);
    localPair=std::make_pair(sPair, tempPair.second);
    //
    return (localPair);
  }else{
    info()<< "Given cluster is invalid!" <<endmsg;
    sPair=std::make_pair(LHCb::VeloChannelID(0), 0.);
    localPair=std::make_pair(sPair,0.);
    //
    return (localPair);
  }
}
//=========================================================================
double VeloClusterPosition::resolution(const double& pitch,
                                       const Pair& resInfo)
{ 
  debug()<< " ==> resolution() " <<endmsg;
  //
  double p_o_0=m_defaultResOneStrip[0];
  double p_o_1=m_defaultResOneStrip[1];
  //
  double p_t_0=m_defaultResTwoStrip[0];
  double p_t_1=m_defaultResTwoStrip[1];
  //
  double resolution=0.;
  int cluSize=static_cast<int>(resInfo.second);
  //
  if(!(resInfo.first)){
    // use default resolution parametrisation for mean LHCb angle
    switch(cluSize){
    case 1: resolution=p_o_0+p_o_1*pitch; break;
    case 2: resolution=p_t_0+p_t_1*pitch; break;
    default: resolution=p_t_0+p_t_1*pitch;
    }
    //
    return (resolution);
  }else{
    // still no good parametrisations available!
    return (0.);
  }  
}
//=========================================================================
toolPair VeloClusterPosition::position(
                         const LHCb::VeloCluster* cluster,
                         Pair& userInfo)
{
  debug()<< " ==> VeloClusterPosition::position" <<endmsg;
  
  if(m_useWeightedMean){
    // if user failed to provide required info fill the container
    if(0==userInfo.first){
      userInfo=std::make_pair(0., 0.);
    }
    return (weightedMeanPos(cluster, userInfo));
  }else if(m_useEtaFit){
    // if user did not provide required info fill the container 
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
    stripPair sPair=std::make_pair(LHCb::VeloChannelID(0), 0.);
    toolPair localPair=std::make_pair(sPair, 0.);
    //
    return (localPair);
  } 
}
//=========================================================================
Pair VeloClusterPosition::fractionalPosMean(
                         const LHCb::VeloCluster* cluster)
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
  for(int i=0; i<stripNumber; i++){
    sc=m_veloDet->channelDistance(chanCont[0],
                                  chanCont[i],
                                  intDistance);
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
  temp=std::make_pair(fractionalPos, intDistance);
  //  
  return  (temp);
}
//
Pair VeloClusterPosition::fractionalPosEta(
                         const LHCb::VeloCluster* cluster,
                         double alphaOfTrack)
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
                     Pair& userInfo, Pair& toolInfo)
{ 
  debug()<< "==> VeloClusterPosition::posAndError" <<endmsg;
  //  
  unsigned int sensor=cluster->channelID().sensor();
  double cluSize=static_cast<double>(cluster->size());
  double pitch=0., clusterPos=0., errorPos=0., rOfPhiCluster=0.;
  double alphaOfTrack=userInfo.first;
  Pair resInfo=std::make_pair(alphaOfTrack, cluSize);
  //
  if(m_veloDet->isRSensor(sensor)||m_veloDet->isPileUpSensor(sensor)){
    setSensType("RSensor");
    clusterPos=m_veloDet->rOfStrip(intDistanceID, fractionalPos);
    if(m_printInfo)
      info()<< " ==> The given cluster is on RType sensor" <<endmsg;
    clusterPos=clusterPos/cm;
    pitch=m_veloDet->rPitch(intDistanceID, fractionalPos);
    errorPos=resolution(pitch/micrometer, resInfo);
    errorPos=errorPos/(pitch/micrometer);
    // return values for cluster position and position errorPos
    toolInfo=std::make_pair(clusterPos, errorPos);
    //
      if(m_printInfo){
        info()<< "cluster pos: " << clusterPos <<endmsg;
      }
  }
  else if(m_veloDet->isPhiSensor(sensor)){
    setSensType("PhiSensor");
    if(!(userInfo.first+userInfo.second)){
      double minRadius=0., maxRadius=0.;
      unsigned int zoneOfCluster=0;
      zoneOfCluster=m_veloDet->zoneOfStrip(intDistanceID);
      minRadius=m_veloDet->rMin(sensor, zoneOfCluster);
      maxRadius=m_veloDet->rMax(sensor, zoneOfCluster);
      double meanRadius=0.5*(maxRadius+minRadius);
      double frac=fractionalPos;
      clusterPos=m_veloDet->phiOfStrip(intDistanceID, frac, meanRadius);
      double phiOf=m_veloDet->phiOfStrip(intDistanceID, meanRadius);
      debug()<< "phi: " << phiOf/degree << ", " << frac <<endmsg;
      //
      if(m_printInfo)
        info()<< " ==> The given cluster is on PhiType sensor" <<endmsg;
      clusterPos=clusterPos/degree;
      //
      if(m_printInfo){
        info()<< "cluster pos: " << clusterPos <<endmsg;
        info()<< "mean radius: " << meanRadius/cm <<endmsg;
      }
      //
      pitch=m_veloDet->phiPitch(intDistanceID, meanRadius);
      errorPos=resolution(pitch/micrometer, resInfo);
      double errorRad=errorPos/(meanRadius/micrometer);
      double errorDeg=errorRad/degree;
      if(m_printInfo){
        info()<< "Error [rad] " << errorRad
              << ", error [deg] " << errorDeg <<endmsg;
      }
      // error as a fraction of the strip distance
      errorPos/=(pitch/micrometer);
      //
      toolInfo=std::make_pair(clusterPos, errorPos);
    }else{
      rOfPhiCluster=userInfo.second;
      clusterPos=m_veloDet->phiOfStrip(intDistanceID, fractionalPos,
                                         rOfPhiCluster);
      if(m_printInfo)
        info()<< " ==> The given cluster is on PhiType sensor" <<endmsg;
      //
      clusterPos=clusterPos/degree;
      pitch=m_veloDet->phiPitch(intDistanceID, rOfPhiCluster);
      errorPos=resolution(pitch/micrometer, resInfo);
      double errorRad=errorPos/(rOfPhiCluster/micrometer);
      double errorDeg=errorRad/degree;
      if(m_printInfo){
        info()<< " Error [rad] " << errorRad
              << ", errorDeg [deg] " << errorDeg <<endmsg;
      }
      errorPos/=(pitch/micrometer);
      //
      toolInfo=std::make_pair(clusterPos, errorPos);
    }
  }
}
//=========================================================================
Pair VeloClusterPosition::etaFrac(const LHCb::VeloCluster* cluster,
                                 double fracPosTrue)
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
std::string VeloClusterPosition::sensType()
{
  return (m_sensType);
}
//==============================================================================
void VeloClusterPosition::setSensType(std::string type)
{
  m_sensType=type;
}
//==============================================================================
//==============================================================================
toolPair VeloClusterPosition::position(const LHCb::VeloCluster* cluster)
{
  //return position using default error parametrizations and radii
  Pair temp=std::make_pair(0., 0.);
  return ( position(cluster, temp) );
}
//==============================================================================
toolPair VeloClusterPosition::position(const LHCb::VeloCluster* cluster,
                                       double radiusOfCluster)
{
  Pair temp=std::make_pair(0., radiusOfCluster);
  return ( position(cluster, temp) );
}
//

