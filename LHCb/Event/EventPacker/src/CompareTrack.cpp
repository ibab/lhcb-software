// $Id: CompareTrack.cpp,v 1.9 2009-09-01 15:17:43 ocallot Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Event/Track.h"

// local
#include "CompareTrack.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CompareTrack
//
// 2008-11-14 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CompareTrack );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CompareTrack::CompareTrack( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = LHCb::TrackLocation::Default );
  declareProperty( "TestName"  , m_testName   = LHCb::TrackLocation::Default+"Test" );
}
//=============================================================================
// Destructor
//=============================================================================
CompareTrack::~CompareTrack() {} 

//=============================================================================
// Main execution
//=============================================================================
StatusCode CompareTrack::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  LHCb::Tracks* old  = get<LHCb::Tracks>( m_inputName  );
  LHCb::Tracks* test = get<LHCb::Tracks>( m_testName  );

  if ( old->size() != test->size() ) {
    err() << "Old Track size " << old->size()
          << " differs form Test " << test->size()
          << endmsg;
    return StatusCode::FAILURE;
  }  
  LHCb::Tracks::const_iterator itOld  = old->begin();
  LHCb::Tracks::const_iterator itTest = test->begin();

  while ( old->end() != itOld ) {
    LHCb::Track* oTrack = (*itOld++);
    LHCb::Track* tTrack = (*itTest++);
    if ( oTrack->key() != tTrack->key() ) {
      warning() << "Wrong key : old " <<  oTrack->key() << " test " << tTrack->key() << endmsg;
    }
    bool isOK = true;
    if ( 1.e-7 < fabs( (oTrack->chi2PerDoF()-tTrack->chi2PerDoF())/tTrack->chi2PerDoF() ) ) isOK = false;
    if ( 0   < abs( oTrack->nDoF() - tTrack->nDoF() ) ) isOK = false;
    if ( oTrack->flags() != tTrack->flags() )              isOK = false;
    if ( oTrack->lhcbIDs().size() != tTrack->lhcbIDs().size() ) isOK = false;
    if ( 1.e-7 < fabs( (oTrack->likelihood() - tTrack->likelihood() )/ tTrack->likelihood() ) ) isOK = false;
    if ( 1.e-7 < fabs( (oTrack->ghostProbability()-tTrack->ghostProbability())/
                       tTrack->ghostProbability()) ) isOK = false;
    unsigned int kk;
    for ( kk = 0 ; oTrack->lhcbIDs().size() != kk ; ++kk ) {
      if ( oTrack->lhcbIDs()[kk].lhcbID() != tTrack->lhcbIDs()[kk].lhcbID() )     isOK = false;
    }
    LHCb::Track::ExtraInfo oExtra = oTrack->extraInfo();
    LHCb::Track::ExtraInfo tExtra = tTrack->extraInfo();
    if ( oExtra.size() != tExtra.size() ) isOK = false;
    LHCb::Track::ExtraInfo::const_iterator oIt = oExtra.begin();
    LHCb::Track::ExtraInfo::const_iterator tIt = tExtra.begin();
    for ( kk = 0; tExtra.size() > kk; ++kk, ++oIt, ++tIt ) {
      if ( (*oIt).first != (*tIt).first ) isOK = false;
      if ( 1.e-7 < fabs( ((*oIt).second - (*oIt).second ) / (*oIt).second ) ) isOK = false;
    }
    
    if ( oTrack->nStates() != tTrack->nStates() ) isOK = false;

    if ( !isOK || MSG::DEBUG >= msgLevel() ) {
      info() << "===== Track key " << oTrack->key() << endmsg;
      info() << format( "Old   chi2 %10.4f  nDoF %6i flags %8x nLhcbID %4d nExtra %4d  nStates %4d",
                        oTrack->chi2PerDoF(), oTrack->nDoF(), oTrack->flags(), 
                        oTrack->lhcbIDs().size(), oExtra.size(), oTrack->nStates() );
      info() << format( " Likelihood %10.6f ghostProba %10.8f",
                        oTrack->likelihood(), oTrack->ghostProbability() )
             << endmsg;
      info() << format( "Test  chi2 %10.4f  nDoF %6i flags %8x nLhcbID %4d nExtra %4d  nStates %4d",
                        tTrack->chi2PerDoF(), tTrack->nDoF(), tTrack->flags(), 
                        tTrack->lhcbIDs().size(), tExtra.size(), tTrack->nStates() );
      info() << format( " Likelihood %10.6f ghostProba %10.8f",
                        tTrack->likelihood(), tTrack->ghostProbability() )
             << endmsg;
      for ( kk = 0 ; oTrack->lhcbIDs().size() != kk ; ++kk ) {
        info() << format( "   old ID %8x   new %8x", 
                          oTrack->lhcbIDs()[kk].lhcbID(), 
                          tTrack->lhcbIDs()[kk].lhcbID() ) << endmsg;
      }
      oIt = oExtra.begin();
      tIt = tExtra.begin();
      for ( kk = 0 ; oExtra.size() != kk ; ++kk, ++oIt, ++tIt ) {
        info() << format( "   old Extra %5d %12.4f     new %5d %12.4f", 
                          (*oIt).first, (*oIt).second, (*tIt).first, (*tIt).second ) 
               << endmsg;
      }
    }

    //== Compare the states. The track number won't be reported...

    for ( kk = 0; (oTrack->nStates() > kk) && (tTrack->nStates() > kk); ++kk ) {
      compareStates( oTrack->states()[kk], tTrack->states()[kk] );
    }
  }
  
  return StatusCode::SUCCESS;
}

