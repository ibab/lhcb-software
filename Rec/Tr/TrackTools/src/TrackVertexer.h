// $Id: TrackVertexer.h,v 1.2 2008-05-28 08:11:33 wouter Exp $
#ifndef TRACKVERTEXER_H 
#define TRACKVERTEXER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "TrackInterfaces/ITrackVertexer.h"            // Interface
#include "GaudiKernel/SymmetricMatrixTypes.h"

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
  
private:
  size_t m_maxNumIter ;    ///< Max number of iterations
  double m_maxDChisq ;     ///< Min change in chisquare to run another iteration
  bool   m_computeMomCov ; ///< Flag to switch on/off computation of momentum covariance matrix
  bool   m_discardFailedFits ; ///< Return 0 pointer if fit fails
};
#endif // TRACKVERTEXER_H
