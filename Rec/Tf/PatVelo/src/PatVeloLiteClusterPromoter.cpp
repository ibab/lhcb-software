// Include files 

// local
#include "PatVeloLiteClusterPromoter.h"
#include "PatVeloRHitManager.h"
#include "PatVeloPhiHitManager.h"
#include "Event/Track.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatVeloLiteClusterPromoter
//
// 2008-10-15 : David Hutchcroft
//-----------------------------------------------------------------------------

namespace Tf {
  // Declaration of the Tool Factory
  DECLARE_TOOL_FACTORY( PatVeloLiteClusterPromoter )
}

  //==========================================================================
  // Standard constructor, initializes variables
  //==========================================================================
Tf::PatVeloLiteClusterPromoter::
PatVeloLiteClusterPromoter( const std::string& type,
			    const std::string& name,
			    const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IPromoteClusters>(this);
  declareProperty( "RHitManagerName", 
		   m_rHitManagerName = "PatVeloRHitManager");
  declareProperty( "PhiHitManagerName", 
		   m_phiHitManagerName = "PatVeloPhiHitManager");
  declareProperty( "FakeVELOClusterLocation", 
		   m_fakeVELOClusterLocation = "Raw/Velo/FakeClusters" );
}
//==========================================================================
// Destructor
//==========================================================================
Tf::PatVeloLiteClusterPromoter::~PatVeloLiteClusterPromoter() {} 
//==========================================================================

StatusCode Tf::PatVeloLiteClusterPromoter::initialize(){
  StatusCode sc = GaudiTool::initialize();
  if(!sc) return sc;
  
  m_rHitManager   = tool<PatVeloRHitManager>  ( "Tf::PatVeloRHitManager", 
						m_rHitManagerName );
  m_phiHitManager = tool<PatVeloPhiHitManager>( "Tf::PatVeloPhiHitManager", 
						m_phiHitManagerName );

  info() << "=== Tool " << name() << " param. ==="<< endmsg
	 << "Will put faked VELO clusters here " << m_fakeVELOClusterLocation
	 << endmsg
	 << "================================================="<< endmsg;

  return StatusCode::SUCCESS;
}

StatusCode Tf::PatVeloLiteClusterPromoter::
promoteClusters(std::vector<LHCb::LHCbID> const & inputIDs ){

  // see if there is already a container to fill
  LHCb::VeloClusters* fakeClusters;
  if(exist<LHCb::VeloClusters>(m_fakeVELOClusterLocation)){
    fakeClusters = get<LHCb::VeloClusters>(m_fakeVELOClusterLocation);    
  }else{
    fakeClusters = new LHCb::VeloClusters();
    put(fakeClusters, m_fakeVELOClusterLocation );
  }
  // make space for new clusters
  fakeClusters->reserve(fakeClusters->size() + inputIDs.size());

  std::vector<LHCb::LHCbID>::const_iterator it;
  for ( it = inputIDs.begin(); inputIDs.end() != it; ++it ){
    if( ! it->isVelo() ) {
      Warning("Non VeloID",StatusCode::SUCCESS,0).ignore();
    }else if( it->isVeloPhi() ){
      int sensor = it->veloID().sensor();
      // add Phi clusters from sensor sensor
      PatVeloPhiHitManager::Station * phiStation =
        *(m_phiHitManager->stationIterAll(sensor));
      int zone =phiStation->sensor()->globalZoneOfStrip(it->veloID().strip());
      PatVeloPhiHit* phiHit = phiStation->hitByLHCbID(zone,*it);
      if(!phiHit){
        Warning("Did not find phi cluster" ,StatusCode::FAILURE,0).ignore();
        if(msgLevel(MSG::DEBUG)) debug() << "Did not find phi cluster for " << *it << endmsg;
        return StatusCode::FAILURE;
      }
      // get core VeloLiteCluster from hit 
      const LHCb::VeloLiteCluster &liteCluster = phiHit->hit()->cluster();
      promoteLiteCluster(liteCluster,fakeClusters);
    }else if( it->isVeloR() ){
      int sensor = it->veloID().sensor();
      // add R clusters from sensor sensor
      PatVeloRHitManager::Station * rStation =
        *(m_rHitManager->stationIterAll(sensor));
      int zone =rStation->sensor()->globalZoneOfStrip(it->veloID().strip());
      PatVeloRHit* rHit = rStation->hitByLHCbID(zone,*it);
      if(!rHit){
        Warning("Did not find R cluster" ,StatusCode::FAILURE,0).ignore();
        if(msgLevel(MSG::DEBUG)) debug() << "Did not find R cluster for " << *it << endmsg;
        return StatusCode::FAILURE;
      }
      // get core VeloLiteCluster from hit 
      const LHCb::VeloLiteCluster &liteCluster = rHit->hit()->cluster();
      promoteLiteCluster(liteCluster,fakeClusters);
    }
  }
  return StatusCode::SUCCESS;
}

void Tf::PatVeloLiteClusterPromoter::
  promoteLiteCluster(LHCb::VeloLiteCluster const &liteCluster,
		     LHCb::VeloClusters* fakeClusters){

  LHCb::VeloCluster * fakeClus;
  if( liteCluster.pseudoSize() == 1){
    if( liteCluster.highThreshold() ){
      fakeClus = 
        new LHCb::VeloCluster(
                              liteCluster,
                              LHCb::VeloCluster::ADCVector(1,
                                                           std::pair<int,unsigned int>(
                                                                                       liteCluster.channelID().strip(),
                                                                                       64)));
    }else{
      fakeClus = 
        new LHCb::VeloCluster(
                              liteCluster,
                              LHCb::VeloCluster::ADCVector(1,
                                                           std::pair<int,unsigned int>(
                                                                                       liteCluster.channelID().strip(),
                                                                                       21)));
    }
  }else{
    // always fake two strip clusters for two or more strip clusters 
    // from pseudosize
    if( liteCluster.highThreshold() ){
      unsigned int secondADC = 
	static_cast<unsigned int>(4. + (liteCluster.interStripFraction()*64.));
      unsigned int firstADC = static_cast<unsigned int>(64 - secondADC);
      LHCb::VeloCluster::ADCVector vec;
      vec.reserve(2);
      vec.push_back(std::pair<int,unsigned int>(liteCluster.channelID().strip(),firstADC));
      vec.push_back(std::pair<int,unsigned int>(liteCluster.channelID().strip()+1,secondADC));
      fakeClus = 
	new LHCb::VeloCluster(liteCluster,vec);
    }else{
      unsigned int secondADC = 
	static_cast<unsigned int>((4. + (liteCluster.interStripFraction()*64.))/3.);
      unsigned int firstADC = static_cast<unsigned int>(21 - secondADC);
      LHCb::VeloCluster::ADCVector vec;
      vec.reserve(2);
      vec.push_back(std::pair<int,unsigned int>(liteCluster.channelID().strip(),firstADC));
      vec.push_back(std::pair<int,unsigned int>(liteCluster.channelID().strip()+1,secondADC));
      fakeClus = 
	new LHCb::VeloCluster(liteCluster,vec);
    }
  }

  fakeClusters->insert(fakeClus,liteCluster.channelID());
  return;
}

