// $Id: CaloCosmicsTrackAlg.cpp,v 1.1 2008-05-22 22:47:59 odescham Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
// from LHCb
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Plane3DTypes.h"
#include "LHCbMath/LineTypes.h"
#include "LHCbMath/GeomFun.h"

// local
#include "CaloCosmicsTrackAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloCosmicsTrackAlg
//
// 2008-04-08 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CaloCosmicsTrackAlg );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloCosmicsTrackAlg::CaloCosmicsTrackAlg( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
{

  declareProperty("TrackTool" , m_trackToolType = "CaloCosmicsTrackTool");
  declareProperty("ForwardTrackContainer"   , m_forward="Calo/Track/Forward");
  declareProperty("BackwardTrackContainer"  , m_backward="Calo/Track/Backward");
  declareProperty("Monitor"   , m_monitor=false);

}
//=============================================================================
// Destructor
//=============================================================================
CaloCosmicsTrackAlg::~CaloCosmicsTrackAlg() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CaloCosmicsTrackAlg::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiHistoeAlg

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  // tools
  m_caloTrack = tool<ICaloCosmicsTrackTool>(m_trackToolType , "TrackTool", this);

  if(m_monitor){
    book1D( "Rec/1"  ,  "Reconstruction performance"   ,  0. , 5.   , 5);
    book2D( "Rec/2"  ,  "Ecal versus Hcal timing slot" ,  -25 , 25. , 2  , -25. , 25. , 2 );
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CaloCosmicsTrackAlg::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  setFilterPassed(false);

  if(m_monitor)fill( histo1D(HistoID("Rec/1"))  ,  0. , 1.); // bin0 = counter

  // create and store container
  LHCb::Tracks* forwards = new LHCb::Tracks();
  LHCb::Tracks* backwards = new LHCb::Tracks();
  put(forwards, m_forward);
  put(backwards, m_backward);

  // process tracking
  StatusCode esc = m_caloTrack->processing();
  if(!m_caloTrack->tracked()){
    debug() << "No track reconstructed" << endreq;
    return StatusCode::SUCCESS;
  }
  
      
  //store tracks
  LHCb::Track* track = new LHCb::Track();
  track->copy( m_caloTrack->track() );
  ( m_caloTrack->forward() ) ?  forwards->insert( track ) : backwards->insert( track );

  // Checks
  debug() << " Track ---- " << endreq;
  debug() << " backward  : "<< track->checkFlag(LHCb::Track::Backward) << endreq;
  debug() << " Timed : " << track->checkFlag(LHCb::Track::Selected) << " / " << m_caloTrack->timed() << endreq;
  debug() << " firstState : "<< track->firstState() << endreq;
  debug() << " Time : " << track->info(LHCb::Track::Likelihood, -999.)<< endreq;
  debug() << " Chi2 : " << track->info(LHCb::Track::MatchChi2, 999.)<< endreq; 
  double z = 8000;
  m_caloTrack->propagate( z );
  debug() << "Propagated track ---" << endreq;
  debug() << "Position : " << m_caloTrack->referencePoint() << endreq;
  debug() << "Position Error : " << sqrt(m_caloTrack->referencePointCovariance()) << endreq;
  debug() << "Time : " << m_caloTrack->time() << endreq;
    
  // Monitor
  if(m_monitor 
     && m_caloTrack->tracked())fill( histo1D(HistoID("Rec/1"))  ,  1. , 1.); // bin1 = reconstructed
  if(m_monitor 
     && m_caloTrack->timed())fill( histo1D(HistoID("Rec/1"))  ,  2. , 1.); // bin2 = timed (ecal or hcal)
  if(m_monitor 
     && m_caloTrack->ecal()->timed() 
     && m_caloTrack->hcal()->timed()){
    fill( histo1D(HistoID("Rec/1"))  ,  3. , 1.); // bin2 = ecal+hcal timing
    fill( histo2D(HistoID("Rec/2"))  , m_caloTrack->ecal()->slot()+12.5, m_caloTrack->hcal()->slot()+12.5, 1. );
  }
  


  if( m_caloTrack->tracked())setFilterPassed(true);
 
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CaloCosmicsTrackAlg::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiTupleAlg::finalize();  // must be called after all other actions
}
