// $Id: ICaloElectron.h,v 1.1 2006-12-04 14:17:48 odescham Exp $
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
#include "CaloUtils/CaloMomentum.h"

static const InterfaceID IID_ICaloElectron ( "ICaloElectron", 1, 0 );

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

  virtual double eOverP(const LHCb::Particle * particle )= 0 ;
  virtual double e(const LHCb::Particle * particle ) = 0 ;
  virtual LHCb::CaloHypo*    hypo(const LHCb::Particle * particle ) = 0 ;
  virtual LHCb::CaloMomentum bremstrahlung(const LHCb::Particle * particle ) = 0 ;
  

protected:

private:

};
#endif // KERNEL_ICALOELECTRON_H
