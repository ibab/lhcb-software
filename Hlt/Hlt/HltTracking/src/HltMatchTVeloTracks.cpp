// $Id: HltMatchTVeloTracks.cpp,v 1.2 2009-10-07 06:38:33 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "Event/Track.h" 
#include "Event/State.h"
// local
#include "HltMatchTVeloTracks.h"
#include "HltBase/HltTrackMerge.h"


//-----------------------------------------------------------------------------
// Implementation file for class : HltMatchTVeloTracks
//
// 2007-03-07 : Alessia Satta
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( HltMatchTVeloTracks )

using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltMatchTVeloTracks::HltMatchTVeloTracks( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IMatchTVeloTracks>(this);
  declareInterface<ITrackMatch>(this);

  declareProperty( "sectorToll",   m_sec_tol  =1.0    );
  
  declareProperty( "zMagCenter",   z_magnet_center =5300     );
  declareProperty( "xMatchCut",   m_x_cut  =30    );
  declareProperty( "yMatchCut",   m_y_cut  =40    ); 
  declareProperty( "spaceMatchCut",   m_space_cut =60     );  
  declareProperty( "ptkickConstant",  m_ptkickConstant  =  1263.0      );
}
//=============================================================================
// Destructor
//=============================================================================
HltMatchTVeloTracks::~HltMatchTVeloTracks() {} 

//=============================================================================
StatusCode HltMatchTVeloTracks::initialize() 
{
 StatusCode sc = GaudiTool::initialize();


  m_fastPTool = tool <ITrackMomentumEstimate>("FastMomentumEstimate");

 return StatusCode::SUCCESS;


}



StatusCode HltMatchTVeloTracks::match3dVelo(const LHCb::Track& veloTrack,
                                            const LHCb::Track& Ttrack,
                                            LHCb::Track& matchedTrack,
                                            double &x_dist,
                                            double &y_dist ){
  
  double trackDxDz = veloTrack.firstState().tx();
  double trackDyDz = veloTrack.firstState().ty();
  double trackX = veloTrack.firstState().x();
  double trackY = veloTrack.firstState().y();
  double trackZ = veloTrack.firstState().z();
  
  
 // double x_velo_mag = 0;
  double yVelo_mag= 0;
  double xVelo_mag= 0;
  //  double x_cut;
  
  //double  xmin = 99999.;
  xVelo_mag=trackDxDz*(z_magnet_center-trackZ)+trackX;
  
  

  double xT=Ttrack.firstState().x();
  double yT=Ttrack.firstState().y();
  double zT=Ttrack.firstState().z();
  double slopexT=Ttrack.firstState().tx();
  
  
    
  yVelo_mag = trackY +  (trackDyDz) * (zT - trackZ);
  double xMuon_magnet=slopexT*(z_magnet_center-zT)+xT;
  x_dist=xVelo_mag - xMuon_magnet;
  y_dist=yVelo_mag-yT;
  if(fabs(x_dist)<m_x_cut&&fabs(y_dist)<m_y_cut){
    //fill output track parameter
    Hlt::TrackMerge(veloTrack,matchedTrack);
    Hlt::TrackMerge(Ttrack,matchedTrack);
    // matchedTrack.copy(veloTrack);
    
    double zMidT =  StateParameters::ZMidT;

    const LHCb::State& veloState= veloTrack.firstState();
    const LHCb::State& TStateAtTheMiddle = Ttrack.closestState(zMidT);

    double qOverP = 0;
    double sigmaQOverP = 0; 
    StatusCode sc = m_fastPTool->calculate(&veloState, &TStateAtTheMiddle 
                                           ,qOverP, sigmaQOverP ,false );
    if (sc.isFailure()) { 
        error() << "failure of fast PT tool..." << endmsg;
        return sc;
    }

    if ( msgLevel(MSG::DEBUG) ) debug() << "new p " << 1.0/qOverP << endmsg;
    
    matchedTrack.firstState().setQOverP(qOverP);   
    matchedTrack.setHistory( Track::TrackMatching );

    // matchedTrack.addToAncestors(veloTrack);
    // matchedTrack.addToAncestors(Ttrack);


    
    // transfer the momentum
    const std::vector<LHCb::State*>& allStates = matchedTrack.states();
    if ( msgLevel(MSG::DEBUG) ) debug() << " N states " << allStates.size() << endmsg;
    for( auto i : allStates ) {
      if(i->qOverP() == 0) i->setQOverP(qOverP);
    }

    return StatusCode::SUCCESS;    
  }  
  return StatusCode::FAILURE;
}

double HltMatchTVeloTracks::momentum(const Track& velotrack,const Track& Ttrack)
{
  double trackDxDz = velotrack.firstState().tx();
  double slopeX=Ttrack.firstState().tx();
  return m_ptkickConstant/(slopeX-trackDxDz);
  
    
}


