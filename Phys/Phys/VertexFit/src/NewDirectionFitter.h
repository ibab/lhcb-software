
#ifndef NEWDIRECTIONFITTER_H
#define NEWDIRECTIONFITTER_H 1

// Include files from STL
#include <vector>
#include <string>

// Include files from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "Kernel/INewDirectionFitter.h"

#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/DiagMatrix.h"


/** @class NewDirectionFitter NewDirectionFitter.h 
 *  
 *
 * Addapted from LifeTimeFitter written by G. Raven
 * fit for lifetime and update  mass
 * provide error matrix of (time, mass) 
 * provide the method for mass-constrained time
 *
 * @author Yuehong Xie
 *
 * @date 04/20/2005
 *
*/

class NewDirectionFitter : public GaudiTool,
                       virtual public INewDirectionFitter {

public:

  /// Standard Constructor
  NewDirectionFitter( const std::string& type, 
                            const std::string& name, 
                            const IInterface* parent);

  /// Standard Destructor
  virtual ~NewDirectionFitter() { }

  StatusCode fit(const Vertex& v, Particle& p,
                 double& lifetime, double& mass,
                 HepSymMatrix& cov,
                 double& chisq) const;

  StatusCode constrainMassForTime( double & time, double & mass,
                                   HepSymMatrix& cov,
                                   double newMass ) const;


private:

  /// Private methods used in NewDirectionFitter.

  bool setup(const Particle& part, const Vertex& vert, 
             HepVector& p,HepVector& O, HepSymMatrix& W) const;
  double iterate(HepVector &p, HepSymMatrix& Cp,
                 const HepVector& O, const HepSymMatrix& W) const;

  bool transform(HepVector &p, HepSymMatrix& Cp,
                 double& lifetime, double& mass,
                 HepSymMatrix& cov) const;

  HepMatrix MatrixE2M(const Particle&) const;

  unsigned   m_maxIter;           ///< Minimum momentum
  double     m_maxDeltaChiSq;     ///< Minimum momentum

};

#endif // NEWDIRECTIONFITTER_H

