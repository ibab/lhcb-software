// $Id: MergedPi0Maker.h,v 1.4 2009-04-21 19:15:41 pkoppenb Exp $
#ifndef MERGEDPI0PARTICLEPARTICLEMAKER_H 
#define MERGEDPI0PARTICLEPARTICLEMAKER_H 1
// Include files
#include "ParticleMakerBase.h"
#include "GaudiKernel/GenericVectorTypes.h"
#include "GaudiKernel/SymmetricMatrixTypes.h"
#include "CaloDet/DeCalorimeter.h"

namespace LHCb{
  class ProtoParticle ;  
  class CaloHypo      ;
};

/** @class MergedPi0Maker MergedPi0Maker.h
 *  
 *  The specialized producer of mergedPi0
 *
 *  @author Olivier Deschamps odescham@in2p3.fr
 *  @date   2006-08-25
 */

class MergedPi0Maker : public ParticleMakerBase{
public:
  
  MergedPi0Maker( const std::string& name,ISvcLocator* pSvcLocator  ) ;
  virtual ~MergedPi0Maker() ;
  virtual StatusCode initialize    () ;   
  virtual StatusCode finalize      () ;

private:
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


protected:
  
  /** accessor to the specialized tool for estimation of
   *  photon parameters 
   */
  
  // confidence level evaluator
  double confLevel        ( const LHCb::ProtoParticle* pp ) const ;
  
private:
  long m_Id;
  double m_pi0Mass;
  DeCalorimeter* m_calo;
  
  // Input PP container
  std::string                        m_input            ;
  // nominal production vertex
  Gaudi::XYZPoint                    m_point            ;
  Gaudi::SymMatrix3x3                m_pointErr         ;
  // techniques for CL evaluation
  bool                               m_useCaloTrMatch   ;
  bool                               m_useCaloDepositID ;
  bool                               m_useShowerShape   ;
  bool                               m_useClusterMass   ;
  // Filters
  double   m_clCut      ;
  double   m_pi0MassWin ;
  double   m_gPtCut     ;
  double   m_pi0PtCut   ;
  double   m_ggDistCut  ;
  std::vector<double> m_parMas;
  unsigned long  m_count[3];
  std::string m_part;
};
#endif // MERGEDPI0PARTICLEPARTICLEMAKER_H
