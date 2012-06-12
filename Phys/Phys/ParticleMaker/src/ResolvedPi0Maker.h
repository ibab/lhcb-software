// $Id: ResolvedPi0Maker.h,v 1.5 2009-04-23 10:39:31 pkoppenb Exp $
#ifndef RESOLVEDPI0MAKER_H 
#define RESOLVEDPI0MAKER_H 1
// Include files
#include "Pi0MakerBase.h"
#include "Kernel/ICaloParticleMaker.h"

namespace LHCb{
  class ProtoParticle ;  
  class CaloHypo      ;
}

/** @class ResolvedPi0Maker ResolvedPi0Maker.h
 *  
 *  The specialized producer of Resolved Pi0
 *  Can also do eta, and anything that decays to two photons.
 *
 *  @author Olivier Deschamps odescham@in2p3.fr
 *  @date   2006-08-25
 */

class ResolvedPi0Maker : public Pi0MakerBase{
public:
  
  
  ResolvedPi0Maker(const std::string& name,ISvcLocator* pSvcLocator ) ;
   virtual ~ResolvedPi0Maker() ;
  
  virtual StatusCode initialize    () ;   
  virtual StatusCode finalize      () ;


private :
  // Make the particles 
  virtual StatusCode makeParticles (LHCb::Particle::Vector & particles ) ; 
  
  /** accessor to the specialized tool for estimation of
   *  photon parameters 
   */
  bool   selPi0(const LHCb::CaloParticle& g1 , 
                const LHCb::CaloParticle& g2);
  StatusCode makePi0(const LHCb::CaloParticle& g1, 
                     const LHCb::CaloParticle& g2, 
                     LHCb::Particle* pi0);

private:
  std::string m_photonMakerType;
  ICaloParticleMaker* m_photonMaker;

  bool m_singlePhotonUse;
  bool m_independantPhotons;
  double m_maxbal;
  double m_minbal;
};
#endif // RESOLVEDPI0MAKER_H
