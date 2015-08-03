// $Id: CompareTwoProngVertex.cpp,v 1.2 2009-07-09 09:44:16 cattanem Exp $
// Include files 

#include "Event/TwoProngVertex.h"
// local
#include "CompareTwoProngVertex.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CompareTwoProngVertex
//
// 2009-01-21 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CompareTwoProngVertex )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CompareTwoProngVertex::CompareTwoProngVertex( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = LHCb::TwoProngVertexLocation::Default );
  declareProperty( "TestName"  , m_testName   = LHCb::TwoProngVertexLocation::Default+"Test" );
}
//=============================================================================
// Destructor
//=============================================================================
CompareTwoProngVertex::~CompareTwoProngVertex() {} 

//=============================================================================
// Main execution
//=============================================================================
StatusCode CompareTwoProngVertex::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  LHCb::TwoProngVertices* old  = get<LHCb::TwoProngVertices>( m_inputName  );
  LHCb::TwoProngVertices* test = get<LHCb::TwoProngVertices>( m_testName  );

  if ( old->size() != test->size() ) {
    err() << "Old TwoProngVertex size " << old->size()
          << " differs form Test " << test->size()
          << endmsg;
    return StatusCode::FAILURE;
  }  
  LHCb::TwoProngVertices::const_iterator itOld  = old->begin();
  LHCb::TwoProngVertices::const_iterator itTest = test->begin();

  while ( old->end() != itOld ) {
    LHCb::TwoProngVertex* oVtx = (*itOld++);
    LHCb::TwoProngVertex* tVtx = (*itTest++);
    if ( oVtx->key() != tVtx->key() ) {
      warning() << "Wrong key : old " <<  oVtx->key() << " test " << tVtx->key() << endmsg;
    }
    bool isOK = true;
    if ( 1.e-7 < fabs( (oVtx->chi2() - tVtx->chi2() )/ oVtx->chi2() ) ) isOK = false;
    if ( oVtx->nDoF() != tVtx->nDoF() ) isOK = false;
    if ( 5.e-5 < fabs( oVtx->position().x() - tVtx->position().x() ) ) isOK = false;
    if ( 5.e-5 < fabs( oVtx->position().y() - tVtx->position().y() ) ) isOK = false;
    if ( 5.e-5 < fabs( oVtx->position().z() - tVtx->position().z() ) ) isOK = false;

    if ( 5.e-8 < fabs( oVtx->momA()[0] - tVtx->momA()[0] ) ) isOK = false;
    if ( 5.e-8 < fabs( oVtx->momA()[1] - tVtx->momA()[1] ) ) isOK = false;
    double oPA = 1./oVtx->momA()[2];
    double tPA = 1./tVtx->momA()[2];
    if ( 5.e-3 < fabs( oPA - tPA ) ) isOK = false;

    if ( 5.e-8 < fabs( oVtx->momB()[0] - tVtx->momB()[0] ) ) isOK = false;
    if ( 5.e-8 < fabs( oVtx->momB()[1] - tVtx->momB()[1] ) ) isOK = false;
    double oPB = 1./oVtx->momB()[2];
    double tPB = 1./tVtx->momB()[2];
    if ( 5.e-3 < fabs( oPB - tPB ) ) isOK = false;

    std::vector<double> oDiag, tDiag, oFrac, tFrac;
    oDiag.push_back( sqrt(oVtx->covMatrix()(0,0)) );
    oDiag.push_back( sqrt(oVtx->covMatrix()(1,1)) );
    oDiag.push_back( sqrt(oVtx->covMatrix()(2,2)) );
    oDiag.push_back( sqrt(oVtx->momcovA()(0,0)) );
    oDiag.push_back( sqrt(oVtx->momcovA()(1,1)) );
    oDiag.push_back( sqrt(oVtx->momcovA()(2,2)) );
    oDiag.push_back( sqrt(oVtx->momcovB()(0,0)) );
    oDiag.push_back( sqrt(oVtx->momcovB()(1,1)) );
    oDiag.push_back( sqrt(oVtx->momcovB()(2,2)) );
    
    tDiag.push_back( sqrt(tVtx->covMatrix()(0,0)) );
    tDiag.push_back( sqrt(tVtx->covMatrix()(1,1)) );
    tDiag.push_back( sqrt(tVtx->covMatrix()(2,2)) );
    tDiag.push_back( sqrt(tVtx->momcovA()(0,0)) );
    tDiag.push_back( sqrt(tVtx->momcovA()(1,1)) );
    tDiag.push_back( sqrt(tVtx->momcovA()(2,2)) );
    tDiag.push_back( sqrt(tVtx->momcovB()(0,0)) );
    tDiag.push_back( sqrt(tVtx->momcovB()(1,1)) );
    tDiag.push_back( sqrt(tVtx->momcovB()(2,2)) );
        
    if ( 5.e-5  < fabs( oDiag[0] - tDiag[0] ) ) isOK = false;
    if ( 5.e-5  < fabs( oDiag[1] - tDiag[1] ) ) isOK = false;
    if ( 5.e-5  < fabs( oDiag[2] - tDiag[2] ) ) isOK = false;
    if ( 5.e-8  < fabs( oDiag[3] - tDiag[3] ) ) isOK = false;
    if ( 5.e-8  < fabs( oDiag[4] - tDiag[4] ) ) isOK = false;
    if ( 5.     < fabs( oDiag[5]*oPA*1.e4 - tDiag[5]*tPA*1.e4 ) ) isOK = false;
    if ( 5.e-8  < fabs( oDiag[6] - tDiag[6] ) ) isOK = false;
    if ( 5.e-8  < fabs( oDiag[7] - tDiag[7] ) ) isOK = false;
    if ( 5.     < fabs( oDiag[8]*oPB*1.e4 - tDiag[8]*tPB*1.e4 ) ) isOK = false;
    
    oFrac.push_back(  oVtx->covMatrix() (1,0) / oDiag[1] / oDiag[0] );
    oFrac.push_back(  oVtx->covMatrix() (2,0) / oDiag[2] / oDiag[0] );
    oFrac.push_back(  oVtx->covMatrix() (2,1) / oDiag[2] / oDiag[1] );
    oFrac.push_back(  oVtx->momposcovA()(0,0) / oDiag[3] / oDiag[0] );
    oFrac.push_back(  oVtx->momposcovA()(0,1) / oDiag[3] / oDiag[1] );
    oFrac.push_back(  oVtx->momposcovA()(0,2) / oDiag[3] / oDiag[2] );
    oFrac.push_back(  oVtx->momposcovA()(1,0) / oDiag[4] / oDiag[0] );
    oFrac.push_back(  oVtx->momposcovA()(1,1) / oDiag[4] / oDiag[1] );
    oFrac.push_back(  oVtx->momposcovA()(1,2) / oDiag[4] / oDiag[2] );
    oFrac.push_back(  oVtx->momcovA()   (1,0) / oDiag[4] / oDiag[3] );
    oFrac.push_back(  oVtx->momposcovA()(2,0) / oDiag[5] / oDiag[0] );
    oFrac.push_back(  oVtx->momposcovA()(2,1) / oDiag[5] / oDiag[1] );
    oFrac.push_back(  oVtx->momposcovA()(2,2) / oDiag[5] / oDiag[2] );
    oFrac.push_back(  oVtx->momcovA()   (2,0) / oDiag[5] / oDiag[3] );
    oFrac.push_back(  oVtx->momcovA()   (2,1) / oDiag[5] / oDiag[4] );
    oFrac.push_back(  oVtx->momposcovB()(0,0) / oDiag[6] / oDiag[0] );
    oFrac.push_back(  oVtx->momposcovB()(0,1) / oDiag[6] / oDiag[1] );
    oFrac.push_back(  oVtx->momposcovB()(0,2) / oDiag[6] / oDiag[2] );
    oFrac.push_back(  oVtx->mommomcov() (0,0) / oDiag[6] / oDiag[3] );
    oFrac.push_back(  oVtx->mommomcov() (0,1) / oDiag[6] / oDiag[4] );
    oFrac.push_back(  oVtx->mommomcov() (0,2) / oDiag[6] / oDiag[5] );
    oFrac.push_back(  oVtx->momposcovB()(1,0) / oDiag[7] / oDiag[0] );
    oFrac.push_back(  oVtx->momposcovB()(1,1) / oDiag[7] / oDiag[1] );
    oFrac.push_back(  oVtx->momposcovB()(1,2) / oDiag[7] / oDiag[2] );
    oFrac.push_back(  oVtx->mommomcov() (1,0) / oDiag[7] / oDiag[3] );
    oFrac.push_back(  oVtx->mommomcov() (1,1) / oDiag[7] / oDiag[4] );
    oFrac.push_back(  oVtx->mommomcov() (1,2) / oDiag[7] / oDiag[5] );
    oFrac.push_back(  oVtx->momcovB()   (1,0) / oDiag[7] / oDiag[6] );
    oFrac.push_back(  oVtx->momposcovB()(2,0) / oDiag[8] / oDiag[0] );
    oFrac.push_back(  oVtx->momposcovB()(2,1) / oDiag[8] / oDiag[1] );
    oFrac.push_back(  oVtx->momposcovB()(2,2) / oDiag[8] / oDiag[2] );
    oFrac.push_back(  oVtx->mommomcov() (2,0) / oDiag[8] / oDiag[3] );
    oFrac.push_back(  oVtx->mommomcov() (2,1) / oDiag[8] / oDiag[4] );
    oFrac.push_back(  oVtx->mommomcov() (2,2) / oDiag[8] / oDiag[5] );
    oFrac.push_back(  oVtx->momcovB()   (2,0) / oDiag[8] / oDiag[6] );
    oFrac.push_back(  oVtx->momcovB()   (2,1) / oDiag[8] / oDiag[7] );
      
    tFrac.push_back(  tVtx->covMatrix() (1,0) / tDiag[1] / tDiag[0] );
    tFrac.push_back(  tVtx->covMatrix() (2,0) / tDiag[2] / tDiag[0] );
    tFrac.push_back(  tVtx->covMatrix() (2,1) / tDiag[2] / tDiag[1] );
    tFrac.push_back(  tVtx->momposcovA()(0,0) / tDiag[3] / tDiag[0] );
    tFrac.push_back(  tVtx->momposcovA()(0,1) / tDiag[3] / tDiag[1] );
    tFrac.push_back(  tVtx->momposcovA()(0,2) / tDiag[3] / tDiag[2] );
    tFrac.push_back(  tVtx->momposcovA()(1,0) / tDiag[4] / tDiag[0] );
    tFrac.push_back(  tVtx->momposcovA()(1,1) / tDiag[4] / tDiag[1] );
    tFrac.push_back(  tVtx->momposcovA()(1,2) / tDiag[4] / tDiag[2] );
    tFrac.push_back(  tVtx->momcovA()   (1,0) / tDiag[4] / tDiag[3] );
    tFrac.push_back(  tVtx->momposcovA()(2,0) / tDiag[5] / tDiag[0] );
    tFrac.push_back(  tVtx->momposcovA()(2,1) / tDiag[5] / tDiag[1] );
    tFrac.push_back(  tVtx->momposcovA()(2,2) / tDiag[5] / tDiag[2] );
    tFrac.push_back(  tVtx->momcovA()   (2,0) / tDiag[5] / tDiag[3] );
    tFrac.push_back(  tVtx->momcovA()   (2,1) / tDiag[5] / tDiag[4] );
    tFrac.push_back(  tVtx->momposcovB()(0,0) / tDiag[6] / tDiag[0] );
    tFrac.push_back(  tVtx->momposcovB()(0,1) / tDiag[6] / tDiag[1] );
    tFrac.push_back(  tVtx->momposcovB()(0,2) / tDiag[6] / tDiag[2] );
    tFrac.push_back(  tVtx->mommomcov() (0,0) / tDiag[6] / tDiag[3] );
    tFrac.push_back(  tVtx->mommomcov() (0,1) / tDiag[6] / tDiag[4] );
    tFrac.push_back(  tVtx->mommomcov() (0,2) / tDiag[6] / tDiag[5] );
    tFrac.push_back(  tVtx->momposcovB()(1,0) / tDiag[7] / tDiag[0] );
    tFrac.push_back(  tVtx->momposcovB()(1,1) / tDiag[7] / tDiag[1] );
    tFrac.push_back(  tVtx->momposcovB()(1,2) / tDiag[7] / tDiag[2] );
    tFrac.push_back(  tVtx->mommomcov() (1,0) / tDiag[7] / tDiag[3] );
    tFrac.push_back(  tVtx->mommomcov() (1,1) / tDiag[7] / tDiag[4] );
    tFrac.push_back(  tVtx->mommomcov() (1,2) / tDiag[7] / tDiag[5] );
    tFrac.push_back(  tVtx->momcovB()   (1,0) / tDiag[7] / tDiag[6] );
    tFrac.push_back(  tVtx->momposcovB()(2,0) / tDiag[8] / tDiag[0] );
    tFrac.push_back(  tVtx->momposcovB()(2,1) / tDiag[8] / tDiag[1] );
    tFrac.push_back(  tVtx->momposcovB()(2,2) / tDiag[8] / tDiag[2] );
    tFrac.push_back(  tVtx->mommomcov() (2,0) / tDiag[8] / tDiag[3] );
    tFrac.push_back(  tVtx->mommomcov() (2,1) / tDiag[8] / tDiag[4] );
    tFrac.push_back(  tVtx->mommomcov() (2,2) / tDiag[8] / tDiag[5] );
    tFrac.push_back(  tVtx->momcovB()   (2,0) / tDiag[8] / tDiag[6] );
    tFrac.push_back(  tVtx->momcovB()   (2,1) / tDiag[8] / tDiag[7] );
      
    unsigned int kk;
    for ( kk = 0 ; oFrac.size() > kk ; ++kk ) {
      if ( 2.e-5 < fabs( oFrac[kk] - tFrac[kk] ) ) isOK = false;
    }
    //== Compare extra info
    LHCb::TwoProngVertex::ExtraInfo oExtra = oVtx->extraInfo();
    LHCb::TwoProngVertex::ExtraInfo tExtra = tVtx->extraInfo();
    if ( oExtra.size() != tExtra.size() ) isOK = false;
    LHCb::TwoProngVertex::ExtraInfo::const_iterator oIt = oExtra.begin();
    LHCb::TwoProngVertex::ExtraInfo::const_iterator tIt = tExtra.begin();
    for ( kk = 0; tExtra.size() > kk; ++kk, ++oIt, ++tIt ) {
      if ( (*oIt).first != (*tIt).first ) isOK = false;
      if ( 1.e-7 < fabs( ((*oIt).second - (*oIt).second ) / (*oIt).second ) ) isOK = false;
    }

    //== Compare tracks
    if ( oVtx->tracks().size()   != tVtx->tracks().size()   ) {
      isOK = false;
    } else {
      for ( kk = 0; oVtx->tracks().size() > kk; kk++ ) {
        const LHCb::Track* dum  = oVtx->tracks()[kk];  // convert smartref to pointers
        const LHCb::Track* dum1 = tVtx->tracks()[kk];
        if ( dum != dum1 ) isOK = false;
      }
    }

    //== compare ParticleID
    const std::vector<LHCb::ParticleID>& oPid = oVtx->compatiblePIDs();
    const std::vector<LHCb::ParticleID>& tPid = tVtx->compatiblePIDs();
    if ( oPid.size() != tPid.size() ) {
      isOK = false;
    } else {
      for ( kk = 0; oPid.size() > kk; kk++ ) {
        if ( oPid[kk] != tPid[kk] ) isOK = false;
      }
    }
     
    

    if ( !isOK || MSG::DEBUG >= msgLevel() ) {
      info() << "===== TwoProngVertex key " << oVtx->key() << endmsg;
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
     // position
      info() << format( "position Old  x%10.5f y%10.5f z%10.5f  txA%10.7f tyA%10.7f p%10.3f  txB%10.7f tyB%10.7f pB%10.3f",
                        oVtx->position().x(), oVtx->position().y(), oVtx->position().z(),
                        oVtx->momA()[0], oVtx->momA()[1], oPA, oVtx->momB()[0], oVtx->momB()[1], oPB )
             << endmsg;
      info() << format( "position Test x%10.5f y%10.5f z%10.5f  txA%10.7f tyA%10.7f p%10.3f  txB%10.7f tyB%10.7f pB%10.3f",
                        tVtx->position().x(), tVtx->position().y(), tVtx->position().z(),
                        tVtx->momA()[0], tVtx->momA()[1], tPA, tVtx->momB()[0], tVtx->momB()[1], tPB )
             << endmsg;
      int indx = 0;
      for ( int krow=0 ; 9 > krow ; ++krow ) {
        info() << format( " Old row%2d : ", krow );
        int kcol;
        for ( kcol=0 ; krow > kcol ; ++kcol ) {
          info() << format( "%8.5f ", oFrac[indx+kcol] );
        }
        info() << format( "  %10.5g", oDiag[krow]) << endmsg;
        info() << format( "Test row%2d : ", krow );
        for ( kcol=0 ; krow > kcol ; ++kcol ) {
          info() << format( "%8.5f ", tFrac[indx+kcol] );
        }
        indx += kcol;
        info() << format( "  %10.5g", tDiag[krow]) << endmsg;
      }
      //== extraInfo
      oIt = oExtra.begin();
      tIt = tExtra.begin();
      for ( kk = 0 ; oExtra.size() != kk ; ++kk, ++oIt, ++tIt ) {
        info() << format( "   old Extra %5d %12.4f     new %5d %12.4f", 
                          (*oIt).first, (*oIt).second, (*tIt).first, (*tIt).second ) 
               << endmsg;
      }
      //== Compatible PID
      info() << " old compatiblePID : ";
      for ( kk=0 ; oPid.size() > kk ; ++kk ) {
        info() << format( " %12d", oPid[kk].pid() );
      }
      info() << endmsg <<  "Test compatiblePID : ";
      for ( kk=0 ; tPid.size() > kk ; ++kk ) {
        info() << format( " %12d", tPid[kk].pid() );
      }
      info() << endmsg;
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
