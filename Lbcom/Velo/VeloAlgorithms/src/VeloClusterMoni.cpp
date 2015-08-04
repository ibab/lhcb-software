// Include files 
// local
#include "VeloClusterMoni.h"

// velo
#include "Kernel/IMCVeloFEType.h"
#include "VeloDet/DeVelo.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VeloClusterMoni
//
// 2005-11-30 : Tomasz Szumlak
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( VeloClusterMoni )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VeloClusterMoni::VeloClusterMoni( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator ),
    m_clusterCont ( LHCb::InternalVeloClusterLocation::Default ),
    m_feCont ( LHCb::MCVeloFELocation::Default ),
    m_veloClusters ( 0 ),
    m_veloFEs ( 0 ),
    m_feTypeTool ( 0 ),
    m_nVeloClusters ( 0. ),
    m_nVeloClusters2 ( 0. ),
    m_nVeloClustersS ( 0. ),
    m_nVeloClustersN ( 0. ),
    m_nVeloClustersO ( 0. ),
    m_nOneStrip ( 0. ),
    m_nTwoStrip ( 0. ),
    m_nThreeStrip ( 0. ),
    m_nFourStrip ( 0. ),
    m_numberOfEvents ( 0 ),
    m_veloDet ( 0 ) 
{
  declareProperty( "PrintInfo", m_printInfo = false );
}
//=============================================================================
// Destructor
//=============================================================================
VeloClusterMoni::~VeloClusterMoni() {}
//=============================================================================
// Initialization
//=============================================================================
StatusCode VeloClusterMoni::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  //
  if( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  m_veloDet = getDet<DeVelo>( DeVeloLocation::Default );
  m_feTypeTool=tool<IMCVeloFEType>("MCVeloFEType/feTypeTool");
  setHistoTopDir("Velo/");
  //
  return StatusCode::SUCCESS;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode VeloClusterMoni::execute() {

  if( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  //
  m_numberOfEvents++;
  StatusCode sc=getData();
  if(sc) veloClusterMonitor();
  //
  return StatusCode::SUCCESS;
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode VeloClusterMoni::finalize() {

  if( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;
  // set value if there is no event processed
  if(m_numberOfEvents==0) m_numberOfEvents=1;
  //
  m_nVeloClusters/=m_numberOfEvents;
  m_nVeloClusters2/=m_numberOfEvents;
  double errnVeloClusters=
   sqrt((m_nVeloClusters2-(m_nVeloClusters*m_nVeloClusters))/m_numberOfEvents);
  //
  m_nOneStrip/=m_numberOfEvents;
  m_nTwoStrip/=m_numberOfEvents;
  m_nThreeStrip/=m_numberOfEvents;
  m_nFourStrip/=m_numberOfEvents;
  double all=m_nOneStrip+m_nTwoStrip+m_nThreeStrip+m_nFourStrip;
  //
  info()<< "======================================================" <<endmsg;
  info()<< "              - VeloClusterMoni table -               " <<endmsg;
  info()<< "------------------------------------------------------" <<endmsg;
  info()<< "| Number of Clusters/Event: " << m_nVeloClusters << " +/- " 
        << errnVeloClusters <<endmsg;
  double allClusters=m_nVeloClustersS+m_nVeloClustersN+m_nVeloClustersO;
  if(allClusters>0){
    info().precision(4);
    info()<< "| Clusters from signal                       " 
          << (m_nVeloClustersS/allClusters)*100
          << "%" <<endmsg;
    info().precision(4);
    info()<< "| Clusters from noise:                       " 
          << (m_nVeloClustersN/allClusters)*100
          << "%" <<endmsg;
    info().precision(4);
    info()<< "| Clusters from other (coupling+spillover):  " 
          << (m_nVeloClustersO/allClusters)*100
          << "%" <<endmsg;
    info()<< "------------------------------------------------------" <<endmsg;
    info()<< "              - Size of clusters (all)-               " <<endmsg;
    info()<< "------------------------------------------------------" <<endmsg;
    info().precision(4);
    info()<< "| 1 strip clusters/event:     "
          << m_nOneStrip << " (" << (m_nOneStrip/all)*100 << "%)"
          <<endmsg;
    info().precision(4);
    info()<< "| 2 strip clusters/event:     "
          << m_nTwoStrip << " (" << (m_nTwoStrip/all)*100 << "%)"
          <<endmsg;
    info().precision(3);
    info()<< "| 3 strip clusters/event:     "
          << m_nThreeStrip << " (" << (m_nThreeStrip/all)*100 << "%)"
          <<endmsg;
    info().precision(3);
    info()<< "| 4 strip clusters/event:     "
          << m_nFourStrip << " (" << (m_nFourStrip/all)*100 << "%)"
          <<endmsg;
  }else{
    if( msgLevel(MSG::DEBUG) ) debug()<< "| ==> No VeloClusters found! " <<endmsg;
  }
  info() << "======================================================" <<endmsg;
  //
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}
//=============================================================================
StatusCode VeloClusterMoni::getData()
{
  if( msgLevel(MSG::DEBUG) ) debug()<< " ==> getData() " <<endmsg;
  //
  m_veloClusters=getIfExists<LHCb::InternalVeloClusters>(m_clusterCont);
  if( NULL == m_veloClusters ){
    return Error( " ==> There are no VeloClusters in TES! " );
  }

  m_veloFEs=getIfExists<LHCb::MCVeloFEs>(m_feCont);
  if( NULL == m_veloFEs ){
    return Error( " ==> There are no MCVeloFEs in TES! " );
  }
  //
  if(m_printInfo){
    info()<< " ==> Number of clusters found in TES: "
          << m_veloClusters->size() <<endmsg;
    info()<< " ==> Number of velo FEs found in TES: "
          << m_veloFEs->size() <<endmsg;
  }
  //
  return (StatusCode::SUCCESS);
}
//==============================================================================
StatusCode VeloClusterMoni::veloClusterMonitor()
{
  if( msgLevel(MSG::DEBUG) ) debug()<< " ==> veloClusterMonitor() " <<endmsg;
  //
  int contSize=m_veloClusters->size();
  m_nVeloClusters+=double(contSize);
  m_nVeloClusters2+=double(contSize*contSize);
  plot(contSize, 100, "Number of VeloClusters/event", 0., 3000., 50);
  //
  LHCb::InternalVeloClusters::iterator cluIt;
  //
  for(cluIt=m_veloClusters->begin(); cluIt!=m_veloClusters->end(); cluIt++){
    //
    double strips=(*cluIt)->size();
    switch(int(strips)){
    case 1: m_nOneStrip++; break;
    case 2: m_nTwoStrip++; break;
    case 3: m_nThreeStrip++; break;
    case 4: m_nFourStrip++; break;
    default : if( msgLevel(MSG::DEBUG) ) debug()<< " ==> cluster has more than four strips!" <<endmsg;
    }
    //
    bool signal=false, noise=false, other=false;
    LHCb::InternalVeloCluster* cluster=(*cluIt);
    clusterType(cluster, signal, noise, other);
    // printout some info about cluster
    if( msgLevel(MSG::DEBUG) && m_printInfo ){
      debug()<< " ==> VeloCluster: " << " sensor number: "
             << (*cluIt)->sensor() << ", first strip in cluster: "
             << (*cluIt)->strip(0) <<endmsg;
    }
    // printout some info about strips
    int cluSize=(*cluIt)->size();
    if( msgLevel(MSG::DEBUG) && m_printInfo ){
      for(int iStrip=0; iStrip<cluSize; iStrip++){
        debug()<< " ==> VeloCluster: " << " strip: "
               << (*cluIt)->strip(iStrip) << ", signal on strip: "
               << (*cluIt)->adcValue(iStrip) <<endmsg;
      }
    }
    //
    double adcSum=0.;
    //
    for(int iStrip=0; iStrip<cluSize; iStrip++){
      adcSum+=double((*cluIt)->adcValue(iStrip));
    }
    //
    unsigned int sensor=(*cluIt)->sensor();
    const DeVeloSensor* sens=m_veloDet->sensor(sensor);
    if(sens->isPileUp())
    if( msgLevel(MSG::DEBUG) ) debug()<< "sensor number: " << sensor <<endmsg;
    double zPosOfClu=sens->z();
    if( msgLevel(MSG::DEBUG) ) debug()<< " z pos: " << zPosOfClu <<endmsg;
    //
    plot2D((*cluIt)->sensor(), (*cluIt)->strip(0), 102,
           "Sensor and first strip number",
           0., 132., 0., 2050., 132, 50);
    plot(adcSum, 103,
         "ADC sum",
         -0.5, 255.5, 256);
    //
    if(signal){
      plot(adcSum, 104,
           "Signal dominated - ADC sum",
           -0.5, 255.5, 256);
      m_nVeloClustersS++;
      plot2D(zPosOfClu/Gaudi::Units::cm, sensor, 114,
             "Z position and sensor number for signal dominated clusters",
             -50., 100., 0., 135, 150, 135);
    }
    if(noise){
      plot(adcSum, 105,
           "Noise dominated - ADC sum",
           -0.5, 255.5, 256);
      m_nVeloClustersN++;
      plot2D(zPosOfClu/Gaudi::Units::cm, sensor, 115,
             "Z position and sensor number for noise dominated clusters",
             -50., 100., 0., 135, 150, 135);
    }
    if(other){
      plot(adcSum, 106,
           "SpillOver/couplings dominated - ADC sum",
           -0.5, 255.5, 256);
      m_nVeloClustersO++;
      plot2D(zPosOfClu, sensor, 116,
             "Z position and sensor number for spillOver/couplings"
             " dominated clusters",
             -50., 100., 0., 135, 150, 135);
    }
    //
    plot(cluSize, 107,
         "Number of strips in cluster",
         -0.5, 5.5, 6);
    if(signal){
      plot(cluSize, 108,
           "Number of strips in cluster - signal dominated",
           -0.5, 5.5, 6);
    }
    if(noise){
      plot(cluSize, 109,
           "Number of strips in cluster - noise dominated",
           -0.5, 5.5, 6);
    }
    if(other){
      plot(cluSize, 110,
           "Number of strips in cluster - spillover/couplings dominated",
           -0.5, 5.5, 6);
    }
  }
  //
  plot(m_nVeloClustersS, 111,
       "Number of velo clusters/event - signal dominated",
       0., 3000., 50);
  plot(m_nVeloClustersN, 112,
       "Number of velo clusters/event - noise dominated",
       0., 3000., 50);
  plot(m_nVeloClustersO, 113,
       "Number of velo clusters/event - spillover/couplings dominated",
       0., 3000., 50);
  //
  return (StatusCode::SUCCESS);
}
//==============================================================================
StatusCode VeloClusterMoni::clusterType(LHCb::InternalVeloCluster* clu,
                                        bool& s, bool& n, bool& o)
{
  LHCb::MCVeloFE* myFE=0;
  int feType=0;
  int nOfStrips=clu->size();
  for(int iStrip=0; iStrip<nOfStrips; iStrip++){
    LHCb::VeloChannelID chanID=clu->channelID(iStrip);
    LHCb::MCVeloFE* FE=m_veloFEs->object(chanID);
    if(0==FE){
      warning()<< " ==> Not found FE for given ChannelID: " 
               << chanID <<endmsg;
    }else{
      if(iStrip==0) myFE=FE;
      if(FE->charge()>myFE->charge()) myFE=FE;
    }
  }
  // s - signal, n - noise, o - coupling or spillover
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
  default: error()<< " ==> Wrong FE Type flag! " <<endmsg;
    return (StatusCode::FAILURE);
  }
  //
  return (StatusCode::SUCCESS);
}
