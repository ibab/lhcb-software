#ifndef TRACKTOOLS_TRACKCHI2CALCULATOR_H 
#define TRACKTOOLS_TRACKCHI2CALCULATOR_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// local
#include "TrackInterfaces/ITrackChi2Calculator.h"

/** @class TrackChi2Calculator TrackChi2Calculator.h "TrackTools/TrackChi2Calculator.h"
 *
 *  This tool can be used to calculate the chi2-distance between
 *  two track states.
 *
 *  @author Jeroen van Tilburg
 *  @date   2003-09-18
 */

class TrackChi2Calculator : public GaudiTool,
                            virtual public ITrackChi2Calculator {
public:
  /// Standard constructor
  TrackChi2Calculator( const std::string& type, 
                       const std::string& name,
                       const IInterface* parent );

  virtual ~TrackChi2Calculator( );  ///< Destructor

  StatusCode initialize();

  /** Calculate the chi2 distance between two track vectors.
   *  The track vectors must be given as (x,y,tx,ty,q/p).
   *  @return StatusCode: Failure if matrix inversion failed  
   *  @param  trackVector1 input 1st track HepVector
   *  @param  trackCov1 input covariance matrix corresponding to 1st vector
   *  @param  trackVector2 input 2nd track HepVector
   *  @param  trackCov2 input covariance matrix corresponding to 2nd vector
   *  @param  chi2 output chi2 distance between the two vectors
   */
  virtual StatusCode calculateChi2( const Gaudi::TrackVector& trackVector1,
                                    const Gaudi::TrackMatrix& trackCov1,
                                    const Gaudi::TrackVector& trackVector2,
                                    const Gaudi::TrackMatrix& trackCov2,
                                    double& chi2 ) const;

private:
  StatusCode calculateChi2( Gaudi::Vector4& trackVector1,
                            Gaudi::Vector4& trackVector2,
                            Gaudi::SymMatrix4x4& trackCov12,
                            double& chi2 ) const;

  /// invert the 5x5 matrix
  StatusCode invertMatrix( Gaudi::TrackMatrix& invC ) const;

  /// invert the 4x4 matrix
  StatusCode invertMatrix( Gaudi::SymMatrix4x4& invC ) const;

  // job options
  // -----------
  /// Re-scale the track covariance matrices with this vector
  std::vector<double> m_scaleVector;
  /// Remove Tx from chi2 contribution in case of matching inside the magnet
  /// with straight lines
  bool m_matchInMagnet;
  /// Add momentum information to the matching chi2 criterium
  bool m_addMomentum;

};
#endif // TRACKTOOLS_TRACKCHI2CALCULATOR_H
