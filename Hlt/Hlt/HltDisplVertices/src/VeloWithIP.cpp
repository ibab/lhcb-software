// $Id: $
// Include files 

#include "VeloWithIP.h"
#include "KalmanFilter/FastVertex.h"
#include "LoKi/Vertices.h"
#include "Event/RecVertex.h"
#include "HltBase/HltUtils.h"
#include "LoKi/PhysTypes.h"
#include "LoKi/Vertices.h"
#include "LoKi/BeamLineFunctions.h"
#include "LoKi/LoKiPhys.h"
#include "LoKi/LoKiTrigger.h"
// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( VeloWithIP );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VeloWithIP::VeloWithIP( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : GaudiAlgorithm( name , pSvcLocator )
{
  declareProperty( "Inputs", m_TracksLocations ) ;
  declareProperty( "Output", m_WithIPTrackLocation = "Hlt2/VeloWithIP/Track" );
  declareProperty( "PVLocation", m_PVLocation = "Hlt/Vertex/PV3D" );
  declareProperty( "DiscardFromPV", m_discardFromPV = false );
  declareProperty( "MinIP", m_ipcut = 0.3 );
  
}
//=============================================================================
// Destructor
//=============================================================================
VeloWithIP::~VeloWithIP() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode VeloWithIP::initialize() {
  if( m_TracksLocations.size() == 0 ){
    m_TracksLocations.push_back("Hlt2/Track/Velo");
  }
  return GaudiAlgorithm::initialize(); // must be executed first

}
//=============================================================================
// Main execution
//=============================================================================
StatusCode VeloWithIP::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  setFilterPassed(false);
  //always()<<m_ipcut<<endreq;
  int inputtrack (0);
  
  LoKi::Types::VFun beamSpotCut = LoKi::Cuts::VX_BEAMSPOTRHO( 1. ) ;
  using namespace LoKi        ;
  using namespace LoKi::Types ;
  using namespace LoKi::Cuts ;
  
  if (!exist<LHCb::RecVertex::Range>( m_PVLocation )) return StatusCode::SUCCESS;
  const LHCb::RecVertex::Range tRV = get<LHCb::RecVertex::Range>( m_PVLocation );
  
  LHCb::Tracks* iptracks = new LHCb::Tracks(); 
  put( iptracks, m_WithIPTrackLocation );
  if (!m_discardFromPV){
    for (std::vector< std::string >::iterator ilocation = m_TracksLocations.begin(); m_TracksLocations.end() != ilocation ; ++ilocation){
      if (!exist<LHCb::Tracks>(*ilocation)) 
        return Warning( (*ilocation) +" not found", StatusCode::SUCCESS, 0);
      const LHCb::Tracks* trackContainer = get<LHCb::Tracks*>( *ilocation ) ;
      
      for( LHCb::Tracks::const_iterator itr = trackContainer->begin() ;
           itr != trackContainer->end(); ++itr ) {
        if ((*itr)->checkFlag(LHCb::Track::Backward))continue;
        inputtrack+=1;
        double minip= 1.e6;
        const LHCb::RecVertex* minV=NULL;
        for (LHCb::RecVertex::Range::const_iterator ipv = tRV.begin();tRV.end()!=ipv;++ipv){
          double ipchi2 = 0.0 ;
          double impact = 0.0 ;
          StatusCode sc = LoKi::FastVertex::distance(*itr,*ipv,impact, ipchi2, true );
          if (sc.isFailure())continue;
          if (impact<minip){
            minip=impact;
            minV = *ipv;
          }
        }
        if (minip > m_ipcut) iptracks ->insert( (*itr) ); 
      }
    }
  }
  else{
    std::vector<int> keys;
    keys.reserve(500);
    const LHCb::Tracks* trackContainer = get<LHCb::Tracks*>( m_TracksLocations[0]) ;
    
    for (LHCb::RecVertex::Range::const_iterator ipv = tRV.begin();tRV.end()!=ipv;++ipv){
      const SmartRefVector< LHCb::Track > & pVtrs =  (*ipv)->tracks();
      for (SmartRefVector< LHCb::Track >::const_iterator iitr = pVtrs.begin(); pVtrs.end()!= iitr; ++iitr ){
        keys.push_back((*iitr)->key());
      }
    }
    std::sort(keys.begin(),keys.end());
    int keyIndex = 0;
    for( LHCb::Tracks::const_iterator itr = trackContainer->begin() ;
         itr != trackContainer->end(); ++itr ) {
      if ((*itr)->key() < keys[keyIndex]){
        if ((*itr)->checkFlag(LHCb::Track::Backward))continue;
        iptracks->insert((*itr));
      }
      else{
        keyIndex++;
      }
    }
  }
  counter ("#input" ) += inputtrack;
  counter ("#passed" ) += iptracks->size();
  if (iptracks->size()>0)  setFilterPassed(true);
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode VeloWithIP::finalize() {

  if( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();
}
