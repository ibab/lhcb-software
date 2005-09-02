#ifndef KERNEL_IPVREFITTER_H
#define KERNEL_IPVREFITTER_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
class StatusCode;
class Vertex;
class Particle;

// Declaration of the interface ID ( interface id, major version, minor version)
static const InterfaceID IID_IPVReFitter("IPVReFitter", 1, 0);


/** @class IPVReFitter IPVReFitter.h Kernel/IPVReFitter.h
 *  Interface for PVReFitter
 *  @author Yuehong Xie
 *  @date   17/08/2005
 */
class IPVReFitter : virtual public IAlgTool {

public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IPVReFitter; }

 
  /// refit PV
  virtual StatusCode reFit(Vertex* ) = 0;

  /// remove track used for a (B) Particle and refit PV
  virtual StatusCode remove(Particle*,  Vertex*) = 0;
 
protected:

private:

};
#endif // KERNEL_IPVREFITTER_H


