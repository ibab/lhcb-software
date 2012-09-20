
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/Track.h"
#include "Event/MCParticle.h"
#include "Linker/LinkedTo.h"
#include "LHCbMath/Line.h"
#include "LHCbMath/GeomFun.h"
#include "Event/MCHit.h"

#include "Event/RecVertex.h"

#include "Event/VPCluster.h"
#include "Event/VPLiteMeasurement.h"
// local
#include "VPVertices.h"

#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/Vector4DTypes.h"
#include "GaudiKernel/IMagneticFieldSvc.h"
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"
#include "Kernel/ParticleID.h"
#include "TrackKernel/TrackTraj.h"

#include "Kernel/ITrajPoca.h"
#include "TrackInterfaces/ITrackVertexer.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "TrackInterfaces/ITrackInterpolator.h"
#include <iterator>


//-----------------------------------------------------------------------------
// Implementation file for class : VPVertices
//
// 2006-05-11 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( VPVertices )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VPVertices::VPVertices( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
{
  declareProperty( "NtupleName"           , m_tupleName = "V0s"    );
  declareProperty( "V0Container", m_v0ContainerName = "Rec/Track/Forward" ) ;

}
//=============================================================================
// Destructor
//=============================================================================
VPVertices::~VPVertices() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode VPVertices::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  m_positiontool =  tool<IVPClusterPosition>("VPClusterPosition");
  m_linkTool = tool<ILHCbIDsToMCHits>("LHCbIDsToMCHits","IDsToMCHits",this);
  m_vP = getDet<DeVP>( DeVPLocation::Default );
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode VPVertices::execute() {

  debug() << "==> Execute" << endmsg;
  LinkedTo<LHCb::MCParticle, LHCb::Track> link( evtSvc(), msgSvc(), LHCb::TrackLocation::Default);

  // Get the primary vertices. Locate the one that's most downstream.
  const LHCb::RecVertices* pvcontainer = get<LHCb::RecVertices>( LHCb::RecVertexLocation::Primary ) ;
  if( pvcontainer->empty() ) 
    return Warning("No primary verticesfound. Skip V0 search",StatusCode::SUCCESS,0) ;
  //always()<<"tag1"<<endreq;
  
  for( LHCb::RecVertices::const_iterator ipv = pvcontainer->begin() ; 
       ipv != pvcontainer->end(); ++ipv ) {
  //always()<<"tag2"<<endreq;
    std::vector< std::pair< const LHCb::MCVertex*, float > > MothVertex ;
  //always()<<"tag3"<<endreq;
    for (SmartRefVector< LHCb::Track >::const_iterator itr = (*ipv)->tracks().begin() ; (*ipv)->tracks().end() != itr ; ++itr ){ 
  //always()<<"tag4"<<endreq;
      const LHCb::Track* tr = (*itr);
  //always()<<"tag5"<<endreq;
      LHCb::MCParticle* part = link.first( tr );
      
  //always()<<"tag6"<<endreq;
  if(part==NULL){ 
        part = link.next();
        continue;
      }
  if(part->originVertex()==NULL){ 
        part = link.next();
        continue;
      }
      Gaudi::XYZPoint mcPosition = part->originVertex()->position();
      if ( mcPosition.z() > 700. ) {  // ignore tracks created after the Velo.
        part = link.next();
        continue;
      }
  //always()<<"tag6a"<<endreq;
      bool Isfrom = false;
  //always()<<"tag6b"<<endreq;
      for (int jj = 0 ; jj <(int) MothVertex.size(); jj++){
  //always()<<"tag6c"<<endreq;
        if(part->primaryVertex  ()->key() == MothVertex[jj].first->key()) MothVertex[jj].second += (float) part->pt();
  //always()<<"tag6d"<<endreq;
      }
  //always()<<"tag6e"<<endreq;
      if (!Isfrom) MothVertex.push_back(std::make_pair(part->primaryVertex  (),(float) part->pt()));
  //always()<<"tag6f"<<endreq;
      part = link.next();
  //always()<<"tag7"<<endreq;
    }
  //always()<<"tag8"<<endreq;
    int bestidex = -1;
    float bestweight = -1;
    for (int jj = 0 ; jj <(int) MothVertex.size(); jj++){
      if(MothVertex[jj].second>bestweight){
        bestweight=MothVertex[jj].second;
        bestidex=jj;  
      }
    }
  //always()<<"tag8.5"<<endreq;
    if (bestidex<-0.5)continue;
    
  //always()<<"tag9"<<endreq;
    Tuple tPoint = nTuple( "PVTuple" );
    tPoint->column( "mc_vx"   ,MothVertex[bestidex].first->position().x() );
    tPoint->column( "mc_vy"   , MothVertex[bestidex].first->position().y() );
    tPoint->column( "mc_vz"   , MothVertex[bestidex].first->position().z() );
    tPoint->column( "vx"   , (*ipv)->position().x() );
    tPoint->column( "vy"   , (*ipv)->position().y() );
    tPoint->column( "vz"   , (*ipv)->position().z() );
    tPoint->column( "vchi2"   , (*ipv)->chi2() );
    tPoint->column( "vchi2ndof"   , (*ipv)->chi2PerDoF() );
    tPoint->column( "ndof"   , (*ipv)->nDoF() );
    tPoint->column( "ntracks"   , (*ipv)->tracks().size() );
    tPoint->column( "evy"   , (*ipv)->covMatrix  () (1,1) );
    tPoint->column( "evx"   , (*ipv)->covMatrix () (0,0) );
    tPoint->column( "evz"   , (*ipv)->covMatrix  () (2,2) );
    
    tPoint->write();
  //always()<<"tag10"<<endreq;
  }
  
  //always()<<"tag11"<<endreq;
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode VPVertices::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiTupleAlg::finalize();  // must be called after all other actions
}

