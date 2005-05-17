// $Id: KalmanFilter.h,v 1.2 2005-05-17 12:38:58 hernando Exp $
#ifndef KALMANFILTER_H 
#define KALMANFILTER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Event/Track.h"
#include "Event/FitNode.h"

#include "Tools/ITrackExtrapolator.h"            
#include "Tools/ITrackProjector.h"
#include "ITrackKalmanFilter.h"  

/** @class KalmanFilter KalmanFilter.h
 *  
 *
 *  @author Jose Angel Hernando Morata 
 *  @date   2005-04-15
 *  reusing the previous code
 *  @author Rutger van der Eijk  07-04-1999
 *  @author Mattiew Needham 
 */

class KalmanFilter : public GaudiTool, public ITrackKalmanFilter {
public: 
  /// Standard constructor
  KalmanFilter( const std::string& type, 
                const std::string& name,
                const IInterface* parent);

  /// Destructor
  virtual ~KalmanFilter( ); 

  StatusCode initialize();
  
  //! fit the track (filter and smoother)
  StatusCode fit(Track& track, const State& seed);
  
  //! filter the track (only filter)
  StatusCode filter(Track& track, const State& seed);
  
  //! filter/update state with this measurement
  StatusCode filter(State& state, Measurement& meas);
  
protected:

  //! initialize the filter to fit7/filter his track
  StatusCode iniKalman(Track& track);
  
  //! predict the state to this node
  StatusCode predict(FitNode& node, State& state);
  
  //! filter this node
  StatusCode filter(FitNode& node);
  
  //! smooth the track
  StatusCode smoother(Track& track);
  
  //! smooth 2 nodes
  StatusCode smooth(FitNode& node0, FitNode& node1);
  
  //! compute the chi2
  void computeChiSq(Track& track);
  
protected:
  
  //! print comment of failure 
  StatusCode failure(const std::string& comment);
  
  // ! check that the contents of the cov matrix are fine
  StatusCode checkInvertMatrix(HepSymMatrix& mat);

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

  //! name of the extrapolator in Gaudi
  std::string m_extrapolatorName;

  //! name of the projector in Gaudi
  std::string m_projectorName;

  };
#endif // KALMANFILTER_H
