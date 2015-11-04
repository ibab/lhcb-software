// Include files 

 // from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "VPTrackMoni.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VPTrackMoni
//
// 2015-01-08 : Christoph Hombach
//-----------------------------------------------------------------------------



// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( VPTrackMoni )

using namespace Gaudi;
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VPTrackMoni::VPTrackMoni( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
{
  declareProperty( "TrackContainer" , m_tracklocation = LHCb::TrackLocation::Default );
  
}
//=============================================================================
// Destructor
//=============================================================================
VPTrackMoni::~VPTrackMoni() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode VPTrackMoni::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  m_veloDet_VP = getDet<DeVP>(  DeVPLocation::Default ) ;
  return StatusCode::SUCCESS;
}


//=============================================================================
// Main execution
//=============================================================================
StatusCode VPTrackMoni::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  
  //Get tracks
  const LHCb::Track::Range tracks = get<LHCb::Track::Range>(m_tracklocation);
  BOOST_FOREACH(const LHCb::Track* track, tracks ) 
  {
    double chi2NDOF = track->chi2PerDoF();
    double probChi2 = track->probChi2();
    double phi      = track->phi();
    double eta      = track->pseudoRapidity();
    

    LHCb::Track::Types type = track->type();
    
    if (type != LHCb::Track::Types::Velo && type != LHCb::Track::Types::Long) continue;
    bool fitStatus = track->checkFitStatus(LHCb::Track::FitStatus::Fitted);

    if (!fitStatus) continue;
    LHCb::Track::ConstNodeRange nodes = track->nodes();
    unsigned int nClusters = nodes.size();
    for(LHCb::Track::ConstNodeRange::iterator inode = nodes.begin(); inode != nodes.end(); ++inode)
    {
      
      if ((*inode)->type() == LHCb::Node::HitOnTrack)
      {if ( ((*inode)->measurement()).type() == LHCb::Measurement::VP )
        {
        
          LHCb::LHCbID nodeID = ((*inode)->measurement()).lhcbID();
      
          LHCb::VPChannelID chID = nodeID.vpID();
          const DeVPSensor* sens = (DeVPSensor*)m_veloDet_VP->sensorOfChannel(chID);
          
          unsigned int module  = sens->module();
          unsigned int station = sens->station();
          bool isRight         = sens->isRight();
          bool isLeft          = sens->isLeft();
          double meas_error      = (*inode)->errMeasure();
          
          
      
          XYZPoint SensEdge_loc(0., 0., 0.);
          XYZPoint SensEdge    = sens->localToGlobal( SensEdge_loc );
          
          XYZPoint cluPos     = (*inode)->state().position();
        
          XYZPoint residual = getResidual(track, sens, cluPos);
          double   nodeResidual = (*inode)->residual();
          Tuple theTuple = nTuple( "VPTrackMonitor" , "" );
          theTuple->column("resX", residual.x() );
          theTuple->column("resY", residual.y() );
          theTuple->column("nodeResidual", nodeResidual ),
          theTuple->column("clusX", cluPos.x() );
          theTuple->column("clusY", cluPos.y() );
          theTuple->column("clusZ", cluPos.z() );
          theTuple->column("sensEdgeX", SensEdge.x() );
          theTuple->column("sensEdgeY", SensEdge.y() );
          theTuple->column("Error", meas_error );
          theTuple->column("module", module    );
          theTuple->column("station", station  );
          theTuple->column("isRight", isRight  );
          theTuple->column("isLeft" , isLeft   );

          theTuple->column("phi"       , phi );
          theTuple->column("eta"       , eta );
          
          theTuple->column("chi2PerDoF", chi2NDOF );
          theTuple->column("probChi2"  , probChi2 );
          theTuple->column("nClusters" , nClusters );
          theTuple->column("TrackType" , type );
      
      
          theTuple->write();
        }
        
      }
    }
  } 
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode VPTrackMoni::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiTupleAlg::finalize();  // must be called after all other actions
}

//=============================================================================

XYZPoint VPTrackMoni::getResidual(const LHCb::Track* track,const DeVPSensor* sens, XYZPoint hit)
{
  debug() << "In getResidual" << endmsg;
  XYZPoint p_e_loc(0., 0., 0.);
  XYZPoint   n_loc(0., 0., 1.);
  XYZPoint p_e = sens->localToGlobal( p_e_loc );
  double n_x   = sens->localToGlobal( n_loc   ).x()-p_e.x();
  double n_y   = sens->localToGlobal( n_loc   ).y()-p_e.y();
  double n_z   = sens->localToGlobal( n_loc   ).z()-p_e.z();
  
  XYZVector slope = track->slopes();
  XYZPoint p_T    = track->position();
  
  debug() << "p_e: " << p_e.x() << " " << p_e.y() << " " << p_e.z() << endmsg;
  debug() << "n: " << n_x << " " << n_y << " " << n_z << endmsg;

  double sl_n   = n_x*slope.x()+n_y*slope.y()+n_z*slope.z();
  double p_eT_n = n_x*(p_e.x()-p_T.x())+n_y*(p_e.y()-p_T.y())+n_z*(p_e.z()-p_T.z());
  double inter_x = p_T.x()+p_eT_n/sl_n*slope.x();
  double inter_y = p_T.y()+p_eT_n/sl_n*slope.y();
  double inter_z = p_T.z()+p_eT_n/sl_n*slope.z();
  
  XYZPoint inter(inter_x, inter_y, inter_z);  
  debug() << "inter: " << inter_x << " " << inter_y << " " << inter_z << endmsg;
  debug() << "hit:   " << hit.x() << " " << hit.y() << " " <<hit.z();
  
  XYZPoint residual(inter.x()-hit.x(),inter.y()-hit.y(),inter.z()-hit.z());
  return residual;
  
}
