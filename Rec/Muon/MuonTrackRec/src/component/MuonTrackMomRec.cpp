// Include files 
#include "GaudiKernel/IIncidentSvc.h" 
#include "GaudiKernel/PhysicalConstants.h" 
#include "Kernel/IBIntegrator.h"

#include "MuonInterfaces/MuonTrack.h"
#include "MuonInterfaces/MuonHit.h"
#include "MuonInterfaces/MuonLogPad.h"
#include "MuonDet/DeMuonDetector.h"
#include "Kernel/ILHCbMagnetSvc.h"
#include "GaudiKernel/IUpdateManagerSvc.h"

// from GSL
#include "gsl/gsl_math.h"
 
// Boost
#include <boost/assign/list_of.hpp>
 
// from TrackEvent
#include "Event/TrackParameters.h"
#include "Event/State.h"
#include "Event/TrackTypes.h"
#include "Event/Track.h"
#include "Event/StateVector.h"

#include "MuonTrackMomRec.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MuonTrackMomRec
//
// 2010-02-10 : Giacomo Graziani
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( MuonTrackMomRec )


MuonTrackMomRec::MuonTrackMomRec( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
: GaudiTool       ( type, name , parent ),
  m_Constant      ( 0 ),
  m_bIntegrator   ( NULL ),
  m_muonDetector  ( NULL ),
  m_zCenter       ( 0    ),
  m_bdlX          ( 0    ),
  m_FieldPolarity ( 0    )
{
  declareInterface<IMuonTrackMomRec>(this);


  std::vector<double> tmp1 = boost::assign::list_of(0.015)(0.29);
  declareProperty( "resParams", m_resParams = tmp1);
  
  std::vector<double> tmp2 = boost::assign::list_of(1.04)(0.14);
  declareProperty( "ParabolicCorrection", m_ParabolicCorrection = tmp2);

  declareProperty( "ConstantCorrection",  m_Constant = 0.*Gaudi::Units::MeV );

}


MuonTrackMomRec::~MuonTrackMomRec() {} 

void MuonTrackMomRec::handle ( const Incident& incident )
{ 
  if ( IncidentType::BeginRun == incident.type() ) {
    if ( msgLevel(MSG::DEBUG) )debug() << "Run change: reinit field maps "<<endmsg;
    initBdl();
  } 
}



StatusCode MuonTrackMomRec::initialize()
{
  StatusCode sc = GaudiTool::initialize() ;
  if ( sc.isFailure() ) return sc;

  m_muonDetector=getDet<DeMuonDetector>(DeMuonLocation::Default);
  if(!m_muonDetector) return StatusCode::FAILURE;

  m_bIntegrator = tool<IBIntegrator>( "BIntegrator" );
  if(!m_bIntegrator) return StatusCode::FAILURE;

  initBdl();

  IUpdateManagerSvc* m_updMgrSvc = svc<IUpdateManagerSvc>("UpdateManagerSvc", true);
  ILHCbMagnetSvc* m_fieldSvc = svc<ILHCbMagnetSvc>("MagneticFieldSvc", true);
  m_updMgrSvc->registerCondition( this,m_fieldSvc,&MuonTrackMomRec::initBdl) ;

  sc = m_updMgrSvc->update(this) ;

  return sc;
}

StatusCode  MuonTrackMomRec::initBdl() {
  // compute integrated B field before M1 at x=y=0
  // we will use this value for all tracks
  Gaudi::XYZPoint  begin( 0., 0., 0. );
  Gaudi::XYZPoint  end( 0., 0.,  m_muonDetector->getStationZ(0) );
  Gaudi::XYZVector bdl;
  
  m_bIntegrator -> calculateBdlAndCenter(begin, end, 0.0001, 
                                         0., m_zCenter, bdl );
  if ( bdl.x() > 0.0 ) {
    m_FieldPolarity =  1;
  } 
  else {
    m_FieldPolarity = -1; 
  }
  
  m_bdlX = bdl.x();
  debug() << "Integrated B field is "<< m_bdlX << " Tm" <<
    "  centered at Z="<< m_zCenter/Gaudi::Units::m << " m"<<endmsg;

  return StatusCode::SUCCESS ;

}

StatusCode MuonTrackMomRec::finalize  ()
{
  return   GaudiTool::finalize() ;
}  
  
StatusCode MuonTrackMomRec::recMomentum(MuonTrack* track, 
                                        LHCb::Track* lbtrack)
{

  StatusCode sc =  StatusCode::SUCCESS;
  double Zfirst = m_muonDetector->getStationZ(0);
  // create a state at the Z of M1
  Gaudi::XYZPoint trackPos(track->bx() + track->sx() * Zfirst,
                           track->by() + track->sy() * Zfirst,
                           Zfirst);
  LHCb::State state( LHCb::StateVector( trackPos,
                                        Gaudi::XYZVector( track->sx(), track->sy(), 1.0 ), 1./10000.));
  
  
  // copied from the TrackPtKick tool by M. Needham
  double q = 0.;
  double p = 1e6 * Gaudi::Units::MeV;
  
  double tX;       
  double xCenter;
  double zeta_trk; 
  double tx_vtx;   
  double zeta_vtx;   

  if ( fabs( m_bdlX ) > TrackParameters::hiTolerance ) {
    //can estimate momentum and charge
    
    //Rotate to the  0-0-z axis and do the ptkick 
    tX       = state.tx();
    xCenter  = state.x() + tX * ( m_zCenter - state.z() );
    
    zeta_trk = -tX / sqrt( 1.0 + tX*tX );
    tx_vtx   = xCenter / m_zCenter;
    zeta_vtx = -tx_vtx/ sqrt( 1.0 + tx_vtx*tx_vtx );
    
    // curvature
    const double curv = ( zeta_trk - zeta_vtx );
    
    // charge
    int sign = 1;
    if( curv < TrackParameters::hiTolerance ) {
      sign *= -1;
    }
    if ( m_bdlX < TrackParameters::hiTolerance ) {
      sign *= -1;      
    }
    q = -1. * m_FieldPolarity*sign;
    
    // momentum
    p = Gaudi::Units::eplus * Gaudi::Units::c_light *fabs(m_bdlX) 
      * sqrt((1.0 +tX*tX+gsl_pow_2(state.ty()))
             /(1.0 +gsl_pow_2(tX)))/fabs(curv);
    
    //   Addition Correction factor for the angle of the track!
    if ( m_ParabolicCorrection.size() == 2u ) {
      //p*= (a + b*tx*tx ) 
      p+= m_Constant;
      p*= ( m_ParabolicCorrection[0] + (m_ParabolicCorrection[1] * tX * tX ));
    }
    
  }  
  else {
    // can't estimate momentum or charge
    error() << "B integral is 0!" << endmsg;
    sc = StatusCode::FAILURE;
    return sc;
  }
  
   
  double qOverP = q / p;  
  const double err2 = gsl_pow_2(m_resParams[0]) + gsl_pow_2(m_resParams[1]/p) ;
  double sigmaQOverP2 = err2*gsl_pow_2(1.0/p);
  
  // fill momentum variables for state
  state.setQOverP(qOverP);

  Gaudi::TrackSymMatrix seedCov;
  seedCov(0,0) = track->errbx()*track->errbx();
  seedCov(2,2) = track->errsx()*track->errsx();
  seedCov(1,1) = track->errby()*track->errby();
  seedCov(3,3) = track->errsy()*track->errsy();
  seedCov(4,4) = sigmaQOverP2;
  state.setCovariance(seedCov);

  state.setLocation( LHCb::State::Muon );

  debug() << "Muon state = " << state << endmsg;

  // set MuonTrack momentum variables (momentum at primary vertex)
  double pz_vtx =  state.p() * sqrt(1- tx_vtx*tx_vtx - state.ty()*state.ty() );
  Gaudi::XYZVector momentum_vtx( tx_vtx * pz_vtx,
                                 state.ty()* pz_vtx,
                                 pz_vtx);
  track->setP(state.p());
  track->setPt(sqrt(momentum_vtx.X()*momentum_vtx.X() + momentum_vtx.Y()*momentum_vtx.Y()));
  track->setqOverP(state.qOverP());
  track->setMomentum(momentum_vtx);



  if (lbtrack) { // create standard LHCb::Track
    lbtrack->addToStates( state );
    
    std::vector<MuonHit*> hits  = track->getHits();
    int ntile=0;    
    for ( std::vector<MuonHit*>::const_iterator h = hits.begin(); h != hits.end(); ++h ){
      const std::vector<LHCb::MuonTileID*> Tiles = (*h)->getLogPadTiles();
      debug()<< " Muon Hits has "<< (*h)->getLogPadTiles().size()<<" tiles in station "<< (*h)->station() <<endmsg;
      for (std::vector<LHCb::MuonTileID*>::const_iterator it = Tiles.begin(); it!= Tiles.end(); ++it){
        debug()<<" Tile info ====== "<< (LHCb::LHCbID) (**it)<<endmsg;
        lbtrack->addToLhcbIDs( (LHCb::LHCbID) ( **it ) );
        ntile++;        
      }
    }
    debug()<< " in total "<<ntile<<" tiles"<<endmsg;
    
    lbtrack->setPatRecStatus(  LHCb::Track::PatRecIDs );
    lbtrack->setType(  LHCb::Track::Muon );
  }
  return sc;
}
