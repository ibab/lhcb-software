// $Id: LagrangeMassVertexFitter.h,v 1.1, 2002/03/08, modified from gcorti 
// Exps program $
#ifndef LAGRANGEMASSVERTEXFITTER_H
#define LAGRANGEMASSVERTEXFITTER_H 1

// Include files from STL
#include <vector>
#include <string>


// Include files from DaVinciTools
#include "Kernel/IMassVertexFitter.h"
#include "Kernel/IVertexFitter.h"
#include "Kernel/IParticleTransporter.h"
#include "Event/Vertex.h"
#include "Event/Particle.h"

// Forward declarations
class IParticlePropertySvc;
class IVertexFitter;
class IParticleTransporter;


/** @class LagrangeMassVertexFitter LagrangeMassVertexFitter.h 
 *  
 * A kinematical constrained fit using Lagrange multipliers method
 *   for 2, 3 and 4 tracks without resonances, for 3 and 4 tracks with
 *   one ressonance and for 4 tracks with two ressonances. 
 * Invariant mass and geometrical vertex constraint for the mother particle.
 * Invariant mass constraint for the ressonances. 
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
  /// Takes the name of the particle to constrain the mass to.
  /// Takes a vector of Particles as input. 
  
  StatusCode fitWithMass(const std::string& motherName, 
                         const ParticleVector&  pList, Vertex& constrVtx,
                         Particle& motherParticle);   
  
  /// Actual operator function.
  /// Takes the name of the particle to constrain the mass to.
  /// Takes two Particles as input.
  
  StatusCode fitWithMass(const std::string& motherName, 
                         Particle& particle1, Particle& particle2, 
                         Vertex& constrVtx, Particle& motherParticle);   
  
  /// Actual operator function.
  /// Takes the name of the particle to constrain the mass to.
  /// Takes three Particles as input. 
  
  StatusCode fitWithMass(const std::string& motherName, 
                         Particle& particle1, Particle& particle2, 
                         Particle& particle3, Vertex& constrVtx, 
                         Particle& motherParticle);
  
  /// Actual operator function.
  /// Takes the name of the particle to constrain the mass to.
  /// Takes four Particles as input. 
  
  StatusCode fitWithMass(const std::string& motherName, 
                         Particle& particle1, Particle& particle2, 
                         Particle& particle3, Particle& particle4,
                         Vertex& constrVtx, 
                         Particle& motherParticle);
  
  
private:
  
  /// Actual operator function. 
  /// Takes the name of the particle to constrain the mass to
  /// and a vector of Particles as input. In this case, there is no ressonance.
  
  StatusCode fitWithNoRess(const std::string& motherName, 
                           const ParticleVector&  pList, Vertex& constrVtx,
                           Particle& motherParticle);  
  
  /// Actual operator function. 
  /// Takes the name of the particle to constrain the mass to, takes the mass
  /// of the sub particle (ressonance) to constraint the mass to, and takes a 
  /// vector of Particles as input. In this case, there is one ressonance. 
  
  StatusCode fitWithOneSubMass(const std::string& motherName, 
                               double subMass,
                               const ParticleVector&  pList, Vertex& constrVtx,
                               Particle& motherParticle);   
  
  /// Actual operator function. 
  /// Takes the name of the particle to constrain the mass to, takes the mass
  /// of the two sub particles (ressonances) to constraint the mass to, 
  /// and takes a vector of Particles as input. 
  /// In this case, there are two ressonances. 
  
  StatusCode fitWithTwoSubMass(const std::string& motherName, 
                               double subMass1,
                               double subMass2,
                               const ParticleVector&  pList, Vertex& constrVtx,
                               Particle& motherParticle);  
  
  
  // number of tracks ( = ntracks ) must be 2, 3 or 4.
  int m_ntracks;
  
  
  /// Private methods used in fitWithNoRess, fitWithOneSubMass 
  /// and fitWithTwoSubMass.
  
  /// Performs one iteration of the constrained fit for 2, 3 or 4 tracks
  /// without ressonance.
  /// It returns the parameters vector (e) and the covariance matrix (Ce) 
  /// after one iteration of the contraint fit.
  
  StatusCode itera(HepVector &e, HepSymMatrix &Ce, HepVector &mass,
                   double massConstr, int dimCe, bool final );
  
  /// Performs one iteration of the constrained fit for 3 or 4 tracks
  /// with one ressonance.
  /// It returns the parameters vector (e) and the covariance matrix (Ce) 
  /// after one iteration of the contraint fit.
  
  StatusCode itera(HepVector &e, HepSymMatrix &Ce, HepVector &mass,
                   double massConstr, double subMassConstr, int dimCe, 
                   bool final );
  
  /// Performs one iteration of the constrained fit for 4 tracks
  /// with two ressonances.
  /// It returns the parameters vector (e) and the covariance matrix (Ce) 
  /// after one iteration of the contraint fit.
  
  StatusCode itera(HepVector &e, HepSymMatrix &Ce, HepVector &mass,
                   double massConstr, double subMass1Constr, 
                   double subMass2Constr,int dimCe, bool final );
  
  
  
  
  
  /// Computes the parameters and the covariance matrix of the
  /// new formed particle. 
  StatusCode nwcov(HepVector &e, HepSymMatrix &Ce, double zcer,
                   HepVector &tpf, HepSymMatrix &Cx, int dimCe );
  
  
  
  
  
  /// Evaluates how well the constrained equations are satisfied
  /// at a given iteration. This method works for the case of 2, 3 or 4
  /// tracks without ressonance.
  /// It returns the invariant mass at this step ( = massConstrCalc ) and 
  /// the difference in the estimated z-vertex ( = diffZver ).
  
  StatusCode evalu(HepVector &e, HepVector &mass,
                   double& massConstrCalc,HepVector & diffZver);
  
  /// Evaluates how well the constrained equations are satisfied
  /// at a given iteration. This method works for the case of 3 or 4
  /// tracks with one ressonance.
  /// It returns the invariant mass at this step ( = massConstrCalc ) and 
  /// the difference in the estimated z-vertex ( = diffZver ).
  
  StatusCode evalu(HepVector &e, HepVector &mass,
                   double subMassConstr,  
                   double& massConstrCalc,HepVector & diffZver);
  
  /// Evaluates how well the constrained equations are satisfied
  /// at a given iteration. This method works for the case of 4
  /// tracks with two ressonances.
  /// It returns the invariant mass at this step ( = massConstrCalc ) and 
  /// the difference in the estimated z-vertex ( = diffZver ).
  
  StatusCode evalu(HepVector &e, HepVector &mass,
                   double subMass1Constr, double subMass2Constr,  
                   double& massConstrCalc,HepVector & diffZver);
  
  
  
  
  
  /// Computes the Chisquare of the contrained fit.
  StatusCode chisq(HepVector &e0,HepVector &e, HepSymMatrix &Ce,
                   double &chis);
  
  /// Cut on the Width in order to use the daughters ot not
  double m_widthCut;    
  IParticlePropertySvc* m_ppSvc;        ///< Reference to ParticlePropertySvc
  IToolSvc* m_pToolSvc;
  IVertexFitter* m_pVertexUnconst;      ///< Reference to VertexFitter
  IParticleTransporter* m_pTransporter; ///< Reference to ParticleTransporter
  std::string m_transporterType;        ///< Type of transporter to use      
};

#endif // LAGRANGEMASSVERTEXFITTER_H

