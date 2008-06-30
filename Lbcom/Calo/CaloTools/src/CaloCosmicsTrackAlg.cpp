// $Id: CaloCosmicsTrackAlg.cpp,v 1.4 2008-06-30 08:32:18 odescham Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
// from LHCb
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Plane3DTypes.h"
#include "LHCbMath/LineTypes.h"
#include "LHCbMath/GeomFun.h"
#include "Event/Track.h"
#include "CaloInterfaces/ICaloCosmicsTool.h"

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
  declareProperty("ForwardTrackContainer"   , m_forward=LHCb::TrackLocation::CaloCosmicsForward);
  declareProperty("BackwardTrackContainer"  , m_backward=LHCb::TrackLocation::CaloCosmicsBackward);
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
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiTupleeAlg

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  // tools
  m_caloTrack = tool<ICaloCosmicsTrackTool>(m_trackToolType , "TrackTool", this);

  if(m_monitor){
    book1D( "Rec/1"   ,  "Reconstruction performance"   ,  0. , 5.   , 5);
    book2D( "Rec/2"   ,  "Ecal versus Hcal timing slot" ,  -25 , 25. , 2  , -25. , 25. , 2 );
    book1D( "Rec/Forward/1"   ,  "Reconstruction performance (forward)"   ,  0. , 5.   , 5);
    book2D( "Rec/Forward/2"   ,  "Ecal versus Hcal timing slot (forward)" ,  -25 , 25. , 2  , -25. , 25. , 2 );
    book1D( "Rec/Backward/1"  ,  "Reconstruction performance (backward)"   ,  0. , 5.   , 5);
    book2D( "Rec/Backward/2"  ,  "Ecal versus Hcal timing slot (backward)" ,  -25 , 25. , 2  , -25. , 25. , 2 );

    book1D( "Asymmetry/1",  " Ecal asymmetry" ,  -1., 1.,50);
    book1D( "Asymmetry/2",  " Hcal asymmetry" ,  -1., 1.,50);

    book1D( "Kernel/1",  " Ecal Kernel" ,  0., 1., 50);
    book1D( "Kernel/2",  " Hcal Kernel" ,  0., 1., 50);
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CaloCosmicsTrackAlg::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  setFilterPassed(false);

  if(m_monitor){
    fill( histo1D(HistoID("Rec/1"))  ,  0. , 1.); // bin0 = counter
    fill( histo1D(HistoID("Rec/Forward/1"))  ,  0. , 1.); // bin0 = counter
    fill( histo1D(HistoID("Rec/Backward/1"))  ,  0. , 1.); // bin0 = counter
  }
  
  
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
  if ( msgLevel(MSG::DEBUG) ) {
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
  }
  

    
  // Monitoring
  std::stringstream dir("");
  dir << ( m_caloTrack->forward()  ?  "Forward/" : "Backward/" );
  if(m_monitor && m_caloTrack->tracked()){    
    fill( histo1D(HistoID("Rec/"+dir.str()+"1"))  ,  1. , 1.); // bin1 = reconstructed
    fill( histo1D(HistoID("Rec/1"))  ,  1. , 1.); // bin1 = reconstructed
    fill( histo1D(HistoID("Asymmetry/1")), m_caloTrack->ecal()->asymmetry(), 1.);
    fill( histo1D(HistoID("Asymmetry/2")), m_caloTrack->hcal()->asymmetry(), 1.);
    fill( histo1D(HistoID("Kernel/1")), m_caloTrack->ecal()->kernel(), 1.);
    fill( histo1D(HistoID("Kernel/2")), m_caloTrack->hcal()->kernel(), 1.);
  }
  if(m_monitor && m_caloTrack->timed()){
    fill( histo1D(HistoID("Rec/"+dir.str()+"1"))  ,  2. , 1.); // bin2 = timed (ecal or hcal)
    fill( histo1D(HistoID("Rec/1"))  ,  2. , 1.); // bin2 = timed (ecal or hcal)
  }
  if(m_monitor && m_caloTrack->ecal()->timed() && m_caloTrack->hcal()->timed()){
    fill( histo1D(HistoID("Rec/"+dir.str()+"1"))  ,  3. , 1.); // bin2 = ecal+hcal timing
    fill( histo2D(HistoID("Rec/"+dir.str()+"2"))  , m_caloTrack->ecal()->slot()+12.5, m_caloTrack->hcal()->slot()+12.5, 1. );
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
