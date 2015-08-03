// $Id: CompareRecVertex.cpp,v 1.2 2009-07-09 09:44:16 cattanem Exp $
// Include files

// from Gaudi
#include "Event/RecVertex.h"

// local
#include "CompareRecVertex.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CompareRecVertex
//
// 2008-11-17 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CompareRecVertex )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  CompareRecVertex::CompareRecVertex( const std::string& name,
                                      ISvcLocator* pSvcLocator)
    : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = LHCb::RecVertexLocation::Primary );
  declareProperty( "TestName"  , m_testName   = LHCb::RecVertexLocation::Primary+"Test" );
}
//=============================================================================
// Destructor
//=============================================================================
CompareRecVertex::~CompareRecVertex() {}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CompareRecVertex::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  LHCb::RecVertices* old  = get<LHCb::RecVertices>( m_inputName  );
  LHCb::RecVertices* test = get<LHCb::RecVertices>( m_testName  );

  if ( old->size() != test->size() ) {
    err() << "Old RecVertex size " << old->size()
          << " differs form Test " << test->size()
          << endmsg;
    return StatusCode::FAILURE;
  }
  LHCb::RecVertices::const_iterator itOld  = old->begin();
  LHCb::RecVertices::const_iterator itTest = test->begin();

  while ( old->end() != itOld ) {
    LHCb::RecVertex* oVtx = (*itOld++);
    LHCb::RecVertex* tVtx = (*itTest++);
    if ( oVtx->key() != tVtx->key() ) {
      warning() << "Wrong key : old " <<  oVtx->key() << " test " << tVtx->key() << endmsg;
    }
    bool isOK = true;
    if ( 1.e-7 < fabs( (oVtx->chi2() - tVtx->chi2() )/ oVtx->chi2() ) ) isOK = false;
    if ( oVtx->nDoF() != tVtx->nDoF() ) isOK = false;
    if ( 5.e-5 < fabs( oVtx->position().x() - tVtx->position().x() ) ) isOK = false;
    if ( 5.e-5 < fabs( oVtx->position().y() - tVtx->position().y() ) ) isOK = false;
    if ( 5.e-5 < fabs( oVtx->position().z() - tVtx->position().z() ) ) isOK = false;

    std::vector<double> oDiag, tDiag, oFrac, tFrac;
    oDiag.push_back( sqrt(oVtx->covMatrix()(0,0)) );
    oDiag.push_back( sqrt(oVtx->covMatrix()(1,1)) );
    oDiag.push_back( sqrt(oVtx->covMatrix()(2,2)) );

    tDiag.push_back( sqrt(tVtx->covMatrix()(0,0)) );
    tDiag.push_back( sqrt(tVtx->covMatrix()(1,1)) );
    tDiag.push_back( sqrt(tVtx->covMatrix()(2,2)) );

    if ( 5.e-5  < fabs( oDiag[0] - tDiag[0] ) ) isOK = false;
    if ( 5.e-5  < fabs( oDiag[1] - tDiag[1] ) ) isOK = false;
    if ( 5.e-5  < fabs( oDiag[2] - tDiag[2] ) ) isOK = false;

    oFrac.push_back(  oVtx->covMatrix()(1,0) / oDiag[1] / oDiag[0] );
    oFrac.push_back(  oVtx->covMatrix()(2,0) / oDiag[2] / oDiag[0] );
    oFrac.push_back(  oVtx->covMatrix()(2,1) / oDiag[2] / oDiag[1] );

    tFrac.push_back(  tVtx->covMatrix()(1,0) / tDiag[1] / tDiag[0] );
    tFrac.push_back(  tVtx->covMatrix()(2,0) / tDiag[2] / tDiag[0] );
    tFrac.push_back(  tVtx->covMatrix()(2,1) / tDiag[2] / tDiag[1] );

    unsigned int kk;
    for ( kk = 0 ; oFrac.size() > kk ; ++kk ) {
      if ( 2.e-5 < fabs( oFrac[kk] - tFrac[kk] ) ) isOK = false;
    }

    LHCb::RecVertex::ExtraInfo oExtra = oVtx->extraInfo();
    LHCb::RecVertex::ExtraInfo tExtra = tVtx->extraInfo();
    if ( oExtra.size() != tExtra.size() ) isOK = false;
    LHCb::RecVertex::ExtraInfo::const_iterator oIt = oExtra.begin();
    LHCb::RecVertex::ExtraInfo::const_iterator tIt = tExtra.begin();
    for ( kk = 0; tExtra.size() > kk; ++kk, ++oIt, ++tIt ) {
      if ( (*oIt).first != (*tIt).first ) isOK = false;
      if ( 1.e-7 < fabs( ((*oIt).second - (*oIt).second ) / (*oIt).second ) ) isOK = false;
    }

    if ( oVtx->tracks().size() != tVtx->tracks().size() ) {
      isOK = false;
    } else {
      for ( kk = 0; oVtx->tracks().size() > kk; kk++ ) {
        const LHCb::Track* dum  = oVtx->tracks()[kk];  // convert smartref to pointers
        const LHCb::Track* dum1 = tVtx->tracks()[kk];
        if ( dum != dum1 ) isOK = false;
      }
    }

    if ( oVtx->weights().size() != tVtx->weights().size() ||
         oVtx->weights().size() != oVtx->tracks().size()  ) 
    {
      isOK = false;
    }
    else 
    {
      for ( kk = 0; oVtx->weights().size() > kk; kk++ ) 
      {
        const float& oW = oVtx->weights()[kk];  
        const float& tW = tVtx->weights()[kk]; 
        if ( 2.e-5 < fabs(oW-tW) ) isOK = false;
      }
    }

    if ( !isOK || MSG::DEBUG >= msgLevel() ) {
      info() << "===== RecVertex key " << oVtx->key() << endmsg;
      info() << format( "Old   Technique %6d  chi2 %12.6f  nDoF%4d nTrack%4d  nExtra%4d",
                        oVtx->technique(), oVtx->chi2(), oVtx->nDoF(), oVtx->tracks().size(), oVtx->extraInfo().size() )
             << endmsg;
      info() << format( "Test  Technique %6d  chi2 %12.6f  nDoF%4d nTrack%4d  nExtra%4d",
                        tVtx->technique(), tVtx->chi2(), tVtx->nDoF(), tVtx->tracks().size(), tVtx->extraInfo().size() )
             << endmsg;
      if ( oVtx->tracks().size() == tVtx->tracks().size()   ) {
        info() << "Old  tracks ";
        for ( kk = 0; oVtx->tracks().size() > kk; kk++ ) info() << " " << oVtx->tracks()[kk];
        info() << endmsg << "Test tracks ";
        for ( kk = 0; tVtx->tracks().size() > kk; kk++ ) info() << " " << tVtx->tracks()[kk];
        info() << endmsg;
      }
      if ( oVtx->weights().size() == tVtx->weights().size()   ) {
        info() << "Old  weights ";
        for ( kk = 0; oVtx->weights().size() > kk; kk++ ) info() << " " << oVtx->weights()[kk];
        info() << endmsg << "Test weights ";
        for ( kk = 0; tVtx->weights().size() > kk; kk++ ) info() << " " << tVtx->weights()[kk];
        info() << endmsg;
      }
      // position
      info() << format( "position Old  x%10.5f y%10.5f z%10.5f",
                        oVtx->position().x(), oVtx->position().y(), oVtx->position().z() )
             << endmsg;
      info() << format( "position Test x%10.5f y%10.5f z%10.5f",
                        tVtx->position().x(), tVtx->position().y(), tVtx->position().z() )
             << endmsg;

      info() << format( " old Diag %10.5f %10.5f %10.5f",
                        oDiag[0], oDiag[1], oDiag[2])
             << endmsg;
      info() << format( "test Diag %10.5f %10.5f %10.5f",
                        tDiag[0], tDiag[1], tDiag[2] )
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
      //== extraInfo
      oIt = oExtra.begin();
      tIt = tExtra.begin();
      for ( kk = 0 ; oExtra.size() != kk ; ++kk, ++oIt, ++tIt ) {
        info() << format( "   old Extra %5d %12.4f     new %5d %12.4f",
                          (*oIt).first, (*oIt).second, (*tIt).first, (*tIt).second )
               << endmsg;
      }
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
