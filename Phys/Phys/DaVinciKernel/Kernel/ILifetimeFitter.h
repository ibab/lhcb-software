// $Id: ILifetimeFitter.h,v 1.1 2005-01-06 10:30:43 pkoppenb Exp $
#ifndef DAVINCIKERNEL_ILIFETIMEFITTER_H 
#define DAVINCIKERNEL_ILIFETIMEFITTER_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
class Vertex;
class Particle;
class StatusCode;

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

  /// Perform mass contrains vertex fit.
  /// inputs: Vertex corresponding to the assumed production point
  ///         Particle itself
  /// output: resulting lifetime and error, chisq.
  virtual StatusCode fit( const Vertex&, const Particle&, 
                          double& lifetime, double& error,
                          double& chisq) const = 0; 

};

#endif // DAVINCIKERNEL_ILIFETIMEFITTER_H
