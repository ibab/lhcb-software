// $Id: LagrangeGeomVertexFitter.h,v 1.1, 2002/03/08, modified from gcorti 
// Exps program $
#ifndef LAGRANGEGEOMVERTEXFITTER_H
#define LAGRANGEGEOMVERTEXFITTER_H 1

// Include files from STL
#include <vector>
#include <string>

// Include files from DaVinciTools
#include "Kernel/IGeomVertexFitter.h"
#include "Kernel/IVertexFitter.h"
#include "Kernel/IParticleTransporter.h"
#include "Event/Vertex.h"
#include "Event/Particle.h"

// Forward declarations
class IParticlePropertySvc;
class IVertexFitter;
class IParticleTransporter;


/** @class LagrangeGeomVertexFitter LagrangeGeomVertexFitter.h 
 *  
 * A kinematical constrained fit using Lagrange multipliers method
 *   for 2, 3 and 4 tracks without resonances, for 3 and 4 tracks with
 *   one ressonance and for 4 tracks with two ressonances. 
 * - geometrical vertex constraint for the mother particle.
 * - geometrical vertex constraint for the mother particle and
 *   invariant mass constraint for the ressonances. 
 * Adapted from Juan Saborido`s program ( written in fortran ). 
 * See the LHCb internal note LHCb/98-051 PHYS. 
 *
 * @author J.  de Mello 
 * @author C. Nunes.  
 *
 * @date 08/03/2002
 *
 */

class LagrangeGeomVertexFitter : public AlgTool,
                                 virtual public IGeomVertexFitter {
  
public:
  
  /// Standard Constructor
  LagrangeGeomVertexFitter( const std::string& type, 
                            const std::string& name, 
                            const IInterface* parent);
  
  /// Standard Destructor
  virtual ~LagrangeGeomVertexFitter() { }
  
  /// Retrieve the Transporter, the Unconstrained Vertex Fitter 
  /// and the ParticlePropertyService.
  StatusCode initialize();    
  
  
  /// Actual operator function.  
  
  StatusCode fitWithGeom(const std::string& motherName, 
                         const ParticleVector&  pList, Vertex& constrVtx,
                         Particle& motherParticle);   
  
  StatusCode fitWithGeom(const std::string& motherName, 
                         Particle& particle1, Particle& particle2, 
                         Vertex& constrVtx, Particle& motherParticle);   
  
  StatusCode fitWithGeom(const std::string& motherName, 
                         Particle& particle1, Particle& particle2, 
                         Particle& particle3, Vertex& constrVtx, 
                         Particle& motherParticle);
  
  StatusCode fitWithGeom(const std::string& motherName, 
                         Particle& particle1, Particle& particle2, 
                         Particle& particle3, Particle& particle4,
                         Vertex& constrVtx, 
                         Particle& motherParticle);
  
  /// Actual operator function.
  /// Takes the name of the ressonance(s) to constrain the mass to. 
  
  StatusCode fitWithGeomAndSubMass(const std::string& motherName, 
                                   Particle& particle1, Particle& particle2,
                                   Vertex& constrVtx, 
                                   Particle& motherParticle);
  
  StatusCode fitWithGeomAndSubMass(const std::string& motherName, 
                                   Particle& particle1, Particle& particle2, 
                                   Particle& particle3,Vertex& constrVtx, 
                                   Particle& motherParticle);
  
  StatusCode fitWithGeomAndSubMass(const std::string& motherName, 
                                   const ParticleVector& pList,
                                   Vertex& constrVtx, 
                                   Particle& motherParticle);
  
  
private:
  
  StatusCode fitWithGeomWithoutRess(const std::string& motherName,
                                    const ParticleVector& pList,
                                    Vertex& constrVtx,
                                    Particle& motherParticle );
  
  StatusCode fitWithGeomAndOneSubMass(const std::string& motherName,
                                      double subMass,
                                      const ParticleVector& pDaughter,
                                      const ParticleVector& pList,
                                      Vertex& constrVtx,
                                      Particle& motherParticle );
  
  StatusCode fitWithGeomAndTwoSubMass(const std::string& motherName,
                                      double subMass1,double subMass2,
                                      const ParticleVector& pDaughter1,
                                      const ParticleVector& pDaughter2,
                                      const ParticleVector& pList,
                                      Vertex& constrVtx,
                                      Particle& motherParticle );
  
  
  
  // number of tracks ( = ntracks ) must be 2, 3 or 4.
  int m_ntracks;
  
  /// Performs one iteration of the constrained fit 
  /// It returns the parameters vector (e) and the covariance matrix (Ce) 
  /// after one iteration of the contraint fit.
  
  StatusCode itera(HepVector& e, HepSymMatrix& Ce,
                   const ParticleVector& pDaughter,
                   double subMassConstr,
                   int dimCe, bool final);
  
  StatusCode itera(HepVector& e, HepSymMatrix& Ce,
                   const ParticleVector& pDaughter1,
                   const ParticleVector& pDaughter2,
                   double subMass1Constr, 
                   double subMass2Constr,
                   int dimCe, bool final); 
  
  StatusCode itera(HepVector& e, HepSymMatrix& Ce,
                   int dimCe, bool final);		   
  
  /// Computes the parameters and the covariance matrix of the
  /// new formed particle. 
  StatusCode nwcov(HepVector &mass, HepVector &e, HepSymMatrix &Ce, 
                   double zcer,HepVector &tpf, HepSymMatrix &Cx, int dimCe );
  
  
  /// Evaluates how well the constrained equations are satisfied
  /// at a given iteration. 
  /// It returns the invariant mass at this step ( = massConstrCalc ) and 
  /// the difference in the estimated z-vertex ( = diffZver ).
  
  StatusCode evalu(HepVector& e, const ParticleVector& pDaughter,
                   double&  subMassConstrCalc, HepVector&  diffZVer);
  
  StatusCode evalu(HepVector& e,const ParticleVector& pDaughter1,
                   const ParticleVector& pDaughter2,
                   double&  subMass1ConstrCalc, 
                   double&  subMass2ConstrCalc,
                   HepVector&  diffZVer);
  
  StatusCode evalu(HepVector& e, HepVector&  diffZVer);
  
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

#endif // LAGRANGEGEOMVERTEXFITTER_H

