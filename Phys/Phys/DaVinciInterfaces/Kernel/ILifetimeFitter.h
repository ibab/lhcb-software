// $Id: ILifetimeFitter.h,v 1.2 2009-08-04 09:45:31 jpalac Exp $
#ifndef DAVINCIKERNEL_ILIFETIMEFITTER_H 
#define DAVINCIKERNEL_ILIFETIMEFITTER_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

#include "Event/Particle.h"
#include "Event/Vertex.h"

/** @class ILifetimeFitter
 *  Interface Class for lifetime constrained vertex fit.
 *
 *  @author G. Raven
 *  @date   05/07/2002
 */

class GAUDI_API ILifetimeFitter : virtual public IAlgTool 
{
 
 public:

  DeclareInterfaceID(ILifetimeFitter, 2, 0);

 public:
  
  /// Get lifetime
  /// inputs: Vertex corresponding to the assumed production point
  ///         LHCb::Particle itself
  /// output: resulting lifetime and error, chisq.
  ///         The lifetime is returned in nanoseconds.
  virtual StatusCode fit( const LHCb::VertexBase&, 
                          const LHCb::Particle&, 
                          double& lifetime, 
                          double& error,
                          double& chisq) const = 0; 

};

#endif // DAVINCIKERNEL_ILIFETIMEFITTER_H
