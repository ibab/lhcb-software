// $Id: TrackKalmanFilter.h,v 1.4 2005-11-21 11:20:57 jvantilb Exp $
#ifndef TRACKFITTER_TRACKKALMANFILTER_H 
#define TRACKFITTER_TRACKKALMANFILTER_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackExtrapolator.h"            
#include "TrackInterfaces/ITrackProjector.h"
#include "TrackInterfaces/ITrackFitter.h"

// from TrackEvent
#include "Event/Track.h"
#include "Event/FitNode.h"

/** @class TrackKalmanFilter TrackKalmanFilter.h
 *  
 *
 *  @author Jose Angel Hernando Morata, Eduardo Rodrigues
 *  @date   2005-04-15
 *  reusing the previous code
 *  @author Rutger van der Eijk  07-04-1999
 *  @author Mattiew Needham 
 */

class TrackKalmanFilter : public GaudiTool,
                          virtual public ITrackFitter {
public: 
  /// Standard constructor
  TrackKalmanFilter( const std::string& type, 
                     const std::string& name,
                     const IInterface* parent );

  /// Destructor
  virtual ~TrackKalmanFilter( );

  StatusCode initialize();

  //! fit a track
  StatusCode fit( Track& track ) ;

  //! predict the state at this node
  StatusCode predict( FitNode& node, State& state );
  
  //! filter this node
  StatusCode filter( FitNode& node, State& state );
  
  //! smooth 2 nodes
  StatusCode smooth( FitNode& node0, const FitNode& node1 );
  
  //! compute the chi2
  void computeChiSq( Track& track );

protected:

  // ! check that the contents of the cov matrix are fine
  StatusCode checkInvertMatrix( const HepSymMatrix& mat );

  // ! check that the contents of the cov matrix are fine
  StatusCode checkPositiveMatrix( HepSymMatrix& mat );

  // ! invert this matrix
  StatusCode invertMatrix( HepSymMatrix& mat );

  //! change of units to facilitate inversion
  void cToG3( HepSymMatrix& C );

  //! change of units to facilitate inversion
  void cToG4( HepSymMatrix& invC );

  //! extrapolator
  ITrackExtrapolator* m_extrapolator;

  //! projector
  ITrackProjector* m_projector;

private:

  // job options
  std::string m_extrapolatorName;   ///<  name of the extrapolator in Gaudi
  std::string m_projectorName;      ///< name of the projector in Gaudi
  double m_storeTransport;          ///< store the transport of the extrapolator

  //! helper to print a failure comment
  StatusCode failure( const std::string& comment );

  bool m_debugLevel;
  
  };
#endif // TRACKFITTER_TRACKKALMANFILTER_H
