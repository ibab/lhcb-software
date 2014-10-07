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
  double confLevel        ( const LHCb::ProtoParticle* pp , bool useSwitch=false) const ;

private:

  DeCalorimeter* m_calo;
  int ClusterCode( const LHCb::ProtoParticle* pp, std::string type ) const;
  std::map<std::string,std::pair<double,double> >m_clusterMasks;
  /// techniques for CL evaluation
  std::vector<std::string> m_clBase   ;
  std::vector<std::string> m_clSwitch;
  std::vector<std::string> m_knownCLs;

  /// Filters
  double m_clCut      ;
  double m_gPtCut     ;
  double m_ggDistCut  ;
  double m_chi2Cut    ;
  std::vector<double> m_parMas;
  bool clFind(std::string technique, bool useSwitch) const;
};

#endif // MERGEDPI0PARTICLEPARTICLEMAKER_H
