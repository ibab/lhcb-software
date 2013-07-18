// $Id: MergedPi0Maker.h,v 1.5 2009-04-23 10:39:31 pkoppenb Exp $
#ifndef MERGEDPI0PARTICLEPARTICLEMAKER_H
#define MERGEDPI0PARTICLEPARTICLEMAKER_H 1
// Include files
#include "Pi0MakerBase.h"

namespace LHCb{
  class ProtoParticle ;
  class CaloHypo      ;
}

/** @class MergedPi0Maker MergedPi0Maker.h
 *
 *  The specialized producer of mergedPi0
 *
 *  @author Olivier Deschamps odescham@in2p3.fr
 *  @date   2006-08-25
 */

class MergedPi0Maker : public Pi0MakerBase
{

public:

  MergedPi0Maker( const std::string& name,ISvcLocator* pSvcLocator  ) ;
  virtual ~MergedPi0Maker() ;
  virtual StatusCode initialize() ;

private:

  // Make the particles
  virtual StatusCode makeParticles (LHCb::Particle::Vector & particles ) ;

protected:

  /** accessor to the specialized tool for estimation of
   *  photon parameters
   */

  // confidence level evaluator
  double confLevel        ( const LHCb::ProtoParticle* pp ) const ;

private:

  DeCalorimeter* m_calo;
  /// techniques for CL evaluation
  bool          m_useCaloTrMatch   ;
  bool          m_useCaloDepositID ;
  bool          m_useShowerShape   ;
  bool          m_useClusterMass   ;
  /// Filters
  double   m_clCut      ;
  double   m_gPtCut     ;
  double   m_ggDistCut  ;
  std::vector<double> m_parMas;

};

#endif // MERGEDPI0PARTICLEPARTICLEMAKER_H
