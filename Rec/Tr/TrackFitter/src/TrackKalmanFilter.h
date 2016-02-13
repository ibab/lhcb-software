// $Id: TrackKalmanFilter.h,v 1.32 2010-04-08 11:46:07 wouter Exp $
#ifndef TRACKFITTER_TRACKKALMANFILTER_H 
#define TRACKFITTER_TRACKKALMANFILTER_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackKalmanFilter.h"

// from TrackEvent
#include "Event/Track.h"

// From LoKi
#include "GaudiKernel/Range.h"

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
                          virtual public ITrackKalmanFilter {
public: 
  /// Standard constructor
  TrackKalmanFilter( const std::string& type, 
                     const std::string& name,
                     const IInterface* parent );

  /// Destructor
  virtual ~TrackKalmanFilter( );

  StatusCode initialize();

  //! fit a track
  StatusCode fit( LHCb::Track& track ) const ;
  
private:

  // job options
  bool m_forceBiDirectionalFit;        ///< Flag for forcing bidirectional fit
  bool m_forceSmooth;             ///< Flag for force the smoothing (for debug reason)
  
  //! helper to print a failure comment
  StatusCode failure( const std::string& comment ) const;

  unsigned int m_DoF;

  bool m_debugLevel;
};
#endif // TRACKFITTER_TRACKKALMANFILTER_H
