// $Id: TrackKalmanFilter.h,v 1.3 2005-10-25 12:50:05 erodrigu Exp $
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

  //! fit a track upstream without a seed state (filter and smooth)
  StatusCode fitUpstream( Track& track ) {
    // TODO: implement this method!
    info() << "Not implemented yet!" << endreq;
    return StatusCode::SUCCESS;
  }

  //! fit a track downstream without a seed state (filter and smooth)
  StatusCode fitDownstream( Track& track ) {
    // TODO: implement this method!
    info() << "Not implemented yet!" << endreq;
    return StatusCode::SUCCESS;
  }

  //! fit the track upstream with a seed state (filter and smooth)
  virtual StatusCode fitUpstream( Track& track, State& seed );

  //! fit the track downstream with a seed state (filter and smooth)
  virtual StatusCode fitDownstream( Track& track, State& seed );
  
  //! filter the track (only filter) with a seed state
  StatusCode filter( Track& track, State& seed );
  
  //! filter/update the state with this measurement
  StatusCode filter( State& state, Measurement& meas );
  
protected:

  //! initialize the Kalman filter
  void iniKalman( Track& track );
  
  //! predict the state at this node
  StatusCode predict( FitNode& node, State& state );
  
  //! filter this node
  StatusCode filter( FitNode& node, State& state );
  
  //! smooth the track
  StatusCode smoother( Track& track );
  
  //! smooth 2 nodes
  StatusCode smooth( FitNode& node0, const FitNode& node1 );
  
  //! compute the chi2
  void computeChiSq( Track& track );

  //! determine track state at various z positions
  StatusCode determineStates( Track& track );

  //! predict state at a z position
  State& predictState( const double zPos );

  //! update the current state
  void updateCurrentState( State& state, FitNode& lastNode );

  //! remove outliers from the node vector
  bool outlierRemoved( Track& track );

  //! inflate the covariance errors before next iteration
  StatusCode reSeed( State& seedState, State& state );

  //! update the reference vector for each measurement before the next iteration
  void updateMeasurements();

  //! clear the (internal) nodes vector
  void clearNodes();

//! determine the z-position of the closest approach to the beam line
//! by linear extrapolation.
  double closestToBeamLine( State& state );

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

protected:

  // internal copy of the nodes of the track (track owns them)
  std::vector<FitNode*> m_nodes;

  // internal copy of the gain matrix in the filter of a state
  HepVector m_K;

protected:

  //! extrapolator
  ITrackExtrapolator* m_extrapolator;

  //! projector
  ITrackProjector* m_projector;

private:

  // job options
  std::string m_extrapolatorName;   ///<  name of the extrapolator in Gaudi
  std::string m_projectorName;      ///< name of the projector in Gaudi
  std::vector<double> m_zPositions; ///< z-positions at which to determine the states
  bool m_statesAtMeasZPos;          ///< store states at measurement-positions?
  bool m_stateAtBeamLine;           ///< add state closest to the beam-line?
  int m_numFitIter;                 ///< number of fit iterations to perform
  double m_chi2Outliers;            ///< chi2 of outliers to be removed
  double m_storeTransport;          ///< store the transport of the extrapolator

  //! helper to print a failure comment
  StatusCode failure( const std::string& comment );

  bool m_debugLevel;
  
  };
#endif // TRACKFITTER_TRACKKALMANFILTER_H
