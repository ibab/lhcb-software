// Include files
// -------------
// from Gaudi
#include "GaudiKernel/GenericVectorTypes.h"
#include "GaudiKernel/SymmetricMatrixTypes.h"

// from TrackEvent
#include "Event/TrackUnitsConverters.h"
#include "Event/TrackTypes.h"

// local
#include "TrackChi2Calculator.h"

using namespace Gaudi;

//-----------------------------------------------------------------------------
// Implementation file for class : TrackChi2Calculator
//
// 2003-09-18 : Jeroen van Tilburg
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( TrackChi2Calculator )

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

  declareProperty( "ScaleVector",   m_scaleVector           );
  declareProperty( "MatchInMagnet", m_matchInMagnet = false );
  declareProperty( "AddMomentum",   m_addMomentum   = false );
}

//=============================================================================
// Destructor
//=============================================================================
TrackChi2Calculator::~TrackChi2Calculator() {}

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
StatusCode TrackChi2Calculator::calculateChi2( const Gaudi::TrackVector& trackVector1,
                                               const Gaudi::TrackSymMatrix& trackCov1,
                                               const Gaudi::TrackVector& trackVector2,
                                               const Gaudi::TrackSymMatrix& trackCov2,
                                               double& chi2 ) const
{
  if ( !m_addMomentum ) { // then the dimension is 4
    Vector4 vec1            = trackVector1.Sub<Vector4>(0);
    Vector4 vec2            = trackVector2.Sub<Vector4>(0);
    SymMatrix4x4 trackCov12 =   trackCov1.Sub<SymMatrix4x4>(0,0)
                              + trackCov2.Sub<SymMatrix4x4>(0,0);
    return calculateChi2( vec1, vec2, trackCov12, chi2 );
  }

  // If momentum information is to be used in the matching
  // -----------------------------------------------------
  // initialize chi2
  chi2 = 0.0;

  // copy tracks info
  TrackVector vec1      = TrackVector( trackVector1 );
  TrackVector vec2      = TrackVector( trackVector2 );
  TrackSymMatrix trackCinv = trackCov1 + trackCov2;
  
  // invert the matrix
  StatusCode sc = invertMatrix( trackCinv );
  if ( sc.isFailure() ) return StatusCode::FAILURE;

  // Remove Tx from chi2 in case of matching inside the magnet
  if ( m_matchInMagnet ) {
    vec1[2] = 0.0;
    vec2[2] = 0.0;
  }

   // Re-scale the chi2-contributions in case of error under/over-estimation
  unsigned int scaleVectorSize = m_scaleVector.size();
  if ( scaleVectorSize > 0 ) {
    for ( unsigned int i = 0; i < 5 && i <= scaleVectorSize ; ++i ) {
      vec1[i] *= sqrt( fabs(m_scaleVector[i]) );
      vec2[i] *= sqrt( fabs(m_scaleVector[i]) );
    }
  }

  // Calculate the chi2 distance between 2 tracks
  chi2 = ROOT::Math::Similarity<double,5>( vec1-vec2, trackCinv );

  return StatusCode::SUCCESS;
}

//=============================================================================
//
//=============================================================================
StatusCode TrackChi2Calculator::calculateChi2( Gaudi::Vector4& trackVector1,
                                               Gaudi::Vector4& trackVector2,
                                               Gaudi::SymMatrix4x4& trackCov12,
                                               double& chi2 ) const
{
  // initialize chi2
  chi2 = 0.0;

  // invert the matrix
  StatusCode sc = invertMatrix( trackCov12 );
  if ( sc.isFailure() ) return StatusCode::FAILURE;

  // Remove Tx from chi2 in case of matching inside the magnet
  if ( m_matchInMagnet ) {
    trackVector1[2] = 0.0;
    trackVector2[2] = 0.0;
  }

  // Re-scale the chi2-contributions in case of error under/over-estimation
  int scaleVectorSize = m_scaleVector.size() ; 
  if ( scaleVectorSize > 0 ) {
    for ( int i = 0; i < 4 && i <= scaleVectorSize ; ++i ) {
      trackVector1[i] *= sqrt( fabs(m_scaleVector[i]) ) ;
      trackVector2[i] *= sqrt( fabs(m_scaleVector[i]) ) ;
    }
  }

  // Calculate the chi2 distance between 2 tracks
  chi2 = ROOT::Math::Similarity<double,4>( trackVector1-trackVector2,
                                           trackCov12 );

  return StatusCode::SUCCESS;
}

//=============================================================================
//
//=============================================================================
StatusCode TrackChi2Calculator::invertMatrix( Gaudi::TrackSymMatrix& invC ) const
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
  for ( unsigned int i = 0; i < 4; ++i ) {
    for ( unsigned int j = 0; j < 4; ++j ) {
      if ( invC(i,j) > 1e20 ) {
        return Warning("old covariance errors too big to invert", StatusCode::FAILURE);
      }
    }
  }
  
  // G3 units 
  TrackUnitsConverters::convertToG3( invC );
  
  bool OK = invC.Invert();
  
  //G4 units
  TrackUnitsConverters::convertToG4( invC );

  return ( OK ) ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

//=============================================================================
//
//=============================================================================
StatusCode TrackChi2Calculator::invertMatrix( Gaudi::SymMatrix4x4& invC ) const
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
  for ( unsigned int i = 0; i < 4; ++i ) {
    for ( unsigned int j = 0; j < 4; ++j ) {
      if ( invC(i,j) > 1e20 ) {
        return Warning("old covariance errors too big to invert", StatusCode::FAILURE);
      }
    }
  }
  
  // G3 units 
  TrackUnitsConverters::convertToG3( invC );
  
  bool OK = invC.Invert();  

  //G4 units
  TrackUnitsConverters::convertToG4( invC );
  
  return ( OK ) ? StatusCode::SUCCESS : StatusCode::FAILURE;  
}

//=============================================================================
