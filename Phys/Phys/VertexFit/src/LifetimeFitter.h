// $Id: LifetimeFitter.h,v 1.1.1.1 2004-08-24 06:28:54 pkoppenb Exp $
#ifndef LIFETIMEFITTER_H
#define LIFETIMEFITTER_H 1

// Include files from STL
#include <vector>
#include <string>

// Include files from Gaudi
#include "GaudiKernel/AlgTool.h"

// Include files from DaVinciTools
#include "DaVinciTools/ILifetimeFitter.h"

// Forward declarations

class HepVector;
class HepSymMatrix;


/** @class LifetimeFitter LifetimeFitter.h 
 *  
 * A constrained fit which parameterizes a particle's flight
 * as decayVertex = productionVertex + momentum * tau/mass
 * and which minimizes a chi^2 to obtain tau given a Particle
 * (and thus its measured decayVertex and measured momentum)
 * and a production Vertex.
 *
 * @author G. Raven
 *
 * @date 04/08/2002
 *
*/

class LifetimeFitter : public AlgTool,
                       virtual public ILifetimeFitter {

public:

  /// Standard Constructor
  LifetimeFitter( const std::string& type, 
                            const std::string& name, 
                            const IInterface* parent);

  /// Standard Destructor
  virtual ~LifetimeFitter() { }

  /// Retrieve the Transporter, the Unconstrained Vertex Fitter 
  /// and the ParticlePropertyService.
  StatusCode initialize();    

  StatusCode fit(const Vertex& v, const Particle& p,
                 double& lifetime, double& lifetimeError,
                 double& chisq) const;
private:

  /// Private methods used in LifetimeFitter.

  bool setup(const Particle& part, const Vertex& vert, 
             double &mass,
             HepVector& p,HepVector& O, HepSymMatrix& W) const;
  /// Performs one iteration of fit;
  /// input is the previous estimate of the parameters
  /// output are the updated parameters, their covariance
  /// matrix and the chisquared
  double iterate(HepVector &p, HepSymMatrix& Cp,
                 const double& mass, const HepVector& O, const HepSymMatrix& W) const;

  unsigned   m_maxIter;           ///< Minimum momentum
  double     m_maxDeltaChiSq;     ///< Minimum momentum

};

#endif // LIFETIMEFITTER_H

