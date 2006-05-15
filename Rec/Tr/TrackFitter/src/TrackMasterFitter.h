// $Id: TrackMasterFitter.h,v 1.2 2006-05-15 16:02:12 jvantilb Exp $
#ifndef TRACKFITTER_TRACKMASTERFITTER_H 
#define TRACKFITTER_TRACKMASTERFITTER_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackExtrapolator.h"            
#include "TrackInterfaces/ITrackFitter.h"
#include "TrackInterfaces/IMeasurementProvider.h"

// from TrackEvent
#include "Event/Track.h"
#include "Event/FitNode.h"

/** @class TrackMasterFitter TrackMasterFitter.h
 *  
 *
 *  @author Jose Angel Hernando Morata, Eduardo Rodrigues
 *  @date   2005-04-15
 *  reusing the previous code
 *  @author Rutger van der Eijk  07-04-1999
 *  @author Matthew Needham 
 */

class TrackMasterFitter : public GaudiTool,
                          virtual public ITrackFitter {
public: 
  /// Standard constructor
  TrackMasterFitter( const std::string& type, 
                     const std::string& name,
                     const IInterface* parent );

  /// Destructor
  virtual ~TrackMasterFitter( );

  StatusCode initialize();

  //! fit a track 
  StatusCode fit( Track& track ) ;

protected:

  //! determine track state at various z positions
  StatusCode determineStates( Track& track );

  //! remove outliers from the node vector
  bool outlierRemoved( Track& track );

  //! inflate the covariance errors before next iteration
  StatusCode reSeed( State& seedState, State& state );

  //! update the reference vector for each measurement before the next iteration
  void update( Track& track );

  //! clear the (internal) nodes vector
  void clearNodes( std::vector<Node*>& nodes );

  //! determine the z-position of the closest approach to the beam line
  //! by linear extrapolation.
  double closestToBeamLine( State& state ) const;

  //! Retrieve the number of nodes with a measurement
  unsigned int nNodesWithMeasurement( const Track& track ) const;

  //! Retrieve the seed state
  const State& seedState( Track& track );

  //! Create the nodes from the measurements
  StatusCode makeNodes( Track& track );

protected:

  //! extrapolator
  ITrackExtrapolator* m_extrapolator;

  // delegate to actual track fitter (which fits from nodes)
  ITrackFitter* m_trackNodeFitter;

  /// measurement provider tool
  IMeasurementProvider* m_measProvider;

private:

  // job options
  std::string m_extrapolatorName;   ///< name of the extrapolator in Gaudi
  std::string m_trackNodeFitterName;///< name of the actual track fitter
  bool m_upstream;                  ///< switch between upstream/downstream fit
  std::vector<double> m_zPositions; ///< z-positions to determine the states
  bool m_statesAtMeasZPos;          ///< store states at measurement-positions?
  bool m_stateAtBeamLine;           ///< add state closest to the beam-line?
  int m_numFitIter;                 ///< number of fit iterations to perform
  double m_chi2Outliers;            ///< chi2 of outliers to be removed
  double m_zBegRich1;               ///< z position begin of RICH1
  double m_zEndRich1;               ///< z position end of RICH1
  double m_zBegRich2;               ///< z position begin of RICH2
  double m_zEndRich2;               ///< z position end of RICH2
  bool   m_increaseErrors;          ///< Increase errors on the initial cov. 
  double m_errorX2;                 ///< Error^2 on x
  double m_errorY2;                 ///< Error^2 on y
  double m_errorTx2;                ///< Error^2 on slope x
  double m_errorTy2;                ///< Error^2 on slope y
  double m_errorP;                  ///< Error on dp/p
  //! helper to print a failure comment
  StatusCode failure( const std::string& comment );

  bool m_debugLevel;
  
  };
#endif // TRACKFITTER_TRACKKALMANFILTER_H
