// Include files 

// from Gaudi
#include "Kernel/IMCVeloFEType.h"

// local
#include "VeloRawClustersMoni.h"

// velo
#include "VeloDet/DeVelo.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VeloRawClustersMoni
//
// 2006-03-17 : Tomasz Szumlak
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( VeloRawClustersMoni )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VeloRawClustersMoni::VeloRawClustersMoni( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : GaudiTupleAlg( name , pSvcLocator ),
    m_clusterCont ( LHCb::VeloClusterLocation::Default ),
    m_VeloFECont ( LHCb::MCVeloFELocation::Default ),
    m_numberOfEvents ( 0 ),
    m_rawClusters ( 0 ),
    m_VeloFEs ( 0 ),
    m_feTypeTool ( 0 ),
    m_veloDet ( 0 ),
    m_nRawClusters ( 0. ),
    m_nRawClusters2 ( 0. ),
    m_nOneStr ( 0. ),
    m_nTwoStr ( 0. ),
    m_nThreeStr ( 0. ),
    m_nFourStr ( 0. ),
    m_nClusS ( 0. ),
    m_nClusN ( 0. ),
    m_nClusO ( 0. ),
    m_associationFailure ( 0. )
{ }
//=============================================================================
// Destructor
//=============================================================================
VeloRawClustersMoni::~VeloRawClustersMoni() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode VeloRawClustersMoni::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  //
  setHistoTopDir("Velo/");
  m_feTypeTool=tool<IMCVeloFEType>("MCVeloFEType");
  m_veloDet=getDet<DeVelo>( DeVeloLocation::Default );
  //
  return StatusCode::SUCCESS;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode VeloRawClustersMoni::execute() {

  if( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  //
  m_numberOfEvents++;
  if( msgLevel(MSG::DEBUG) ) debug()<< " number of evts: " << m_numberOfEvents <<endmsg;
  StatusCode dataStatus=getData();
  StatusCode moniStatus;
  if(dataStatus.isSuccess()){
    moniStatus=rawVeloClusterMonitor();
  }else{
    return ( dataStatus );
  }
  if(moniStatus.isFailure()){
    return Error("Raw cluster monitoring failed!", moniStatus );
  }
  //
  return ( StatusCode::SUCCESS );
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode VeloRawClustersMoni::finalize() {

  if( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;
  // set value if there is no event processed
  if(m_numberOfEvents==0) m_numberOfEvents=1;
  //
  m_nRawClusters/=m_numberOfEvents;
  m_nRawClusters2/=m_numberOfEvents;
  double errnVeloClusters=
   sqrt((m_nRawClusters2-(m_nRawClusters*m_nRawClusters))/m_numberOfEvents);
  //
  m_nOneStr/=m_numberOfEvents;
  m_nTwoStr/=m_numberOfEvents;
  m_nThreeStr/=m_numberOfEvents;
  m_nFourStr/=m_numberOfEvents;
  double all=m_nOneStr+m_nTwoStr+m_nThreeStr+m_nFourStr;
  //
  info()<< "======================================================" <<endmsg;
  info()<< "              - VeloClusterMoni table -               " <<endmsg;
  info()<< "------------------------------------------------------" <<endmsg;
  info()<< "| Number of Clusters/Event: " << m_nRawClusters << " +/- " 
        << errnVeloClusters <<endmsg;
  double allClusters=m_nClusS+m_nClusN+m_nClusO;
  if(allClusters>0){
    info().precision(4);
    info()<< "| Clusters from signal:                      " 
          << (m_nClusS/allClusters)*100
          << "%" <<endmsg;
    info().precision(4);
    info()<< "| Clusters from noise:                       " 
          << (m_nClusN/allClusters)*100
          << "%" <<endmsg;
    info().precision(4);
    info()<< "| Clusters from other (coupling+spillover):  " 
          << (m_nClusO/allClusters)*100
          << "%" <<endmsg;
    info()<< "------------------------------------------------------" <<endmsg;
    info()<< "              - Size of clusters (all)-               " <<endmsg;
    info()<< "------------------------------------------------------" <<endmsg;
    info().precision(4);
    info()<< "| 1 strip clusters/event:     "
          << m_nOneStr << " (" << (m_nOneStr/all)*100 << "%)"
          <<endmsg;
    info().precision(4);
    info()<< "| 2 strip clusters/event:     "
          << m_nTwoStr << " (" << (m_nTwoStr/all)*100 << "%)"
          <<endmsg;
    info().precision(3);
    info()<< "| 3 strip clusters/event:     "
          << m_nThreeStr << " (" << (m_nThreeStr/all)*100 << "%)"
          <<endmsg;
    info().precision(3);
    info()<< "| 4 strip clusters/event:     "
          << m_nFourStr << " (" << (m_nFourStr/all)*100 << "%)"
          <<endmsg;
    info()<< "------------------------------------------------------" <<endmsg;
    info()<< "              - Association failure -                 " <<endmsg;
    info()<< "------------------------------------------------------" <<endmsg;
    info().precision(6);
    info()<< " Relative number of un-associated clusters: "
          << (m_associationFailure/(all*m_numberOfEvents))*100 << " % " <<endmsg;
  }else{
    info()<< "| ==> No VeloClusters found! " <<endmsg;
  }
  info()<< "======================================================" <<endmsg;
  //
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}
//=============================================================================
StatusCode VeloRawClustersMoni::getData()
{
  if( msgLevel(MSG::DEBUG) ) debug()<< " ==> getData() " <<endmsg;
  //
  m_VeloFEs=getIfExists<LHCb::MCVeloFEs>(m_VeloFECont);
  if( NULL == m_VeloFEs ){
    return Error( " ==> There is no VeloFEs in TES " );
  }
  //
  m_rawClusters=getIfExists<LHCb::VeloClusters>(m_clusterCont);
  if( NULL == m_rawClusters ){
    return Error( " ==> There is no VeloClusters in TES " );
  }
  //
  if( msgLevel(MSG::DEBUG) ) {
    debug()<< " ==> Number of VeloFE found in TES at: "
           << m_VeloFECont << " is:"
           << m_VeloFEs->size() <<endmsg;
    //
    debug()<< " ==> Number of clusters found in TES at: "
           << m_clusterCont << " is: " 
           << m_rawClusters->size() <<endmsg;
  }
  //
  return ( StatusCode::SUCCESS );
}
//==============================================================================
StatusCode VeloRawClustersMoni::rawVeloClusterMonitor()
{
  if( msgLevel(MSG::DEBUG) ) debug()<< " ==> rawVeloClusterMonitor() " <<endmsg;
  //
  int contSize=m_rawClusters->size();
  if(!contSize){
    if( msgLevel(MSG::DEBUG) ) debug() <<"Empty cluster container! - Skiping monitor" << endmsg;
    return StatusCode::SUCCESS;
  }
  m_nRawClusters+=double(contSize);
  m_nRawClusters2+=double(contSize*contSize);
  plot(contSize, 100,
       "Number of Velo Clusters/event",
       0., 2000., 100);
  //  
  LHCb::VeloClusters::const_iterator cluIt;
  //
  for(cluIt=m_rawClusters->begin(); cluIt!=m_rawClusters->end(); cluIt++){
    // local cluster
    LHCb::VeloCluster* cluster=(*cluIt);
    // cluster size - number of strips in cluster
    unsigned int clusterSize=cluster->size();
    if( msgLevel(MSG::DEBUG) ) {
      debug()<< " ==> Cluster size: " << clusterSize <<endmsg;
      // adc values
      debug()<< " The given cluster has: " << clusterSize << " strip(s) "
             << " the ADC value(s) is/are: " <<endmsg;
    }
    //
    switch(int(clusterSize)){
    case 1: m_nOneStr++; break;
    case 2: m_nTwoStr++; break;
    case 3: m_nThreeStr++; break;
    case 4: m_nFourStr++; break;
    default : if( msgLevel(MSG::DEBUG) ) debug()<< " ==> Cluster has more than four strips! " <<endmsg;
    }
    // find type of given cluster
    bool signal=false, noise=false, other=false;
    StatusCode typeStatus=clusterType(cluster, signal, noise, other);
    if(typeStatus.isFailure()){
      Warning(" ==> Unknown cluster type! ");
      return ( typeStatus );
    }
    //
    // method adcValue(stripNumber) returns value of the ADC count
    // from given strip, adcValue() calls stripValues() method which
    // returns ADCVector of pairs (stripNumber, adcValue)
    //
    if( msgLevel(MSG::DEBUG) ) {
      for(unsigned int i=0; i<clusterSize; i++){
        debug()<< " Strip from cluster [" << i 
               << "] = " << cluster->adcValue(i) 
               << ", number of the strip on sensor: " 
               << cluster->strip(i) <<endmsg;
      }
    }
    //
    // channelID info (this channel is used to create VeloLightCluster)
    //
    LHCb::VeloChannelID channel=cluster->channelID();
    unsigned int sensNumber=channel.sensor();
    const DeVeloSensor* sensor=m_veloDet->sensor(sensNumber);
    if(sensor==0){
      return Error("==> No valid pointer to sensor - Check your CondDB!!");
    }
    plot2D(sensor->z()/Gaudi::Units::cm, sensNumber, 101,
           "Z position and sensor number of the cluster",
           -50., 100., 0, 135, 150, 135);
    //
    if( msgLevel(MSG::DEBUG) ) debug()<< " Central channel of given cluster: " 
                                      << channel << ", sensor number: " << sensNumber
                                      << ", strip: " << channel.strip() <<endmsg;
    //
    // total charge colected on the strips
    double tCharge=cluster->totalCharge();
    if( msgLevel(MSG::DEBUG) ) {
      if(tCharge>126.&&tCharge<128.){
        debug()<< "clusters ADC: " << tCharge
               << "this clu strip: " << cluster->channelID().strip()
               << "and sensor: " << cluster->channelID().sensor() <<endmsg;
      }
      debug()<< " Total charge of the cluster: " << tCharge <<endmsg;
    }
    plot(tCharge, 102,
         "ADC sum for all clusters",
         -0.5, 260.5, 261);
    plot2D(sensNumber, cluster->firstChannel().strip(), 103,
           "Sensor and first strip number",
           0., 135., 0., 2050., 135, 2050);
    plot(clusterSize, 104,
         "Number of strips in cluster",
         -0.5, 5.5, 6);
    //
    if(signal){
      m_nClusS++;
      plot(tCharge, 105,
           "Signal dominated ADC sum",
           -0.5, 260.5, 261);
      plot2D(sensor->z()/Gaudi::Units::cm, sensNumber, 106,
             "Z position and sensor number for signal dominated clusters",
             -50., 100., 0, 135, 150, 135);
      plot(clusterSize, 107,
           "Number of strips in cluster - signal dominated",
           -0.5, 5.5, 6);
    }
    //
    if(noise){
      m_nClusN++;
      plot(tCharge, 108,
           "Noise dominated ADC sum",
           -0.5, 260.5, 261);
      plot2D(sensor->z()/Gaudi::Units::cm, sensNumber, 109,
             "Z position and sensor number for noise dominated clusters",
             -50., 100., 0, 135, 150, 135);
      plot(clusterSize, 110,
           "Number of strips in cluster - noise dominated",
           -0.5, 5.5, 6);
    }
    //
    if(other){
      m_nClusO++;
      plot(tCharge, 111,
           "Other dominated ADC sum",
           -0.5, 260.5, 260);
      plot2D(sensor->z()/Gaudi::Units::cm, sensNumber, 112,
             "Z position and sensor number for other dominated clusters",
             -50., 100., 0, 135, 150, 135);
      plot(clusterSize, 113,
           "Number of strips in cluster - spollOver/couplings dominated",
           -0.5, 5.5, 6);
    }
    //
    // fractional position of the centre of the given cluster
    double fracPos=cluster->interStripFraction();
    if( msgLevel(MSG::DEBUG) ) debug()<< " Fractional position: " << fracPos <<endmsg;
    plot(fracPos, 114,
         "Inter strip position of cluster centre",
         0., 1., 100);
    //
    // pseudo size of the cluster
    unsigned int pseudoSize=cluster->pseudoSize();
    if( msgLevel(MSG::DEBUG) ) debug()<< " Pseudo size: " << pseudoSize <<endmsg;
    plot(pseudoSize, 115,
         "Cluster pseudo size",
         -0.5, 5.5, 6);
    //
    // high threshold bit
    bool thresholdBit=cluster->highThreshold();
    if( msgLevel(MSG::DEBUG) ) debug()<< " Signal bit: " << thresholdBit <<endmsg;
    plot(thresholdBit, 116, 
         "Cluster high threshold",
         -1., 2., 30);
    //
    // first and last channels
    LHCb::VeloChannelID firstChan=cluster->firstChannel();
    LHCb::VeloChannelID lastChan=cluster->lastChannel();
    if( msgLevel(MSG::DEBUG) ) {
      debug()<< " First channel info: strip - " << firstChan.strip()
             << ", sensor - " << firstChan.sensor() <<endmsg;
      debug()<< " Last channel info: strip - " << lastChan.strip()
             << ", sensor - " << lastChan.sensor() <<endmsg;
    }
    // channels vector
    std::vector<LHCb::VeloChannelID> channels=cluster->channels();
    if( msgLevel(MSG::DEBUG) ) debug()<< " Number of VeloChannelIDs inside the channel vector: "
                                      << channels.size() <<endmsg;
    // type of the cluster
    if(cluster->isRType()) debug()<< " RType cluster " <<endmsg;
    if(cluster->isPhiType()) debug()<< " PhiType cluster " <<endmsg;
    if(cluster->isPileUp()) debug()<< " PileUpType cluster" <<endmsg;
    // ADC signals
    if( msgLevel(MSG::DEBUG) ) {
      LHCb::VeloCluster::ADCVector localADC=cluster->stripValues();
      for(unsigned int i=0; i<localADC.size(); i++){
        int stripNum=localADC[i].first;
        unsigned int adcValue=localADC[i].second;
        debug()<< " ADC value of the strip " << stripNum << " is: "
               << adcValue <<endmsg;
      }
    }
  }
  //
  return ( StatusCode::SUCCESS );
}
//=============================================================================
StatusCode VeloRawClustersMoni::clusterType(LHCb::VeloCluster* clu,
                                            bool& s, bool& n, bool& o)
{
  if( msgLevel(MSG::DEBUG) ) debug()<< " ==> clusterType() " <<endmsg;
  //
  LHCb::MCVeloFE* myFE=0;
  int feType=0;
  int strips=clu->size();
  std::vector<LHCb::VeloChannelID> channels=clu->channels();
  for(int iStrip=0; iStrip<strips; iStrip++){
    LHCb::MCVeloFE* anFE=m_VeloFEs->object(channels[iStrip]);
    if(anFE==0){
      if( msgLevel(MSG::DEBUG) ) {
        debug()<< " ==> Not found FE object for given cluster with channel:" 
               << channels[iStrip].strip() << ", " 
               << channels[iStrip].sensor() <<endmsg; 
        LHCb::VeloChannelID cch=clu->channelID();
        debug()<< " -> cluster description: "
               << " size: " << (clu->size())
               << " ADC total: " << (clu->totalCharge())
               << " centre strip: " << cch.strip()
               <<endmsg;
      }
      ++m_associationFailure;
      return ( StatusCode::SUCCESS );
    }else{
      if(iStrip==0) myFE=anFE;
      if(anFE->charge()>myFE->charge()) myFE=anFE;
    }
  }
  // s - signal, n - noise, o - other
  if((!s)&&(!n)&&(!o)){
    m_feTypeTool->FEType(myFE, feType);
  }else{
    s=false;
    n=false;
    o=false;
    m_feTypeTool->FEType(myFE, feType);
  }
  //
  switch(feType){
  case 0: s=true; break;
  case 1: n=true; break;
  case 2: o=true; break;
  default : return Error( " ==> Wrong FE type flag! " );
  }
  //
  return ( StatusCode::SUCCESS );
}
//--
