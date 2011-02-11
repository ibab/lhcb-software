// $Id: Pi0MakerBase.h,v 1.1 2009-04-23 10:39:31 pkoppenb Exp $
#ifndef PI0MAKERBASE_H 
#define PI0MAKERBASE_H 1
// Include files
#include "ParticleMakerBase.h"
#include "GaudiKernel/GenericVectorTypes.h"
#include "GaudiKernel/SymmetricMatrixTypes.h"
#include "Kernel/ICaloParticleMaker.h"

namespace LHCb{
  class ProtoParticle ;  
  class CaloHypo      ;
}

/** @class Pi0MakerBase Pi0MakerBase.h
 *  
 *  Base class for pi0 makers
 *
 *  @author Olivier Deschamps odescham@in2p3.fr
 *  @date   2006-08-25
 *  @author P.Koppenburg 2009-03-23
 */

class Pi0MakerBase : public ParticleMakerBase{
public:
  
  
  Pi0MakerBase(const std::string& name,ISvcLocator* pSvcLocator ) ;
  virtual ~Pi0MakerBase() ;
  virtual StatusCode initialize    () ;   
  virtual StatusCode finalize      () ;
  
protected:
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
  

protected:
  long m_Id;  ///< pid
  double m_Mass; ///< mass
  
  /// nominal production vertex
  Gaudi::XYZPoint                    m_point            ;
  /// nominal production vertex error
  Gaudi::SymMatrix3x3                m_pointErr         ;

  double   m_MassWin ; ///< mass window
  double   m_PtCut   ; ///< Pt cut
  unsigned long  m_count[3];
};
#endif // PI0MAKERBASE_H
