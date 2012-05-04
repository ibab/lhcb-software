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
#include "GaudiKernel/VectorMap.h"
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
  
  using namespace LoKi        ;
  using namespace LoKi::Types ;
  using namespace LoKi::Cuts ;
  
  if (!exist<LHCb::RecVertex::Range>( m_PVLocation )) return StatusCode::SUCCESS;
  const LHCb::RecVertex::Range tRV = get<LHCb::RecVertex::Range>( m_PVLocation );
  if (msgLevel(MSG::VERBOSE)) verbose() << "tRV size " << tRV.size() << endmsg ;
  
  LHCb::Tracks* iptracks = new LHCb::Tracks(); 
  put( iptracks, m_WithIPTrackLocation );
  if (!m_discardFromPV){
    for (std::vector< std::string >::iterator ilocation = m_TracksLocations.begin(); 
         m_TracksLocations.end() != ilocation ; ++ilocation){
      if (!exist<LHCb::Tracks>(*ilocation)) 
        return Warning( (*ilocation) +" not found", StatusCode::SUCCESS, 0);
      const LHCb::Tracks* trackContainer = get<LHCb::Tracks*>( *ilocation ) ;
      
      if (msgLevel(MSG::VERBOSE)) verbose() << "Tracks size " << trackContainer->size() << endmsg ;
      
      for( LHCb::Tracks::const_iterator itr = trackContainer->begin() ;
           itr != trackContainer->end(); ++itr ) {
        if (msgLevel(MSG::VERBOSE)) verbose() << "Track " << (*itr)->key() << " " 
                                              << (*itr)->checkFlag(LHCb::Track::Backward) << endmsg ;
        
        if ((*itr)->checkFlag(LHCb::Track::Backward))continue;
        inputtrack+=1;
        double minip= 1.e6;
        for (LHCb::RecVertex::Range::const_iterator ipv = tRV.begin();tRV.end()!=ipv;++ipv){
          double ipchi2 = 0.0 ;
          double impact = 0.0 ;
          StatusCode sc = LoKi::FastVertex::distance(*itr,*ipv,impact, ipchi2, true );
          if (msgLevel(MSG::VERBOSE)) verbose() << "IP " << impact << " " << ipchi2 << " sc=" << sc << endmsg ;
          if (sc.isFailure())continue;
          if (impact<minip){
            minip=impact;
          }
        }
        if (minip > m_ipcut) {
          iptracks ->insert( (*itr) ); 
          if (msgLevel(MSG::VERBOSE)) verbose() << "inserting track " << endmsg ;
        }
      }
    }
  }
  else{
    std::vector<int> keys;
    const LHCb::Tracks* trackContainer = get<LHCb::Tracks*>( m_TracksLocations[0]) ;
    for (LHCb::RecVertex::Range::const_iterator ipv = tRV.begin();tRV.end()!=ipv;++ipv){
      const SmartRefVector< LHCb::Track > & pVtrs =  (*ipv)->tracks();
      for (SmartRefVector< LHCb::Track >::const_iterator iitr = pVtrs.begin(); pVtrs.end()!= iitr; ++iitr ){
        if (msgLevel(MSG::VERBOSE)) verbose() << "Push_back " << (*iitr)->key() << endmsg ;
        keys.push_back((*iitr)->key());
      }
    }
    std::sort(keys.begin(),keys.end());
    unsigned int keyIndex = 0;
    for( LHCb::Tracks::const_iterator itr = trackContainer->begin() ;
         itr != trackContainer->end(); ++itr ) {
      if ( keyIndex > keys.size()) {
        warning() << "Key index " << keyIndex << " larger than keys size " << keys.size() << endmsg ;
        return StatusCode::FAILURE ;
      }
      if( keyIndex == keys.size() ){
        if (msgLevel(MSG::VERBOSE)) verbose() << "List finninshed Insert ? " << (*itr)->key() << " " << (*itr)->checkFlag(LHCb::Track::Backward)   << endmsg ;
        if ((*itr)->checkFlag(LHCb::Track::Backward))continue;
        iptracks->insert((*itr));
        continue;
      }
      if ((*itr)->key() < keys[keyIndex]){
        if (msgLevel(MSG::VERBOSE)) verbose() << "Insert ? " << (*itr)->key() << " " << (*itr)->checkFlag(LHCb::Track::Backward)   << endmsg ;
        if ((*itr)->checkFlag(LHCb::Track::Backward))continue;
        iptracks->insert((*itr));
      }
      else{
        keyIndex++;
      }
    }
    //if(maxKey2<maxkeyIndex)fatal() << "Key in PV does not exist in velo 2"<< endmsg ;

  }
  if (msgLevel(MSG::DEBUG))  debug() << " input " << inputtrack << " output " << iptracks->size() << endmsg ;

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
