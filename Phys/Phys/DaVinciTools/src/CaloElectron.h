// $Id: CaloElectron.h,v 1.1 2006-12-04 14:13:36 odescham Exp $
#ifndef CALOELECTRON_H 
#define CALOELECTRON_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/ICaloElectron.h"            // Interface
//from LHCb
#include "Event/CaloHypo.h"
#include "Event/ProtoParticle.h"
#include "Event/Particle.h"
#include "Event/Track.h"


/** @class CaloElectron CaloElectron.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2006-11-30
 */
class CaloElectron : public GaudiTool, virtual public ICaloElectron {
public: 
  /// Standard constructor
  CaloElectron( const std::string& type, 
              const std::string& name,
              const IInterface* parent);

  virtual ~CaloElectron( ); ///< Destructor
  
  LHCb::CaloHypo*    hypo(const  LHCb::Particle* particle);
  LHCb::CaloMomentum bremstrahlung(const  LHCb::Particle* particle);
  double e(const  LHCb::Particle* particle);
  double eOverP( const LHCb::Particle* particle);
  

protected:
  LHCb::CaloHypo* calo(const  LHCb::Particle* particle, std::string typ);

private:

};
#endif // CALOELECTRON_H
