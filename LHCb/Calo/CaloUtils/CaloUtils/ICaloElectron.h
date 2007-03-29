// $Id: ICaloElectron.h,v 1.1 2007-03-29 16:34:12 odescham Exp $
#ifndef KERNEL_ICALOELECTRON_H 
#define KERNEL_ICALOELECTRON_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
// from LHCb
#include "Event/CaloHypo.h"
#include "Event/Particle.h"
#include "Event/State.h"

#include "CaloDet/DeCalorimeter.h"

namespace LHCb{
  class CaloMomentum; 
}

static const InterfaceID IID_ICaloElectron ( "ICaloElectron", 2, 0 );

/** @class ICaloElectron ICaloElectron.h Kernel/ICaloElectron.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2006-11-30
 */
class ICaloElectron : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ICaloElectron; }

  virtual StatusCode         initialize() = 0;
  virtual bool               set(const  LHCb::Particle* particle) = 0;
  virtual bool               set(const  LHCb::ProtoParticle* proto) = 0;
  virtual double             eOverP()= 0 ;
  virtual double             ecalE() = 0 ;
  virtual LHCb::CaloHypo*    electron() = 0 ;
  virtual LHCb::CaloHypo*    bremstrahlung() = 0 ;
  virtual LHCb::CaloMomentum bremCaloMomentum() = 0 ;

  virtual LHCb::State        closestState(std::string toWhat = "hypo") =0;
  virtual LHCb::State        caloState(CaloPlane::Plane plane = CaloPlane::ShowerMax , double deltaShower = 0. ) = 0;
  virtual double             caloTrajectoryZ(CaloPlane::Plane refPlane = CaloPlane::ShowerMax,std::string toWhat = "hypo") =0;
  virtual double             caloTrajectoryL(CaloPlane::Plane refPlane = CaloPlane::ShowerMax ,std::string toWhat = "hypo") =0 ;

protected:

private:

};
#endif // KERNEL_ICALOELECTRON_H
