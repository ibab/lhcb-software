// $Id: LagrangeMassVertexFitter.h,v 1.1, 2002/03/08, modified from gcorti 
// Exp`s program $
#ifndef LAGRANGEMASSVERTEXFITTER_H
#define LAGRANGEMASSVERTEXFITTER_H 1

// Include files from STL
#include <vector>
#include <string>

// Include files from Gaudi
#include "GaudiKernel/AlgTool.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/DiagMatrix.h"

// Include files from DaVinciTools
#include "DaVinciTools/IMassVertexFitter.h"
#include "DaVinciTools/IVertexFitter.h"
#include "Event/Vertex.h"
#include "Event/Particle.h"

// Forward declarations
class IParticlePropertySvc;
class IVertexFitter;
class IParticleTransporter;


/** @class LagrangeMassVertexFitter LagrangeMassVertexFitter.h 
 *  
 * A kinematical constrained fit using Lagrange multipliers method
 * for 2 or 3 tracks. Invariant mass and geometrical vertex constraint.
 * Adapted from Juan Saborido`s program ( written in fortran ). 
 * See the LHCb internal note LHCb/98-051 PHYS. 
 *
 * @author J.  de Mello 
 * @author C. Nunes.  
 *
 * @date 08/03/2002
 *
*/

class LagrangeMassVertexFitter : public AlgTool,
                                 virtual public IMassVertexFitter {

public:

  /// Standard Constructor
  LagrangeMassVertexFitter( const std::string& type, 
                            const std::string& name, 
                            const IInterface* parent);

  /// Standard Destructor
  virtual ~LagrangeMassVertexFitter() { }

  /// Retrieve the Transporter, the Unconstrained Vertex Fitter 
  /// and the ParticlePropertyService.
  StatusCode initialize();    

  /// Actual operator function. 
  /// Takes the name of the particle to constrain the mass to
  /// and a vector of Particles as input.
  StatusCode fitWithMass(const std::string& motherName, 
                         const ParticleVector&  pList, Vertex& constrVtx,
                         Particle& motherParticle);   

  /// Actual operator function.
  /// Takes the name of the particle to constrain the mass to
  /// and two Particles as input.
  StatusCode fitWithMass(const std::string& motherName, 
                         Particle& particle1, Particle& particle2, 
                         Vertex& constrVtx, Particle& motherParticle);   

  /// Actual operator function.
  /// Takes the name of the particle to constrain the mass to
  /// and three Particles as input.
  StatusCode fitWithMass(const std::string& motherName, 
                         Particle& particle1, Particle& particle2, 
                         Particle& particle3, Vertex& constrVtx, 
                         Particle& motherParticle);
private:

  /// Private methods used in lagrangeFitter.

  /// Performs one iteration of the constraint fit for 2 or 3 particles.
  /// It returns the parameters vector (e) and the covariance matrix (Ce) 
  /// after one iteration of the contraint fit.
  StatusCode itera(HepVector &e, HepSymMatrix &Ce, HepVector &mass,
                   double massConstr, int dimCe, bool final );

  /// Computes the parameters and the covariance matrix of the
  /// new formed particle. 
  StatusCode nwcov(HepVector &e, HepSymMatrix &Ce, double zcer,
                   HepVector &tpf, HepSymMatrix &Cx, int dimCe );

  /// Evaluates how well the constraint equations are satisfied
  /// at a given iteration. 
  /// It returns the invariant mass at this step ( = massConstrCalc ) and 
  /// the difference in the estimated z-vertex ( = diffZver ).
  StatusCode evalu(HepVector &e, HepVector &mass,
		   double& massConstrCalc,double diffZver);

  /// Computes the Chisquare of the contrained fit.
  StatusCode chisq(HepVector &e0,HepVector &e, HepSymMatrix &Ce,
                   double &chis);
  
  int m_ntracks;                        ///< Number of tracks to fit
 
  IParticlePropertySvc* m_ppSvc;        ///< Reference to ParticlePropertySvc
  IVertexFitter* m_pVertexUnconst;      ///< Reference to VertexFitter
  IParticleTransporter* m_pTransporter; ///< Reference to ParticleTransporter
};

#endif // LAGRANGEMASSVERTEXFITTER_H

