#ifndef DAVINCITOOLS_ICHEATEDLIFETIMEFITTER_H 
#define DAVINCITOOLS_ICHEATEDLIFETIMEFITTER_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
class MCVertex;
class MCParticle;

class Vertex;
class Particle;
class StatusCode;

static const InterfaceID IID_ICheatedLifetimeFitter("ICheatedLifetimeFitter", 1 , 0); 

/** @class ICheatedLifetimeFitter
 *  Interface Class for cheated lifetime fit.
 *
 *  @author Yuehong Xie
 *  @date   20/04/2005
 */

class ICheatedLifetimeFitter : virtual public IAlgTool {
 public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ICheatedLifetimeFitter; }

  virtual StatusCode fit( const Vertex&, const Particle&,
                          double& lifetime, double& error,
                          double& chisq) const = 0;

  virtual StatusCode fitWithMCPV( const MCVertex&, const Particle&,
                          double& lifetime, double& error,
                          double& chisq) const = 0;

  virtual StatusCode fitWithMCPB( const Vertex&, const Particle&,
                          const MCParticle&,
                          double& lifetime, double& error,
                          double& chisq) const = 0;

  virtual StatusCode fitWithMCVB( const Vertex&, const Particle&,
                          const MCParticle&,
                          double& lifetime, double& error,
                          double& chisq) const = 0;

};

#endif // DAVINCITOOLS_ICHEATEDLIFETIMEFITTER_H
