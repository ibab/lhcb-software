// $Id: TrackKalmanFilter.h,v 1.19 2007-09-04 08:34:59 wouter Exp $
#ifndef TRACKFITTER_TRACKKALMANFILTER_H 
#define TRACKFITTER_TRACKKALMANFILTER_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackExtrapolator.h"            
#include "TrackInterfaces/ITrackProjectorSelector.h"
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

protected:

  //! predict the state at this node
  StatusCode predict( LHCb::FitNode& node, LHCb::State& state, 
                      const Gaudi::TrackVector& refVec  ) const;  

  //! predict the state at this node for the reverse fit
  StatusCode predictReverseFit( const LHCb::FitNode& prevNode, 
                                const LHCb::FitNode& aNode,
                                LHCb::State& aState ) const;

  //! predict the state at this node for the reverse fit
  StatusCode project( LHCb::FitNode& aNode, const LHCb::State& aState ) const;

  //! filter this node
  StatusCode filter( LHCb::FitNode& node, LHCb::State& state ) const;
  
  //! smooth 2 nodes
  StatusCode smooth( LHCb::FitNode& node0, const LHCb::FitNode& node1, bool upstream ) const;

  //! smoother for bidirectional fit nodes
  StatusCode biSmooth( LHCb::FitNode& node0 ) const;

  // ! check that the contents of the cov matrix are fine
  StatusCode checkInvertMatrix( const Gaudi::TrackSymMatrix& mat ) const;

  // ! check that the contents of the cov matrix are fine
  StatusCode checkPositiveMatrix( const Gaudi::TrackSymMatrix& mat ) const;

  // ! invert this matrix
  StatusCode invertMatrix( Gaudi::TrackSymMatrix& mat ) const;

private:

  //! extrapolator
  ITrackExtrapolator* m_extrapolator;

  //! projector selector
  ITrackProjectorSelector* m_projectorSelector;


  // job options
  std::string m_extrapolatorName;   ///<  name of the extrapolator in Gaudi
  std::string m_projectorSelectorName;      ///< name of the projector selector in Gaudi
  bool m_storeTransport;            ///< store the transport of the extrapolator
  bool m_biDirectionalFit;          ///< Flag for bidirectional fit
  
  //! helper to print a failure comment
  StatusCode failure( const std::string& comment ) const;

  bool m_debugLevel;
  
  };
#endif // TRACKFITTER_TRACKKALMANFILTER_H
