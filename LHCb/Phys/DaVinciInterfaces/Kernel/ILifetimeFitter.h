// $Id: ILifetimeFitter.h,v 1.1.1.1 2009-07-22 20:54:51 jpalac Exp $
#ifndef DAVINCIKERNEL_ILIFETIMEFITTER_H 
#define DAVINCIKERNEL_ILIFETIMEFITTER_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

#include "Event/Particle.h"
#include "Event/Vertex.h"

static const InterfaceID IID_ILifetimeFitter("ILifetimeFitter", 1 , 0); 

/** @class ILifetimeFitter
 *  Interface Class for lifetime constrained vertex fit.
 *
 *  @author G. Raven
 *  @date   05/07/2002
 */

class ILifetimeFitter : virtual public IAlgTool {
 public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ILifetimeFitter; }

  /// Get lifetime
  /// inputs: Vertex corresponding to the assumed production point
  ///         LHCb::Particle itself
  /// output: resulting lifetime and error, chisq.
  virtual StatusCode fit( const LHCb::VertexBase&, const LHCb::Particle&, 
                          double& lifetime, double& error,
                          double& chisq) const = 0; 

};

#endif // DAVINCIKERNEL_ILIFETIMEFITTER_H
