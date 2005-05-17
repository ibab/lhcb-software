// $Id: CheatedLifetimeFitter.h,v 1.2 2005-05-17 19:11:12 xieyu Exp $
#ifndef CHEATEDLIFETIMEFITTER_H
#define CHEATEDLIFETIMEFITTER_H 1

// Include files from STL
#include <vector>
#include <string>

// Include files from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "DaVinciMCTools/ICheatedLifetimeFitter.h"

#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/DiagMatrix.h"

#include "Event/MCParticle.h"

/** @class CheatedLifetimeFitter CheatedLifetimeFitter.h 
 *  
 * This tool is addapted from LifetimeFitter written by G. Raven.
 * It performs lifetime fit with MC primary vertex, secondary vertex or momentum.
 *
 * @author Yuehong Xie
 *
 * @date 21/04/2005
 *
*/

class CheatedLifetimeFitter : public GaudiTool,
                       virtual public ICheatedLifetimeFitter {

public:

  /// Standard Constructor
  CheatedLifetimeFitter( const std::string& type, 
                            const std::string& name, 
                            const IInterface* parent);

  /// Standard Destructor
  virtual ~CheatedLifetimeFitter() { }

  /// Retrieve the Transporter, the Unconstrained Vertex Fitter 
  /// and the ParticlePropertyService.
  StatusCode initialize();    

  StatusCode fit(const Vertex& v, const Particle& p,
                 double& lifetime, double& lifetimeError,
                 double& chisq) const;
  StatusCode fitWithMCPV(const MCVertex& v, const Particle& p,
                 double& lifetime, double& lifetimeError,
                 double& chisq) const;
                                                                                
  StatusCode fitWithMCPB(const Vertex& v, const Particle& p,
                 const MCParticle& mcp,
                 double& lifetime, double& lifetimeError,
                 double& chisq) const;
  StatusCode fitWithMCVB(const Vertex& v, const Particle& p,
                 const MCParticle& mcp,
                 double& lifetime, double& lifetimeError,
                 double& chisq) const;

private:

  /// Private methods used in CheatedLifetimeFitter.

  bool setup(const Particle& part, const Vertex& vert, 
             double& mass,
             HepVector& p, HepVector& O, HepSymMatrix& W) const;
  /// Performs one iteration of fit;
  /// input is the previous estimate of the parameters
  /// output are the updated parameters, their covariance
  /// matrix and the chisquared
  double iterate(HepVector &p, HepSymMatrix& Cp,
                 const double& mass, const HepVector& O, const HepSymMatrix& W) const;

  bool setupWithMCPV(const Particle& part, const MCVertex& vert,
             double& mass,
             HepVector& p,HepVector& O, HepSymMatrix& W) const;
                                                                                
  double iterateWithMCPV(HepVector &p, HepSymMatrix& Cp,
                 const double& mass, const HepVector& O,
                 const HepSymMatrix& W) const;

  bool setupWithMCPB(const Particle& part, const MCParticle& mcpart,
             const Vertex& vert,
             HepVector3D & vmom,
             double& mass,
             HepVector& p,HepVector& O, HepSymMatrix& W) const;
                                                                                
  double iterateWithMCPB(HepVector &p, HepSymMatrix& Cp,
                 const HepVector3D & vmom,
                 const double& mass, const HepVector& O,
                 const HepSymMatrix& W) const;

  bool setupWithMCVB(const Particle& part, const MCParticle& mcpart,
             const Vertex& vert,
             double& mass,
             HepVector& p,HepVector& O, HepSymMatrix& W) const;
                                                                                
  double iterateWithMCVB(HepVector &p, HepSymMatrix& Cp,
                 const double& mass, const HepVector& O,
                 const HepSymMatrix& W) const;


  unsigned   m_maxIter;           ///< Minimum momentum
  double     m_maxDeltaChiSq;     ///< Minimum momentum

};

#endif // CHEATEDLIFETIMEFITTER_H

