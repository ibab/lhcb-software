// $Id: TrackVertexer.h,v 1.1 2007-12-03 12:56:58 wouter Exp $
#ifndef TRACKVERTEXER_H 
#define TRACKVERTEXER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "TrackInterfaces/ITrackVertexer.h"            // Interface

/** @class TrackVertexer TrackVertexer.h
 *  
 *
 *  @author Wouter HULSBERGEN
 *  @date   2007-11-07
 */
class TrackVertexer : public GaudiTool, virtual public ITrackVertexer {
public: 
  /// Standard constructor
  TrackVertexer( const std::string& type, 
                 const std::string& name,
                 const IInterface* parent);

  virtual ~TrackVertexer( ) ; ///< Destructor
  
  /// Create a vertex from two track states
  LHCb::TwoProngVertex* fit(const LHCb::State& stateA, const LHCb::State& stateB) const ;
  
private:
  size_t m_maxNumIter ;    ///< Max number of iterations
  double m_maxDChisq ;     ///< Min change in chisquare to run another iteration
  bool   m_computeMomCov ; ///< Flag to switch on/off computation of momentum covariance matrix
};
#endif // TRACKVERTEXER_H
