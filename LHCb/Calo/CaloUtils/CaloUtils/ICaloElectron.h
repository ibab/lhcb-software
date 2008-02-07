// $Id: ICaloElectron.h,v 1.3 2008-02-07 12:25:54 cattanem Exp $
#ifndef ICALOELECTRON_H 
#define ICALOELECTRON_H 1

// Include files
// from STL
#include <string>

#include "CaloInterfaces/IPart2Calo.h"
// from LHCb
#include "CaloUtils/CaloMomentum.h"

// Forward declarations
namespace LHCb 
{
  class Particle;
  class ProtoParticle;
}

static const InterfaceID IID_ICaloElectron ( "ICaloElectron", 3, 0 );

/** @class ICaloElectron ICaloElectron.h Kernel/ICaloElectron.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2006-11-30
 */
class ICaloElectron : virtual public IPart2Calo {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ICaloElectron; }

  virtual StatusCode         initialize() = 0;
  virtual bool  set(const  LHCb::Particle* particle, 
                    std::string det = DeCalorimeterLocation::Ecal,
                    CaloPlane::Plane plane = CaloPlane::ShowerMax,
                    double delta =0 ) = 0;
  virtual bool  set(const  LHCb::ProtoParticle* proto, 
                    std::string det = DeCalorimeterLocation::Ecal,
                    CaloPlane::Plane plane = CaloPlane::ShowerMax,
                    double delta =0 ) = 0;
  virtual double             eOverP()= 0 ;
  virtual double             ecalE() = 0 ;
  virtual LHCb::CaloHypo*    electron() = 0 ;
  virtual LHCb::CaloHypo*    bremstrahlung() = 0 ;
  virtual LHCb::CaloMomentum bremCaloMomentum() = 0 ;
  virtual LHCb::State        closestState(std::string toWhat = "hypo") =0;

  virtual double             caloTrajectoryZ(CaloPlane::Plane refPlane = CaloPlane::ShowerMax,std::string toWhat = "hypo") =0;
  virtual double             caloTrajectoryL(CaloPlane::Plane refPlane = CaloPlane::ShowerMax ,std::string toWhat = "hypo") =0 ;

protected:

private:

};
#endif // ICALOELECTRON_H
