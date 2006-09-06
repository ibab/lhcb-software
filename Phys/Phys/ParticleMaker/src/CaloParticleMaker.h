// $Id: CaloParticleMaker.h,v 1.1 2006-09-06 15:21:47 odescham Exp $
#ifndef CALOPARTICLEMAKER_H 
#define CALOPARTICLEMAKER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/ICaloParticleMaker.h"           
#include "Kernel/IParticleMaker.h"            // Interface


/** @class CaloParticleMaker CaloParticleMaker.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2006-08-29
 */
class CaloParticleMaker : public GaudiTool, virtual public IParticleMaker {
public: 
  /// Standard constructor
  CaloParticleMaker( const std::string& type, 
                     const std::string& name,
                     const IInterface* parent);

  virtual ~CaloParticleMaker( ); ///< Destructor
  virtual StatusCode makeParticles (LHCb::Particle::ConstVector & particles ) ;
  virtual StatusCode initialize    () ;   
  virtual StatusCode finalize      () ;

protected:

private:
  std::string    m_MakerType;
  ICaloParticleMaker* m_Maker;
};
#endif // CALOPARTICLEMAKER_H
