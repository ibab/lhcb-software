#ifndef KERNEL_IPVREFITTER_H
#define KERNEL_IPVREFITTER_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
class StatusCode;

namespace LHCb 
{
  class Particle;
  class VertexBase;
}

/** @class IPVReFitter IPVReFitter.h Kernel/IPVReFitter.h
 *  Interface for PVReFitter
 *  @author Yuehong Xie
 *  @date   17/08/2005
 */
class GAUDI_API IPVReFitter : virtual public IAlgTool {

public:

  DeclareInterfaceID(IPVReFitter, 2, 0);
  
  /// refit PV
  virtual StatusCode reFit(LHCb::VertexBase*) const = 0;

  /// remove track used for a (B) LHCb::Particle and refit PV
  virtual StatusCode remove(const LHCb::Particle*,  
                            LHCb::VertexBase*) const = 0;

};
#endif // KERNEL_IPVREFITTER_H


