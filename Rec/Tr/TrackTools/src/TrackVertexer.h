// $Id: TrackVertexer.h,v 1.2 2008-05-28 08:11:33 wouter Exp $
#ifndef TRACKVERTEXER_H 
#define TRACKVERTEXER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/SymmetricMatrixTypes.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrackInterfaces/ITrackVertexer.h"            // Interface
#include "TrackInterfaces/ITrackStateProvider.h"
#include "Kernel/ITrajPoca.h"

/** @class TrackVertexer TrackVertexer.h
 *  
 *  This implements the Billoir-Fruhwirth-Regler algorithm.
 *
 *  @author Wouter HULSBERGEN
 *  @date   2007-11-07
 */

class TrackVertexer : public GaudiTool, virtual public ITrackVertexer {
public: 
  // Needs to be moved to interface
  
  /// Standard constructor
  TrackVertexer( const std::string& type, 
                 const std::string& name,
                 const IInterface* parent);

  virtual ~TrackVertexer( ) ; ///< Destructor
  
  /// Create a vertex from two track states
  LHCb::TwoProngVertex* fit(const LHCb::State& stateA, const LHCb::State& stateB) const ;
  
  /// Create a veretx from a set of states
  LHCb::RecVertex* fit(const StateContainer& states) const ;

  /// Create a vertex from a set of tracks.
  LHCb::RecVertex* fit(const TrackContainer& tracks) const ;
  
  /// Compute decaylength and IP chi2 wrt to PV. returns true if successful
  bool computeDecayLength(const LHCb::TwoProngVertex& vertex,
			  const LHCb::RecVertex& pv,
			  double& chi2,double& decaylength,double& decaylengtherr) const ;
  
  /// Return the ip chi2 for a track (uses stateprovider)
  double ipchi2( const LHCb::Track& track, const LHCb::RecVertex& pv) const ;
  
  /// Return the ip chi2 for a track state
  double ipchi2( const LHCb::State& state, const LHCb::RecVertex& pv) const ;
  
  /// initialize
  StatusCode initialize() ;

  /// finalize
  StatusCode finalize() ;

private:
  ToolHandle<ITrackStateProvider> m_stateprovider ;
  ToolHandle<ITrajPoca> m_pocatool ;
  size_t m_maxNumIter ;    ///< Max number of iterations
  double m_maxDChisq ;     ///< Min change in chisquare to run another iteration
  bool   m_computeMomCov ; ///< Flag to switch on/off computation of momentum covariance matrix
  bool   m_discardFailedFits ; ///< Return 0 pointer if fit fails
};
#endif // TRACKVERTEXER_H
