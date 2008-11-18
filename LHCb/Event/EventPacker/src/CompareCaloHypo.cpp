// $Id: CompareCaloHypo.cpp,v 1.1.1.1 2008-11-18 17:12:59 ocallot Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "Event/CaloHypo.h"

// local
#include "CompareCaloHypo.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CompareCaloHypo
//
// 2008-11-14 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CompareCaloHypo );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CompareCaloHypo::CompareCaloHypo( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = LHCb::CaloHypoLocation::Electrons );
  declareProperty( "TestName"  , m_testName   = LHCb::CaloHypoLocation::Electrons+"Test" );
}
//=============================================================================
// Destructor
//=============================================================================
CompareCaloHypo::~CompareCaloHypo() {}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CompareCaloHypo::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  LHCb::CaloHypos* old  = get<LHCb::CaloHypos>( m_inputName  );
  LHCb::CaloHypos* test = get<LHCb::CaloHypos>( m_testName  );

  if ( old->size() != test->size() ) {
    err() << "Old CaloHypo size " << old->size()
          << " differs form Test " << test->size()
          << endmsg;
    return StatusCode::FAILURE;
  }
  LHCb::CaloHypos::const_iterator itOld  = old->begin();
  LHCb::CaloHypos::const_iterator itTest = test->begin();

  while ( old->end() != itOld ) {
    LHCb::CaloHypo* oHypo = (*itOld++);
    LHCb::CaloHypo* tHypo = (*itTest++);
    if ( oHypo->key() != tHypo->key() ) {
      warning() << "Wrong key : old " <<  oHypo->key() << " test " << tHypo->key() << endmsg;
    }
    unsigned int kk;
    bool isOK = true;

    if ( oHypo->hypothesis() != tHypo->hypothesis() ) isOK = false;
    if ( 1.e-7 < fabs( (oHypo->lh() - tHypo->lh() ) / oHypo->lh() ) ) isOK = false;
    LHCb::CaloPosition* oPos = oHypo->position();
    LHCb::CaloPosition* tPos = tHypo->position();
    std::vector<double> oDiag, tDiag, oFrac, tFrac;
    if ( oPos == 0 && tPos != 0 ) isOK = false;
    if ( oPos != 0 && tPos == 0 ) isOK = false;
    if ( oPos != 0 && tPos != 0 ) {
      if ( 5.e-5 < fabs( oPos->z() - tPos->z() ) ) isOK = false;
      if ( 5.e-5 < fabs( oPos->x() - tPos->x() ) ) isOK = false;
      if ( 5.e-5 < fabs( oPos->y() - tPos->y() ) ) isOK = false;
      if ( 5.e-3 < fabs( oPos->e() - tPos->e() ) ) isOK = false;
     
      if ( 5.e-5 < fabs( oPos->center()(0) - tPos->center()(0) ) ) isOK = false;
      if ( 5.e-5 < fabs( oPos->center()(1) - tPos->center()(1) ) ) isOK = false;

      oDiag.push_back( sqrt(oPos->covariance()(0,0)) );
      oDiag.push_back( sqrt(oPos->covariance()(1,1)) );
      oDiag.push_back( sqrt(oPos->covariance()(2,2)) );
      oDiag.push_back( sqrt(oPos->spread()(0,0)) );
      oDiag.push_back( sqrt(oPos->spread()(1,1)) );
      
      tDiag.push_back( sqrt(tPos->covariance()(0,0)) );
      tDiag.push_back( sqrt(tPos->covariance()(1,1)) );
      tDiag.push_back( sqrt(tPos->covariance()(2,2)) );
      tDiag.push_back( sqrt(tPos->spread()(0,0)) );
      tDiag.push_back( sqrt(tPos->spread()(1,1)) );

      if ( 5.e-5  < fabs( oDiag[0] - tDiag[0] ) ) isOK = false;
      if ( 5.e-5  < fabs( oDiag[1] - tDiag[1] ) ) isOK = false;
      if ( 5.e-3  < fabs( oDiag[2] - tDiag[2] ) ) isOK = false;
      if ( 5.e-5  < fabs( oDiag[3] - tDiag[3] ) ) isOK = false;
      if ( 5.e-5  < fabs( oDiag[4] - tDiag[4] ) ) isOK = false;
      
      oFrac.push_back(  oPos->covariance()(1,0) / oDiag[1] / oDiag[0] );
      oFrac.push_back(  oPos->covariance()(2,0) / oDiag[2] / oDiag[0] );
      oFrac.push_back(  oPos->covariance()(2,1) / oDiag[2] / oDiag[1] );
      oFrac.push_back(  oPos->spread()(1,0)     / oDiag[3] / oDiag[4] );
      
      tFrac.push_back(  tPos->covariance()(1,0) / tDiag[1] / tDiag[0] );
      tFrac.push_back(  tPos->covariance()(2,0) / tDiag[2] / tDiag[0] );
      tFrac.push_back(  tPos->covariance()(2,1) / tDiag[2] / tDiag[1] );
      tFrac.push_back(  tPos->spread()(1,0)     / tDiag[3] / tDiag[4] );
      
      unsigned int kk;
      for ( kk = 0 ; oFrac.size() > kk ; ++kk ) {
        if ( 2.e-5 < fabs( oFrac[kk] - tFrac[kk] ) ) isOK = false;
      }
      
    }
    if ( oHypo->digits().size()   != tHypo->digits().size()   ) {
      isOK = false;
    } else {
      for ( kk = 0; oHypo->digits().size() > kk; kk++ ) {
        const LHCb::CaloDigit* dum  = oHypo->digits()[kk];  // convert smartref to pointers
        const LHCb::CaloDigit* dum1 = tHypo->digits()[kk];
        if ( dum != dum1 ) isOK = false;
      }
    }
     
    if ( oHypo->clusters().size() != tHypo->clusters().size() ) {
      isOK = false;
    } else {
      for ( kk = 0; oHypo->clusters().size() > kk; kk++ ) {
        const LHCb::CaloCluster* dum  = oHypo->clusters()[kk];  // convert smartref to pointers
        const LHCb::CaloCluster* dum1 = tHypo->clusters()[kk];
        if ( dum != dum1 ) isOK = false;
      }
    }

    if ( oHypo->hypos().size()    != tHypo->hypos().size()    ) {
      isOK = false;
    } else {
      for ( kk = 0; oHypo->hypos().size() > kk; kk++ ) {
        const LHCb::CaloHypo* dum  = oHypo->hypos()[kk];  // convert smartref to pointers
        const LHCb::CaloHypo* dum1 = tHypo->hypos()[kk];
        if ( dum != dum1 ) isOK = false;
      }
    }

    if ( !isOK || MSG::DEBUG >= msgLevel() ) {
      info() << "===== CaloHypo key " << oHypo->key() << endmsg;
      info() << format( "Old   Hypothesis %6d  likelihood %12.6f  nDigit%4d  nClusters%4d nHypo%4d", 
                        oHypo->hypothesis(), oHypo->lh(), oHypo->digits().size(),
                        oHypo->clusters().size(), oHypo->hypos().size() ) << endreq;
      info() << format( "Test  Hypothesis %6d  likelihood %12.6f  nDigit%4d  nClusters%4d nHypo%4d", 
                        tHypo->hypothesis(), tHypo->lh(), tHypo->digits().size(),
                        tHypo->clusters().size(), tHypo->hypos().size() ) << endreq;

      if ( oHypo->digits().size() == tHypo->digits().size()   ) {
        info() << "Old  digits ";
        for ( kk = 0; oHypo->digits().size() > kk; kk++ ) info() << " " << oHypo->digits()[kk];
        info() << endreq << "Test digits ";
        for ( kk = 0; tHypo->digits().size() > kk; kk++ ) info() << " " << tHypo->digits()[kk];
        info() << endreq;
      }
      if ( oHypo->clusters().size() == tHypo->clusters().size()   ) {
        info() << "Old  clusters ";
        for ( kk = 0; oHypo->clusters().size() > kk; kk++ ) info() << " " << oHypo->clusters()[kk];
        info() << endreq << "Test clusters ";
        for ( kk = 0; tHypo->clusters().size() > kk; kk++ ) info() << " " << tHypo->clusters()[kk];
        info() << endreq;
      }
      if ( oHypo->hypos().size() == tHypo->hypos().size()   ) {
        info() << "Old  hypos ";
        for ( kk = 0; oHypo->hypos().size() > kk; kk++ ) info() << " " << oHypo->hypos()[kk];
        info() << endreq << "Test hypos ";
        for ( kk = 0; tHypo->hypos().size() > kk; kk++ ) info() << " " << tHypo->hypos()[kk];
        info() << endreq;
      }
      // position
      if ( 0 != oPos && 0 != tPos ) {
        info() << format( "position Old  z%10.5f x%10.5f y%10.5f e%12.3f   cx%10.5f cy%10.5f",
                          oPos->z(), oPos->x(), oPos->y(), oPos->e(), oPos->center()(0), oPos->center()(1) )
               << endreq;
        info() << format( "position Test z%10.5f x%10.5f y%10.5f e%12.3f   cx%10.5f cy%10.5f",
                          tPos->z(), tPos->x(), tPos->y(), tPos->e(), tPos->center()(0), tPos->center()(1) )
               << endreq;

        info() << format( " old Diag %10.5f %10.5f %12.3f %10.5f %10.5f", 
                          oDiag[0], oDiag[1], oDiag[2], oDiag[3], oDiag[4] )
               << endreq;
        info() << format( "test Diag %10.5f %10.5f %12.3f %10.5f %10.5f", 
                          tDiag[0], tDiag[1], tDiag[2], tDiag[3], tDiag[4] )
             << endreq;
        info() << " old Frac ";
        for ( kk = 0 ; oFrac.size() > kk ; ++kk ) {
          info () << format( " %8.5f", oFrac[kk] );
        }
        info() << endreq << "test Frac ";
        for ( kk = 0 ; tFrac.size() > kk ; ++kk ) {
          info () << format( " %8.5f", tFrac[kk] );
        }
        info() << endreq;
      }
    }
  }

  return StatusCode::SUCCESS;
}
//=============================================================================