//=========================================================================
//  Compare two states
//=========================================================================
void CompareTrack::compareStates ( const LHCb::State* oSta, const LHCb::State* tSta ) {

    bool isOK = true;
    if ( 5.e-5 < fabs( oSta->z() - tSta->z() ) ) isOK = false;
    if ( 5.e-5 < fabs( oSta->x() - tSta->x() ) ) isOK = false;
    if ( 5.e-5 < fabs( oSta->y() - tSta->y() ) ) isOK = false;
    if ( 5.e-8 < fabs( oSta->tx() - tSta->tx() ) ) isOK = false;
    if ( 5.e-8 < fabs( oSta->ty() - tSta->ty() ) ) isOK = false;
    double oP = 1./oSta->qOverP();
    double tP = 1./tSta->qOverP();
    if ( 5.e-3 < fabs( oP - tP ) ) isOK = false;

    std::vector<double> oDiag;
    oDiag.push_back( sqrt(oSta->errX2()) );
    oDiag.push_back( sqrt(oSta->errY2()) );
    oDiag.push_back( sqrt(oSta->errTx2()) );
    oDiag.push_back( sqrt(oSta->errTy2()) );
    oDiag.push_back( sqrt(oSta->errQOverP2() ) );

    std::vector<double> tDiag;
    tDiag.push_back( sqrt(tSta->errX2()) );
    tDiag.push_back( sqrt(tSta->errY2()) );
    tDiag.push_back( sqrt(tSta->errTx2()) );
    tDiag.push_back( sqrt(tSta->errTy2()) );
    tDiag.push_back( sqrt(tSta->errQOverP2() ) );
   
    if ( 5.e-5  < fabs( oDiag[0] - tDiag[0] ) ) isOK = false;
    if ( 5.e-5  < fabs( oDiag[1] - tDiag[1] ) ) isOK = false;
    if ( 5.e-8  < fabs( oDiag[2] - tDiag[2] ) ) isOK = false;
    if ( 5.e-8  < fabs( oDiag[3] - tDiag[3] ) ) isOK = false;
    if ( 5.     < fabs( oDiag[4]*oP*1.e5 - tDiag[4]*tP*1.e5 ) ) isOK = false;

    std::vector<double> oFrac;
    oFrac.push_back(  oSta->covariance()(1,0) / oDiag[1] / oDiag[0] );
    oFrac.push_back(  oSta->covariance()(2,0) / oDiag[2] / oDiag[0] );
    oFrac.push_back(  oSta->covariance()(2,1) / oDiag[2] / oDiag[1] );
    oFrac.push_back(  oSta->covariance()(3,0) / oDiag[3] / oDiag[0] );
    oFrac.push_back(  oSta->covariance()(3,1) / oDiag[3] / oDiag[1] );
    oFrac.push_back(  oSta->covariance()(3,2) / oDiag[3] / oDiag[2] );
    oFrac.push_back(  oSta->covariance()(4,0) / oDiag[4] / oDiag[0] );
    oFrac.push_back(  oSta->covariance()(4,1) / oDiag[4] / oDiag[1] );
    oFrac.push_back(  oSta->covariance()(4,2) / oDiag[4] / oDiag[2] );
    oFrac.push_back(  oSta->covariance()(4,3) / oDiag[4] / oDiag[3] );

    std::vector<double> tFrac;
    tFrac.push_back(  tSta->covariance()(1,0) / tDiag[1] / tDiag[0] );
    tFrac.push_back(  tSta->covariance()(2,0) / tDiag[2] / tDiag[0] );
    tFrac.push_back(  tSta->covariance()(2,1) / tDiag[2] / tDiag[1] );
    tFrac.push_back(  tSta->covariance()(3,0) / tDiag[3] / tDiag[0] );
    tFrac.push_back(  tSta->covariance()(3,1) / tDiag[3] / tDiag[1] );
    tFrac.push_back(  tSta->covariance()(3,2) / tDiag[3] / tDiag[2] );
    tFrac.push_back(  tSta->covariance()(4,0) / tDiag[4] / tDiag[0] );
    tFrac.push_back(  tSta->covariance()(4,1) / tDiag[4] / tDiag[1] );
    tFrac.push_back(  tSta->covariance()(4,2) / tDiag[4] / tDiag[2] );
    tFrac.push_back(  tSta->covariance()(4,3) / tDiag[4] / tDiag[3] );

    unsigned int kk;
    for ( kk = 0 ; oFrac.size() > kk ; ++kk ) {
      if ( 2.e-5 < fabs( oFrac[kk] - tFrac[kk] ) ) isOK = false;
    }

    if ( MSG::VERBOSE >= msgLevel() ) isOK = false; //== force printing
    if ( !isOK ) {
      info() << "=== State differ: " << endmsg;
      info() << "     old " 
             << format( " %12.5f %12.5f %12.5f %12.9f %12.9f %12.3f",
                        oSta->z(), oSta->x(), oSta->y(), 
                        oSta->tx(), oSta->ty(), 1./oSta->qOverP() )
             << endmsg;
      info() << "    test " 
             << format( " %12.5f %12.5f %12.5f %12.9f %12.9f %12.3f",
                        tSta->z(), tSta->x(), tSta->y(), 
                        tSta->tx(), tSta->ty(), 1./tSta->qOverP() )
             << endmsg;
      info() << format( " old Diag %10.5f %10.5f %12.9f %12.9f %12.3f", 
                        oDiag[0], oDiag[1], oDiag[2], oDiag[3], oDiag[4]*oP*oP )
             << endmsg;
      info() << format( "test Diag %10.5f %10.5f %12.9f %12.9f %12.3f", 
                        tDiag[0], tDiag[1], tDiag[2], tDiag[3], tDiag[4]*tP*tP )
             << endmsg;
      info() << " old Frac ";
      for ( kk = 0 ; oFrac.size() > kk ; ++kk ) {
        info () << format( " %8.5f", oFrac[kk] );
      }
      info() << endmsg << "test Frac ";
      for ( kk = 0 ; tFrac.size() > kk ; ++kk ) {
        info () << format( " %8.5f", tFrac[kk] );
      }
      info() << endmsg;
    }    

}
//=============================================================================
