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
  : base_class ( type, name , parent )
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
Tf::PatVeloLiteClusterPromoter::~PatVeloLiteClusterPromoter() = default;
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
promoteClusters(std::vector<LHCb::LHCbID> const & inputIDs ) const{

  // see if there is already a container to fill
  LHCb::VeloClusters* fakeClusters = getIfExists<LHCb::VeloClusters>(m_fakeVELOClusterLocation);
  if(!fakeClusters) {
    fakeClusters = new LHCb::VeloClusters();
    put(fakeClusters, m_fakeVELOClusterLocation );
  }
  // make space for new clusters
  fakeClusters->reserve(fakeClusters->size() + inputIDs.size());

  for (const auto& id : inputIDs ){
    if( ! id.isVelo() ) {
      Warning("Non VeloID",StatusCode::SUCCESS,0).ignore();
    }else if( id.isVeloPhi() ){
      int sensor = id.veloID().sensor();
      // add Phi clusters from sensor sensor
      PatVeloPhiHitManager::Station * phiStation =
        *(m_phiHitManager->stationIterAll(sensor));
      int zone =phiStation->sensor()->globalZoneOfStrip(id.veloID().strip());
      PatVeloPhiHit* phiHit = phiStation->hitByLHCbID(zone,id);
      if(!phiHit){
        Warning("Did not find phi cluster" ,StatusCode::FAILURE,0).ignore();
        if(msgLevel(MSG::DEBUG)) debug() << "Did not find phi cluster for " << id << endmsg;
        return StatusCode::FAILURE;
      }
      // get core VeloLiteCluster from hit
      promoteLiteCluster( phiHit->hit()->cluster(), fakeClusters );
    }else if( id.isVeloR() ){
      int sensor = id.veloID().sensor();
      // add R clusters from sensor sensor
      PatVeloRHitManager::Station * rStation =
        *(m_rHitManager->stationIterAll(sensor));
      int zone =rStation->sensor()->globalZoneOfStrip(id.veloID().strip());
      PatVeloRHit* rHit = rStation->hitByLHCbID(zone,id);
      if(!rHit){
        Warning("Did not find R cluster" ,StatusCode::FAILURE,0).ignore();
        if(msgLevel(MSG::DEBUG)) debug() << "Did not find R cluster for " << id << endmsg;
        return StatusCode::FAILURE;
      }
      // get core VeloLiteCluster from hit
      promoteLiteCluster( rHit->hit()->cluster(), fakeClusters );
    }
  }
  return StatusCode::SUCCESS;
}

void Tf::PatVeloLiteClusterPromoter::
  promoteLiteCluster(LHCb::VeloLiteCluster const &liteCluster,
		     LHCb::VeloClusters* fakeClusters) const {

  std::unique_ptr<LHCb::VeloCluster> fakeClus;
  if( liteCluster.pseudoSize() == 1){
    auto adc = ( liteCluster.highThreshold() ? 64u : 21u );
    fakeClus.reset( new LHCb::VeloCluster( liteCluster,
                                      {{ liteCluster.channelID().strip(), adc }}) );
  }else{
    // always fake two strip clusters for two or more strip clusters
    // from pseudosize
    unsigned int secondADC, firstADC;
    if( liteCluster.highThreshold() ){
      secondADC = static_cast<unsigned int>(4. + liteCluster.interStripFraction()*64.);
      firstADC = static_cast<unsigned int>(64 - secondADC);
    }else{
      secondADC = static_cast<unsigned int>((4. + liteCluster.interStripFraction()*64.)/3.);
      firstADC = static_cast<unsigned int>(21 - secondADC);
    }
    fakeClus.reset( new LHCb::VeloCluster(liteCluster,
                                      { { liteCluster.channelID().strip(),firstADC },
                                        { liteCluster.channelID().strip()+1,secondADC } } ) );
  }
  fakeClusters->insert(fakeClus.release(),liteCluster.channelID());
}

