// $Id: VtxChecker.h,v 1.2 2007-10-17 07:27:55 jpalac Exp $
#ifndef VTXCHECKER_H
#define VTXCHECKER_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DaVinciTupleAlgorithm.h"

#include "Kernel/Particle2MCLinker.h"

/** @class VtxChecker VtxChecker.h
 *
 *  VtxChecker is an algorithm to check
 *  vertex fit performances using MC truth.
 *  Taking as input the reconstructed
 *  particles, it fills a ntuple with
 *  fitted values, true values and pulls
 *  of vertex position and momentum.
 *
 *  @author Yuehong Xie
 *  @date   2006-5-25
 */
class VtxChecker : public DaVinciTupleAlgorithm
{

public:
  /// Standard constructor
  VtxChecker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~VtxChecker( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  StatusCode printParticle(const LHCb::Particle::ConstVector& ) const; ///< Nicely print a Particle
  StatusCode printParticle(const LHCb::Particle* p) const; ///< Nicely print a Particle
  template<class TYPE> StatusCode printMatrix(const TYPE& p, int line, int cols=7) const; ///< print matrix line
  StatusCode printVertex(const LHCb::Vertex* v) const; ///< Nicely print a Particle
  StatusCode printVertex(const LHCb::Vertex::ConstVector&) const; ///< Nicely print a Particle

  StatusCode plotParameters(const LHCb::Particle* P, const LHCb::MCParticle* MCP, const std::string&) const ;

private:

  Particle2MCLinker* m_pLinker;
  std::string m_particlePath;

  int m_pidToCheck;

  double isBChild(const LHCb::MCParticle* ) const;

};
#endif
