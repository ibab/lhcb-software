// $Id: TrackKalmanFilter.h,v 1.10 2006-06-19 20:41:12 jvantilb Exp $
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
  StatusCode fit( LHCb::Track& track ) ;

  //! predict the state at this node
  StatusCode predict( LHCb::FitNode& node, LHCb::State& state );
  
  //! filter this node
  StatusCode filter( LHCb::FitNode& node, LHCb::State& state );
  
  //! smooth 2 nodes
  StatusCode smooth( LHCb::FitNode& node0, const LHCb::FitNode& node1 );

  //! smoother for bidirectional fit nodes
  StatusCode biSmooth( LHCb::FitNode& node0, const LHCb::FitNode& node1 );
  
  //! compute the chi2
  void computeChi2( LHCb::Track& track );

private:

  // ! check that the contents of the cov matrix are fine
  StatusCode checkInvertMatrix( const Gaudi::TrackSymMatrix& mat );

  // ! check that the contents of the cov matrix are fine
  StatusCode checkPositiveMatrix( Gaudi::TrackSymMatrix& mat );

  // ! invert this matrix
  StatusCode invertMatrix( Gaudi::TrackSymMatrix& mat );

  //! extrapolator
  ITrackExtrapolator* m_extrapolator;

  //! projector
  ITrackProjector* m_projector;


  // job options
  std::string m_extrapolatorName;   ///<  name of the extrapolator in Gaudi
  std::string m_projectorName;      ///< name of the projector in Gaudi
  bool m_storeTransport;            ///< store the transport of the extrapolator
  bool m_biDirectionalFit;          ///< Flag for bidirectional fit

  //! helper to print a failure comment
  StatusCode failure( const std::string& comment );

  bool m_debugLevel;
  
  };
#endif // TRACKFITTER_TRACKKALMANFILTER_H
