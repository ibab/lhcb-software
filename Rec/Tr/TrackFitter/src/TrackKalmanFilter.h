// $Id: TrackKalmanFilter.h,v 1.1 2005-06-29 15:35:02 erodrigu Exp $
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
 *  @author Jose Angel Hernando Morata 
 *  @date   2005-04-15
 *  reusing the previous code
 *  @author Rutger van der Eijk  07-04-1999
 *  @author Mattiew Needham 
 */

class TrackKalmanFilter : public GaudiTool, public ITrackFitter {
public: 
  /// Standard constructor
  TrackKalmanFilter( const std::string& type, 
                     const std::string& name,
                     const IInterface* parent);

  /// Destructor
  virtual ~TrackKalmanFilter( );

  StatusCode initialize();
  
  //! fit the track (filter and smoother)
  StatusCode fit(Track& track) {
    //track.setHistoryFit( TrackKeys::Kalman );
    info() << " not implemented yet!" << track.nMeasurements() << endreq;
    return StatusCode::SUCCESS;
  }

  //! fit the track (filter and smoother)
  StatusCode fit(Track& track, const State& seed);
  
  //! filter the track (only filter)
  StatusCode filter(Track& track, const State& seed);
  
  //! filter/update state with this measurement
  StatusCode filter(State& state, Measurement& meas);
  
protected:

  //! initialize the filter to fit/filter his track
  StatusCode iniKalman(Track& track);
  
  //! Predict the state to this node
  StatusCode predict(FitNode& node, State& state);
  
  //! filter this node
  StatusCode filter(FitNode& node);
  
  //! smooth the track
  StatusCode smoother(Track& track);
  
  //! smooth 2 nodes
  StatusCode smooth(FitNode& node0, const FitNode& node1);
  
  //! compute the chi2
  void computeChiSq(Track& track);
  
protected:
  
  // ! check that the contents of the cov matrix are fine
  StatusCode checkInvertMatrix(const HepSymMatrix& mat);

  // ! check that the contents of the cov matrix are fine
  StatusCode checkPositiveMatrix(HepSymMatrix& mat);

  // ! invert this matrix
  StatusCode invertMatrix(HepSymMatrix& mat);
  
  //! change of units to facilitate inversion
  StatusCode cToG3(HepSymMatrix& C);
  
  //! change of units to facilitate inversion
  StatusCode cToG4(HepSymMatrix& invC);

protected:

  //! internal clone of the state (pointer onwer by Kalman)
  State* m_state;

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

  //! print comment of failure 
  StatusCode failure(const std::string& comment);

  bool m_debugLevel;
  
  };
#endif // TRACKFITTER_TRACKKALMANFILTER_H
