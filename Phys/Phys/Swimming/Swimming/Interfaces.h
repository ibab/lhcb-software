// $Id: $
#ifndef SWIMMINGINTERFACES_H 
#define SWIMMINGINTERFACES_H 1

// Include files
#include <GaudiKernel/IService.h>
#include <GaudiKernel/IInterface.h>
#include <GaudiKernel/Vector3DTypes.h>

// Event
#include <Event/RecVertex.h>

namespace LHCb {
   class Particle;
   class Track;
}
class IPVReFitter;

namespace Swimming {

namespace Interfaces {

   class GAUDI_API IService : virtual public extend_interfaces1< ::IService> {
public: 

   virtual ~IService() {};

   virtual Gaudi::XYZVector offset() const = 0;

   virtual void setOffset(const Gaudi::XYZVector& offset) = 0;

   virtual LHCb::RecVertex::ConstVector getOfflinePVs(const LHCb::Particle* particle,
                                                      const IPVReFitter* refitter = 0) = 0;

   virtual void setSignalTracks(const LHCb::Particle* candidate = 0) = 0;
   virtual const std::vector<const LHCb::Track*>& getSignalTracks() const = 0;
   
   DeclareInterfaceID(Interfaces::IService,1,0);

};
}
}
#endif // SWIMMINGINTERFACES_H
