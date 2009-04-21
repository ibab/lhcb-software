// $Id: ResolvedPi0Maker.h,v 1.4 2009-04-21 19:15:41 pkoppenb Exp $
#ifndef RESOLVEDPI0MAKER_H 
#define RESOLVEDPI0MAKER_H 1
// Include files
#include "ParticleMakerBase.h"
#include "GaudiKernel/GenericVectorTypes.h"
#include "GaudiKernel/SymmetricMatrixTypes.h"
#include "CaloDet/DeCalorimeter.h"
#include "Kernel/ICaloParticleMaker.h"

namespace LHCb{
  class ProtoParticle ;  
  class CaloHypo      ;
};

/** @class ResolvedPi0Maker ResolvedPi0Maker.h
 *  
 *  The specialized producer of mergedPi0
 *
 *  @author Olivier Deschamps odescham@in2p3.fr
 *  @date   2006-08-25
 */

class ResolvedPi0Maker : public ParticleMakerBase{
public:
  
  
  ResolvedPi0Maker(const std::string& name,ISvcLocator* pSvcLocator ) ;
   virtual ~ResolvedPi0Maker() ;
  
  // Make the particles 
  virtual StatusCode makeParticles (LHCb::Particle::Vector & particles ) ;
  virtual void setPoint ( const Gaudi::XYZPoint pos ){ m_point = pos; }
  virtual void setPoint ( const Gaudi::XYZPoint pos, const Gaudi::SymMatrix3x3 cov ){
    m_point = pos; 
    m_pointErr = cov;
  }
  virtual void setPoint    ( const LHCb::Vertex* vert ){
    m_point  = vert->position(); 
    m_pointErr = vert->covMatrix();
  }
 
  virtual StatusCode initialize    () ;   
  virtual StatusCode finalize      () ;


protected:
  
  /** accessor to the specialized tool for estimation of
   *  photon parameters 
   */
  
  bool   selPi0(LHCb::CaloParticle g1 ,LHCb::CaloParticle g2);
  StatusCode makePi0(LHCb::CaloParticle g1 , LHCb::CaloParticle g2, LHCb::Particle* pi0);
  // setters

private:
  long m_Id;
  double m_pi0Mass;
  
  // nominal production vertex
  Gaudi::XYZPoint                    m_point            ;
  Gaudi::SymMatrix3x3                m_pointErr         ;
  //
  std::string m_photonMakerType;
  ICaloParticleMaker* m_photonMaker;

  bool m_singlePhotonUse;
  bool m_independantPhotons;
  double   m_pi0MassWin ;
  double   m_pi0PtCut   ;
  unsigned long  m_count[3];
  std::string m_part;
};
#endif // RESOLVEDPI0MAKER_H
