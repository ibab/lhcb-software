// $Id: CompareTrStateL.cpp,v 1.1.1.1 2005-06-02 08:54:21 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/TrStateL.h"
// local
#include "CompareTrStateL.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CompareTrStateL
//
// 2005-03-16 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<CompareTrStateL>          s_factory ;
const        IAlgFactory& CompareTrStateLFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CompareTrStateL::CompareTrStateL( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = TrStateLLocation::Default );
  declareProperty( "TestName"  , m_testName   = TrStateLLocation::Default+"Test" );
}
//=============================================================================
// Destructor
//=============================================================================
CompareTrStateL::~CompareTrStateL() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CompareTrStateL::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode CompareTrStateL::execute() {

  debug() << "==> Execute" << endmsg;

  TrStateLs* old  = get<TrStateLs>( m_inputName );
  TrStateLs* test = get<TrStateLs>( m_testName  );

  if ( old->size() != test->size() ) {
    info() << "Old TrStateL size " << old->size()
           << " differs form Test " << test->size()
           << endreq;
    return StatusCode::FAILURE;
  }  
  TrStateLs::const_iterator itOld = old->begin();
  TrStateLs::const_iterator itTest = test->begin();

  while ( old->end() != itOld ) {
    TrStateL* oSta = (*itOld++);
    TrStateL* tSta = (*itTest++);
    if ( oSta->key() != tSta->key() ) {
      info() << "Wrong key : old " <<  oSta->key() << " test " << tSta->key() << endreq;
    }
    bool isOK = true;
    if ( 5.e-5 < fabs( oSta->z() - tSta->z() ) ) isOK = false;
    if ( 5.e-5 < fabs( oSta->x() - tSta->x() ) ) isOK = false;
    if ( 5.e-5 < fabs( oSta->y() - tSta->y() ) ) isOK = false;
    if ( 5.e-8 < fabs( oSta->tx() - tSta->tx() ) ) isOK = false;
    if ( 5.e-8 < fabs( oSta->ty() - tSta->ty() ) ) isOK = false;

    if ( MSG::VERBOSE >= msgLevel() ) isOK = false; //== force printing
    if ( !isOK ) {
      info() << "=== TrStateL key " << oSta->key() << endreq;
      info() << "     old " 
             << format( " %12.5f %12.5f %12.5f %12.9f %12.9f",
                        oSta->z(), oSta->x(), oSta->y(), 
                        oSta->tx(), oSta->ty() )
             << endreq;
      info() << "    test " 
             << format( " %12.5f %12.5f %12.5f %12.9f %12.9f",
                        tSta->z(), tSta->x(), tSta->y(), 
                        tSta->tx(), tSta->ty() )
             << endreq;
    }
    isOK = true; 
    std::vector<double> oDiag;
    oDiag.push_back( sqrt(oSta->eX2()) );
    oDiag.push_back( sqrt(oSta->eY2()) );
    oDiag.push_back( sqrt(oSta->eTx2()) );
    oDiag.push_back( sqrt(oSta->eTy2()) );

    std::vector<double> tDiag;
    tDiag.push_back( sqrt(tSta->eX2()) );
    tDiag.push_back( sqrt(tSta->eY2()) );
    tDiag.push_back( sqrt(tSta->eTx2()) );
    tDiag.push_back( sqrt(tSta->eTy2()) );
   
    if ( 5.e-5  < fabs( oDiag[0] - tDiag[0] ) ) isOK = false;
    if ( 5.e-5  < fabs( oDiag[1] - tDiag[1] ) ) isOK = false;
    if ( 5.e-8  < fabs( oDiag[2] - tDiag[2] ) ) isOK = false;
    if ( 5.e-8  < fabs( oDiag[3] - tDiag[3] ) ) isOK = false;

    std::vector<double> oFrac;
    oFrac.push_back(  oSta->stateCov()[1][0] / oDiag[1] / oDiag[0] );
    oFrac.push_back(  oSta->stateCov()[2][0] / oDiag[2] / oDiag[0] );
    oFrac.push_back(  oSta->stateCov()[2][1] / oDiag[2] / oDiag[1] );
    oFrac.push_back(  oSta->stateCov()[3][0] / oDiag[3] / oDiag[0] );
    oFrac.push_back(  oSta->stateCov()[3][1] / oDiag[3] / oDiag[1] );
    oFrac.push_back(  oSta->stateCov()[3][2] / oDiag[3] / oDiag[2] );

    std::vector<double> tFrac;
    tFrac.push_back(  tSta->stateCov()[1][0] / tDiag[1] / tDiag[0] );
    tFrac.push_back(  tSta->stateCov()[2][0] / tDiag[2] / tDiag[0] );
    tFrac.push_back(  tSta->stateCov()[2][1] / tDiag[2] / tDiag[1] );
    tFrac.push_back(  tSta->stateCov()[3][0] / tDiag[3] / tDiag[0] );
    tFrac.push_back(  tSta->stateCov()[3][1] / tDiag[3] / tDiag[1] );
    tFrac.push_back(  tSta->stateCov()[3][2] / tDiag[3] / tDiag[2] );

    unsigned int kk;
    for ( kk = 0 ; oFrac.size() > kk ; ++kk ) {
      if ( 2.e-5 < fabs( oFrac[kk] - tFrac[kk] ) ) isOK = false;
    }

    if ( MSG::VERBOSE >= msgLevel() ) isOK = false; //== force printing
    if ( !isOK ) {
      info() << format( " old Diag %10.5f %10.5f %12.9f %12.9f", 
                        oDiag[0], oDiag[1], oDiag[2], oDiag[3] )
             << endreq;
      info() << format( "test Diag %10.5f %10.5f %12.9f %12.9f", 
                        tDiag[0], tDiag[1], tDiag[2], tDiag[3] )
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

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CompareTrStateL::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
