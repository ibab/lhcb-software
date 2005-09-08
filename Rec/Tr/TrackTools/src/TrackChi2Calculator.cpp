// Include files
// -------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from CLHEP
#include "CLHEP/Units/SystemOfUnits.h"

// local
#include "TrackChi2Calculator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackChi2Calculator
//
// 2003-09-18 : Jeroen van Tilburg
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<TrackChi2Calculator>          s_factory ;
const        IToolFactory& TrackChi2CalculatorFactory = s_factory ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackChi2Calculator::TrackChi2Calculator( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  // interfaces
  declareInterface<ITrackChi2Calculator>(this);

  declareProperty( "ScaleVector",   m_scaleVector );
  declareProperty( "MatchInMagnet", m_matchInMagnet = false );
//  declareProperty( "AddMomentum",   m_addMomentum = false );
}

//=============================================================================
// Destructor
//=============================================================================
TrackChi2Calculator::~TrackChi2Calculator() {};

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackChi2Calculator::initialize() {

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;  // error already reported by base class

  return StatusCode::SUCCESS;
}

//=============================================================================
//
//=============================================================================
StatusCode TrackChi2Calculator::calculateChi2( const HepVector& trackVector1,
                                               const HepSymMatrix& trackCov1,
                                               const HepVector& trackVector2,
                                               const HepSymMatrix& trackCov2,
                                               double& chi2 ) const
{
  // initialize chi2
  chi2 = 0.0;

  // If momentum information must not be used: take the right
  // sub-vectors and sub-matrices.
  //int nParam = 4;
  int nParam = 5;
  //if ( m_addMomentum ) nParam = 5;
  HepVector vec1 = trackVector1.sub(1,nParam);
  HepVector vec2 = trackVector2.sub(1,nParam);
  HepSymMatrix trackCinv = trackCov1.sub(1,nParam) + trackCov2.sub(1,nParam);

  // invert the matrix
  StatusCode sc = invertMatrix( trackCinv );
  if ( sc.isFailure() ) return StatusCode::FAILURE;

  // Remove Tx from chi2 in case of matching inside the magnet
  if ( m_matchInMagnet ) {
    vec1(3) = 0.0;
    vec2(3) = 0.0;
  }

  // Re-scale the chi2-contributions in case of error under/over-estimation
  int scaleVectorSize = m_scaleVector.size() ; 
  if (scaleVectorSize > 0 ) {
    for (int i = 1; i <= nParam && i <= scaleVectorSize ; ++i) {
      vec1(i) *= sqrt( fabs(m_scaleVector[i-1]) ) ;
      vec2(i) *= sqrt( fabs(m_scaleVector[i-1]) ) ;
    }
  }

  // Calculate the chi2 distance between 2 tracks
  chi2 = trackCinv.similarity(vec1 - vec2);

  return StatusCode::SUCCESS;

}

//=============================================================================
//
//=============================================================================
StatusCode TrackChi2Calculator::invertMatrix( HepSymMatrix& invC ) const
{
  // This routine is taken from TrKalmanSmoother.cpp. It rescales 
  // the matrix before it actually calls the DSINV wrapper.
  
  // Invert previous node covariance matrix
  // What follows may seem strange - trust me it works - you 
  // are strongly recommended NOT to change it. It turns out that 
  // the choice of MeV, mm as units is BAD - the inversion simply fails 
  // for numerical reasons. Therefore it is necessary to change back to G3 
  // units, invert then go back to G4 units 
  // M. Needham 13/6/2000

  // check that the elements are not too large else dsinv will crash 
  for (int i=0; i<4; ++i){
    for (int j=0; j<4; ++j){
      if (invC[i][j] > 1e20){
        warning() << "old covariance errors too big to invert" << endmsg;
        return StatusCode::FAILURE;
      }
    }
  }
  
  // G3 units 
  cToG3(invC);
  
  int ifail;
  invC.invert(ifail);
  
  //G4 units
  cToG4(invC);
  
  if (ifail !=0){
    warning() << "failed to invert covariance matrix, failure code=" 
              << ifail << endmsg; 
    return StatusCode::FAILURE;
  }
  else {
    return StatusCode::SUCCESS;
  }
  
}

//=============================================================================
//
//=============================================================================
StatusCode TrackChi2Calculator::cToG3(HepSymMatrix& C) const
{
  // cov matrix
  C[0][0] /= cm2;
  C[1][0] /= cm2; 
  C[0][2] /= cm;
  C[0][3] /= cm;
 
  C[1][1] /= cm2; 
  C[1][2] /= cm;
  C[1][3] /= cm;

  if ( C.num_row() > 4 ) {
    C[0][4] /= cm*GeV;
    C[1][4] /= cm*GeV;
    C[2][4] /= GeV;
    C[3][4] /= GeV;
    C[4][4] /= GeV*GeV;
  }  

  return StatusCode::SUCCESS;
}

//=============================================================================
//
//=============================================================================
StatusCode TrackChi2Calculator::cToG4(HepSymMatrix& invC) const
{
  // cov matrix
  invC[0][0] /= cm2;
  invC[0][1] /= cm2; 
  invC[0][2] /= cm;
  invC[0][3] /= cm;

  invC[1][1] /= cm2; 
  invC[1][2] /= cm;
  invC[1][3] /= cm;

  if ( invC.num_row() > 4 ) {
    invC[0][4] /= cm*GeV;
    invC[1][4] /= cm*GeV;
    invC[2][4] /= GeV;
    invC[3][4] /= GeV;
    invC[4][4] /= GeV*GeV;
  }  

  return StatusCode::SUCCESS;
}

//=============================================================================
