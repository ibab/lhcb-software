// $Id: PhotonMakerAlg.h,v 1.1 2009-04-21 19:15:41 pkoppenb Exp $
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
}

/** @class PhotonMakerAlg PhotonMakerAlg.h
 *  
 *  Call PhotonMaker
 *
 *  @author P. Koppenburg
 *  @date   2009-04-21
 */

class PhotonMakerAlg : public ParticleMakerBase{
public:
  
  PhotonMakerAlg(const std::string& name,ISvcLocator* pSvcLocator ) ;
   virtual ~PhotonMakerAlg() ;
  
  // Make the particles 
  virtual StatusCode makeParticles (LHCb::Particle::Vector & particles ) ;
 
  virtual StatusCode initialize    () ;   
  virtual StatusCode finalize      () ;


protected:
  
private:

  std::string m_photonMakerType;
  ICaloParticleMaker* m_photonMaker;

};
#endif // RESOLVEDPI0MAKER_H
