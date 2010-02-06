// $Id: ResidualsM1.cpp,v 1.1 2010-02-06 11:14:28 svecchi Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Event/Track.h"
#include "Event/State.h"
#include "MuonTrackRec/MuonLogPad.h"

// Interface
#include "MuonDet/DeMuonDetector.h"
#include "MuonTrackRec/IMuonHitDecode.h"
#include "MuonTrackRec/IMuonPadRec.h"
#include "MuonDet/IMuonFastPosTool.h"
#include "TrackInterfaces/ITrackExtrapolator.h"

// Tools for histogramming
#include "AIDA/IHistogram1D.h"

#include <boost/lexical_cast.hpp>
// local
#include "ResidualsM1.h"
using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : ResidualsM1
//
// 2010-01-22 : Stefania Vecchi
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ResidualsM1 );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ResidualsM1::ResidualsM1( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
{
  declareProperty( "TracksLocation"     , m_tTracksLocation      = TrackLocation::Default     );
  declareProperty( "Extrapolator"       , m_nExtrapolator        = "TrackLinearExtrapolator"  );
  declareProperty( "WhichStation"       , m_iMS =0);
  declareProperty( "nFOI"               , m_nFOI =6.);
  declareProperty( "doHistos"           , m_do_histos = true);
  declareProperty( "doNtuple"           , m_do_ntuple = false);
}
//=============================================================================
// Destructor
//=============================================================================
ResidualsM1::~ResidualsM1() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode ResidualsM1::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  m_extrapolator   = tool<ITrackExtrapolator>(  m_nExtrapolator , "Extrapolator"  , this );
  m_muonDet=getDet<DeMuonDetector>(DeMuonLocation::Default); //non funziona, si rompe
  m_zM1 = m_muonDet->getStationZ(m_iMS);


  // 1)

  debug() << "init hit decoding" << endmsg;  
  m_decTool = tool<IMuonHitDecode>( "MuonMonHitDecode" );//,"DecodingTool",this); // alternative is "MuonMonHitDecode"
  if( !m_decTool ) {
    err() << "error retrieving the MuonHitDecode tool" << endreq;
    return StatusCode::FAILURE;
  }
  
  // 2)
  m_padTool = tool<IMuonPadRec>( "MuonPadRec" );
  if( !m_padTool ){    
    err() << "error retrieving the MuonRecPad tool " << endreq;
    return StatusCode::FAILURE;
  }
  
  // 3)
  // tile positioning tool
  m_posTool = tool<IMuonFastPosTool>("MuonDetPosTool");
  if( !m_posTool ){
    error() << "error retrieving the positioning tool " << endreq;
    return StatusCode::FAILURE;
  }

  std::string name;
  for (int iR=0; iR<nREGIONS; ++iR){
    // in the "signal" region +/- 6 error units respect the track extrapolation point
    unsigned int nbin=100;
    double max = 80. + 40.*float(iR);
    double min = -max;
    name = "resX_sig_ASide_M1R"+ boost::lexical_cast<std::string>(iR+1);
    m_S_resx_a[iR] = book1D( name, name, min, max, nbin );
    name = "resY_sig_ASide_M1R"+ boost::lexical_cast<std::string>(iR+1);
    m_S_resy_a[iR] = book1D( name, name, min, max, nbin );
    name = "resX_sig_CSide_M1R"+ boost::lexical_cast<std::string>(iR+1);
    m_S_resx_c[iR] = book1D( name, name, min, max, nbin );
    name = "resY_sig_CSide_M1R"+ boost::lexical_cast<std::string>(iR+1);
    m_S_resy_c[iR] = book1D( name, name, min, max, nbin );
  
    // in the "background" region +/- 6 error units respect the opposite track extrapolation point
    name = "resX_bkg_ASide_M1R"+ boost::lexical_cast<std::string>(iR+1);
    m_B_resx_a[iR] = book1D( name, name, min, max, nbin );
    name = "resY_bkg_ASide_M1R"+ boost::lexical_cast<std::string>(iR+1);
    m_B_resy_a[iR] = book1D( name, name, min, max, nbin );
    name = "resX_bkg_CSide_M1R"+ boost::lexical_cast<std::string>(iR+1);
    m_B_resx_c[iR] = book1D( name, name, min, max, nbin );
    name = "resY_bkg_CSide_M1R"+ boost::lexical_cast<std::string>(iR+1);
    m_B_resy_c[iR] = book1D( name, name, min, max, nbin );
  }
  

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ResidualsM1::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  const Tracks*       tTracks = NULL; 

  if ( exist<LHCb::Tracks>( m_tTracksLocation )){
    tTracks = get<Tracks>( m_tTracksLocation );
    if (tTracks->size()==0)   return StatusCode::SUCCESS;

    // retrieve logical channels 
    const std::vector<MuonLogHit*> * logHits =  m_decTool->hits();
    // retrieve the pads 
    if(logHits) {
      StatusCode sc = m_padTool->buildLogicalPads(logHits);
      if (!sc) return sc;
    }
    const std::vector<MuonLogPad*>* coords = m_padTool->pads(); 

    if( coords == NULL || coords->size() == 0 ) {
      debug() << " No hits retrieved , skip event" << endmsg;
      return StatusCode::SUCCESS;    
    }
  
    Tuple m_Tuple = nTuple("residuals","residuals");
    
    int ntTracks(0);
    info()<<" Found "<<tTracks->size() << " tracks in the container "<<endmsg;
    for ( Tracks::const_iterator t = tTracks->begin(), tEnd = tTracks->end(); t != tEnd; ++t ) {
      if((*t)->chi2PerDoF()>10) continue;

      
      /// Get the T state closest to this z
      State* tState = &(*t)->closestState( m_zM1 );
      StatusCode sc = m_extrapolator->propagate( *tState, m_zM1 , LHCb::ParticleID(13)  );
      if ( !sc.isSuccess() ) {
        Warning( "Could not propagate Track state on M1"   , StatusCode::FAILURE, 5 );
        continue;
      }
      
      double x_fit = tState->x();
      double y_fit = tState->y();
      double z_fit = m_zM1;
      double d2x_fit = tState->errX2();
      double d2y_fit = tState->errY2();
      double tx_fit = tState->tx();
      double ty_fit = tState->ty();
      double d2tx_fit = tState->errTx2();
      double d2ty_fit = tState->errTy2();
      int chnum(-99);
      int regnum(-99);
      sc = m_muonDet->Pos2StChamberNumber( x_fit, y_fit, m_iMS , chnum, regnum);
      if ( !sc.isSuccess() ) continue;
      

      ntTracks++;
      if(ntTracks >  m_track_max - 1 ) continue;  // to protect the ntuple filling
      
      m_x_fit.push_back(x_fit);
      m_y_fit.push_back(y_fit);
      m_z_fit.push_back(z_fit);
      m_dx_fit.push_back(sqrt(d2x_fit));
      m_dy_fit.push_back(sqrt(d2y_fit));
      
      m_tx_fit.push_back(tx_fit);
      m_ty_fit.push_back(ty_fit);
      m_dtx_fit.push_back(sqrt(d2tx_fit));
      m_dty_fit.push_back(sqrt(d2ty_fit));
      
      m_momentum.push_back(float((*t)->charge())*(*t)->p());
      m_chi2.push_back((*t)->chi2PerDoF());
      m_region.push_back(regnum);
      
      
      std::vector<float> m_x_hit( m_hit_max, 0.);
      std::vector<float> m_y_hit( m_hit_max, 0.);
      std::vector<float> m_z_hit( m_hit_max, 0.);
      std::vector<float> m_Dx_hit( m_hit_max, 0.);
      std::vector<float> m_Dy_hit( m_hit_max, 0.);
      
      std::vector<float> m_x_noise( m_hit_max, 0.);
      std::vector<float> m_y_noise( m_hit_max, 0.);
      std::vector<float> m_z_noise( m_hit_max, 0.);
      std::vector<float> m_Dx_noise( m_hit_max, 0.);
      std::vector<float> m_Dy_noise( m_hit_max, 0.);
      
      int noKFOI(0), nKoFOI(0);

      for(std::vector<MuonLogPad*>::const_iterator ihT = coords->begin(); ihT != coords->end() ; ihT++ ) { // loop on all the hits
        if( !(*ihT)->truepad() ) {
          debug() << " Skip this logical pad since it is not a paired logical channel "<< endmsg;
          continue;
        }
        if ( m_iMS == int((*ihT)->tile()->station()) ) { // only the Chosen station

          double x_hit,dx_hit,y_hit,dy_hit,z_hit,dz_hit;
          StatusCode sc = m_posTool->calcTilePos(*(*ihT)->tile(),x_hit,dx_hit,y_hit,dy_hit,z_hit,dz_hit);
          if ( !sc.isSuccess() ) continue;
          
          double deltaX = fabs( x_fit - x_hit ) / ( sqrt( d2x_fit + pow(dx_hit,2)) );
          double deltaY = fabs( y_fit - y_hit ) / ( sqrt( d2y_fit + pow(dy_hit,2)) );          

          if ( deltaX < m_nFOI && deltaY < m_nFOI ) {

            sc = m_extrapolator->propagate( *tState, z_hit,  LHCb::ParticleID(13)  );
 
           if( sc.isSuccess() ) {

              AIDA::IHistogram1D *tempx, *tempy;
              
              tempx = x_hit > 0 ? m_S_resx_a[ (*ihT)->tile()->region() ] : m_S_resx_c[ (*ihT)->tile()->region() ];
              tempy = x_hit > 0 ? m_S_resy_a[ (*ihT)->tile()->region() ] : m_S_resy_c[ (*ihT)->tile()->region() ];	  	  
              
              tempx->fill( tState->x() - x_hit );  // X residuals on the same Z as the hit
              tempy->fill( tState->y() - y_hit );  // Y residuals on the same Z as the hit
              
              if( noKFOI < m_hit_max ) {
                m_x_hit[noKFOI] =  x_hit ;
                m_y_hit[noKFOI] =  y_hit ;
                m_z_hit[noKFOI] =  z_hit ;
                m_Dx_hit[noKFOI]= sqrt( pow(dx_hit,2) + d2x_fit );
                m_Dy_hit[noKFOI]= sqrt( pow(dy_hit,2) + d2y_fit );
              }              
              noKFOI ++; // Number of hits in the right FOI
            }            
          }
          
          // fill the Hits in the opposite quadrant
          deltaX = fabs( x_fit + x_hit ) / ( sqrt( d2x_fit + pow(dx_hit,2)) );
          deltaY = fabs( y_fit + y_hit ) / ( sqrt( d2y_fit + pow(dy_hit,2)) );          

          if ( deltaX < m_nFOI && deltaY < m_nFOI ) {            
            sc = m_extrapolator->propagate( *tState, z_hit,  LHCb::ParticleID(13)  );

            if( sc.isSuccess() ) {              
 
             AIDA::IHistogram1D *tempx, *tempy;
              
              tempx = x_hit > 0 ? m_B_resx_a[ (*ihT)->tile()->region() ] : m_B_resx_c[ (*ihT)->tile()->region() ];
              tempy = x_hit > 0 ? m_B_resy_a[ (*ihT)->tile()->region() ] : m_B_resy_c[ (*ihT)->tile()->region() ];	  	  
              
              tempx->fill( tState->x() - x_hit ); // X residuals on the same Z as the hit
              tempy->fill( tState->y() - y_hit ); // Y residuals on the same Z as the hit
              
              if( nKoFOI < m_hit_max ) {                
                m_x_noise[nKoFOI] =  x_hit;
                m_y_noise[nKoFOI] =  y_hit;
                m_z_noise[nKoFOI] =  z_hit;
                m_Dx_noise[nKoFOI]= sqrt( pow(dx_hit,2) + d2x_fit );
                m_Dy_noise[nKoFOI]= sqrt( pow(dy_hit,2) + d2y_fit );
              }              
              nKoFOI ++; // Number of hits in the opposite FOI
            }            
          }          
        } // end select the station
      } // end of loop on all  hits        
        
      mm_x_hit.push_back(m_x_hit);
      mm_y_hit.push_back(m_y_hit);
      mm_z_hit.push_back(m_z_hit);
      mm_Dx_hit.push_back(m_Dx_hit);
      mm_Dy_hit.push_back(m_Dy_hit);

      mm_x_noise.push_back(m_x_noise);
      mm_y_noise.push_back(m_y_noise);
      mm_z_noise.push_back(m_z_noise);
      mm_Dx_noise.push_back(m_Dx_noise);
      mm_Dy_noise.push_back(m_Dy_noise);

      m_hit.push_back(noKFOI);
      m_noise.push_back(nKoFOI);        
      
      debug()<<
        " found a track "<<x_fit<<" "<<y_fit<<" n. Hits in FOI ="<<noKFOI<<
        " n. Hits in opposite Quadrant FOI "<< nKoFOI<<endmsg;
    }
    
    if(m_do_ntuple){      
      m_Tuple->farray("x_fit",m_x_fit.begin(),m_x_fit.end(),"Tracks",m_track_max);
      m_Tuple->farray("y_fit",m_y_fit.begin(),m_y_fit.end(),"Tracks",m_track_max);
      m_Tuple->farray("z_fit",m_z_fit.begin(),m_z_fit.end(),"Tracks",m_track_max);
      m_Tuple->farray("dx_fit",m_dx_fit.begin(),m_dx_fit.end(),"Tracks",m_track_max);
      m_Tuple->farray("dy_fit",m_dy_fit.begin(),m_dy_fit.end(),"Tracks",m_track_max);
      m_Tuple->farray("tx_fit",m_tx_fit.begin(),m_tx_fit.end(),"Tracks",m_track_max);
      m_Tuple->farray("ty_fit",m_ty_fit.begin(),m_ty_fit.end(),"Tracks",m_track_max);
      m_Tuple->farray("dtx_fit",m_dtx_fit.begin(),m_dtx_fit.end(),"Tracks",m_track_max);
      m_Tuple->farray("dty_fit",m_dty_fit.begin(),m_dty_fit.end(),"Tracks",m_track_max);
      m_Tuple->farray("chi2_fit",m_chi2.begin(),m_chi2.end(),"Tracks",m_track_max);
      m_Tuple->farray("momentum",m_momentum.begin(),m_momentum.end(),"Tracks",m_track_max);
      m_Tuple->farray("Region"  , m_region.begin() ,m_region.end()  ,"Tracks",m_track_max);
      
      m_Tuple->farray("hit",m_hit.begin(),m_hit.end(), "Tracks",m_track_max); //N. of Hits in the signal region

      //for each Hits in the right position
      m_Tuple->fmatrix("hit_x",mm_x_hit.begin(),mm_x_hit.end(),m_hit_max, "Tracks",m_track_max); // x Hits
      m_Tuple->fmatrix("hit_y",mm_y_hit.begin(),mm_y_hit.end(),m_hit_max, "Tracks",m_track_max); // y Hits
      m_Tuple->fmatrix("hit_z",mm_z_hit.begin(),mm_z_hit.end(),m_hit_max, "Tracks",m_track_max); // z Hits
      m_Tuple->fmatrix("hit_Dx",mm_Dx_hit.begin(),mm_Dx_hit.end(),m_hit_max, "Tracks",m_track_max); //Delta x
      m_Tuple->fmatrix("hit_Dy",mm_Dy_hit.begin(),mm_Dy_hit.end(),m_hit_max, "Tracks",m_track_max); //Delta y
      
      // Noise
      m_Tuple->farray("noise",m_noise.begin(),m_noise.end(), "Tracks",m_track_max); // N. of Hits in the opposite quadrant
      //for each Hits in the opposite quadrant
      m_Tuple->fmatrix("noise_x",mm_x_noise.begin(),mm_x_noise.end(),m_hit_max, "Tracks",m_track_max); // x Hits 
      m_Tuple->fmatrix("noise_y",mm_y_noise.begin(),mm_y_noise.end(),m_hit_max, "Tracks",m_track_max); // y Hits
      m_Tuple->fmatrix("noise_z",mm_z_noise.begin(),mm_z_noise.end(),m_hit_max, "Tracks",m_track_max); // z Hits
      m_Tuple->fmatrix("noise_Dx",mm_Dx_noise.begin(),mm_Dx_noise.end(),m_hit_max, "Tracks",m_track_max); //Delta x
      m_Tuple->fmatrix("noise_Dy",mm_Dy_noise.begin(),mm_Dy_noise.end(),m_hit_max, "Tracks",m_track_max); //Delta y
      
      m_Tuple->write();      
    }
  }
  clearArrays();
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ResidualsM1::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiTupleAlg::finalize();  // must be called after all other actions
}

//=============================================================================
void ResidualsM1::clearArrays() {
  m_x_fit.clear();
  m_y_fit.clear();
  m_z_fit.clear();
  m_dx_fit.clear();
  m_dy_fit.clear();
  m_tx_fit.clear();
  m_ty_fit.clear();
  m_dtx_fit.clear();
  m_dty_fit.clear();

  m_chi2.clear();
  m_momentum.clear();
  m_region.clear();
  

  m_hit.clear();
  m_noise.clear();
  mm_x_hit.clear();
  mm_y_hit.clear();
  mm_z_hit.clear();
  mm_Dx_hit.clear();
  mm_Dy_hit.clear();
  mm_x_noise.clear();
  mm_y_noise.clear();
  mm_z_noise.clear();
  mm_Dx_noise.clear();
  mm_Dy_noise.clear();
}

